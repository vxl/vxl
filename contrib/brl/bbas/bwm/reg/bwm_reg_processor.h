#ifndef bwm_reg_processor_h_
#define bwm_reg_processor_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief Finds the corresponding match between two images
//
// \verbatim
//   November 27, 2007
// Modifications
//  None
// \endverbatim
//
//-----------------------------------------------------------------------------
// current method does an exhaustive search. The scale of edge detection
// is adjusted to match the GSD of model and search images.

#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vil/vil_image_resource_sptr.h>
#include <brip/brip_roi.h>

class bwm_reg_processor
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:

  // Constructors/Initializers/Destructors-------------------------------------

bwm_reg_processor(vil_image_resource_sptr const& model_image,
                  vpgl_rational_camera<double> const& model_cam,
                  vgl_point_3d<double> const& world_point,
                  vgl_plane_3d<double> const& world_plane,
                  vil_image_resource_sptr const& search_image,
                  vpgl_rational_camera<double> const& search_cam
                  );
  ~bwm_reg_processor(){}

  //: finds the point corresponding to the model point in the search image
  bool match(vgl_point_2d<double> const& model_point,
             double model_radius,
             double proj_error,
             double distance_threshold,
             int& tcol, int& trow);

    // protected:
  // INTERNALS-----------------------------------------------------------------
 protected:
  bool extract_model_region(vgl_point_2d<double> const& model_point,
                            double model_radius);
  double diagonal_gsd(vil_image_resource_sptr const& image,
                      vpgl_rational_camera<double> const& cam);
  double scale();
  bool extract_model_edges();
  bool transfer_model_edges();
  bool extract_search_region(vgl_point_2d<double> const& model_point,
                             double proj_error);
  bool extract_search_edges();
  bool match_edges(int& tcol, int& trow);
  vcl_vector<vsol_digital_curve_2d_sptr>& search_edges(){return search_edges_;}
  vcl_vector<vsol_digital_curve_2d_sptr>& model_edges(){return model_edges_;}
  // Data Members--------------------------------------------------------------

 private:
  double distance_threshold_;
  vil_image_resource_sptr model_image_;
  vpgl_rational_camera<double> model_cam_;
  vil_image_resource_sptr search_image_;
  vpgl_rational_camera<double> search_cam_;
  vgl_point_3d<double> world_point_;
  vgl_plane_3d<double> world_plane_;
  bgeo_lvcs lvcs_;
  brip_roi model_roi_;
  brip_roi search_roi_;
  vcl_vector<vsol_digital_curve_2d_sptr> model_edges_;
  vcl_vector<vsol_digital_curve_2d_sptr> trans_model_edges_;
  unsigned trans_model_cols_;
  unsigned trans_model_rows_;
  vcl_vector<vsol_digital_curve_2d_sptr> search_edges_;
};

#endif
