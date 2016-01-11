#ifndef vil3d_fill_boundary_h_
#define vil3d_fill_boundary_h_
//:
// \file
// \brief Fill in contour bounded regions in slices of 3D image
// \author Kola Babalola

//: Compute a mask where the regions in each slice of a 3D image bounded by contours are set to "on"
#include <vil3d/vil3d_image_view.h>

//: Compute a mask where the regions in each slice of a 3D image bounded by contours are set to "on".
void vil3d_fill_boundary(vil3d_image_view<bool>& image);

//: Follow the current boundary in the current slice.
//  labelling boundary pixels and background pixels
//  that border the boundary.
void label_boundary_and_bkg(vil3d_image_view<int> &image,
                            int i,int j, int k,
                            int boundary_label,
                            int background_label);

//:  Fill interior of current boundary.
void fill_boundary(vil3d_image_view<int> &image, int j, int k, int boundary_label, int background_label);

//:  Reset background pixels to 0
void reset_background(vil3d_image_view<int> &image, int background_label);


#endif
