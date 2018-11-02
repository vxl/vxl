// This is mul/vil3d/vil3d_chord.h
#ifndef vil3d_chord_h_
#define vil3d_chord_h_
//:
// \file
// \author Tim Cootes
// \brief Horizontal line used in 3D images, with integer co-ordinates

#include <vector>
#include <iostream>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/vil3d_image_view.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>

//: Horizontal line used in 3D images, with integer co-ordinates
class vil3d_chord
{
private:
  int start_x_;
  int end_x_;
  int y_;
  int z_;
public:
    //: Constructor
  vil3d_chord() : start_x_(0),end_x_(-1),y_(0),z_(0) {}

    //: Constructor
  vil3d_chord(int start_x, int end_x, int y, int z)
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
  bool operator==(const vil3d_chord& c) const;
};

//: Computes number of voxels in region
//  Assumes chords do not overlap
inline unsigned vil3d_volume(const std::vector<vil3d_chord>& chords)
{
  if (chords.size()==0) return 0;
  unsigned n=0;
  std::vector<vil3d_chord>::const_iterator c = chords.begin();
  for (;c!=chords.end();++c) n+=c->length();
  return n;
}


    //: Write to binary stream
inline void vil3d_chord::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,start_x_);
  vsl_b_write(bfs,end_x_);
  vsl_b_write(bfs,y_);
  vsl_b_write(bfs,z_);
}

//: Read from binary stream
inline void vil3d_chord::b_read(vsl_b_istream& bfs)
{
  vsl_b_read(bfs,start_x_);
  vsl_b_read(bfs,end_x_);
  vsl_b_read(bfs,y_);
  vsl_b_read(bfs,z_);
}

inline bool vil3d_chord::operator==(const vil3d_chord& c) const
{
  return start_x_ ==c.start_x_ && end_x_==c.end_x_ && y_==c.y_ && z_==c.z_;
}

//: Print
inline std::ostream& operator<<(std::ostream& os, const vil3d_chord& c)
{
  return os<<"(["<<c.start_x()<<","<<c.end_x()<<"],"<<c.y()<<","<<c.z()<<")";
}

//: Save
inline void vsl_b_write(vsl_b_ostream& bfs, const vil3d_chord& t)
{
  t.b_write(bfs);
}

//: Save
inline void vsl_b_write(vsl_b_ostream& bfs,
                        const std::vector<vil3d_chord>& t)
{
  vsl_b_write(bfs,unsigned(t.size()));
  for (auto i : t) i.b_write(bfs);
}

//: Load
inline void vsl_b_read(vsl_b_istream& bfs, vil3d_chord& t)
{
  t.b_read(bfs);
}

//: Load
inline void vsl_b_read(vsl_b_istream& bfs,
                       std::vector<vil3d_chord>& t)
{
  unsigned n;
  vsl_b_read(bfs,n);
  t.resize(n);
  for (unsigned i=0;i<n;++i) t[i].b_read(bfs);
}

//: Print
inline void vsl_print_summary(std::ostream& os, const vil3d_chord& t)
{
  os<<t;
}

//: Represent a 3D voxel region as a vector of chords.
class vil3d_region : public std::vector<vil3d_chord>
{
public:
  //: Number of voxels in region
  unsigned volume() const
  {
    unsigned v=0;
    for (unsigned i=0;i<size();++i)
      v+=unsigned(operator[](i).length());
    return v;
  }

  //: Bounding box of this region
  vgl_box_3d<int> bounds() const
  {
    vgl_box_3d<int> box;
    for (unsigned i=0;i<size();++i)
    {
      const vil3d_chord& c=operator[](i);
      box.add(vgl_point_3d<int>(c.start_x(),c.y(),c.z()));
      box.add(vgl_point_3d<int>(c.end_x(),c.y(),c.z()));
    }

    return box;
  }

  //: Translate region by given amount
  void translate_by(int x, int y, int z)
  {
    for (unsigned i=0;i<size();++i)
    {
      vil3d_chord& c=operator[](i);
      c=vil3d_chord(c.start_x()+x,c.end_x()+x,c.y()+y,c.z()+z);
    }
  }
};

//: Write value v into pixels defined by chord.
template<class T>
inline void vil3d_fill_chord(vil3d_image_view<T>& image, const vil3d_chord& c, T v)
{
  if (image.size()==0) return;
  if (c.y()<0 || c.y()>=image.nj()) return;
  if (c.z()<0 || c.z()>=image.nk()) return;
  int ilo=c.start_x();  if (ilo<0) ilo=0;
  int ihi=c.end_x(); if (ihi>=image.ni()) ihi=image.ni()-1;
  for (int i=ilo;i<=ihi;++i) image(i,c.y(),c.z())=v;
}

//: Write value v into pixels defined by set of chords in region.
template<class T>
inline void vil3d_fill_region(vil3d_image_view<T>& image, const vil3d_region& r, T v)
{
  for (unsigned i=0;i<r.size();++i)
    vil3d_fill_chord(image,r[i],v);
}



#endif // vil3d_chord_h_
