// This is core/vpgl/algo/vpgl_backproject_dem.cxx
#include <limits>
#include <algorithm>
#include "vpgl_backproject.h"
#include "vpgl_backproject_dem.h"
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vpgl/algo/vpgl_ray.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_math.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/algo/vnl_brent_minimizer.h>
class dem_bproj_cost_function : public vnl_cost_function{
public:
  dem_bproj_cost_function(vil_image_view<float> const& dem_view, vpgl_geo_camera* geo_cam, vgl_ray_3d<double> const& ray, bool verbose=false) :
    vnl_cost_function(1), ray_(ray), geo_cam_(geo_cam), dview_(&dem_view), verbose_(verbose){}
  //: x is the parameter that runs along the ray, with x=0 at the ray origin
  double f(vnl_vector<double> const& x) override{
    //get the lon and lat values for a given parameter value
    vgl_point_3d<double> org = ray_.origin(), rayp;
    vgl_vector_3d<double> dir = ray_.direction();
    rayp = org + x[0]*dir;
    double lon = rayp.x(), lat = rayp.y(), elev = rayp.z();
    double ud, vd;
    //determine the dem pixel coordinates and check bounds
    geo_cam_->project(lon, lat, elev, ud, vd);
    int u = static_cast<int>(ud+0.5), v = static_cast<int>(vd+0.5);
    int ni = dview_->ni(), nj = dview_->nj();
    if(u<0||u>=ni||v<0||v>=nj){
      if(verbose_) std::cout << "warning: dem backprojection cost function - outside DEM bounds" << std::endl;
      return std::numeric_limits<double>::max();
    }
    // within bounds so get squared distance between dem z and ray z
    double z = (*dview_)(u,v);
    return (elev-z)*(elev-z);
  }

private:
  vgl_ray_3d<double> ray_;
  vpgl_geo_camera* geo_cam_;
  const vil_image_view<float>* dview_;
  bool verbose_;
};
vpgl_backproject_dem::vpgl_backproject_dem( vil_image_resource_sptr const& dem, double zmin, double zmax):verbose_(false), min_samples_(5000.0), tail_fract_(0.025), dem_(dem)
{
  //construct a geo_camera for the dem (an orthographic view looking straight down)
  if(!vpgl_geo_camera::init_geo_camera(dem_, geo_cam_)){
    if(verbose_) std::cout << "WARNING! - units unknown in vpgl_backproject_dem - assume degrees meters" << std::endl;
  }
  if(!geo_cam_)
    return;
  //get the image of elevations
  dem_view_ = dem_->get_view();
  unsigned ni = dem_view_.ni(), nj = dem_view_.nj();

  // get the center of the dem
  unsigned nhi = ni/2, nhj = nj/2;
  double lon, lat;
  geo_cam_->img_to_global(nhi, nhj, lon, lat);
  double elev = dem_view_(nhi, nhj);
  geo_center_.set(lon, lat, elev);

  // get the corners of the dem
  geo_cam_->img_to_global(0, 0, lon, lat);
  elev = dem_view_(0, 0);
  dem_corners_.emplace_back(lon, lat, elev);

  geo_cam_->img_to_global(ni-1, 0, lon, lat);
  elev = dem_view_(ni-1, 0);
  dem_corners_.emplace_back(lon, lat, elev);

  geo_cam_->img_to_global(ni-1, nj-1, lon, lat);
  elev = dem_view_(ni-1, nj-1);
  dem_corners_.emplace_back(lon, lat, elev);

  geo_cam_->img_to_global(0, nj-1, lon, lat);
  elev = dem_view_(0, nj-1);
  dem_corners_.emplace_back(lon, lat, elev);

  // check for appropriate zmin/zmax inputs
  if (zmax > zmin) {
    z_min_ = zmin;
    z_max_ = zmax;

  } else {
    if(verbose_) std::cout << "Calculating Z-range from DEM..." << std::endl;
    //get the bounds on elevation by sampling according to a fraction of the dem area
    //compute the pixel interval (stride) for sampling the fraction
    double area = ni*nj;
    double stride_area = area/min_samples_;
    auto stride_interval = static_cast<unsigned>(std::sqrt(stride_area));

    // sample elevations
    std::vector<double> z_samples;
    float zmin_calc=std::numeric_limits<float>::max(), zmax_calc=-zmin_calc;
    for(unsigned j = 0; j<nj; j+=stride_interval)
      for(unsigned i = 0; i<ni; i+=stride_interval){
        float z = dem_view_(i,j);
        if(z <= 0.0)
          continue;
        z_samples.push_back(z);
      }
    // sort the samples to remove tails due to DEM errors
    std::sort(z_samples.begin(), z_samples.end());
    // remove the tails and compute min max elevations
    auto ns = static_cast<double>(z_samples.size());
    auto band_size = static_cast<unsigned>(ns*tail_fract_);
    for(unsigned k = band_size; k<(ns-band_size); ++k){
      double z = z_samples[k];
      if(z<zmin_calc) zmin_calc = static_cast<float>(z);
      if(z>zmax_calc) zmax_calc = z;
    }
    //the final elevation bounds
    z_min_ = zmin_calc;
    z_max_ = zmax_calc;
  }

  if(verbose_) std::cout << "[ZMIN,ZMAX]=[" << z_min_ << "," << z_max_ << "]" << std::endl;
}
vpgl_backproject_dem::~vpgl_backproject_dem(){
  if(geo_cam_)
    delete geo_cam_;
  geo_cam_ = nullptr;
}
// the function to backproject onto the dem using vgl objects
bool vpgl_backproject_dem::bproj_dem(const vpgl_camera<double>* cam,
                 vgl_point_2d<double> const& image_point,
                 double max_z, double min_z,
                 vgl_point_3d<double> const& initial_guess,
                 vgl_point_3d<double> & world_point,
                 double error_tol){
if(verbose_) std::cout << "vpgl_backproj_dem " << image_point << " max_z " << max_z << " min_z " << min_z << " init_guess " << initial_guess << " error tol " << error_tol << std::endl;
  //compute the ray corresponding to the image point
  double dz = (max_z - min_z);
  vgl_point_2d<double> initial_xy(initial_guess.x(), initial_guess.y());
  vgl_ray_3d<double> ray;
  if(!vpgl_ray::ray(cam, image_point, initial_xy, max_z, dz, ray)){
    if(verbose_) std::cout << " compute camera ray failed - Fatal!" << std::endl;
    return false;
  }
  vgl_point_3d<double> origin = ray.origin();
  //find min parameter on ray
  vgl_vector_3d<double> dir = ray.direction();
  if(std::fabs(dir.z())<0.001){
    if(verbose_) std::cout << "Ray parallel to XY plane - Fatal!" << std::endl;
    return false;
  }
  //inital guess at ray parameter
  double t = (initial_guess.z() - max_z)/dir.z();

  //check if guess is inside DEM - could be too conservative for very oblique cameras
  vgl_point_3d<double> guess_point = origin + t*dir;
  double lon = guess_point.x(), lat = guess_point.y(),elev = guess_point.z();
  double ud, vd;
  //Note that elevation has no effect in the project function - just required to meet the interface of an abstract camera
  geo_cam_->project(lon, lat, elev, ud, vd);
  int u = static_cast<int>(ud+0.5), v = static_cast<int>(vd+0.5);
  int ni = dem_view_.ni(), nj = dem_view_.nj();
  if(u<0||u>=ni||v<0||v>=nj){
    if(verbose_) std::cout << "Initial guess for DEM intersection is outside DEM bounds - Fatal!" << std::endl;
    return false;
  }
  // construct the brent minimizer
  // note the brent minimzier is not fully integrated with the vil_nonlinear_minimizer interface
  // thus the non standard call for error below
  dem_bproj_cost_function c(dem_view_, geo_cam_, ray, verbose_);
  vnl_brent_minimizer brm(c);
  double tmin = brm.minimize(t);
  double error = brm.f_at_last_minimum();
  if(error>error_tol)
    return false;

  world_point = origin + tmin*dir;

  if(verbose_) std::cout << "success! ray/dem intersection " << world_point  << std::endl;
  return true;
}

bool vpgl_backproject_dem::bproj_dem(const vpgl_camera<double>* cam, vnl_double_2 const& image_point,
                                     double max_z, double min_z, vnl_double_3 const& initial_guess, vnl_double_3& world_point,
                                     double error_tol){
  vgl_point_2d<double> img_pt;
  vgl_point_3d<double> init_guess;
  vgl_point_3d<double> wrld_pt;
  img_pt.set(image_point[0], image_point[1]);
  init_guess.set(initial_guess[0],initial_guess[1],initial_guess[2]);
  bool good = this->bproj_dem(cam, img_pt, max_z, min_z, init_guess, wrld_pt, error_tol);

  if(!good){
    return false;
  }
  world_point[0]=wrld_pt.x(); world_point[1]=wrld_pt.y(); world_point[2]=wrld_pt.z();
  return true;
}

bool vpgl_backproject_dem::bproj_dem(vpgl_rational_camera<double> const& rcam, vnl_double_2 const& image_point, double max_z, double min_z,
                                     vnl_double_3 const& initial_guess, vnl_double_3& world_point, double error_tol){

  const auto* cam = dynamic_cast<const vpgl_camera<double>* >(&rcam);
  return this->bproj_dem(cam, image_point, max_z, min_z, initial_guess, world_point, error_tol);
}

bool vpgl_backproject_dem::bproj_dem(vpgl_rational_camera<double> const& rcam, vgl_point_2d<double> const& image_point, double max_z, double min_z,
                                     vgl_point_3d<double> const& initial_guess, vgl_point_3d<double>& world_point, double error_tol){

  const auto* cam = dynamic_cast<const vpgl_camera<double>* >(&rcam);
  return this->bproj_dem(cam, image_point, max_z, min_z, initial_guess, world_point, error_tol);
}
