#ifndef vgl_box_2d_h
#define vgl_box_2d_h
#ifdef __GNUC__
#pragma interface
#endif


//:
//  \file
//  \brief Contains class to implement a bounding box.
//  \author Don Hamilton 15/2/2000
//  \author Peter Tu

//
//  Modifications
//  IMS (Manchester) 14/03/2001: Tidied up the documentation + added binary_io
//

#include <vcl_iostream.h>

template <class Type>
class vgl_point_2d;

//: Represents a 2D box
//  A 2d box with sides aligned with the x and y axes.
//  Also supports operations required of a bounding box for geometric region
//  tests.
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

template <class Type>
class vgl_box_2d {

  // PUBLIC INTERFACE----------------------------------------------------------
public:

  // Constructors/Destructors--------------------------------------------------

  //: Default constructor (creates undefined box)
  vgl_box_2d() {}

  //: Construct using lower-left and upper-right co-ordinates
  vgl_box_2d(const Type min_position[2],
             const Type max_position[2] );


  //: Construct using lower-left and upper-right co-ordinates
  vgl_box_2d(const vgl_point_2d<Type>& min_position,
             const vgl_point_2d<Type>& max_pos);

  //: Construct using ranges in x and y
  vgl_box_2d(Type xmin, Type xmax, Type ymin, Type ymax);

  //: Construct using lower-left, width and height
  vgl_box_2d(const Type min_position[2],
             Type width, Type height);

  //: Construct using lower-left, width and height
  vgl_box_2d(const vgl_point_2d<Type>& min_position, Type width, Type height);

  //: Copy constructor
  vgl_box_2d(const vgl_box_2d& that) { *this = that; }

  //: Destructor
  ~vgl_box_2d() {}

  //: Copy operator
  vgl_box_2d& operator=(const vgl_box_2d& that){
    // gcc 2.95 does not allow :
    //this->min_pos_ = that.min_pos_;
    //this->max_pos_ = that.max_pos_;
    min_pos_[0] = that.min_pos_[0]; min_pos_[1] = that.min_pos_[1];
    max_pos_[0] = that.max_pos_[0]; max_pos_[1] = that.max_pos_[1];
    return *this;
  }


  // Data Access---------------------------------------------------------------
  //: Get min x
  inline Type get_min_x() const {return min_pos_[0];}
  //: Get min y
  inline Type get_min_y() const {return min_pos_[1];}
  //: Get max x
  inline Type get_max_x() const {return max_pos_[0];}
  //: Get max y
  inline Type get_max_y() const {return max_pos_[1];}

  //: Get x component of centroid
  Type get_centroid_x() const;
  //: Get y component of centroid
  Type get_centroid_y() const;

  //: Get width of box
  Type get_width()  const;

  //: Get height of box
  Type get_height() const;

  //: Lower left corner of box
  vgl_point_2d<Type> get_min_point() const;

  //: Upper right corner of box
  vgl_point_2d<Type> get_max_point() const;

  //: Centroid of box
  vgl_point_2d<Type> get_centroid_point() const;

  //: Set left side of box (other side ordinates unchanged)
  inline void set_min_x(Type min_x) {min_pos_[0]=min_x;}
  //: Set bottom of box (other side ordinates unchanged)
  inline void set_min_y(Type min_y) {min_pos_[1]=min_y;}
  //: Set right side (other side ordinates unchanged)
  inline void set_max_x(Type max_x) {max_pos_[0]=max_x;}
  //: Set top (other side ordinates unchanged)
  inline void set_max_y(Type max_y) {max_pos_[1]=max_y;}

  //: Move box so centroid lies at centroid_x (width and height unchanged)
  void set_centroid_x(Type centroid_x);
  //: Move box so centroid lies at centroid_y (width and height unchanged)
  void set_centroid_y(Type centroid_y);

  //: Modify width, retaining centroid at current position
  void set_width(Type width);
  //: Modify height, retaining centroid at current position
  void set_height(Type height);

  //: Modify bottom left.  Top right only changed if necessary to avoid -ive size
  void setmin_position(Type min_position[2]);
  //: Modify top right.  Bottom left only changed if necessary to avoid -ive size
  void setmax_position(Type max_position[2]);
  //: Modify bottom left.  Top right only changed if necessary to avoid -ive size
  void set_min_point(vgl_point_2d<Type>& min_point);
  //: Modify top right.  Bottom left only changed if necessary to avoid -ive size
  void set_max_point(vgl_point_2d<Type>& max_point);

  //: Move box so centroid lies at given position (width, height unchanged)
  void set_centroid(Type centroid[2]);

  //: Move box so centroid lies at given position (width, height unchanged)
  void set_centroid(vgl_point_2d<Type>& centroid);

  // Data Control--------------------------------------------------------------

  //: Return true if (x,y) inside box, ie x_min<=x<=x_max etc
  bool contains(Type const& x, Type const& y) const {
    return (x >= min_pos_[0] &&
      x <= max_pos_[0] &&
      y >= min_pos_[1] &&
      y <= max_pos_[1]);
  }

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

private:
  // Helpers-------------------------------------------------------------------
};

//: Print to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_box_2d<Type>& p) {
  return p.print(s);
}

//: Read from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_box_2d<Type>& p) {
  return p.read(is);
}

//: Return box defining intersection between in1 and in2
//  Empty box (0,0,0,0) returned if no intersection.
template <class Type>
vgl_box_2d<Type> intersect(vgl_box_2d<Type> const& in1, vgl_box_2d<Type> const& in2);

#define VGL_BOX_2D_INSTANTIATE(T) extern "Include vgl_box_2d.txx"

#endif // vgl_box_2d_h
