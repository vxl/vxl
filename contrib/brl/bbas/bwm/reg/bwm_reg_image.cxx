#include "bwm_reg_image.h"
#include "bwm_reg_utils.h"
#include <vil/vil_image_resource.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_2d.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <sdet/sdet_detector.h>

bwm_reg_image::bwm_reg_image(vil_image_resource_sptr const& image,
                             vpgl_rational_camera<double> const& rcam,
                             vgl_point_3d<double> const& world_point,
                             vgl_plane_3d<double> const& world_plane) :
  edges_valid_(false), edges_back_projected_(false), image_(image),
  camera_(rcam), world_point_(world_point), world_plane_(world_plane),
  roi_(0), radius_(0)
{
  lvcs_ =  vpgl_lvcs(world_point_.y(),world_point_.x(),world_point_.z(),
                     vpgl_lvcs::wgs84,vpgl_lvcs::DEG,vpgl_lvcs::METERS);
}

// find the gsd in meters/per/pixel along the diagonal of the image
double bwm_reg_image::diagonal_gsd()
{
  //get the image diagonal
  double ni = static_cast<double>(image_->ni()),
    nj = static_cast<double>(image_->nj());
  double image_diag = std::sqrt(ni*ni + nj*nj);
  if(image_diag == 0)
    return -1;
  //get rough diagonal in world coordinates
  vgl_point_2d<double> ul(0,0), lr(ni, nj);
  vgl_point_3d<double> wul, wlr;
  bool success = vpgl_backproject::bproj_plane(camera_, ul,
                                               world_plane_,
                                               world_point_,
                                               wul);
  if(!success)
    return -1;
  success = vpgl_backproject::bproj_plane(camera_, lr,
                                               world_plane_,
                                               world_point_,
                                               wlr);
  if(!success)
    return -1;
  // convert upper left position to meters
    double xul, yul, zul;
    lvcs_.global_to_local(wul.x(), wul.y(), wul.z(),
                       vpgl_lvcs::wgs84,
                       xul,yul,zul,
                       vpgl_lvcs::DEG,vpgl_lvcs::METERS);
  // convert lower right position to meters
  double xlr, ylr, zlr;
  lvcs_.global_to_local(wlr.x(), wlr.y(), wlr.z(),
                       vpgl_lvcs::wgs84,
                       xlr,ylr,zlr,
                       vpgl_lvcs::DEG,vpgl_lvcs::METERS);

  double world_diag = std::sqrt((xlr-xul)*(xlr-xul)+(ylr-yul)*(ylr-yul));
  //shouldn't happen
  if(world_diag==0)
    return -1;
  double diag_gsd = world_diag/image_diag;
  return diag_gsd;
}

void bwm_reg_image::compute_region_of_interest(float sigma)
{
    // project world point into the image as the center of the region of interest
  vgl_point_2d<double> center_point =  camera_.project(world_point_);

  // convert to local coordinates since projection error is in meters
  double lx, ly, lz;
  lvcs_.global_to_local(world_point_.x(),world_point_.y(),
                        world_point_.z(),vpgl_lvcs::wgs84,
                        lx,ly,lz,
                        vpgl_lvcs::DEG,vpgl_lvcs::METERS);

  lx+=radius_; //deviation is roughly isotropic so x is as good as any

  // convert back to geographic coordinates
  double lon, lat, elev;
  lvcs_.local_to_global(lx, ly, lz,vpgl_lvcs::wgs84, lon, lat ,elev,
                        vpgl_lvcs::DEG,vpgl_lvcs::METERS);

  vgl_point_3d<double> radius_point_3d(lon, lat, elev);

  // get the deviated point in image space
  vgl_point_2d<double> radius_image_point =
    camera_.project(radius_point_3d);

  // the error in image coordinates
  double error_radius =
    vgl_distance<double>(radius_image_point, center_point);

  // convert to an image roi
  unsigned ni = image_->ni(), nj = image_->nj();
  roi_ = brip_roi(ni, nj);
  double sigma_3 = 3.0*sigma;//account for smoothing kernel
  double xmin = center_point.x() -  error_radius - sigma_3;
  double ymin = center_point.y() -  error_radius - sigma_3;
  double xmax = center_point.x() +  error_radius + sigma_3;
  double ymax = center_point.y() +  error_radius + sigma_3;
  // clip if outside image bounds
  if(xmin<0) xmin = 0;
  if(ymin<0) ymin = 0;
  if(xmax>ni) xmax = ni;
  if(ymax>nj) ymax = nj;
  unsigned ixmin = static_cast<unsigned>(xmin);
  unsigned iymin = static_cast<unsigned>(ymin);
  unsigned ixsize = static_cast<unsigned>(xmax - xmin +1);
  unsigned iysize = static_cast<unsigned>(ymax - ymin +1);
  roi_.add_region(ixmin, iymin, ixsize, iysize);
}

bool bwm_reg_image::compute_edges(float sigma, float noise_thresh,
                                  double radius_3d)
{
  edges_valid_ = false;
  edges_back_projected_ = false;
  if(!image_||!image_->ni()||!image_->nj()){
    std::cerr << "In bwm_reg_image::compute_edges() -"
             << " no image data\n";
    return false;
    }
  radius_ = radius_3d;
  this->compute_region_of_interest(sigma);
  sdet_detector_params dp;
  dp.smooth = sigma;
  dp.noise_multiplier=noise_thresh;
  dp.aggressive_junction_closure=0;
  dp.borderp = false;
  sdet_detector det(dp);
  det.SetImage(image_, roi_);
  det.DoContour();
  edges_2d_.clear();
  if(!det.get_vsol_edges(edges_2d_))
    {
    std::cerr << "In bwm_reg_image::compute_edges() -"
             << " edge detection failed\n";
    return false;
    }
  edges_valid_ = true;
  return true;
}

bool bwm_reg_image::edges_2d(std::vector<vsol_digital_curve_2d_sptr>& edges)
{
  if(!edges_valid_)
    {
      edges.clear();
      return false;
    }
  edges = edges_2d_;
  return true;
}

bool bwm_reg_image::edges_3d(std::vector<vsol_digital_curve_3d_sptr>& edges)
{
  if(!edges_valid_)
    {
    std::cerr << "In bwm_reg_image::compute_edges() -"
             << " no 2-d edges to back-project\n";

      edges.clear();
      return false;
    }

if(!edges_back_projected_)
  edges_back_projected_ = bwm_reg_utils::back_project_edges(edges_2d_,
                                                            camera_,
                                                            world_plane_,
                                                            world_point_,
                                                            edges_3d_);
 if(!edges_back_projected_)
   {
    std::cerr << "In bwm_reg_image::compute_edges() -"
             << " back-projection of edges failed\n";
     edges.clear();
     return false;
   }
 edges = edges_3d_;
 return true;
}
