#include <cmath>
#include <algorithm>
#include <fstream>
#include <iomanip>

#include "vnl/vnl_random.h"
#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_cross.h"
#include "vnl/vnl_det.h"
#include "vnl/vnl_inverse.h"
#include "vgl/vgl_homg_point_2d.h"
#include <vgl/algo/vgl_intersection.h>
#include "vgl/vgl_ray_3d.h"
#include "vil/vil_save.h"
#include "vil/vil_image_view.h"
#include "vpgl/vpgl_local_rational_camera.h"

#include "acal_f_utils.h"


static bool abs_less(const double& a, const double& b)
{
	return fabs(a) < fabs(b);
}


vpgl_camera_double_sptr
acal_f_utils::load_camera(std::string const& cam_path)
{
  vpgl_camera_double_sptr cam;
  bool local = true;
    //rational camera may be local, therefore we check if it's local first
  cam = read_local_rational_camera<double>(cam_path);
  if ( !cam ) {
    std::cout << "Rational camera isn't local... trying global" << std::endl;
    cam = read_rational_camera<double>(cam_path);
    local = false;
  }
  if (!cam) {
    std::cout << "[" + cam_path + "] is not a valid rational camera path" << std::endl;
  }
  return cam;
}


// The correspondence in image1, match_pair.uv1_ defines an epipolar line in image2 given the fundamental matrix F.
// That is,  l' = F uv1. The epipolar line distance is dot_product(l', uv2). The fundamental matrix is normalized so that
// the epipolar line distance is Euclidean distance in pixels.
size_t
acal_f_utils::filter_matches(vnl_matrix_fixed<double, 3, 3> const& F, std::vector<acal_match_pair>& corrs)
{
  size_t n_matches = corrs.size();

  // need enough matches to estimate inlier error
  if(n_matches < 10)
    return 0;

  // the fundamental matrix will be adjusted to minimize the epipolar line errors
  vnl_matrix_fixed<double, 3, 3> F_adj = F;

  // estimate average the epipolar line error (algebraic distance == Euclidean distance)
  std::vector<double> alg_error;
  for(size_t i = 0; i<n_matches; ++i){
    vnl_vector_fixed<double, 3> c1(1.0), c2(1.0);
    c1[0]=corrs[i].corr1_.pt_.x(); c1[1]=corrs[i].corr1_.pt_.y();
    c2[0]=corrs[i].corr2_.pt_.x(); c2[1]=corrs[i].corr2_.pt_.y();
    vnl_vector_fixed<double, 3> temp = F*(c1);
    double alg = dot_product(temp, c2);
    alg_error.push_back(alg);
  }

  // sort the errors in increasing order
  std::sort(alg_error.begin(), alg_error.end(), abs_less);

  // find the average of the lowest 10% error values
  size_t n_10_low = 0.1*n_matches;
  if(n_10_low < 5)
    n_10_low = 5;
  double avg = 0.0;
  for(size_t i = 0; i<n_10_low; ++i)
    avg += alg_error[i];
  avg /= n_10_low;
  //std::cout << "Average error - lowest 10% " << avg << std::endl;

  // correct the F matrix e parameter by the signed average 10% error
  // only the e element of the affine F matrix is affected
  F_adj[2][2] -= avg;

  // recompute the average error with the corrected F matrix
  alg_error.clear();
  for(size_t i = 0; i<n_matches; ++i){
    vnl_vector_fixed<double, 3> c1(1.0), c2(1.0);
    c1[0]=corrs[i].corr1_.pt_.x(); c1[1]=corrs[i].corr1_.pt_.y();
    c2[0]=corrs[i].corr2_.pt_.x(); c2[1]=corrs[i].corr2_.pt_.y();
    vnl_vector_fixed<double, 3> temp = F_adj*(c1);
    double alg = dot_product(temp, c2);
    alg_error.push_back(alg);
  }
  std::sort(alg_error.begin(), alg_error.end(), abs_less);
  double favg = 0;
  for(size_t i = 0; i<n_10_low; ++i)
    favg += fabs(alg_error[i]);
  favg /= n_10_low;
  if(favg < 0.5)
    favg = 0.5;

  // favg is now the corrected error for the lowest 10% of the population
  // std::cout << "Corrected Error - lowest 10% or 0.5 " << favg << std::endl;

  // filter the matches on epipolar line distance
  // compare to and absolute distance threshold and a threshold based on 10% average absolute error
  f_params fp;
  size_t n_filt_matches = 0;// return value
  std::vector<acal_match_pair> temp_corr;
  for (size_t i = 0; i < n_matches; ++i) {
    vnl_vector_fixed<double, 3> c1(1.0), c2(1.0);
    c1[0]=corrs[i].corr1_.pt_.x(); c1[1]=corrs[i].corr1_.pt_.y();
    c2[0]=corrs[i].corr2_.pt_.x(); c2[1]=corrs[i].corr2_.pt_.y();
    vnl_vector_fixed<double, 3> temp = F_adj*(c1);
    double falg = fabs(dot_product(temp, c2));
    if (falg > fp.max_epi_dist_)
      continue;
    if(falg > favg*fp.epi_dist_mul_)
      continue;
    n_filt_matches++;
    temp_corr.push_back(corrs[i]);
  }

  // remove duplicate matches
  // it can be the case that two matches define exactly the same image locations
  corrs.clear(); // write the filtered set back into the input
  for(size_t i = 0; i<n_filt_matches; ++i){
    vnl_vector_fixed<double, 3> ci(1.0);
    ci[0]=temp_corr[i].corr1_.pt_.x(); ci[1]=temp_corr[i].corr1_.pt_.y();

    // find equal correspondence matches
    bool eq = false;
    for(size_t j = i+1; j<n_filt_matches; ++j){
      vnl_vector_fixed<double, 3> cj(1.0);
      cj[0]=temp_corr[j].corr1_.pt_.x(); cj[1]=temp_corr[j].corr1_.pt_.y();
      vnl_vector_fixed<double, 3>  dif = ci - cj;
      double mag = dif.magnitude();
      eq = mag < 0.01;
    }

    // end of equal check
    if (eq)
      continue;
    corrs.push_back(temp_corr[i]);
  }

  n_filt_matches = corrs.size();
  return n_filt_matches;
}


double
acal_f_utils::ray_uncertainty(vpgl_affine_camera<double> const& acam1, vpgl_affine_camera<double> const& acam2)
{
  double ret = 0.0;
  vgl_vector_3d<double> r1 = acam1.ray_dir(); r1/= r1.length();
  vgl_vector_3d<double> r2 = acam2.ray_dir(); r2/= r2.length();
  double dp =  dot_product(r1, r2);
  ret = 1.0/(1.0-(dp*dp));
  return ret;
}


bool
acal_f_utils::intersect_tracks_with_3d(
    std::map<size_t, vpgl_affine_camera<double> >& cams,
    // tracks            cam_id   corr pt
    std::vector< std::map<size_t, vgl_point_2d<double> > > const& tracks,
    std::map<size_t, vgl_point_3d<double> >& inter_pts,
    std::map<size_t, std::map<size_t, vgl_point_2d<double> > > & projected_tracks,
    vnl_matrix<double> const& ray_covariance,
    size_t cam_mask)
{
  size_t n = cams.size();
  if(n < 2){
    std::cout << "require at least 2 cams "<< std::endl;
    return false;
  }
  size_t n_tracks = tracks.size();
  if(n_tracks ==0) {
    std::cout << "require at least 1 track " << std::endl;
    return false;
  }

  for (size_t t = 0; t < n_tracks; ++t)
  {
    const std::map<size_t, vgl_point_2d<double> >& track = tracks[t];
    std::vector<vgl_ray_3d<double> >  track_rays;
    for (std::map<size_t, vgl_point_2d<double> >::const_iterator cit = track.begin();
         cit != track.end(); ++cit)
    {
      size_t cam_idx = cit->first;
      if (cam_idx == cam_mask)
        continue;
      const vpgl_affine_camera<double>& cam = cams[cam_idx];
      vgl_homg_point_2d<double> img_pt(cit->second);
      vgl_ray_3d<double> ray = cam.backproject_ray(img_pt);
      track_rays.push_back(ray);
    }
    if(track_rays.size()<2)
      continue;
    vgl_point_3d<double> inter_pt;
    if(ray_covariance.rows() == 0 ||ray_covariance.cols() == 0){
      if (!vgl_intersection(track_rays, inter_pt))
        continue;
    }else{
      if (!vgl_intersection(track_rays, ray_covariance, inter_pt))
        continue;
      else{//
        size_t n = track_rays.size();
        std::cout << " Rays " << std::endl;
        for (size_t i = 0; i < n; ++i) {
          std::cout << track_rays[i] << std::endl;
        }
        std::cout << "plane covariance\n"<< ray_covariance << std::endl;
      }//
    }
    inter_pts[t] = inter_pt;

    //       cam id  corr pt
    std::map<size_t, vgl_point_2d<double> > proj_pts;
    for (std::map<size_t, vgl_point_2d<double> >::const_iterator cit = track.begin();
         cit != track.end(); ++cit) {
      size_t cam_idx = cit->first;
      const vpgl_affine_camera<double>& cam = cams[cam_idx];
      proj_pts[cam_idx] = cam.project(inter_pt);
    }
    projected_tracks[t] = proj_pts;
  }

  if(projected_tracks.size() == 0) {
    std::cout << "in ::intersect_tracks_with_3d, rays do not intersect for any track" << std::endl;
    return false;
  }
  return true;
}



bool
acal_f_utils::intersect_tracks(
    std::map<size_t, vpgl_affine_camera<double> > & cams,
    // tracks             cam_id  corr pt
    std::vector< std::map<size_t, vgl_point_2d<double> > > const& tracks,
    std::map<size_t, std::map<size_t, vgl_point_2d<double> > >& projected_tracks,
    size_t cam_mask,
    bool verbose)
{
  size_t n_tracks = tracks.size();
  if(n_tracks < 1){
    std::cout << "require at least 1 track "<< std::endl;
    return false;
  }

  for(size_t t = 0; t<n_tracks; ++t)
  {
    const std::map<size_t, vgl_point_2d<double> >& track = tracks[t];
    std::vector<vgl_ray_3d<double> >  track_rays;
    for(std::map<size_t, vgl_point_2d<double> >::const_iterator cit = track.begin();
        cit != track.end(); ++cit){
      size_t cam_idx = cit->first;
      if(cam_idx == cam_mask)
        continue;
      const vpgl_affine_camera<double>& cam = cams[cam_idx];
      vgl_homg_point_2d<double> img_pt(cit->second);
      vgl_ray_3d<double> ray = cam.backproject_ray(img_pt);
      track_rays.push_back(ray);
    }
    if(track_rays.size()<2)
      continue;
    vgl_point_3d<double> inter_pt;
    if(!vgl_intersection(track_rays, inter_pt)){
      continue;
    }
    //       cam id  corr pt
    std::map<size_t, vgl_point_2d<double> > proj_pts;
    for(std::map<size_t, vgl_point_2d<double> >::const_iterator cit = track.begin();
        cit != track.end(); ++cit)
    {
      size_t cam_idx = cit->first;
      const vpgl_affine_camera<double>& cam = cams[cam_idx];
      proj_pts[cam_idx] = cam.project(inter_pt);
    }
    projected_tracks[t] = proj_pts;
  }

  if(projected_tracks.size() == 0){
    std::cout << "in ::intersect_tracks, rays do not intersect for any track" << std::endl;
    return false;
  }
  return true;
}


bool
acal_f_utils::write_f_matches(
    std::string fmatches_path,
    std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > > const& matches,
    std::map<size_t, std::string> & image_paths)
{
  if(matches.size() == 0){
    std::cout << "no matches to write" << std::endl;
    return false;
  }

  std::ofstream ostr(fmatches_path.c_str());
  if(!ostr){
    std::cout << "can't open " << fmatches_path << " to write fmatches"<< std::endl;
    return false;
  }

  // write doc section
  ostr << "#Pairwise matches between many images, filtered by affine F." << std::endl;
  ostr << "#Each pairwise matching record is in following format:" << std::endl;
  ostr << "#[index of image1] [filepath of image1]" << std::endl;
  ostr << "#[index of image2] [filepath of image2]" << std::endl;
  ostr << "#[N = number of matches between the two images]" << std::endl;
  ostr << "# N feature match records follow the number, and each feature match is" << std::endl;
  ostr << "#[Feature index in image1] [X] [Y] [Feature index in image2] [X] [Y]" << std::endl;
  ostr << "#You should repeat the reading until end of the file" << std::endl;
  ostr << std::endl;

  // write the correspondences
  for(std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > >::const_iterator it1 =  matches.begin();
      it1 != matches.end(); ++it1)
  {
    size_t img_idx1 = it1->first;
    const std::map<size_t, std::vector<acal_match_pair> >& temp = it1->second;
    for(std::map<size_t, std::vector<acal_match_pair> >::const_iterator it2 = temp.begin();
        it2 != temp.end(); ++it2){
      size_t img_idx2 = it2->first;
      const std::vector<acal_match_pair>& matches = it2->second;
      size_t n = matches.size();
      ostr << img_idx1 << ' ' << image_paths[img_idx1] << std::endl;
      ostr << img_idx2 << ' ' << image_paths[img_idx2] << std::endl;
      ostr << n << std::endl;
      for(size_t i = 0; i<n; ++i) {
        ostr << matches[i].corr1_.id_ << ' ' << matches[i].corr1_.pt_.x() << ' ' << matches[i].corr1_.pt_.y() << ' '
             << matches[i].corr2_.id_ << ' ' << matches[i].corr2_.pt_.x() << ' ' << matches[i].corr2_.pt_.y() << std::endl;
      }
	    ostr << std::endl;
    }
  }
  return true;
}


bool
acal_f_utils::read_f_matches(
    std::string fmatches_path,
    std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > >& matches,
    std::map<size_t, std::string>& image_paths)
{
  std::ifstream istr(fmatches_path.c_str());
  if(!istr){
    std::cout << "can't open " << fmatches_path << " to read fmatches"<< std::endl;
    return false;
  }

  // skip doc section
  std::string temp;
  for (size_t i = 0; i < 8; ++i)
	std::getline(istr, temp);

  std::string temp_i, temp_j;
  size_t img_idx_i, img_idx_j;

  while(istr >> img_idx_i >> temp_i){
    istr >> img_idx_j >> temp_j;

    image_paths[img_idx_i] = temp_i;
    image_paths[img_idx_j] = temp_j;

    size_t n_tracks;
    istr >> n_tracks;
    for(size_t t = 0; t<n_tracks; ++t){
      size_t corr_id_i, corr_id_j;
      double ui, vi, uj, vj;
      istr >> corr_id_i >> ui >> vi >> corr_id_j >> uj >> vj;
	  vgl_point_2d<double> pi(ui, vi), pj(uj, vj);
	  acal_corr corr1(corr_id_i, pi), corr2(corr_id_j, pj);
      acal_match_pair mp(corr1, corr2);
      matches[img_idx_i][img_idx_j].push_back(mp);
    }
  }

  if (matches.size() == 0) {
	  std::cout << "no matches read from file" << std::endl;
	  return false;
  }
  return true;
}


bool
acal_f_utils::write_affine_cameras(
    std::map<size_t, vpgl_affine_camera<double> >& acams,
    std::string affine_cam_path)
{
  std::ofstream ostr(affine_cam_path.c_str());
  if(!ostr){
    std::cout << "Can't open " << affine_cam_path << " to write affine cameras" << std::endl;
    return false;
  }
  ostr << acams.size() << std::endl;
  for(std::map<size_t, vpgl_affine_camera<double> >::iterator ait = acams.begin();
      ait != acams.end(); ++ait){
    size_t aidx = ait->first;
    ostr << aidx << std::endl;
	ostr << ait->second.viewing_distance() << std::endl;
    ostr << ait->second;// emits a crlf
  }
  ostr.close();
  return true;
}


bool acal_f_utils::read_affine_cameras(
    std::string affine_cam_path,
    std::map<size_t, vpgl_affine_camera<double> >& acams)
{
  // Open affine_cam_path for reading
  std::ifstream istr(affine_cam_path.c_str());
  if(!istr) {
    std::cerr << "Can't open " << affine_cam_path << " to read affine cameras" << std::endl;
    return false;
  }

  // Read the first line, which should be the number of cameras
  size_t ncams, cam_idx;
  istr >> ncams;
  if(ncams == 0) {
    std::cerr << "no cameras to read" << std::endl;
    return false;
  }

  // For each camera
  for(size_t i = 0; i<ncams; ++i) {

    // Read in the camera ID
    istr >> cam_idx;

    // Read in the viewing distance
    double vd;
    istr >> vd;

    // Create an affine camera
    vpgl_affine_camera<double> acam;

    // Read in the 3x4 affine camera matrix
    istr >> acam;

    // Store the viewing distance in the camera
    acam.set_viewing_distance(vd);

    // Save camera into input map
    acams[cam_idx] = acam;

    if (!istr.good()) {
      // An error state flag was set for the input stream, so something went wrong
      std::cerr << "Can't open " << affine_cam_path << " to read affine cameras" << std::endl;
      return false;
    }
  }

  // Close affine_cam_path
  istr.close();

  // Return success
  return true;
}


bool
acal_f_utils::datetime_from_iname(std::string const& iname, dtime& datetime)
{
  // parse image name for collection time
  // example 14OCT18160722
  // path is of the form yyMMMddhhmmss
  // y - year num, M month char, d day num, h hour num, m minute num, s second num
  // extract the date time characters from the file name string
  std::string month_array[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
  unsigned year = 0, month = 0, day = 0, hours = 0, minutes = 0, seconds = 0;
  std::string year_str, month_str, day_str, hour_str, min_str, sec_str;
  std::string::const_iterator sit = iname.begin();
  year_str.push_back(*(sit++)); year_str.push_back(*(sit++));
  month_str.push_back(*(sit++)); month_str.push_back(*(sit++)); month_str.push_back(*(sit++));
  month = 0;

  bool found = false;
  for(size_t i = 0; i<12&&!found; ++i) {
    if(month_str == month_array[i]) {
      found = true;
      month = static_cast<unsigned>(i+1);
    }
  }
  if(!found){
    std::cout << "Image file name prefix not of the form 14OCT18160722" << std::endl;
    return false;
  }

  day_str.push_back(*(sit++)); day_str.push_back(*(sit++));
  hour_str.push_back(*(sit++)); hour_str.push_back(*(sit++));
  min_str.push_back(*(sit++)); min_str.push_back(*(sit++));
  sec_str.push_back(*(sit++)); sec_str.push_back(*(sit));

  // use the strings as streams
  std::stringstream yearss(year_str), dayss(day_str), hourss(hour_str), minss(min_str), secss(sec_str);
  yearss >> year;
  if(year > 60)// earliest possible 20th century image (if this code is still being used in 2060 I will be amazed! JLM)
    year += 1900;//Y2K issue for old images
  else
    year += 2000;
  dayss >> day;  hourss >> hours; minss >> minutes; secss >> seconds;
  datetime = dtime(year, month, day, hours, minutes, seconds);
  return true;
}


// compute t1 - t0
int dtime::time_diff(dtime const& t0, dtime const& t1)
{
  int year_dif = (t1.year_-t0.year_)*31536000;     //seconds/year
  int month_dif = (t1.month_ - t0.month_)*2628288; //average seconds/month (should use acutal length of each month)
  int day_dif = (t1.day_ - t0.day_)*86400;         //seconds/day
  int hour_dif = (t1.hours_-t0.hours_)*3600;       //seconds/hour
  int minute_dif = (t1.minutes_-t0.minutes_)*60;    //seconds/minute
  int second_dif = (t1.seconds_-t0.seconds_);       //seconds
  return (year_dif + month_dif + day_dif + hour_dif + minute_dif + second_dif);
}
bool dtime::time_less(dtime const& ta, dtime const& tb){
  unsigned y = 0, m = 0, d = 0, h = 0, mi = 0, s = 0;
  dtime zero(y, m, d, h, mi, s);
  int ta_sec = time_diff(zero, ta);
  int tb_sec = time_diff(zero, tb);
  return ta_sec < tb_sec;
}
