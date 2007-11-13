#ifndef vil3d_exp_distance_transform_h_
#define vil3d_exp_distance_transform_h_
//:
//  \file
//  \brief Apply non-linear filter to a 3D image.
//  \author Tim Cootes

#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_plane.h>
#include <vil3d/algo/vil3d_make_edt_filter.h>
#include <vil3d/algo/vil3d_max_product_filter.h>

//: Apply nonlinear filter to 3D image
//  Place an exponential decay kernel, with width to half maxima
//  of (width_i,width_j,width_k) in each dimension, at every voxel.
//  Replace each voxel with the maximum of all such kernels passing
//  through.  This is closely related to applying a distance transform
//  to the log of the image.
template<class T>
void vil3d_exp_distance_transform(vil3d_image_view<T>& image,
                           double width_i,
                           double width_j,
                           double width_k,
                           int r)
{
  // Construct filter
  vil3d_structuring_element se;
  vcl_vector<double> f;
  vil3d_make_edt_filter(width_i,width_j,width_k,r,se,f);

  for (unsigned p=0;p<image.nplanes();++p)
  {
    vil3d_image_view<T> image_p = vil3d_plane(image,p);

    // Forward pass
    vil3d_max_product_filter(image_p,se,f);
    unsigned ni = image.ni();
    unsigned nj = image.nj();
    unsigned nk = image.nk();

    vcl_ptrdiff_t istep = image.istep();
    vcl_ptrdiff_t jstep = image.jstep();
    vcl_ptrdiff_t kstep = image.kstep();

    vil3d_image_view<T> flipped_image(&image(ni-1,nj-1,nk-1,p),
                                 ni,nj,nk,1,
                                 -istep,-jstep,-kstep,
                                 image.planestep());
    // Backward pass
    vil3d_max_product_filter(flipped_image,se,f);
  }
}


#endif // vil3d_exp_distance_transform_h_
