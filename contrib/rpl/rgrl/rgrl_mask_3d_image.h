#ifndef rgrl_mask_3d_image_h_
#define rgrl_mask_3d_image_h_

//:
// \file
// \brief  mask of 3D image
//         Disregarding the shape, each mask also provides 
//         a bounding box.  
// 
// \author Charlene Tsai
// \date   Oct 2003
// \verbatim
// Modifications
// Oct. 2006  Gehua Yang (RPI) - move rgrl_mask_3d_image into separate file
// \endverbatim

#include <vil3d/vil3d_image_view.h>
#include <rgrl/rgrl_mask.h>

class rgrl_mask_3d_image
  : public rgrl_mask
{
 public:
  rgrl_mask_3d_image( const vil3d_image_view< vxl_byte > & in_mask,
                      int org_x = 0, int org_y = 0, int org_z = 0 );

  //: True if a point is inside the region
  bool inside( vnl_vector< double > const& pt ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_mask_3d_image, rgrl_mask );

 private:
  void update_bounding_box();

 private:
  vil3d_image_view< vxl_byte > mask_image_;
  int org_x_, org_y_, org_z_;
};

#endif
