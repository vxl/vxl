// This is vpgl/ihog/ihog_super_res.h
#ifndef ihog_super_res_h_
#define ihog_super_res_h_
//:
// \file
// \brief A class to compute super resolved images
// \author Matt Leotta
// \date 5/02/04
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_string.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_box_2d.h>
#include "ihog_image.h"
#include "ihog_region_sptr.h"


//: A class to compute super resolved images
class ihog_super_res 
{
public:
  //: Constructor
  ihog_super_res(vcl_vector<ihog_image<vxl_byte> >& images, int block_size=10);

  //: Destructor
  ~ihog_super_res(){}

  //: Compute the higher resolution image
  vil_image_view<double> compute_high_res( ihog_region_sptr region,
                                           vcl_string path, double blur=0.25);

  //: Compute the largest region to estimate
  ihog_region_sptr compute_region(double area_mag=4.0);

protected:

  //: Compute the weights and data vector for all images
  void compute_weights( vnl_matrix<double>& data, 
                        vnl_matrix<double>& weights,
                        const vgl_box_2d<int>& bounds ) const;

  //: Compute the weights and data vector for the given index
  void compute_weights( int index, 
                        vcl_list<vnl_vector<double> >& data, 
                        vcl_list<vnl_vector<double> >& weights,
                        const vgl_box_2d<int>& bounds ) const;

  //; Sample from the estimated point spread function 
  double point_spread( const vgl_point_2d<double>& m, 
                       const vgl_point_2d<double>& p ) const;

  //: The region (determines the size of the resolved image)
  ihog_region_sptr region_;

  //: The input images
  vcl_vector<ihog_image<vxl_byte> >& images_;

  //: block size
  int bsi_;
  int bsj_;

  double blur_;
};

#endif // ihog_super_res_h_

