// This is vxl/vgl/vgl_box_2d.h
#ifndef vgl_box_2d_h
#define vgl_box_2d_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
//  \brief Contains class to implement a bounding box.
//  \author Don Hamilton
//          Peter Tu
//  \date   15/2/2000
//
// \verbatim
//  Modifications
//  IMS (Manchester) 14/03/2001: Tidied up the documentation + added binary_io
//  Amitha Perera    10/07/2001: Deprecated get_*() in favour of *(), as agreed in Zurich.
//  Peter Vanroose    5/10/2001: Added operator==() and is_empty()
//  Peter Vanroose    6/10/2001: Added method add(vgl_point_2d<T>) to enlarge a box
//  Peter Vanroose    7/10/2001: Removed deprecated get_*() functions
//  Peter Vanroose     Feb.2002: brief doxygen comment placed on single line
//  Peter Vanroose   12Sep.2002: Added method add(vgl_box_2d<T>) to enlarge a box
// \endverbatim

#include <vcl_iosfwd.h>
#include <vgl/vgl_fwd.h> // forward declare vgl_point_2d

//: Represents a 2D box
//  A 2d box with sides aligned with the \a x and \a y axes.
//  Also supports operations required of a bounding box for geometric region
//  tests.
//  A box can be empty; this is the case when MaxPosition is to the left or
//  below MinPosition.
//  \verbatim
//                                  MaxPosition
//                    O------------O
//                    |            |
//                    |            |
//                    |  Centroid  |
//                    |      o     |
//                    |            |
//        Y           |            |
//        |           |            |
//        |           O------------O
//        |       MinPosition
//        O------X
// \endverbatim
// If you are using a vgl_box_2d<int> to indicate a window on an image, do not forget
// that your axes will be flipped. You could think of the window as follows.
//  \verbatim
//        O------X
//        |       MinPosition
//        |             O------------O
//        |             |            |
//        Y             |            |
//                      |  Centroid  |
//                      |      o     |
//                      |            |
//                      |            |
//                      |            |
//                      O------------O
//                               MaxPosition
// \endverbatim
// \sa vgl_box_3d


template <class Type>
class vgl_box_2d
{
 public:

  // Constructors/Destructor---------------------------------------------------

  //: Default constructor (creates empty box)
  vgl_box_2d();

  //: Construct using lower-left and upper-right co-ordinates
  vgl_box_2d(const Type min_position[2],
             const Type max_position[2] );

  //: Construct using lower-left and upper-right co-ordinates
  vgl_box_2d(const vgl_point_2d<Type>& min_pos,
             const vgl_point_2d<Type>& max_pos);

  //: Construct using ranges in \a x and \a y
  vgl_box_2d(Type xmin, Type xmax, Type ymin, Type ymax);

  //: Construct using lower-left, width and height
  vgl_box_2d(const Type min_position[2],
             Type width, Type height);

  //: Construct using lower-left, width and height
  vgl_box_2d(const vgl_point_2d<Type>& min_position, Type width, Type height);

  //: Copy constructor
  inline vgl_box_2d(const vgl_box_2d& that) { *this = that; }

  //: Destructor
  inline ~vgl_box_2d() {}

  //: Copy assignment operator
  inline vgl_box_2d& operator=(vgl_box_2d<Type> const& that){
    min_pos_[0] = that.min_x(); min_pos_[1] = that.min_y();
    max_pos_[0] = that.max_x(); max_pos_[1] = that.max_y();
    return *this;
  }

  //: Equality test
  inline bool operator==(vgl_box_2d<Type> const& that) const {
    // All empty boxes are equal:
    if (that.is_empty() ) return is_empty();
    return  min_x() == that.min_x() && min_y() == that.min_y()
         && max_x() == that.max_x() && max_y() == that.max_y();
  }

  // Data Access---------------------------------------------------------------

  //: Get min \a x
  inline Type min_x() const {return min_pos_[0];}
  //: Get min \a y
  inline Type min_y() const {return min_pos_[1];}
  //: Get max \a x
  inline Type max_x() const {return max_pos_[0];}
  //: Get max \a y
  inline Type max_y() const {return max_pos_[1];}

  //: Get centroid point
  vgl_point_2d<Type> centroid() const;
  //: Get \a x component of centroid
  Type centroid_x() const;
  //: Get \a y component of centroid
  Type centroid_y() const;

  //: Get width of this box (= \a x dimension)
  Type width()  const;

  //: Get height of this box (= \a y dimension)
  Type height() const;

  //: Get volume (area) of this box
  Type area() const { return width()*height(); }
  Type volume() const { return width()*height(); }

  //: Lower left corner of box
  vgl_point_2d<Type> min_point() const;

  //: Upper right corner of box
  vgl_point_2d<Type> max_point() const;

  //: Set left side of box (other side ordinates unchanged)
  inline void set_min_x(Type m) {min_pos_[0]=m;}
  //: Set bottom of box (other side ordinates unchanged)
  inline void set_min_y(Type m) {min_pos_[1]=m;}
  //: Set right side (other side ordinates unchanged)
  inline void set_max_x(Type m) {max_pos_[0]=m;}
  //: Set top (other side ordinates unchanged)
  inline void set_max_y(Type m) {max_pos_[1]=m;}

  //: Move box so centroid lies at cx (width and height unchanged)
  void set_centroid_x(Type cx);
  //: Move box so centroid lies at cy (width and height unchanged)
  void set_centroid_y(Type cy);

  //: Modify width, retaining centroid at current position
  void set_width(Type width);
  //: Modify height, retaining centroid at current position
  void set_height(Type height);

  //: Modify bottom left. Top right only changed if necessary to avoid empty box
  void setmin_position(Type const min_position[2]);
  //: Modify top right. Bottom left only changed if necessary to avoid empty box
  void setmax_position(Type const max_position[2]);
  //: Modify bottom left. Top right only changed if necessary to avoid empty box
  void set_min_point(vgl_point_2d<Type> const& min_pt);
  //: Modify top right. Bottom left only changed if necessary to avoid empty box
  void set_max_point(vgl_point_2d<Type> const& max_pt);

  //: Move box so centroid lies at given position (width, height unchanged)
  void set_centroid(Type const centroid[2]);

  //: Move box so centroid lies at given position (width, height unchanged)
  void set_centroid(vgl_point_2d<Type> const& centroid);

  // Data Control--------------------------------------------------------------

  //: Return true if this box is empty
  inline bool is_empty() const {
    return min_x() > max_x() || min_y() > max_y();
  }

  //: Add a point to this box
  // Do this by possibly enlarging the box so that the point just falls within the box.
  // Adding a point to an empty box makes it a size zero box only containing p.
  void add(vgl_point_2d<Type> const& p);

  //: Make the convex union of two boxes
  // Do this by possibly enlarging this box so that the corner points of the
  // given box just fall within the box.
  // Adding an empty box does not change the current box.
  void add(vgl_box_2d<Type> const& b);

  //: Return true iff the point p is inside this box
  bool contains(vgl_point_2d<Type> const& p) const;

  //: Return true iff the corner points of b are inside this box
  bool contains(vgl_box_2d<Type> const& b) const;

  //: Return true if \a (x,y) inside box, ie \a x_min <= \a x <= \a x_max etc
  inline bool contains(Type const& x, Type const& y) const {
    return x >= min_x() && x <= max_x() && y >= min_y() && y <= max_y();
  }

  //: Make the box empty
  void empty();

  //: Write "<vgl_box_2d x0,y0 to x1,y1>" to stream
  vcl_ostream& print(vcl_ostream&) const;

  //: Write "x0 y0 x1 y1(endl)" to stream
  vcl_ostream& write(vcl_ostream&) const;

  //: Read x0,y0,x1,y1 from stream
  vcl_istream& read(vcl_istream&);

  // INTERNALS-----------------------------------------------------------------
 protected:
  // Data Members--------------------------------------------------------------
  Type min_pos_[2];
  Type max_pos_[2];
};

//: Print to stream
// \relates vgl_box_2d
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_box_2d<Type>& p);

//: Read from stream
// \relates vgl_box_2d
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_box_2d<Type>& p);

//: Return box defining intersection between in1 and in2
//  Empty box (0,0,0,0) returned if no intersection.
// \relates vgl_box_2d
template <class Type>
vgl_box_2d<Type> intersect(vgl_box_2d<Type> const& in1, vgl_box_2d<Type> const& in2);

//: Return box which bounds p1 and p2 (ie p1,p2 are any two of the corners)
// \relates vgl_box_2d
template <class Type>
vgl_box_2d<Type> vgl_bounding_box_2d(const vgl_point_2d<Type>& p1,
                                     const vgl_point_2d<Type>& p2);

#define VGL_BOX_2D_INSTANTIATE(T) extern "please include vgl/vgl_box_2d.txx first"

#endif // vgl_box_2d_h
