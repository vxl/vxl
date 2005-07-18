// This is mul/mbl/mbl_chord_3d.h
#ifndef mbl_chord_3d_h_
#define mbl_chord_3d_h_
//:
// \file
// \author Tim Cootes
// \brief Horizontal line used in 3D images, with integer co-ordinates

#include <vsl/vsl_binary_io.h>

//: Horizontal line used in 3D images, with integer co-ordinates
class mbl_chord_3d
{
private:
  int start_x_;
  int end_x_;
  int y_;
  int z_;
public:
    //: Constructor
  mbl_chord_3d() : start_x_(0),end_x_(-1),y_(0),z_(0) {}

    //: Constructor
  mbl_chord_3d(int start_x, int end_x, int y, int z)
    : start_x_(start_x), end_x_(end_x), y_(y), z_(z) {}

    //: X-ordinate of start
  int start_x() const { return start_x_; }

    //: X-ordinate of end
  int end_x() const { return end_x_; }

    //: y-ordinate
  int y() const { return y_; }

    //: z-ordinate
  int z() const { return z_; }

    //: Length
  int length() const { return 1+end_x_-start_x_; }

    //: Write to binary stream
  inline void b_write(vsl_b_ostream& bfs) const;

    //: Read from binary stream
  inline void b_read(vsl_b_istream& bfs);

    //: Comparison
  bool operator==(const mbl_chord_3d& c) const;
};

    //: Write to binary stream
inline void mbl_chord_3d::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,start_x_);
  vsl_b_write(bfs,end_x_);
  vsl_b_write(bfs,y_);
  vsl_b_write(bfs,z_);
}

//: Read from binary stream
inline void mbl_chord_3d::b_read(vsl_b_istream& bfs)
{
  vsl_b_read(bfs,start_x_);
  vsl_b_read(bfs,end_x_);
  vsl_b_read(bfs,y_);
  vsl_b_read(bfs,z_);
}

inline bool mbl_chord_3d::operator==(const mbl_chord_3d& c) const
{
  return start_x_ ==c.start_x_ && end_x_==c.end_x_ && y_==c.y_ && z_==c.z_;
}

//: Print
inline vcl_ostream& operator<<(vcl_ostream& os, const mbl_chord_3d& c)
{
  return os<<"(["<<c.start_x()<<","<<c.end_x()<<"],"<<c.y()<<","<<c.z()<<")";
}

//: Save
inline void vsl_b_write(vsl_b_ostream& bfs, const mbl_chord_3d& t)
{
  t.b_write(bfs);
}

//: Load
inline void vsl_b_read(vsl_b_istream& bfs, mbl_chord_3d& t)
{
  t.b_read(bfs);
}

//: Print
inline void vsl_print_summary(vcl_ostream& os, const mbl_chord_3d& t)
{
  os<<t;
}

#endif // mbl_chord_3d_h_
