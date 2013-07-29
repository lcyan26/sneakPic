#ifndef BASE_SVG_ITEM_H
#define BASE_SVG_ITEM_H

#include <unordered_map>
#include <QString>

#include "common/tree_structure.h"

#define SVG_ITEM                                   \
public:                                            \
virtual svg_item_type type () const override;      \
static svg_item_type static_type ();               \
static const char *static_type_name ();            \
static svg_namespaces_t static_ns_type ();         \
static const char *static_ns_URI ();               \
virtual const char *type_name () const override;   \
virtual svg_namespaces_t namespace_type () const;  \
private:                                           \

class QXmlStreamWriter;

class abstract_attribute;
class svg_document;
class svg_item_defs;
class abstract_renderer_item;
class svg_graphics_item;

enum class svg_namespaces_t;
enum class svg_item_type;
enum class svg_inherit_type;
enum class overlay_item_type;
enum class svg_attribute_type;

class abstract_svg_item : public tree_structure<abstract_svg_item>
{
  svg_document *m_document;
  std::unordered_map<std::string, abstract_attribute *> m_attributes;

  /// If item is a part of a tree generated by "use" element, m_original_item is an id of the original item
  /// Otherwise, it is empty
  QString m_original_id;
  QString m_own_id;

public:
  abstract_svg_item (svg_document *document);
  virtual ~abstract_svg_item ();

  virtual svg_item_type type () const = 0;
  virtual svg_namespaces_t namespace_type () const = 0;
  virtual const char *type_name () const = 0;

  virtual const char *namespace_uri () const;
  virtual const char *namespace_name () const;

  svg_document *document () const { return m_document; }

  void add_attribute (abstract_attribute *attribute);
  void remove_attribute (abstract_attribute *attribute);

  void write (QXmlStreamWriter &writer) const;
  virtual bool read_item (const QString &/*data*/) { return true; }

  bool has_name () const;
  QString name () const;

  bool is_xml_class (const QString &class_name) const;

  /// returns attribute for the current item
  template <typename T>
  T *get_attribute () const
  {
    abstract_attribute *attribute = get_attribute (T::static_type_name ());
    if (!attribute || attribute->type () != T::static_type ())
      return nullptr;
    return static_cast< T *> (attribute);
  }

  /// returns attribute with respect to styling and css
  template <typename T>
  const T *get_computed_attribute (const T *default_val = static_cast<const T *> (T::default_value ())) const
  {
    const abstract_attribute *attribute = get_computed_attribute (T::static_type_name (), T::static_inherit_type (), T::static_type ());
    /// if not found, return default value
    if (!attribute || attribute->type () != T::static_type ())
      attribute = default_val;

    return static_cast<const T *>(attribute);
  }

  /// checks for correctness
  bool check ();

  bool is_cloned () const;
  /// creates cloned item, for "use" item
  abstract_svg_item *create_clone ();

  void get_used_namespaces (std::map<QString, QString> &map) const;
  void process_after_read ();

  virtual const svg_graphics_item *to_graphics_item () const { return nullptr; }
  virtual svg_graphics_item *to_graphics_item () { return nullptr; }

protected:
  virtual bool write_item (QString &/*data*/) const { return true; }
  virtual bool check_item () = 0;

private:
  QString full_name (const QString &namespace_name, const QString &local_name) const;
  void add_to_container ();
  void remove_from_container ();
  abstract_attribute *get_attribute (const char *data) const;
  const abstract_attribute *get_computed_attribute (const char *data, svg_inherit_type inherit_type, svg_attribute_type attr_type) const;
  const abstract_attribute *find_attribute_in_selectors (const char *data, const abstract_svg_item *item, svg_attribute_type attr_type) const;
  const abstract_svg_item *get_original_item () const;
  void create_id_by_attr ();
  void create_unique_name ();

};

#endif // BASE_SVG_ITEM_H

