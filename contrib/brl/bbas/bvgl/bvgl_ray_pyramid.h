#ifndef bvgl_ray_pyramid_h_
#define bvgl_ray_pyramid_h_
//:
// \file
// \brief Helper classes to annotate polygonal regions in images
// \author Ozge C. Ozcanli ozge@lems.brown.edu
// \date   September 26, 2008
//
// \verbatim
//  Modifications
//   <None yet>
// \endverbatim
#include <vgl/vgl_ray_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>



class bvgl_ray_pyramid //: public vbl_ref_count
{
 public:
  bvgl_ray_pyramid(vbl_array_2d<vgl_ray_3d<float> >& finest_level, unsigned numLevels); 
  ~bvgl_ray_pyramid() {}

  // Data Access---------------------------------------------------------------
  vgl_ray_3d<float> const& operator() (vcl_size_t scale, vcl_size_t i, vcl_size_t j) const { return pyramid_[scale][i][j]; }
  vgl_ray_3d<float>      & operator() (vcl_size_t scale, vcl_size_t i, vcl_size_t j)       { return pyramid_[scale][i][j]; }

  // Ray image access----------------------------------------------------------
  vbl_array_2d<vgl_ray_3d<float> > const& operator() (vcl_size_t scale) const { return pyramid_[scale]; }
  vbl_array_2d<vgl_ray_3d<float> > &      operator() (vcl_size_t scale)       { return pyramid_[scale]; }

 private:
  vcl_vector<vbl_array_2d<vgl_ray_3d<float> > > pyramid_; 
  vbl_array_2d<vgl_ray_3d<float> > scale_down(vbl_array_2d<vgl_ray_3d<float> >& toScale); 
  
  vcl_vector<vbl_array_2d<float> > angles_; 
  
};

#endif // bvgl_ray_pyramid_h_
