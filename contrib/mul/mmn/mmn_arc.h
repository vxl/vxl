#ifndef mmn_arc_h_
#define mmn_arc_h_
//:
// \file
// \brief Representation of topological arc joining two vertices
// \author Tim Cootes


#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>

//: Representation of topological arc joining two vertices
class mmn_arc
{
public:
  unsigned v1,v2;

  mmn_arc() :v1(999999),v2(999999) {};
  mmn_arc(unsigned i1, unsigned i2) : v1(i1),v2(i2) {};

  //: Return smallest node index
  unsigned min_v() const { return (v1<v2?v1:v2); }
  //: Return largest node index
  unsigned max_v() const { return (v1<v2?v2:v1); }
};

inline bool operator==(const mmn_arc& t1, const mmn_arc& t2)
{
  if ((t1.v1==t2.v1) && (t1.v2==t2.v2)) return true;
  if ((t1.v1==t2.v2) && (t1.v2==t2.v1)) return true;
  return false;
}

inline bool operator!=(const mmn_arc& t1, const mmn_arc& t2)
{ return !(t1==t2); }

inline vcl_ostream& operator<<(vcl_ostream& os, const mmn_arc& t)
{
  return os<<"{"<<t.v1<<","<<t.v2<<"}";
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

#endif // mmn_arc_h_

