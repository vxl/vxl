// This is core/vgl/vgl_box_3d.h
#ifndef vgl_box_3d_h
#define vgl_box_3d_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Contains class to represent a cartesian 3D bounding box.
// \author Don Hamilton, Peter Tu
// \date   15/2/2000
//
// \verbatim
//  Modifications
//   Peter Vanroose, 28 Feb.2000: lots of minor corrections
//   NPC (Manchester) 14/03/2001: Tidied up the documentation + added binary_io
//   Peter Vanroose, 10 Jul.2001: Deprecated get_*() in favour of *(), and explicit casts
//   Peter Vanroose,  5 Oct.2001: Added operator==() and methods is_empty() and contains()
//   Peter Vanroose,  6 Oct.2001: Added method add(vgl_point_3d<T>) to enlarge a box
//   Peter Vanroose,  7 Oct.2001: Removed deprecated get_*() functions
//   Peter Vanroose,    Feb.2002: brief doxygen comment placed on single line
//   Peter Vanroose, 12 Sep.2002: Added method add(vgl_box_3d<T>) to enlarge a box
//   Peter Vanroose, 13 May 2003: Constructor interface change (compat with vgl_box_2d)
//   Peter Vanroose  15 Oct 2003: Removed deprecated constructors without 5th arg
//   Peter Vanroose  16 Oct 2003: Corner pts given to constructor may now be in any order
//   Peter Vanroose  16 Oct 2003: Added intersect(box1,box2)
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_fwd.h> // forward declare vgl_point_3d

//: Represents a cartesian 3D box
//  A 3d box with sides aligned with \a x, \a y and \a z axes. Supports operations
//  required of a bounding box for geometric volume tests.
//
//  A box can be empty; this is what the default constructor creates, or what
//  is left after applying the empty() method.  Use the add() methods to enlarge
//  a box, and use the contains() methods to check for inclusion of a point or
//  an other box.
//
//  To make the convex union of two boxes, use box1.add(box2).
//  \verbatim
//                                 MaxPosition
//                       |<--width-->|
//                       O-----------O  ---
//                      /           /|   ^
//                     /           / |   |
//                    O-----------O  | height
//                    |       o   |  |   |
//                    |  centroid |  |   v
//                    |           |  O  ---
//     Y              |           | /   /_____depth
//     |   Z          |           |/   /
//     |  /           O-----------O  ---
//     | /         MinPosition
//     O-----X
// \endverbatim
// \sa vgl_box_2d

template <class Type>
class vgl_box_3d
{
 public:

  //: Default constructor (creates empty box)
  vgl_box_3d();

  //: Construct using two corner points
  vgl_box_3d(Type const min_position[3],
             Type const max_position[3]);

  //: Construct using two corner points
  vgl_box_3d(vgl_point_3d<Type> const& min_pos,
             vgl_point_3d<Type> const& max_pos);

  //: Construct from ranges in \a x,y,z (take care with order of inputs).
  //  The \a x range is given by the 1st and 4th coordinates,
  //  the \a y range is given by the 2nd and 5th coordinates,
  //  the \a z range is given by the 3rd and 6th coordinates.
  vgl_box_3d(Type xmin, Type ymin, Type zmin,
             Type xmax, Type ymax, Type zmax);

  enum point_type { centre=0, min_pos, max_pos };

  //: Construct a box sized width x height x depth at a given reference point.
  //  The box will either be centered at ref_point or will have ref_point
  //  as its min-position or max-position, as specified by the 5th argument.
  vgl_box_3d(Type const ref_point[3],
             Type width, Type height, Type depth,
             point_type);

  //: Construct a box sized width x height x depth at a given reference point.
  //  The box will either be centered at ref_point or will have ref_point
  //  as its min-position or max-position, as specified by the 5th argument.
  vgl_box_3d(vgl_point_3d<Type> const& ref_point,
             Type width, Type height, Type depth,
             point_type);

  //: Equality test
  inline bool operator==(vgl_box_3d<Type> const& b) const {
    // All empty boxes are equal:
    if (b.is_empty()) return is_empty();
    return min_x() == b.min_x() && min_y() == b.min_y() && min_z() == b.min_z()
        && max_x() == b.max_x() && max_y() == b.max_y() && max_z() == b.max_z();
  }

  // Data Access---------------------------------------------------------------

  //: Get width of this box (= \a x dimension)
  Type width() const;
  //: Get height of this box (= \a y dimension)
  Type height() const;
  //: Get depth of this box (= \a z dimension)
  Type depth() const;

  //: Get volume of this box
  inline Type volume() const { return width()*height()*depth(); }

  //: Get min \a x
  inline Type min_x() const { return min_pos_[0]; }
  //: Get min \a y
  inline Type min_y() const { return min_pos_[1]; }
  //: Get min \a z
  inline Type min_z() const { return min_pos_[2]; }

  //: Get max \a x
  inline Type max_x() const { return max_pos_[0]; }
  //: Get max \a y
  inline Type max_y() const { return max_pos_[1]; }
  //: Get max \a z
  inline Type max_z() const { return max_pos_[2]; }

  //: Get the centroid point
  vgl_point_3d<Type> centroid() const;
  //: Get \a x component of centroid
  Type centroid_x() const;
  //: Get \a y component of centroid
  Type centroid_y() const;
  //: Get \a z component of centroid
  Type centroid_z() const;

  //: Return lower left corner of box
  vgl_point_3d<Type> min_point() const;

  //: Return upper right corner of box
  vgl_point_3d<Type> max_point() const;

  // Data Control--------------------------------------------------------------

  //: Return true if this box is empty
  inline bool is_empty() const {
    return min_x() > max_x() || min_y() > max_y() || min_z() > max_z();
  }

  //: Add a point to this box.
  // Do this by possibly enlarging the box so that the point just falls within the box.
  // Adding a point to an empty box makes it a size zero box only containing p.
  void add(vgl_point_3d<Type> const& p);

  //: Make the convex union of two boxes.
  // Do this by possibly enlarging this box so that the corner points of the
  // given box just fall within the box.
  // Adding an empty box does not change the current box.
  void add(vgl_box_3d<Type> const& b);

  //: Return true iff the point p is inside this box
  bool contains(vgl_point_3d<Type> const& p) const;

  //: Return true iff the corner points of b are inside this box
  bool contains(vgl_box_3d<Type> const& b) const;

  //: Return true if \a (x,y,z) is inside this box, ie \a x_min <= \a x <= \a x_max etc
  inline bool contains(Type const& x, Type const& y, Type const& z) const {
    return x >= min_x() && x <= max_x() &&
           y >= min_y() && y <= max_y() &&
           z >= min_z() && z <= max_z();
  }

  //: Make the box empty
  void empty();

  //: Set min \a x ordinate of box (other sides unchanged)
  inline void set_min_x(Type m) { min_pos_[0]=m; }
  //: Set min \a y ordinate of box (other sides unchanged)
  inline void set_min_y(Type m) { min_pos_[1]=m; }
  //: Set min \a z ordinate of box (other sides unchanged)
  inline void set_min_z(Type m) { min_pos_[2]=m; }

  //: Set max \a x ordinate of box (other sides unchanged)
  inline void set_max_x(Type m) { max_pos_[0]=m; }
  //: Set max \a y ordinate of box (other sides unchanged)
  inline void set_max_y(Type m) { max_pos_[1]=m; }
  //: Set max \a z ordinate of box (other sides unchanged)
  inline void set_max_z(Type m) { max_pos_[2]=m; }

  //: Move box so centroid lies at cx (size unchanged)
  void set_centroid_x(Type cx);
  //: Move box so centroid lies at cy (size unchanged)
  void set_centroid_y(Type cy);
  //: Move box so centroid lies at cz (size unchanged)
  void set_centroid_z(Type cz);

  //: Set width (x), centroid unchanged
  void set_width(Type width);
  //: Set height (y), centroid unchanged
  void set_height(Type height);
  //: Set depth (z), centroid unchanged
  void set_depth(Type depth);

  //: Modify min corner point. Max corner point only changed if necessary to avoid empty box
  void set_min_position(Type const m[3]);
  //: Modify max corner point. Min corner point only changed if necessary to avoid empty box
  void set_max_position(Type const m[3]);
  //: Modify min corner point. Max corner point only changed if necessary to avoid empty box
  void set_min_point(vgl_point_3d<Type> const& min_pt);
  //: Modify max corner point. Min corner point only changed if necessary to avoid empty box
  void set_max_point(vgl_point_3d<Type> const& max_pt);
  //: Move box so centroid lies at c (size unchanged)
  inline void set_centroid(Type const c[3]) { set_centroid_x(c[0]); set_centroid_y(c[1]); set_centroid_z(c[2]); }
  //: Move box so centroid lies at c (size unchanged)
  inline void set_centroid(vgl_point_3d<Type> const& c) { set_centroid_x(c.x()); set_centroid_y(c.y()); set_centroid_z(c.z()); }

  // I/O-----------------------------------------------------------------------

  //: Write "<vgl_box_3d x0,y0,z0 to x1,y1,z1>" to stream
  vcl_ostream& print(vcl_ostream&) const;

  //: Write "x0 y0 z0 x1 y1 z1(endl)" to stream
  vcl_ostream& write(vcl_ostream&) const;

  //: Read x0,y0,z0,x1,y1,z1 from stream
  vcl_istream& read(vcl_istream&);

  // INTERNALS-----------------------------------------------------------------
 protected:
  // Data Members--------------------------------------------------------------
  Type min_pos_[3];
  Type max_pos_[3];
};


//: Write box to stream
// \relates vgl_box_3d
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, vgl_box_3d<Type> const& p);

//: Read box from stream
// \relates vgl_box_3d
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_box_3d<Type>& p);

//: Return box defining intersection between boxes a and b.
//  Empty box returned if no intersection.
// \relates vgl_box_2d
template <class Type>
vgl_box_3d<Type> intersect(vgl_box_3d<Type> const& a, vgl_box_3d<Type> const& b);

#define VGL_BOX_3D_INSTANTIATE(T) extern "please include vgl/vgl_box_3d.txx first"

#endif // vgl_box_3d_h
