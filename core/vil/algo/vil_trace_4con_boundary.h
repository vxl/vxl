#ifndef vil_trace_4con_boundary_h_
#define vil_trace_4con_boundary_h_

//:
// \file
// \brief Function to trace 4-connected boundary around region in bool image
// \author Tim Cootes

#include <vil/vil_image_view.h>
#include <vcl_vector.h>

//: Trace 4-connected boundary around region in boolean image
//  Assumes that (i0,j0) is a boundary point.
//  Searches for the boundary pixels and runs around until it gets back to beginning.
//  On exit the boundary points are given by (bi[k],bj[k])
//
// Known problems: If (i0,j0) is in the middle of a 1 pixel wide line connecting
// two regions, then only one of the regions will be explored, and the algorithm
// will terminate when it gets back to the `neck'.  This can be avoided by
// ensuring (i0,j0) is at an extremal part of the object before calling this
// function.  When used in the standard raster scan of the image looking for
// regions, this will not arise.
void vil_trace_4con_boundary(vcl_vector<int>& bi, vcl_vector<int>& bj,
                             const vil_image_view<bool>& image,
                             int i0, int j0);



#endif // vil_trace_4con_boundary_h_
