// This is mul/mbl/mbl_chord.h
#ifndef mbl_chord_h_
#define mbl_chord_h_
//:
// \file
// \author Tim Cootes
// \brief Horizontal line used in images, with integer co-ordinates

#include <iostream>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Horizontal line used in images, with integer co-ordinates

class mbl_chord
{
private:
  int start_x_;
  int end_x_;
  int y_;
public:
    //: Constructor
  mbl_chord() : start_x_(0),end_x_(-1),y_(0) {}

    //: Constructor
  mbl_chord(int start_x, int end_x, int y)
    : start_x_(start_x), end_x_(end_x), y_(y) {}

    //: X-ordinate of start
  int start_x() const { return start_x_; }

    //: X-ordinate of end
  int end_x() const { return end_x_; }

    //: y-ordinate
  int y() const { return y_; }

    //: Length
  int length() const { return 1+end_x_-start_x_; }

    //: Write to binary stream
  inline void b_write(vsl_b_ostream& bfs) const;

    //: Read from binary stream
  inline void b_read(vsl_b_istream& bfs);

    //: Comparison
  bool operator==(const mbl_chord& c) const;
};

    //: Write to binary stream
inline void mbl_chord::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,start_x_);
  vsl_b_write(bfs,end_x_);
  vsl_b_write(bfs,y_);
}

//: Read from binary stream
inline void mbl_chord::b_read(vsl_b_istream& bfs)
{
  vsl_b_read(bfs,start_x_);
  vsl_b_read(bfs,end_x_);
  vsl_b_read(bfs,y_);
}

inline bool mbl_chord::operator==(const mbl_chord& c) const
{
  return start_x_ ==c.start_x_ && end_x_==c.end_x_ && y_==c.y_;
}

//: Print
inline std::ostream& operator<<(std::ostream& os, const mbl_chord& c)
{
  return os<<"(["<<c.start_x()<<","<<c.end_x()<<"],"<<c.y()<<")";
}

//: Save
inline void vsl_b_write(vsl_b_ostream& bfs, const mbl_chord& t)
{
  t.b_write(bfs);
}

//: Load
inline void vsl_b_read(vsl_b_istream& bfs, mbl_chord& t)
{
  t.b_read(bfs);
}

//: Print
inline void vsl_print_summary(std::ostream& os, const mbl_chord& t)
{
  os<<t;
}

#endif // mbl_chord_h_
