#ifndef GUI_DOCUMENT_H
#define GUI_DOCUMENT_H

#include <QObject>
#include <QString>

class QTimer;
class settings_t;
class rendered_items_cache;
class renderer_thread;
class events_queue;
class svg_document;
class svg_painter;
class tools_container;
class gl_widget;
class actions_applier;

enum class gui_action_id;

class gui_document : public QObject
{
  Q_OBJECT
public:
  settings_t *m_settings;

  QTimer *update_timer;
  rendered_items_cache *m_cache;
  renderer_thread      *m_renderer_thread;
  events_queue         *m_queue;

  svg_document *m_doc;
  svg_painter  *m_painter;
  tools_container *m_tools_container;
  actions_applier *m_applier;

public:
  gui_document (settings_t *settings);
  ~gui_document ();

  svg_painter *create_painter (gl_widget *widget);
  svg_painter *painter () const { return m_painter; }

  bool open_file (const QString &filename);
  bool save_file (const QString &filename);

  QString get_filename () const;
  bool action_triggered (gui_action_id id);

private slots:
  void update_timeout ();
  bool undo ();
  bool redo ();
};
  

#endif // GUI_DOCUMENT_H
