#ifndef mmn_dependancy_h_
#define mmn_dependancy_h_

//:
// \file
// \brief Store information about which node a given node depends on.
// \author Tim Cootes

#include <iostream>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Value to indicate no valid arc
constexpr unsigned mmn_no_arc = 99999;

//: Value to indicate no valid triangle
constexpr unsigned mmn_no_tri = 99999;

//: Store information about which node a given node depends on
//  If n_dep==1, then v0 depends only on v1 through arc1
//  If n_dep==2 then v0 depends on v1 and v2, through arc1 and arc2 and tri1
class mmn_dependancy
{
 public:
  unsigned v0,v1,v2;
  unsigned arc1, arc2, arc12;
  unsigned n_dep;
  unsigned tri1;

  //: Default constructor
  mmn_dependancy()
    : v0(0), v1(0),v2(0),
      arc1(0),arc2(0),arc12(0),
      n_dep(0),tri1(mmn_no_tri) {}

  //: Construct with a single dependancy
  mmn_dependancy(unsigned u0, unsigned u1, unsigned a1)
    : v0(u0), v1(u1), v2(9999),
      arc1(a1), arc2(mmn_no_arc),arc12(mmn_no_arc),
      n_dep(1),tri1(mmn_no_tri) {}

  //: Construct with a dual dependancy but no triplet relation
  mmn_dependancy(unsigned u0, unsigned u1, unsigned u2,
                 unsigned a1, unsigned a2, unsigned a12)
    : v0(u0), v1(u1), v2(u2),
      arc1(a1), arc2(a2),arc12(a12),
      n_dep(2), tri1(mmn_no_tri) {}

  //: Construct with a dual dependancy, including triplet
  mmn_dependancy(unsigned u0, unsigned u1, unsigned u2,
                 unsigned a1, unsigned a2, unsigned a12, unsigned t1)
    : v0(u0), v1(u1), v2(u2),
      arc1(a1), arc2(a2),arc12(a12),
      n_dep(2), tri1(t1) {}
};

inline std::ostream& operator<<(std::ostream& os, const mmn_dependancy& t)
{
  os<<'{';
  if (t.n_dep==1) os<<t.v0<<':'<<t.v1<<'}';
  if (t.n_dep==2) os<<t.v0<<":("<<t.v1<<','<<t.v2<<")}";
  return os;
}

inline void vsl_b_write(vsl_b_ostream& bfs, const mmn_dependancy& t)
{
  vsl_b_write(bfs,short(1)); // Version 1
  vsl_b_write(bfs,t.v0);
  vsl_b_write(bfs,t.v1);
  vsl_b_write(bfs,t.v2);
  vsl_b_write(bfs,t.arc1);
  vsl_b_write(bfs,t.arc2);
  vsl_b_write(bfs,t.arc12);
  vsl_b_write(bfs,t.tri1);
  vsl_b_write(bfs,t.n_dep);
}

inline void vsl_b_read(vsl_b_istream& bfs, mmn_dependancy& t)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case 1:
      vsl_b_read(bfs,t.v0);
      vsl_b_read(bfs,t.v1);
      vsl_b_read(bfs,t.v2);
      vsl_b_read(bfs,t.arc1);
      vsl_b_read(bfs,t.arc2);
      vsl_b_read(bfs,t.arc12);
      vsl_b_read(bfs,t.tri1);
      vsl_b_read(bfs,t.n_dep);
      return;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

#endif // mmn_dependancy_h_
