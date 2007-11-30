#include <vcl_cmath.h>
#include <vnl/vnl_numeric_traits.h>
#include <vil/vil_image_resource.h>
#include <vsol/vsol_point_2d.h>
#include <sdet/sdet_detector.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vgl/vgl_distance.h>
#include "bwm_reg_processor.h"
#include "bwm_reg_matcher.h"

bwm_reg_processor::
bwm_reg_processor(vil_image_resource_sptr const& model_image,
                  vpgl_rational_camera<double> const& model_cam,
                  vgl_point_3d<double> const& world_point,
                  vgl_plane_3d<double> const& world_plane,
                  vil_image_resource_sptr const& search_image,
                  vpgl_rational_camera<double> const& search_cam
                  ): model_image_(model_image), model_cam_(model_cam),
                     search_image_(search_image), search_cam_(search_cam),
                   world_point_(world_point), world_plane_(world_plane)
{
  distance_threshold_ = vnl_numeric_traits<double>::maxval;
  lvcs_ =  bgeo_lvcs(world_point_.y(),world_point_.x(),world_point_.z(),
                     bgeo_lvcs::wgs84,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
}

// define the region of interest around the model point
bool bwm_reg_processor::
extract_model_region(vgl_point_2d<double> const& model_point,
                     double model_radius)
{
  if(!model_image_)
    return false;
  unsigned ni = model_image_->ni(), nj = model_image_->nj();
  model_roi_ = brip_roi(ni, nj);
  unsigned xc = static_cast<unsigned>(model_point.x());
  unsigned yc = static_cast<unsigned>(model_point.y());
  unsigned r = static_cast<unsigned>(model_radius);
  model_roi_.add_region(xc, yc, r);
  return true;
}

// find the gsd in meters/per/pixel along the diagonal of a given image
double bwm_reg_processor::diagonal_gsd(vil_image_resource_sptr const& image,
                                       vpgl_rational_camera<double> const& cam
                    )
{
  //get the image diagonal
  double ni = static_cast<double>(image->ni()), nj = static_cast<double>(image->nj());
  double image_diag = vcl_sqrt(ni*ni + nj*nj);
  if(image_diag == 0)
    return -1;
  //get rough diagonal in world coordinates
  vgl_point_2d<double> ul(0,0), lr(ni, nj);
  vgl_point_3d<double> wul, wlr;
  bool success = vpgl_backproject::bproj_plane(model_cam_, ul,
                                               world_plane_,
                                               world_point_,
                                               wul);
  if(!success)
    return -1;
  success = vpgl_backproject::bproj_plane(model_cam_, lr,
                                               world_plane_,
                                               world_point_,
                                               wlr);
  if(!success)
    return -1;
  // convert upper left position to meters
    double xul, yul, zul;
    lvcs_.global_to_local(wul.x(), wul.y(), wul.z(),
                       bgeo_lvcs::wgs84,
                       xul,yul,zul,
                       bgeo_lvcs::DEG,bgeo_lvcs::METERS);
  // convert lower right position to meters
  double xlr, ylr, zlr;
  lvcs_.global_to_local(wlr.x(), wlr.y(), wlr.z(),
                       bgeo_lvcs::wgs84,
                       xlr,ylr,zlr,
                       bgeo_lvcs::DEG,bgeo_lvcs::METERS);

  double world_diag = vcl_sqrt((xlr-xul)*(xlr-xul)+(ylr-yul)*(ylr-yul));
  //shouldn't happen
  if(world_diag==0)
    return -1;

  double diag_gsd = world_diag/image_diag;
  return diag_gsd;
}
//find the proper smoothing for the model image to duplicate
//the resolution of the search image. For example, multispectral
//images have 5-8 sigma of the model image
// this function returns gsd_search/gsd_model
double bwm_reg_processor::scale()
{
  //get the gsd of the model image
  double model_gsd = this->diagonal_gsd(model_image_, model_cam_);
  if(model_gsd<0) return -1;

  //get the gsd of the search image
  double search_gsd = this->diagonal_gsd(search_image_, search_cam_);
  if(search_gsd<0) return -1;

  return search_gsd/model_gsd;
}
// run edge detection at the appropriate scale on the model image
bool bwm_reg_processor::extract_model_edges()
{
  if(!model_image_) return false;
  sdet_detector_params dp;
  double scale = this->scale();
  if(scale<=0)
    return false;
  dp.smooth = 1.0f;
  if(scale>1.0)
    dp.smooth = static_cast<float>(scale);
  dp.noise_multiplier=2.0f;
  dp.aggressive_junction_closure=1;
  dp.borderp = false;

  if (!model_image_||!model_image_->ni()||!model_image_->nj())
    {
      vcl_cerr << "In bwm_reg_processor::extract_model_edges() - no image\n";
      return false;
    }
  sdet_detector det(dp);
  
  det.SetImage(model_image_, model_roi_);

  det.DoContour();
  model_edges_.clear();
  if(!det.get_vsol_edges(model_edges_))
    {
      vcl_cerr << "In bwm_reg_processor::extract_model_edges() -"
               << " edge detection failed\n";
      return false;
    }
  return true;

}
// map the edges into the search image space
bool bwm_reg_processor::transfer_model_edges()
{
  trans_model_edges_.clear();
  //get the bounds on the transferred model edges
  double dcmin = vnl_numeric_traits<double>::maxval, dcmax = 0;
  double drmin = dcmin, drmax = 0;
  for(vcl_vector<vsol_digital_curve_2d_sptr>::iterator cit =
        model_edges_.begin(); cit != model_edges_.end(); ++cit)
    {
      vsol_digital_curve_2d_sptr trans_dc = new vsol_digital_curve_2d();
      for(unsigned i = 0; i<(*cit)->size(); ++i)
      {
        vgl_point_2d<double> p = (*cit)->point(i)->get_p();
        vgl_point_3d<double> p3d;
        bool success = vpgl_backproject::bproj_plane(model_cam_, p,
                                                     world_plane_,
                                                     world_point_,
                                                     p3d);
        if(!success)
          return false;
        //the transferred edgel
        vgl_point_2d<double> tp = search_cam_.project(p3d);
        vsol_point_2d_sptr vtp = new vsol_point_2d(tp);
        trans_dc->add_vertex(vtp);
        double c = tp.x(), r = tp.y();
        if(c<dcmin) dcmin = c;
        if(c>dcmax) dcmax = c;
        if(r<drmin) drmin = r;
        if(r>drmax) drmax = r;
      }
      trans_model_edges_.push_back(trans_dc);
    }
  trans_model_cols_ = static_cast<unsigned>(dcmax-dcmin+1);
  trans_model_rows_ = static_cast<unsigned>(drmax-drmin+1);
  return true;
}
// define the region of interest around the search point
bool bwm_reg_processor::
extract_search_region(vgl_point_2d<double> const& model_point,
                      double proj_error)
{
  
  if(!search_image_)
    return false;


  //get the back projection of the model image point onto the world plane
  vgl_point_3d<double> model_point_3d;

  bool success = vpgl_backproject::bproj_plane(model_cam_, model_point,
                                               world_plane_,
                                               world_point_,
                                               model_point_3d);
  if(!success)
    return false;
  // forward project into the search image
  vgl_point_2d<double> search_point =  search_cam_.project(model_point_3d);
  
  // convert to local coordinates since projection error is in meters
  double lx, ly, lz;
  lvcs_.global_to_local(model_point_3d.x(),model_point_3d.y(),
                        model_point_3d.z(),bgeo_lvcs::wgs84,
                        lx,ly,lz,
                        bgeo_lvcs::DEG,bgeo_lvcs::METERS);

  lx+=proj_error; //deviation is roughly isotropic so x is as good as any
  
  // convert back to geographic coordinates
  double lon, lat, elev;
  lvcs_.local_to_global(lx, ly, lz,bgeo_lvcs::wgs84, lon, lat ,elev,
                        bgeo_lvcs::DEG,bgeo_lvcs::METERS);
  
  vgl_point_3d<double> radius_point_3d(lon, lat, elev);

  // get the deviated point in image space
  vgl_point_2d<double> radius_image_point =
    search_cam_.project(radius_point_3d);

  // the error in image coordinates
  double error_radius =
    vgl_distance<double>(radius_image_point, search_point);
  
  // convert to an image roi
  unsigned ni = search_image_->ni(), nj = search_image_->nj();
  search_roi_ = brip_roi(ni, nj);
  double xmin = search_point.x() - trans_model_cols_ - error_radius;
  double ymin = search_point.y() - trans_model_rows_ - error_radius; 
  double xmax = search_point.x() + trans_model_cols_ + error_radius;
  double ymax = search_point.y() + trans_model_rows_ + error_radius;
  // clip if outside image bounds
  if(xmin<0) xmin = 0;
  if(ymin<0) ymin = 0;
  if(xmax>ni) xmax = ni;
  if(ymax>nj) ymax = nj;
  unsigned ixmin = static_cast<unsigned>(xmin);
  unsigned iymin = static_cast<unsigned>(ymin);
  unsigned ixsize = static_cast<unsigned>(xmax - xmin +1);
  unsigned iysize = static_cast<unsigned>(ymax - ymin +1);
  search_roi_.add_region(ixmin, iymin, ixsize, iysize);
  return true;
}
// run edge detection on the search image at appropriate resolution
bool bwm_reg_processor::extract_search_edges()
{
  if(!search_image_) return false;
  sdet_detector_params dp;
  double scale = this->scale();
  if(scale<=0)
    return false;
  dp.smooth = 1.0f;
  if(scale<1.0)
    dp.smooth = static_cast<float>(1.0/scale);
  dp.noise_multiplier=2.0f;
  dp.aggressive_junction_closure=1;
  dp.borderp = false;

  if (!search_image_||!search_image_->ni()||!search_image_->nj())
    {
      vcl_cerr << "In bwm_reg_processor::extract_search_edges() - no image\n";
      return false;
    }
  sdet_detector det(dp);
  
  det.SetImage(search_image_, search_roi_);

  det.DoContour();
  search_edges_.clear();
  if(!det.get_vsol_edges(search_edges_))
    {
      vcl_cerr << "In bwm_reg_processor::extract_search_edges() -"
               << " edge detection failed\n";
      return false;
    }
  return true;
}

// carry out geometric matching using a champher
bool bwm_reg_processor::match_edges(int& tcol, int& trow)
{
  bwm_reg_matcher rm(trans_model_edges_,
                     search_roi_.cmin(0), search_roi_.rmin(0), 
                     search_roi_.csize(0), search_roi_.rsize(0),
                     search_edges_);
  bool success = rm.match(tcol, trow, distance_threshold_);
  return true;
}

bool bwm_reg_processor::match(vgl_point_2d<double> const& model_point,
                              double model_radius,
                              double proj_error,
                              double distance_threshold,
                              int& tcol, int& trow)
{
  distance_threshold_ = distance_threshold;
  this->extract_model_region(model_point, model_radius);
  this->extract_model_edges();
  this->transfer_model_edges();
  this->extract_search_region(model_point, proj_error);
  this->extract_search_edges();
  return this->match_edges(tcol, trow);
}
