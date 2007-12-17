#ifndef bwm_reg_image_h_
#define bwm_reg_image_h_

#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vil/vil_image_resource_sptr.h>
#include <brip/brip_roi.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_digital_curve_3d_sptr.h>
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief Registration process data associated with an image
//
// \verbatim
//   December 13, 2007
// Modifications
//  None
// \endverbatim
//
//-----------------------------------------------------------------------------

class bwm_reg_image : public vbl_ref_count
{
 public:
  bwm_reg_image(){}
  bwm_reg_image(vil_image_resource_sptr const& image,
                vpgl_rational_camera<double> const& rcam,
                vgl_point_3d<double> const& world_point,
                vgl_plane_3d<double> const& world_plane);

  void set_image(vil_image_resource_sptr const& image){image_ = image;}
  void set_camera(vpgl_rational_camera<double> const& rcam){camera_=rcam;}
  void set_world_point(vgl_point_3d<double> const& world_point)
    {world_point_ = world_point;}
  void set_world_plane(vgl_plane_3d<double> const& world_plane)
    {world_plane_ = world_plane;}

  //: The GSD of the image along the diagonal
  double diagonal_gsd();

  //: World point projected into the model image
  vgl_point_2d<double> proj_world_pt();

  //: The region of interest for computing edges
  brip_roi roi(){return roi_;}

  //: Compute edges
  bool compute_edges(float sigma, float noise_thresh,
                     double radius_3d);
  //: Edges on the image
  bool edges_2d(vcl_vector<vsol_digital_curve_2d_sptr>& edges);
  
  //: Edges projected onto the world plane
  bool edges_3d(vcl_vector<vsol_digital_curve_3d_sptr>& edges);

 protected:
  void compute_region_of_interest(float sigma);
  bool edges_valid_;
  bool edges_back_projected_;
  vil_image_resource_sptr image_;
  vpgl_rational_camera<double> camera_;
  vgl_point_3d<double> world_point_;
  bgeo_lvcs lvcs_;
  vgl_plane_3d<double> world_plane_;
  brip_roi roi_;
  double radius_;
  vcl_vector<vsol_digital_curve_2d_sptr> edges_2d_;
  vcl_vector<vsol_digital_curve_3d_sptr> edges_3d_;
};

#endif
