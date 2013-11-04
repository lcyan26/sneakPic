#ifndef GEOM_HELPERS_H
#define GEOM_HELPERS_H

#include <QPointF>

#include "common/math_defs.h"

namespace geom_helpers
{
  inline bool are_line (QPointF points[3])
  {

    return are_equal (points[0].x () * points[1].y () + points[1].x () * points[2].y () + points[2].x () * points[0].y (), 
                      points[0].x () * points[2].y () + points[1].x () * points[0].y () + points[2].x () * points[1].y ());
  }

  inline bool are_line (QPointF first, QPointF second, QPointF third)
  {
    QPointF points[] = { first, second, third };
    return are_line (points);
  }
};

#endif // GEOM_HELPERS_H