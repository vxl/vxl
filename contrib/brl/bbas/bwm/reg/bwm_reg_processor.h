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
// The matcher is the classic approach where a set of step edges is computed
// for a model and then projected via a 3-d plane onto a search image. Only
// edges that are within a tolerance on orientation are considered to be
// eligble for a match. The test for a match is based on a histogram of 
// distances between eligible edges in the model and the search segmentations.
// There is a minimum acceptable cumulative probabiity of edges lying within
// the distance threshold, to produce a successful match.
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_digital_curve_3d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vil/vil_image_resource_sptr.h>
#include <brip/brip_roi.h>
#include <vpgl/vpgl_rational_camera.h>
#include "bwm_reg_image_sptr.h"

class bwm_reg_processor
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:

  // Constructors/Initializers/Destructors-------------------------------------
  // Constructor for a single model image
  bwm_reg_processor(vil_image_resource_sptr const& model_image,
                    vpgl_rational_camera<double> const& model_cam,
                    vgl_point_3d<double> const& world_point,
                    vgl_plane_3d<double> const& world_plane,
                    vil_image_resource_sptr const& search_image,
                    vpgl_rational_camera<double> const& search_cam
                  );
  // Constructor for other mode as well as model
  // The model image (EO) is applied to the other_mode_image
  // The model edges that are successful are kept for 
  // matching against the search image
  bwm_reg_processor(vil_image_resource_sptr const& model_image,
                    vpgl_rational_camera<double> const& model_cam,
                    vil_image_resource_sptr const& other_mode_image,
                    vpgl_rational_camera<double> const& other_mode_cam,
                    vgl_point_3d<double> const& world_point,
                    vgl_plane_3d<double> const& world_plane,
                    vil_image_resource_sptr const& search_image,
                    vpgl_rational_camera<double> const& search_cam
                    );
  ~bwm_reg_processor(){}

  //: Registers the world point in the search image
  // model radius - the 1/2 width of the image processing region in meters
  // proj_error - the expected error in sensor positioning
  // distance_threshold - the maximum alignment error allowed
  // angle_threshold - the maxium orientation error for an edge  match
  // min_probabiliy - the minimum acceptable cumulative probability of 
  //                  edges lying inside the distance threshold
  // model_noise_threshold - the edge detection signal threshold for model
  // search_noise_threshold - the edge detection signal threshold for search
bool match(double radius,
           double proj_error,
           double distance_threshold,
           double angle_threshold,
           unsigned min_curve_length,
           double min_probability,
           float model_noise_threshold,
           float search_noise_threshold,
           int& tcol, int& trow);

  vcl_vector<vsol_digital_curve_2d_sptr> search_curves();
    
  vcl_vector<vsol_digital_curve_2d_sptr> trans_model_curves()
    {return trans_model_edges_;}
   
    // protected:
  // INTERNALS-----------------------------------------------------------------
 protected:
  //: compute the edge detection smoothing scale (sigma) based on image GSD
  bool scale(bwm_reg_image_sptr& image1, bwm_reg_image_sptr& image2,
             float& sigma1, float& sigma2);
  //: Filter irrelevant edges
  bool filter(double model_radius,
              double proj_error,
              double filter_distance,
              double angle_threshold,
              unsigned min_curve_length,
              float model_noise_threshold,
              float other_noise_threshold);

  bool match_edges(int& tcol, int& trow);

  // Data Members--------------------------------------------------------------

 private:
  double distance_threshold_;
  double angle_threshold_;
  unsigned min_curve_length_;
  double min_probability_;
  vpgl_rational_camera<double> model_cam_;
  vpgl_rational_camera<double> search_cam_;
  vpgl_rational_camera<double> other_mode_cam_;
  bwm_reg_image_sptr model_image_;
  bwm_reg_image_sptr other_mode_image_;
  bwm_reg_image_sptr search_image_;
  vgl_point_3d<double> world_point_;
  vgl_plane_3d<double> world_plane_;
  vcl_vector<vsol_digital_curve_2d_sptr> trans_model_edges_;
  vcl_vector<vsol_digital_curve_3d_sptr> filtered_model_edges_;
};

#endif
