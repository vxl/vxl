/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "gtrl_polygon.h"

gtrl_polygon::gtrl_polygon( const vcl_vector<gtrl_vertex_sptr> ps)
  : ps_(ps)
{
}


// simple and efficient point in polygon test.
//   from comp.graphics.algorithms faq
bool gtrl_polygon::inside( const gtrl_vertex_sptr point) const
{
    bool c = false;

    for (unsigned int i = 0, j = ps_.size()-1; i < ps_.size(); j = i++)
      {
        if ((((ps_[i]->y()<=point->y()) && (point->y()<ps_[j]->y())) ||
             ((ps_[j]->y()<=point->y()) && (point->y()<ps_[i]->y()))) &&
          (point->x() < (ps_[j]->x() - ps_[i]->x()) * (point->y() - ps_[i]->y())
          / (ps_[j]->y() - ps_[i]->y()) + ps_[i]->x()))
          c = !c;
      }

  return c;
}
