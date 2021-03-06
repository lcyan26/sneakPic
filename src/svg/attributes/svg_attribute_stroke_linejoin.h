#ifndef SVG_ATTRIBUTE_STROKE_LINEJOIN_H
#define SVG_ATTRIBUTE_STROKE_LINEJOIN_H

#include "svg/attributes/abstract_attribute.h"

#include <QString>
#include <unordered_map>

class svg_attribute_stroke_linejoin : public abstract_attribute
{
  SVG_ATTRIBUTE

  Qt::PenJoinStyle m_linejoin;
public:
  svg_attribute_stroke_linejoin ();
  virtual ~svg_attribute_stroke_linejoin ();

  Qt::PenJoinStyle get_stroke_linejoin () const { return m_linejoin; }

  virtual bool read (const char *data, bool from_css = false) override;
  virtual bool write (QString &data, bool to_css = false) const override;

  void set_value (Qt::PenJoinStyle value) { m_linejoin = value; };
  Qt::PenJoinStyle value () const {return m_linejoin; }
};
#endif // SVG_ATTRIBUTE_STROKE_LINEJOIN_H
