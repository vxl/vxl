#ifndef vil3d_make_distance_filter_h_
#define vil3d_make_distance_filter_h_
//:
//  \file
//  \brief Compute filter for a distance transform.
//  \author Tim Cootes

#include <vil3d/algo/vil3d_structuring_element.h>

//: Create filter such that d[a] is distance from origin of se[a]
//  Pixel widths are (width_i,width_j,width_k).  Elements are 
//  selected so that only voxels visited before in a raster scan 
//  of the image are included.  Distance transforms require two
//  passes, a forward and reverse pass.
//
//  Consider only voxels within 1 voxel (1 norm) of origin.
//  Indexes (27-1)/2 = 13 voxels.
void vil3d_make_distance_filter_r1(
                           double width_i, 
                           double width_j, 
                           double width_k,
                           vil3d_structuring_element& se,
                           vcl_vector<double>& d);

//: Create filter such that d[a] is distance from origin of se[a]
//  Pixel widths are (width_i,width_j,width_k).  Elements are 
//  selected so that only voxels visited before in a raster scan 
//  of the image are included.  Distance transforms require two
//  passes, a forward and reverse pass.
//
//  Consider only voxels within r voxels (1 norm) of origin.
void vil3d_make_distance_filter(
                           double width_i, 
                           double width_j, 
                           double width_k,
                           int r,
                           vil3d_structuring_element& se,
                           vcl_vector<double>& d);

#endif // vil3d_make_distance_filter_h_
