#ifndef vgl_box_3d_h
#define vgl_box_3d_h
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vgl/vgl_box_3d.h

//:
// \file
// \brief Describe a 3D box.
// \author Don Hamilton, Peter Tu
//
// \verbatim
// Modifications
//  Peter Vanroose, Feb 28 2000: lots of minor corrections
//  NPC (Manchester) 14/03/2001: Tidied up the documentation + added binary_io
// \endverbatim

#include <vcl_iostream.h>
#include <vcl_string.h>

template <class Type>
class vgl_point_3d;

//: Represents a 3D box
//  A 3d box with sides aligned with x, y and z axes. Supports operations
//  required of a bounding box for geometric volume tests.
//
//  \verbatim
//                                 MaxPosition
//                       |<--width-->|
//                       O-----------O  ---
//                      /           /|   ^
//                     /           / |   |
//                    O-----------O  | depth
//                    |           |  |   |
//                    |  centroid |  |   v
//                    |     o     |  O  ---
//     Z              |           | /   /_____height
//     |   Y          |           |/   /
//     |  /           O-----------O  ---
//     | /         MinPosition
//     O-----X
// \endverbatim

template <class Type>
class vgl_box_3d {

   // PUBLIC INTERFACE----------------------------------------------------------

public:

  //: Default constructor (creates undefined box)
  vgl_box_3d() {}

  //: Construct from min corner and max corner
  vgl_box_3d(Type const min_position[3],
             Type const max_position[3] );

  //: Construct from min corner and max corner
  vgl_box_3d(vgl_point_3d<Type> const& min_pos,
             vgl_point_3d<Type> const& max_pos);

  //: Construct from ranges in x,y,z (take care with order of inputs)
  vgl_box_3d(Type xmin, Type ymin, Type zmin,
             Type xmax, Type ymax, Type zmax);

  //: Construct width x height x depth box centred at centroid
  vgl_box_3d(const Type centroid[3],
             Type width, Type height, Type depth);

  //: Construct width x height x depth box centred at centroid
  vgl_box_3d(vgl_point_3d<Type> const& centroid,
             Type width, Type height, Type depth);

#if 0
  // default copy constructor:
  vgl_box_3d(vgl_box_3d const& that) {
    this->min_pos_[0] = that.min_pos_[0]; this->max_pos_[0] = that.max_pos_[0];
    this->min_pos_[1] = that.min_pos_[1]; this->max_pos_[1] = that.max_pos_[1];
    this->min_pos_[2] = that.min_pos_[2]; this->max_pos_[2] = that.max_pos_[2];
  }
  // default destructor:
  ~vgl_box_3d() {}
#endif

  // Operators----------------------------------------------------------------
  // Default assignment operator:
#if 0
  vgl_box_3d& operator=(vgl_box_3d const& that){
    this->min_pos_[0] = that.min_pos_[0]; this->max_pos_[0] = that.max_pos_[0];
    this->min_pos_[1] = that.min_pos_[1]; this->max_pos_[1] = that.max_pos_[1];
    this->min_pos_[2] = that.min_pos_[2]; this->max_pos_[2] = that.max_pos_[2];
    return *this;
  }
#endif

  // Data Access---------------------------------------------------------------

  //: Width (x)
  inline Type get_width() const { return max_pos_[0]-min_pos_[0]; }
  //: Height (y)
  inline Type get_height()const { return max_pos_[1]-min_pos_[1]; }
  //: Depth (z)
  inline Type get_depth() const { return max_pos_[2]-min_pos_[2]; }

  //: min x
  inline Type get_min_x() const { return min_pos_[0]; }
  //: min y
  inline Type get_min_y() const { return min_pos_[1]; }
  //: min z
  inline Type get_min_z() const { return min_pos_[2]; }
  //vgl_point_3d<Type> get_min_point() const;

  //: max x
  inline Type get_max_x() const { return max_pos_[0]; }
  //: max y
  inline Type get_max_y() const { return max_pos_[1]; }
  //: max z
  inline Type get_max_z() const { return max_pos_[2]; }

  //vgl_point_3d<Type> get_max_point() const;

  //: Get x component of centroid
  inline Type get_centroid_x() const { return 0.5*(min_pos_[0]+max_pos_[0]); }
  //: Get y component of centroid
  inline Type get_centroid_y() const { return 0.5*(min_pos_[1]+max_pos_[1]); }
  //: Get z component of centroid
  inline Type get_centroid_z() const { return 0.5*(min_pos_[2]+max_pos_[2]); }

#if 0
  vgl_point_3d<Type> get_centroid_point() const;
#endif

  // Data Control--------------------------------------------------------------

  //: Set min x ordinate of box (other sides unchanged)
  inline void set_min_x(Type min_x) {min_pos_[0]=min_x;}
  //: Set min y ordinate of box (other sides unchanged)
  inline void set_min_y(Type min_y) {min_pos_[1]=min_y;}
  //: Set min z ordinate of box (other sides unchanged)
  inline void set_min_z(Type min_z) {min_pos_[2]=min_z;}

  //: Set max x ordinate of box (other sides unchanged)
  inline void set_max_x(Type max_x) {max_pos_[0]=max_x;}
  //: Set max y ordinate of box (other sides unchanged)
  inline void set_max_y(Type max_y) {max_pos_[1]=max_y;}
  //: Set max z ordinate of box (other sides unchanged)
  inline void set_max_z(Type max_z) {max_pos_[2]=max_z;}

  //: Move box so centroid lies at centroid_x (size unchanged)
  void set_centroid_x(Type centroid_x);
  //: Move box so centroid lies at centroid_y (size unchanged)
  void set_centroid_y(Type centroid_y);
  //: Move box so centroid lies at centroid_z (size unchanged)
  void set_centroid_z(Type centroid_z);

  //: Set width (x), centroid unchanged
  void set_width( Type width);
  //: Set height (y), centroid unchanged
  void set_height(Type height);
  //: Set depth (z), centroid unchanged
  void set_depth( Type depth);

  //: Set min corner (max corner unchanged)
  inline void set_min_position(Type const m[3]) { min_pos_[0]=m[0]; min_pos_[1]=m[1]; min_pos_[2]=m[2]; }
  //: Set max corner (min corner unchanged)
  inline void set_max_position(Type const m[3]) { max_pos_[0]=m[0]; max_pos_[1]=m[1]; max_pos_[2]=m[2]; }
  //: Set min corner (max corner unchanged)
  void set_min_point(vgl_point_3d<Type> const& min_point);
  //: Set max corner (min corner unchanged)
  void set_max_point(vgl_point_3d<Type> const& max_point);
  //: Move box so centroid lies at c (size unchanged)
  inline void set_centroid(Type const c[3]) { set_centroid_x(c[0]); set_centroid_y(c[1]); set_centroid_z(c[2]); }
  //: Move box so centroid lies at centroid (size unchanged)
  void set_centroid(vgl_point_3d<Type> const& centroid);

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
template <class Type>
inline vcl_ostream&  operator<<(vcl_ostream& s, vgl_box_3d<Type> const& p) {
  return p.print(s);
}

//: Read box from stream
template <class Type>
inline vcl_istream&  operator>>(vcl_istream& is,  vgl_box_3d<Type>& p) {
  return p.read(is);
}

#endif // vgl_box_3d_h
