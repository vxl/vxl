#ifndef mmn_dependancy_h_
#define mmn_dependancy_h_

//:
// \file
// \brief Store information about which node a given node depends on.
// \author Tim Cootes

#include <vsl/vsl_binary_io.h>

//: Store information about which node a given node depends on
//  If n_dep==1, then v0 depends only on v1 through arc1
//  If n_dep==2 then v0 depends on v1 and v2, through arc1 and arc2
class mmn_dependancy
{
public:
  unsigned v0,v1,v2;
  unsigned arc1, arc2, arc12;
  unsigned n_dep;

  //: Default constructor
  mmn_dependancy() :v0(0), v1(0),v2(0),arc1(0),arc2(0),arc12(0),n_dep(0) {};

  //: Construct with a single dependancy
  mmn_dependancy(unsigned u0, unsigned u1, unsigned a1) 
    : v0(u0), v1(u1), v2(9999), arc1(a1), arc2(9999), n_dep(1) {};

  //: Construct with a dual dependancy
  mmn_dependancy(unsigned u0, unsigned u1, unsigned u2,
                 unsigned a1, unsigned a2, unsigned a12) 
    : v0(u0), v1(u1), v2(u2), arc1(a1), arc2(a2),arc12(a12), n_dep(2) {};

};

inline vcl_ostream& operator<<(vcl_ostream& os, const mmn_dependancy& t)
{
  os<<"{";
  if (t.n_dep==1) os<<t.v0<<":"<<t.v1<<"}";
  if (t.n_dep==2) os<<t.v0<<":("<<t.v1<<","<<t.v2<<")}";
  return os;
}

inline void vsl_b_write(vsl_b_ostream& bfs, const mmn_dependancy& t)
{
  vsl_b_write(bfs,t.v0);
  vsl_b_write(bfs,t.v1);
  vsl_b_write(bfs,t.v2);
  vsl_b_write(bfs,t.arc1);
  vsl_b_write(bfs,t.arc2);
  vsl_b_write(bfs,t.arc12);
  vsl_b_write(bfs,t.n_dep);
}

inline void vsl_b_read(vsl_b_istream& bfs, mmn_dependancy& t)
{
  vsl_b_read(bfs,t.v0);
  vsl_b_read(bfs,t.v1);
  vsl_b_read(bfs,t.v2);
  vsl_b_read(bfs,t.arc1);
  vsl_b_read(bfs,t.arc2);
  vsl_b_read(bfs,t.arc12);
  vsl_b_read(bfs,t.n_dep);
}

#endif // mmn_dependancy_h_

