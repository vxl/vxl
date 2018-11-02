#ifndef mmn_triplet_h_
#define mmn_triplet_h_
//:
// \file
// \brief Representation of three vertex indices, eg a triangle
// \author Tim Cootes


#include <iostream>
#include <vector>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Representation of three vertex indices, eg a triangle
//  For comparison, order not important.
class mmn_triplet
{
 public:
  unsigned v1,v2,v3;

  mmn_triplet() :v1(999999),v2(999999),v3(999999) {}
  mmn_triplet(unsigned i1, unsigned i2, unsigned i3)
    : v1(i1),v2(i2),v3(i3) {}

  //: Return smallest node index
  unsigned min_v() const
  {
    if (v1<v2) return v1<v3?v1:v3;
    else       return v2<v3?v2:v3;
  }

  //: Return middle node index
  unsigned mid_v() const
  {
    if (v1<v2)
    {
      if (v2<v3) return v2;          // 1-2-3
      else       return v3<v1?v1:v3; // 3-1-2 or 1-3-2
    }
    if (v1<v3) return v1;          // 2-1-3
    else       return v3<v2?v2:v3; // 3-2-1 or 2-3-1
 }

  //: Return largest node index
  unsigned max_v() const
  {
    if (v1>v2) return v1>v3?v1:v3;
    else       return v2>v3?v2:v3;
  }
};

inline bool operator==(const mmn_triplet& t1, const mmn_triplet& t2)
{
  if (t1.v1==t2.v1)
  {
    if (t1.v2==t2.v2) return t1.v3==t2.v3;  // Order 1-2-3
    if (t1.v2==t2.v3) return t1.v3==t2.v2;  // Order 1-3-2
    return false;
  }
  if (t1.v1==t2.v2)
  {
    if (t1.v2==t2.v1) return t1.v3==t2.v3;  // Order 2-1-3
    if (t1.v2==t2.v3) return t1.v3==t2.v1;  // Order 3-1-2
    return false;
  }
  if (t1.v1==t2.v3)
  {
    if (t1.v2==t2.v1) return t1.v3==t2.v2;  // Order 2-3-1
    if (t1.v2==t2.v2) return t1.v3==t2.v1;  // Order 3-2-1
    return false;
  }
  return false;
}

inline bool operator!=(const mmn_triplet& t1, const mmn_triplet& t2)
{ return !(t1==t2); }

inline std::ostream& operator<<(std::ostream& os, const mmn_triplet& t)
{
  return os<<'{'<<t.v1<<','<<t.v2<<','<<t.v3<<'}';
}

inline void vsl_b_write(vsl_b_ostream& bfs, const mmn_triplet& t)
{
  vsl_b_write(bfs,t.v1);
  vsl_b_write(bfs,t.v2);
  vsl_b_write(bfs,t.v3);
}

inline void vsl_b_read(vsl_b_istream& bfs, mmn_triplet& t)
{
  vsl_b_read(bfs,t.v1);
  vsl_b_read(bfs,t.v2);
  vsl_b_read(bfs,t.v3);
}

inline void vsl_b_write(vsl_b_ostream& bfs, const std::vector<mmn_triplet>& a)
{
  short version_no = 1;
  vsl_b_write(bfs,version_no);
  vsl_b_write(bfs,a.size());
  for (auto i : a) vsl_b_write(bfs,i);
}

inline void vsl_b_read(vsl_b_istream& bfs, std::vector<mmn_triplet>& a)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  unsigned n;
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,n);
      a.resize(n);
      for (unsigned i=0;i<n;++i) vsl_b_read(bfs,a[i]);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(bfs,vector<mmn_triplet>)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


#endif // mmn_triplet_h_
