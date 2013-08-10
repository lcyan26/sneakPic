#include "svg/svg_document.h"

#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "common/memory_deallocation.h"
#include "common/debug_utils.h"

#include "svg/attributes/abstract_attribute.h"
#include "svg/attributes/svg_attribute_type.h"
#include "svg/attributes/svg_attribute_factory.h"

#include "svg/items/svg_item_factory.h"
#include "svg/items/svg_item_type.h"
#include "svg/items/svg_items_container.h"
#include "svg/items/abstract_svg_item.h"
#include "svg/items/svg_item_svg.h"
#include "svg/items/svg_graphics_item.h"
#include "svg/items/svg_character_data.h"

#include "svg/css/selectors_container.h"

#include "svg/svg_namespaces.h"

#include "renderer/renderer_items_container.h"
#include "renderer/abstract_renderer_item.h"
#include "renderer/rendered_items_cache.h"
#include "attributes/abstract_attribute.h"



svg_document::svg_document (settings_t *settings)
{
  m_item_factory = new svg_item_factory (this);
  m_attribute_factory = new svg_attribute_factory (this);
  m_item_container = new svg_items_container;
  m_selectors = new selectors_container;
  m_root = nullptr;
  item_svg = nullptr;
  m_last_overlay_num = 0;
  m_settings = settings;
}

svg_document::~svg_document ()
{
  FREE (m_root);
  FREE (m_item_factory);
  FREE (m_attribute_factory);
  FREE (m_item_container);
  FREE (m_selectors);
}

static inline QString get_namespace_name (const QXmlStreamNamespaceDeclarations &declarations, const QString &uri)
{
  for (int i = 0; i < declarations.size (); i++)
    if (declarations[i].namespaceUri () == uri)
      return declarations[i].prefix ().toString ();

  return QString ();
}

bool svg_document::read_file (const QString &filename_arg)
{
  filename = filename_arg;

  QFile file (filename);
  if (!file.open (QIODevice::ReadOnly))
    return false;


  QXmlStreamReader reader (&file);
  abstract_svg_item *cur_item = nullptr;
  while (!reader.atEnd ())
    {
      switch (reader.readNext ())
        {
          case QXmlStreamReader::Invalid:
          case QXmlStreamReader::NoToken:
            return false;
          case QXmlStreamReader::Comment:
          case QXmlStreamReader::DTD:
          case QXmlStreamReader::EndDocument:
          case QXmlStreamReader::EntityReference:
          case QXmlStreamReader::ProcessingInstruction:
          case QXmlStreamReader::StartDocument:
            break;

          case QXmlStreamReader::Characters:
            {
              if (!cur_item)
                DEBUG_PAUSE ("cur_item must not be nullptr");

              QString char_data = reader.text ().toString ().trimmed ();
              if (!char_data.isEmpty ())
                {
                  svg_character_data *data = new svg_character_data (this, reader.text ().toUtf8 ().constData ());
                  cur_item->insert_child (nullptr, data);
                }
              break;
            }
          case QXmlStreamReader::EndElement:
            {
              if (!cur_item)
                DEBUG_PAUSE ("cur_item must not be nullptr");

              cur_item->item_read_complete ();
              cur_item = cur_item->parent ();
              break;
            }
          case QXmlStreamReader::StartElement:
            {
              cur_item = process_new_item (reader, cur_item);
              break;
            }

        }
    }

  if (reader.hasError () || !m_root)
    return false;

  if (m_root->type () == svg_item_type::SVG)
    item_svg = static_cast<svg_item_svg *> (m_root);
  else
    return false;

  if (!item_svg->check ())
    return false;

  svg_graphics_item *graphics_item = item_svg->to_graphics_item ();
  if (!graphics_item)
    return false;

  graphics_item->update_bbox ();
  return true;
}

QString svg_document::get_filename ()
{
  return filename;
}

bool svg_document::write_file (const QString &filename_arg)
{
  filename = filename_arg;
  QFile file (filename);
  if (!file.open (QIODevice::WriteOnly))
    return false;

  QXmlStreamWriter writer (&file);
  writer.setAutoFormatting(true);
  writer.writeDefaultNamespace (svg_namespaces::svg_uri ());
  std::map<QString, QString> namespaces;
  m_root->get_used_namespaces (namespaces);
  for (auto namespace_pair : namespaces)
    writer.writeNamespace (namespace_pair.first, namespace_pair.second);
  writer.writeStartDocument();
  m_root->write (writer);
  writer.writeEndDocument();
  return false;
}

bool svg_document::get_doc_dimensions (double &width, double &height)
{
  if (!item_svg)
    return false;

  width = item_svg->width ();
  height = item_svg->height ();
  return true;
}

renderer_items_container *svg_document::create_rendered_items (rendered_items_cache *cache)
{
  renderer_items_container *renderer_items = new renderer_items_container;
  renderer_items->set_cache (cache);
  if (cache)
    cache->clear_selection_mapping ();

  create_renderer_item (renderer_items, item_svg);
  renderer_items->set_root (item_svg->name ().toStdString ());
  renderer_items->root ()->update_bbox ();
  return renderer_items;
}

void svg_document::create_renderer_item (renderer_items_container *renderer_items, abstract_svg_item *svg_item)
{
  svg_graphics_item *graphics_item = svg_item->to_graphics_item ();
  if (!graphics_item)
    return;
  abstract_renderer_item *renderer_item = graphics_item->create_renderer_item ();
  if (!renderer_item)
    return;

  renderer_items->add_item (renderer_item);
  if (svg_item->parent ())
    renderer_items->add_child (svg_item->parent ()->name ().toStdString (), svg_item->name ().toStdString ());

  for (abstract_svg_item *child = svg_item->first_child (); child; child = child->next_sibling ())
    create_renderer_item (renderer_items, child);
}

abstract_svg_item *svg_document::process_new_item (QXmlStreamReader &reader, abstract_svg_item *cur_item)
{
  QString namespace_uri = reader.namespaceUri ().toString ();
  QString name = reader.name ().toString ();
  QString namespace_name = get_namespace_name (reader.namespaceDeclarations (), namespace_uri);
  abstract_svg_item *child_item = m_item_factory->create_item (name, namespace_uri, namespace_name);

  QXmlStreamAttributes attributes = reader.attributes ();

  for (int i = 0; i < attributes.size (); i++)
    {
      QStringRef attribute_namespace_uri = attributes[i].namespaceUri ();
      QStringRef attribute_name = attributes[i].name ();
      QStringRef attribute_namespace_name = attributes[i].prefix ();
      abstract_attribute *attribute = m_attribute_factory->create_attribute (child_item,
        attribute_name.toLatin1 ().constData (), attribute_namespace_uri.toLatin1 ().constData (), attribute_namespace_name.toLatin1 ().constData ());

      if (attribute->inherit_type () != svg_inherit_type::NONE && attributes[i].value () == "inherit")
        attribute->set_is_inherited (true);

      if (attribute->read (attributes[i].value ().toLatin1 ().constData ()))
        child_item->add_attribute (attribute);
      else
        FREE (attribute);
    }

  child_item->process_after_read ();
  if (cur_item)
    cur_item->insert_child (nullptr, child_item);
  else
    m_root = child_item;
  return child_item;
}

QString svg_document::create_overlay_name ()
{
  return QString ("#overlay%1").arg (m_last_overlay_num++);
}
