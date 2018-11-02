#ifndef mmn_arc_h_
#define mmn_arc_h_
//:
// \file
// \brief Representation of topological arc joining two vertices
// \author Tim Cootes


#include <iostream>
#include <vector>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Representation of topological arc joining two vertices
class mmn_arc
{
 public:
  unsigned v1,v2;

  mmn_arc() :v1(999999),v2(999999) {}
  mmn_arc(unsigned i1, unsigned i2) : v1(i1),v2(i2) {}

  //: Return smallest node index
  unsigned min_v() const { return v1<v2?v1:v2; }
  //: Return largest node index
  unsigned max_v() const { return v1<v2?v2:v1; }
};

inline bool operator==(const mmn_arc& t1, const mmn_arc& t2)
{
  if ((t1.v1==t2.v1) && (t1.v2==t2.v2)) return true;
  if ((t1.v1==t2.v2) && (t1.v2==t2.v1)) return true;
  return false;
}

inline bool operator!=(const mmn_arc& t1, const mmn_arc& t2)
{ return !(t1==t2); }

inline std::ostream& operator<<(std::ostream& os, const mmn_arc& t)
{
  return os<<'{'<<t.v1<<','<<t.v2<<'}';
}

inline std::ostream& operator<<(std::ostream& os,
                               const std::vector<mmn_arc>& arcs)
{
  for (auto arc : arcs) os<<arc;
  return os;
}

inline void vsl_b_write(vsl_b_ostream& bfs, const mmn_arc& t)
{
  vsl_b_write(bfs,t.v1);
  vsl_b_write(bfs,t.v2);
}

inline void vsl_b_read(vsl_b_istream& bfs, mmn_arc& t)
{
  vsl_b_read(bfs,t.v1);
  vsl_b_read(bfs,t.v2);
}

inline void vsl_b_write(vsl_b_ostream& bfs, const std::vector<mmn_arc>& a)
{
  short version_no = 1;
  vsl_b_write(bfs,version_no);
  vsl_b_write(bfs,a.size());
  for (auto i : a) vsl_b_write(bfs,i);
}

inline void vsl_b_read(vsl_b_istream& bfs, std::vector<mmn_arc>& a)
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
      std::cerr << "I/O ERROR: vsl_b_read(bfs,vector<arc>)\n"
               << "           Unknown version number "<< version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


#endif // mmn_arc_h_
