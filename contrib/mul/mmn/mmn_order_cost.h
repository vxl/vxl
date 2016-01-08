#ifndef mmn_order_cost_h_
#define mmn_order_cost_h_
//:
// \file
// \brief Functions to re-arrange cost data to match index rank
// \author Tim Cootes

#include <vil/vil_image_view.h>

//: (i0,i1,i2) gives ordering of v0,v1,v2
//  Each i is {0,1,2}, indicating rank of equivalent v
//  ie if i0=1, then v0 is the middle value
//  if i1=2, then v1 is the largest value
void mmn_get_order(unsigned v0, unsigned v1, unsigned v2,
                   unsigned& i0, unsigned& i1, unsigned& i2);

//: (i0,i1,i2) gives rank of v0,v1,v2
//  I.e. v[i0] is smallest, v[i2] is largest
void mmn_get_rank(unsigned v0, unsigned v1, unsigned v2,
                  unsigned& i0, unsigned& i1, unsigned& i2);

//: Create new view of data, with axes re-arranged
//  \a c(i1,i2,i3) is the value associated with i1 on node v1,
//  i2 on node v2 etc.
//  The i-axis of the returned image is associated with the node
//  min(v1,v2,v3), the j axis with mid(v1,v2,v3) and the k with
//  max(v1,v2,v3)
//  The inverse operation is given by mmn_unorder_cost
vil_image_view<double> mmn_order_cost(const vil_image_view<double>& c,
                                      unsigned v1, unsigned v2, unsigned v3);

//: Make new view of data, with axes re-arranged. Inverse of mmn_order_cost
//  The i-axis of c is associated with the node min(v1,v2,v3),
//  the j axis with mid(v1,v2,v3) and the k with max(v1,v2,v3)
//  Returns image r, where r(i1,i2,i3) is the value associated
//  with i1 on node v1, i2 on node v2 etc.
vil_image_view<double> mmn_unorder_cost(const vil_image_view<double>& c,
                                        unsigned v1, unsigned v2, unsigned v3);

#endif // mmn_order_cost_h_
