// This is core/vgl/algo/vgl_rtree_c.h
#ifndef vgl_rtree_c_h_
#define vgl_rtree_c_h_
//:
// \file
// \brief C helper classes for vgl_rtree
// \author J.L. Mundy
// \date November 14, 2008
// \verbatim
//  Modifications
//   <None yet>
// \endverbatim
//
// vgl_rtree stores elements of type V with regions described by
// bounds type B. The C helper class implements the bounding predicates
// between V and B. Thus V and B remain independent of each other.
//
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_area.h>
template <class V, class B, class C> class vgl_rtree_probe;

//: vgl_rtree Class C for V=vgl_point_2d<T>, B = vgl_box_2d<T>
template <class T>
class vgl_rtree_point_box_2d
{
  // only static methods
  vgl_rtree_point_box_2d() = delete;
  ~vgl_rtree_point_box_2d() = delete;

 public:
  typedef vgl_point_2d<T> v_type;
  typedef vgl_box_2d<T> b_type;
  typedef T t_type;
  // Operations------
  static void  init  (vgl_box_2d<T>& b, vgl_point_2d<T> const& p)
  { b = vgl_box_2d<T>();  b.add(p); }

  static void  update(vgl_box_2d<T>& b, vgl_point_2d<T> const& p)
  { b.add(p); }

  static void  update(vgl_box_2d<T>& b0, vgl_box_2d<T> const &b1)
  { b0.add(b1.min_point());  b0.add(b1.max_point()); }

  static bool  meet(vgl_box_2d<T> const& b, vgl_point_2d<T> const& p)
  { return b.contains(p); }

  static bool  meet(vgl_box_2d<T> const& b0, vgl_box_2d<T> const& b1) {
    vgl_box_2d<T> bint = vgl_intersection<T>(b0, b1);
    return !bint.is_empty();
  }

  static float volume(vgl_box_2d<T> const& b)
  { return static_cast<float>(vgl_area(b)); }

  // point meets for a polygon, used by generic rtree probe
  static bool meets(vgl_point_2d<T> const& v, vgl_polygon<T> poly)
  { return poly.contains(v); }

  // box meets for a polygon, used by generic rtree probe
  static bool meets(vgl_box_2d<T> const& b, vgl_polygon<T> poly)
  { return vgl_intersection<T>(b, poly); }
};


//: vgl_rtree Class C for V=vgl_box_2d<T>, B = vgl_rbox_2d<T>
//  Need to distinguish bounds type from stored element type,
//  so create minimal subclass of vgl_box_2d
template <class Type>
class vgl_bbox_2d : public vgl_box_2d<Type>
{
 public:
  //: Default constructor (creates empty box)
  vgl_bbox_2d() = default;

  //: Construct using two corner points
  vgl_bbox_2d(Type const min_position[2],
              Type const max_position[2])
  : vgl_box_2d<Type>(min_position[2], max_position[2]) {}

  //: Construct using two corner points
  vgl_bbox_2d(vgl_point_2d<Type> const& min_pos,
              vgl_point_2d<Type> const& max_pos)
  : vgl_box_2d<Type>(min_pos, max_pos) {}

  //: Construct using ranges in \a x (first two args) and \a y (last two)
  vgl_bbox_2d(Type xmin, Type xmax, Type ymin, Type ymax)
  : vgl_box_2d<Type>(xmin, xmax, ymin, ymax) {}

  //: Equality test
  inline bool operator==(vgl_bbox_2d<Type> const& b) const {
    // All empty boxes are equal:
    if (b.is_empty()) return this->is_empty();
    return  this->min_x() == b.min_x() && this->min_y() == b.min_y()
         && this->max_x() == b.max_x() && this->max_y() == b.max_y();
  }
};

template <class T>
class vgl_rtree_box_box_2d
{
  // only static methods
  vgl_rtree_box_box_2d() = delete;
  ~vgl_rtree_box_box_2d() = delete;

 public:
  typedef vgl_box_2d<T> v_type;
  typedef vgl_bbox_2d<T> b_type;
  typedef T t_type;
  // Operations------
  static void  init  (vgl_bbox_2d<T>& b, vgl_box_2d<T> const& v)
  { b = vgl_bbox_2d<T>();  b.add(v.min_point()); b.add(v.max_point()); }

  static void  update(vgl_bbox_2d<T>& b, vgl_box_2d<T> const& v)
  { b.add(v.min_point()); b.add(v.max_point()); }

  static void  update(vgl_bbox_2d<T>& b0, vgl_bbox_2d<T> const &b1)
  { b0.add(b1.min_point());  b0.add(b1.max_point()); }

  static bool  meet(vgl_bbox_2d<T> const& b0, vgl_box_2d<T> const& v) {
    bool resultf =(b0.contains(v.min_point()) || b0.contains(v.max_point()));
    bool resultr =(v.contains(b0.min_point()) || v.contains(b0.max_point()));
    return resultf||resultr;
  }

  static bool  meet(vgl_bbox_2d<T> const& b0, vgl_bbox_2d<T> const& b1) {
    bool resultf =(b0.contains(b1.min_point()) || b0.contains(b1.max_point()));
    bool resultr =(b1.contains(b0.min_point()) || b1.contains(b0.max_point()));
    return resultf||resultr;
  }

  static float volume(vgl_box_2d<T> const& b)
  { return static_cast<float>(vgl_area(b)); }

  // box_2d meets for a polygon, used by generic rtree probe
  static bool meets(vgl_box_2d<T> const& b, vgl_polygon<T> poly)
  { return vgl_rtree_point_box_2d<T>::meets(b, poly); }

  static bool meets(vgl_bbox_2d<T> const& b, vgl_polygon<T> poly)
  { return vgl_rtree_point_box_2d<T>::meets(b, poly); }
};

template <class V, class B, class C>
class vgl_rtree_polygon_probe : public vgl_rtree_probe<V, B, C>
{
  typedef typename C::t_type T;
  vgl_polygon<T> poly_;
 public:
  vgl_rtree_polygon_probe(vgl_polygon<T> const& poly): poly_(poly) {}

  //: return true if the probe "meets" the given object.
  bool meets(V const &v) const override {return C::meets(v, poly_); }
  bool meets(B const &b) const override {return C::meets(b, poly_); }
};

#endif // vgl_rtree_c_h_
