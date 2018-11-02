#ifndef bil_trace_4con_boundary_h_
#define bil_trace_4con_boundary_h_
//:
// \file
// \brief Function to trace 4-connected boundary around region in bool image
// \author Tim Cootes

#include <iostream>
#include <vector>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Trace 4-connected boundary around region in boolean image
//  Assumes that (i0,j0) is a boundary point.
//  Searches for the boundary pixels and runs around until it gets back to beginning.
//  On exit the boundary points are given by (bi[k],bj[k])
//
// Point to note: If (i0,j0) is in the middle of a 1 pixel wide line connecting
// two regions, then the point will occur twice in the list, once as the first
// point, then once in the middle of the list as we trace through it to get
// to the other side of the blob.
// \sa bil_trace_8con_boundary
// \relatesalso vil_image_view
void bil_trace_4con_boundary(std::vector<int>& bi, std::vector<int>& bj,
                             const vil_image_view<bool>& image,
                             int i0, int j0);


#endif // bil_trace_4con_boundary_h_
