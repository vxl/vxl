//:
// \file
// \brief Functions to re-arrange cost data to match index rank
// \author Tim Cootes

#include <iostream>
#include <vector>
#include "mmn_order_cost.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: (i0,i1,i2) gives ordering of v0,v1,v2
//  Each i is {0,1,2}, indicating rank of equivalent v
//  ie if i0=1, then v0 is the middle value
//  if i1=2, then v1 is the largest value
void mmn_get_order(unsigned v0, unsigned v1, unsigned v2,
                   unsigned& i0, unsigned& i1, unsigned& i2)
{
  if (v0<v1)
  {
    if (v1<v2) { i0=0; i1=1; i2=2; return; }  // 0-1-2
    if (v2<v0) { i0=1; i1=2; i2=0; return; }  // 2-0-1
    i0=0; i1=2; i2=1; return;  // 0-2-1
  }
  if (v0<v2) { i0=1; i1=0; i2=2; return; }  // 1-0-2
  if (v2<v1) { i0=2; i1=1; i2=0; return; }  // 2-1-0
  i0=2; i1=0; i2=1; return;  // 1-2-0
}

//: (i0,i1,i2) gives rank of v0,v1,v2
// I.e. v[i0] is smallest, v[i2] is largest
void mmn_get_rank(unsigned v0, unsigned v1, unsigned v2,
                  unsigned& i0, unsigned& i1, unsigned& i2)
{
  if (v0<v1)
  {
    if (v1<v2) { i0=0; i1=1; i2=2; return; }  // 0-1-2
    if (v2<v0) { i0=2; i1=0; i2=1; return; }  // 2-0-1
    i0=0; i1=2; i2=1; return;  // 0-2-1
  }
  if (v0<v2) { i0=1; i1=0; i2=2; return; }  // 1-0-2
  if (v2<v1) { i0=2; i1=1; i2=0; return; }  // 2-1-0
  i0=1; i1=2; i2=0; return;  // 1-2-0
}

//: Create new view of data, with axes re-arranged
//  \a c(i1,i2,i3) is the value associated with i1 on node v1,
//  i2 on node v2 etc.
//  The i-axis of the returned image is associated with the node
//  min(v1,v2,v3), the j axis with mid(v1,v2,v3) and the k with
//  max(v1,v2,v3)
//  The inverse operation is given by mmn_unorder_cost
vil_image_view<double> mmn_order_cost(const vil_image_view<double>& c,
                                      unsigned v1, unsigned v2, unsigned v3)
{
  unsigned i1,i2,i3;
  mmn_get_rank(v1,v2,v3, i1,i2,i3);
  std::vector<unsigned> n(3);
  n[0]=c.ni(); n[1]=c.nj(); n[2]=c.nplanes();
  std::vector<std::ptrdiff_t> step(3);
  step[0]=c.istep(); step[1]=c.jstep(); step[2]=c.planestep();

  return vil_image_view<double>(c.top_left_ptr(),
                                n[i1],n[i2],n[i3],
                                step[i1],step[i2],step[i3]);
}

//: Make new view of data, with axes re-arranged. Inverse of mmn_order_data
//  The i-axis of c is associated with the node min(v1,v2,v3),
//  the j axis with mid(v1,v2,v3) and the k with max(v1,v2,v3)
//  Returns image r, where r(i1,i2,i3) is the value associated
//  with i1 on node v1, i2 on node v2 etc.
vil_image_view<double> mmn_unorder_cost(const vil_image_view<double>& c,
                                        unsigned v1, unsigned v2, unsigned v3)
{
  unsigned i1,i2,i3;
  mmn_get_order(v1,v2,v3, i1,i2,i3);
  std::vector<unsigned> n(3);
  n[0]=c.ni(); n[1]=c.nj(); n[2]=c.nplanes();
  std::vector<std::ptrdiff_t> step(3);
  step[0]=c.istep(); step[1]=c.jstep(); step[2]=c.planestep();

  return vil_image_view<double>(c.top_left_ptr(),
                                n[i1],n[i2],n[i3],
                                step[i1],step[i2],step[i3]);
}
