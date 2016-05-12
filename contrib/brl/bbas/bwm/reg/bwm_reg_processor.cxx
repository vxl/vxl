// This is brl/bbas/bwm/reg/bwm_reg_processor.cxx
#include "bwm_reg_processor.h"
//
#include <vnl/vnl_numeric_traits.h>
#include <vil/vil_image_resource.h>
#include <vsol/vsol_point_2d.h>
#include <sdet/sdet_detector.h>
#include <brip/brip_roi.h>
#include <vpgl/algo/vpgl_backproject.h>
#include "bwm_reg_matcher.h"
#include "bwm_reg_image.h"
#include "bwm_reg_utils.h"

bwm_reg_processor::
bwm_reg_processor(vil_image_resource_sptr const& model_image,
                  vpgl_rational_camera<double> const& model_cam,
                  vgl_point_3d<double> const& world_point,
                  vgl_plane_3d<double> const& world_plane,
                  vil_image_resource_sptr const& search_image,
                  vpgl_rational_camera<double> const& search_cam
                 ) : model_cam_(model_cam), search_cam_(search_cam),
                     world_point_(world_point), world_plane_(world_plane)
{
  distance_threshold_ = vnl_numeric_traits<double>::maxval;
  angle_threshold_ = vnl_numeric_traits<double>::maxval;
  min_curve_length_ = vnl_numeric_traits<unsigned>::maxval;
  min_probability_ = 1.0;
  model_image_ = new bwm_reg_image(model_image, model_cam,
                                   world_point, world_plane);
  search_image_ = new bwm_reg_image(search_image, search_cam,
                                    world_point, world_plane);
}

bwm_reg_processor::
bwm_reg_processor(vil_image_resource_sptr const& model_image,
                  vpgl_rational_camera<double> const& model_cam,
                  vil_image_resource_sptr const& other_mode_image,
                  vpgl_rational_camera<double> const& other_mode_cam,
                  vgl_point_3d<double> const& world_point,
                  vgl_plane_3d<double> const& world_plane,
                  vil_image_resource_sptr const& search_image,
                  vpgl_rational_camera<double> const& search_cam
                 ) : model_cam_(model_cam), search_cam_(search_cam),
                     other_mode_cam_(other_mode_cam),
                     world_point_(world_point), world_plane_(world_plane)
{
  distance_threshold_ = vnl_numeric_traits<double>::maxval;
  model_image_ = new bwm_reg_image(model_image, model_cam,
                                   world_point, world_plane);

  other_mode_image_ = new bwm_reg_image(other_mode_image,
                                        other_mode_cam,
                                        world_point, world_plane);

  search_image_ = new bwm_reg_image(search_image, search_cam,
                                    world_point, world_plane);
}
// compute the appropriate scale for detecting edges
// based on the GSD of image1 vs. image2
bool bwm_reg_processor::scale(bwm_reg_image_sptr& image1,
                              bwm_reg_image_sptr& image2,
                              float& sigma1, float& sigma2)
{
  float base_sigma = 1.5f;
  //get the gsd of image1
  double gsd1 = image1->diagonal_gsd();
  if (gsd1<0) return false;

  //get the gsd of image1
  double gsd2 = image2->diagonal_gsd();
  if (gsd2<0) return false;

  float r = static_cast<float>(gsd2/gsd1);

  sigma1 = base_sigma*r;
  sigma2 = base_sigma;
  if (r<1.0){
    sigma1 = base_sigma;
    sigma2 = base_sigma/r;
  }
  return true;
}

// Using an image with the same mode as the search image
// match the edges of the model and remove any that are
// not aligned with the other mode edges
bool bwm_reg_processor::filter(double model_radius,
                               double proj_error,
                               double filter_distance,
                               double angle_threshold,
                               unsigned min_curve_length,
                               float model_noise_threshold,
                               float other_noise_threshold)
{
  float model_sigma = 1.0f, other_mode_sigma = 1.0f;
  if (!scale(model_image_, other_mode_image_, model_sigma, other_mode_sigma)){
    std::cerr << "In bwm_reg_processor::filter - scale computation failed\n";
    return false;
  }

  bool success =  model_image_->compute_edges(model_sigma,
                                              model_noise_threshold,
                                              model_radius);

  success = success &&
    other_mode_image_->compute_edges(other_mode_sigma, other_noise_threshold,
                                     model_radius+ proj_error);

  if (!success){
    std::cerr << "In bwm_reg_processor::filter - edge detection failed\n";
    return false;
  }

  std::vector<vsol_digital_curve_3d_sptr> model_edges_3d;
  if (!model_image_->edges_3d(model_edges_3d)){
    std::cerr << "In bwm_reg_processor::filter - no model edges\n";
    return false;
  }

  bwm_reg_utils::project_edges(model_edges_3d, other_mode_cam_,
                               trans_model_edges_);

  std::vector<vsol_digital_curve_2d_sptr> other_edges;
  if (!other_mode_image_->edges_2d(other_edges)){
    std::cerr << "In bwm_reg_processor::filter - no other mode edges\n";
    return false;
  }

  brip_roi roi = other_mode_image_->roi();
  bwm_reg_matcher rm(trans_model_edges_,
                     roi.cmin(0), roi.rmin(0),
                     roi.csize(0), roi.rsize(0),
                     other_edges);
  std::vector<vsol_digital_curve_2d_sptr> close_edges;
  if (!rm.close_edges(filter_distance, angle_threshold, min_curve_length,
                     close_edges)){
    std::cerr << "In bwm_reg_processor::filter - no close edges\n";
    return false;
  }

  bwm_reg_utils::back_project_edges(close_edges, other_mode_cam_, world_plane_,
                                    world_point_, filtered_model_edges_);
  return true;
}
// carry out geometric matching using a champher
bool bwm_reg_processor::match_edges(int& tcol, int& trow)
{
  if (!filtered_model_edges_.size())
    if (!model_image_->edges_3d(filtered_model_edges_)){
      std::cerr << "In bwm_reg_processor::match_edges - no model edges\n";
      return false;
    }

  trans_model_edges_.clear();
  bwm_reg_utils::project_edges(filtered_model_edges_, search_cam_,
                               trans_model_edges_);

  std::vector<vsol_digital_curve_2d_sptr> search_edges;
  if (!search_image_->edges_2d(search_edges)){
    std::cerr << "In bwm_reg_processor::match_edges - no search edges\n";
    return false;
  }

  brip_roi roi = search_image_->roi();
  bwm_reg_matcher rm(trans_model_edges_,
                     roi.cmin(0), roi.rmin(0),
                     roi.csize(0), roi.rsize(0),
                     search_edges);
  bool success = rm.match(tcol, trow, distance_threshold_,
                          angle_threshold_,
                          min_probability_);
  return success;
}

bool bwm_reg_processor::match(double radius,
                              double proj_error,
                              double distance_threshold,
                              double angle_threshold,
                              unsigned min_curve_length,
                              double min_probability,
                              float model_noise_threshold,
                              float search_noise_threshold,
                              int& tcol, int& trow)
{
  distance_threshold_ = distance_threshold;
  min_probability_ = min_probability;
  angle_threshold_ = angle_threshold;
  min_curve_length_ = min_curve_length;
  if (other_mode_image_)
    if (!this->filter(radius, proj_error, distance_threshold_,
                     angle_threshold_,
                     min_curve_length_,
                     model_noise_threshold,
                     search_noise_threshold))
      return false;

  float model_sigma = 1.0f, search_sigma = 1.0f;
  if (!scale(model_image_, search_image_, model_sigma, search_sigma)){
    std::cerr << "In bwm_reg_processor::match - scale computation failed\n";
    return false;
  }

  bool success = true;
  if (!filtered_model_edges_.size()){
    success = success &&
      model_image_->compute_edges(model_sigma,
                                  model_noise_threshold,
                                  radius);
  }

  success = success &&
    search_image_->compute_edges(search_sigma, search_noise_threshold,
                                 radius+ proj_error);

  if (!success) {
    std::cerr << "In bwm_reg_processor::match - edge detection failed\n";
    return false;
  }

  return this->match_edges(tcol, trow);
}

std::vector<vsol_digital_curve_2d_sptr> bwm_reg_processor::search_curves()
{
  std::vector<vsol_digital_curve_2d_sptr> curves;
  if (!search_image_->edges_2d(curves))
    std::cerr << "In bwm_reg_processor::search_curves() - no curves\n";
  return curves;
}
