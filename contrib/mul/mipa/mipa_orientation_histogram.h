#ifndef mipa_orientation_histogram_h_
#define mipa_orientation_histogram_h_
//:
// \file
// \brief Functions to compute histogram of orientations (HOGs)
// \author Tim Cootes

#include <vil/vil_image_view.h>

//: Generate an image containing histograms of oriented gradients (HOG)
//  At each pixel in src, compute angle and quantise into n_angles.
//  If full360, then angle range is 0-360, else it is 0-180.
//  hog_image is set to have n_angles planes.
//  hog_image(i,j,k) gives the weighted sum of pixels with angle k
//  in cell (i,j), corresponding to the i,j-th cell_size square block.
//
//  The corner of cell(0,0) is at src(1,1), to ignore border pixels.
//
//  Number of cells (size of hog_image) chosen so every cell entirely
//  within src.  Thus hog_image.ni()=(src.ni()-2)/cell_size.
//  If  bilin_interp=true then histogram contributions are added to neighbouring bins
//  with weights inversely related to distance to centre of bin
template<class srcT, class sumT>
void mipa_orientation_histogram(const vil_image_view<srcT>& src,
                                vil_image_view<sumT>& hog_image,
                                unsigned n_angles,
                                unsigned cell_size,
                                bool full360=true,
                                bool bilin_interp=true);

#endif // mipa_orientation_histogram_h_
