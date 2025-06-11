#include "vpgl_nitf_RSM_camera_extractor.h"
#include "vpgl_nitf_rational_camera.h"
#include <vpgl/vpgl_affine_camera.h>
#include <vil/vil_load.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vil/file_formats/vil_nitf2_image_subheader.h>
#include <vil/file_formats/vil_nitf2_des.h>
#include <limits>
#include <iomanip>

void RSM_ECA_adjustable_parameter_metadata::print(std::ostream & os)
{
  if (!defined_){
    os << "print: RSM_ECA is not defined" << std::endl;    return;
  }
    os << "\n=== extracted from the RSMECA TRE definition ===" << std::endl;
  os << "N adjustable parameters " << num_adj_params_ << " original number " << num_original_adj_params_ << std::endl;
  os << "local coordinate system origin (ECEF)" << translation_ << std::endl;
  os << "local coordinate system rotation (rel. ECEF)\n" << rotation_ << std::endl;
  os << "adjustable parameter covariance indices I* image space, G* ground space:" << std::endl;
  for (auto itr = covar_index_.begin(); itr != covar_index_.end(); ++itr)
  {
    if (itr->second > 0)
      os << itr->first << ' ' << itr->second << std::endl;
  }
  os << "\nIndependent subgroup covariance and correlation data:" << std::endl;
  for (size_t i = 0; i < independent_subgroup_covariance_.size(); ++i)
  {
    os << "\nindependent covariance block" << std::endl;
    os << independent_subgroup_covariance_[i] << std::endl;
    os << "correlation flag (" << correlation_flags_[i] << ")" << std::endl;
    os << "(cor. tau) piecewise correlation segments " << std::endl;
    for (size_t s = 0; s < correlation_segments_[i].size(); ++s)
      os << correlation_segments_[i][s].first << "  " << correlation_segments_[i][s].second << std::endl;
  }
  os << "\nMapping matrix (phi)\n" << phi_ << std::endl;
  if(unmodeled_error_){
    os << "\n=====   Unmodeled Error ==== " << std::endl;
    os << "unmodeled (row var) (col var) (row col covar)" << std::endl;
    os <<"          " << unmodeled_row_variance_ << "         " << unmodeled_col_variance_ 
       << "         " << unmodeled_row_col_variance_ << std::endl;
    os << "piecewise row correlation function" << std::endl;
    std::tuple<size_t, std::vector<std::pair<double, double> > >& useg_row = unmodeled_row_correlation_;
    for (size_t s = 0; s < std::get<0>(useg_row); ++s) {
      os << "cor " << std::get<1>(useg_row)[s].first << " tau " <<
        std::get<1>(useg_row)[s].second << std::endl;
    }
    os << "piecewise col correlation function" << std::endl;
    std::tuple<size_t, std::vector<std::pair<double, double> > >& useg_col = unmodeled_col_correlation_;
    for (size_t s = 0; s < std::get<0>(useg_col); ++s) {
      os << "cor " << std::get<1>(useg_col)[s].first << " tau " <<
        std::get<1>(useg_col)[s].second << std::endl;
    }
  }
}
void RSM_ECB_adjustable_parameter_metadata::print(std::ostream& os)
{
  if (!defined_){
    os << "print: RSM_ECB is not defined" << std::endl;    return;
  }
  os << "\n=== extracted from the RSMECB TRE definition ===" << std::endl;
  os << "N active adjustable parameters " << num_active_adj_params_
     << " original number " << num_original_adj_params_ << std::endl;
  if (rect_local_coordinate_system_) {
    os << "Defined Local Rectangular CS" << std::endl;
    os << "local rect. coordinate system origin (ECEF)" << rect_translation_ << std::endl;
    os << "local rect. coordinate system rotation (rel. ECEF)\n" << rect_rotation_ << std::endl;
  }
  if (image_adjustable_params_) {
    os << "Using image adjustments" << std::endl;
    os << "N image adjustable params " << n_image_adjustable_params_ << std::endl;
    os << "N image row adjustable params " << n_image_row_adjustable_params_ << std::endl;
    os << "N image col adjustable params " << n_image_col_adjustable_params_ << std::endl;
    os << " image parameter id     x   y   z  power" << std::endl;
    for (auto itr = image_row_xyz_powers_.begin(); itr != image_row_xyz_powers_.end(); ++itr)
      os << "row " << std::setw(3) << itr->first << "                 " << std::get<0>(itr->second) << "   "
         << std::get<1>(itr->second) << "   " << std::get<2>(itr->second) << std::endl;
    
    for (auto itr = image_col_xyz_powers_.begin(); itr != image_col_xyz_powers_.end(); ++itr)
      os << "col " << std::setw(3) << itr->first << "                 " << std::get<0>(itr->second) << "   "
         << std::get<1>(itr->second) << "   " << std::get<2>(itr->second) << std::endl;
  }
  if (ground_adjustable_params_) {
    os << "N ground adjustable params " << n_ground_adjustable_params_ << std::endl;
    for (size_t idx : ground_adjust_param_idx_)
      os << idx << ' ';
    os << std::endl;
  }
  os << "X, Y, Z normalization scale and offset " << std::endl;
  std::vector<std::string> coor = { "X", "Y", "Z" };
  os << "coordinate   offset  scale " << std::endl;
  for (size_t id = 0; id < 3; ++id)
    os << "     " << coor[id] << "          " << xyz_norm_[id].first << "   " << xyz_norm_[id].second << std::endl;

  os << "Number of basis adjustable params " << n_basis_adjustable_params_ << std::endl;
  os << "A matrix\n" << A_matrix_ << "    ======     " << std::endl;
  
  os << "Number of independent covariance subgroups " << num_independent_subgroups_ << std::endl;
  for (size_t ig = 0; ig < num_independent_subgroups_; ++ig) {
    vnl_matrix<double>& cov = independent_covar_[ig];
    os << "cov[" << ig << "]\n" << cov ;
    if (corr_analytic_functions_.count(ig) > 0) {
      os << "coorelation function " << std::endl;
      std::tuple<double, double, double, double >& func = corr_analytic_functions_[ig];
      os << "A " << std::get<0>(func) << " alpha " << std::get<1>(func)
         << " beta " << std::get<2>(func) << " T " << std::get<3>(func) << std::endl;
    }
    else if (corr_piecewise_functions_.count(ig) > 0) {
      std::tuple<size_t, std::vector<std::pair<double, double> > >& seg_func =
        corr_piecewise_functions_[ig];
      for (size_t s = 0; s < std::get<0>(seg_func); ++s) {
        os << "cor " << std::get<1>(seg_func)[s].first << " tau " <<
          std::get<1>(seg_func)[s].second << std::endl;
      }
    }
  }
  os << "\nMapping matrix (map)\n" << mapping_matrix_ << std::endl;
  if(unmodeled_error_){
    os << "\n=====   Unmodeled Error ==== " << std::endl;
    os << "unmodeled (row var) (col var) (row col covar)" << std::endl;
    os <<"          " << unmodeled_row_variance_ << "         " << unmodeled_col_variance_ 
       << "         " << unmodeled_row_col_variance_ << std::endl;
    if (unmodeled_analytic_) {
      std::tuple<double, double, double, double >& func_row = unmodeled_row_analytic_function_;
     std::tuple<double, double, double, double >& func_col = unmodeled_col_analytic_function_;
     os << "A_row " << std::get<0>(func_row) << " alpha_row " << std::get<1>(func_row)
        << " beta_row " << std::get<2>(func_row) << " T_row " << std::get<3>(func_row) << std::endl;
     os << "A_col " << std::get<0>(func_col) << " alpha_col " << std::get<1>(func_col)
       << " beta_col " << std::get<2>(func_col) << " T_col " << std::get<3>(func_col) << std::endl;
    } else {
      os << "piecewise row correlation function" << std::endl;
      std::tuple<size_t, std::vector<std::pair<double, double> > >& useg_row = unmodeled_row_piecewise_function_;
      for (size_t s = 0; s < std::get<0>(useg_row); ++s) {
        os << "cor " << std::get<1>(useg_row)[s].first << " tau " <<
          std::get<1>(useg_row)[s].second << std::endl;
      }
      os << "piecewise col correlation function" << std::endl;
      std::tuple<size_t, std::vector<std::pair<double, double> > >& useg_col = unmodeled_col_piecewise_function_;
      for (size_t s = 0; s < std::get<0>(useg_col); ++s) {
        os << "cor " << std::get<1>(useg_col)[s].first << " tau " <<
          std::get<1>(useg_col)[s].second << std::endl;
      }
    }
  }
}


// create a composite class that operates on Cartesian local coordinates
// similar to local_rational_camera, used to determine view angles and gsd
class local_RSM_camera : public vpgl_camera<double>
{
public:
  local_RSM_camera(const vpgl_lvcs & lvcs, vpgl_RSM_camera<double> * RSM_cam_ptr)
    : lvcs_(lvcs)
    , RSM_cam_ptr_(RSM_cam_ptr)
  {}

  // implement pure virtual methods
  virtual vpgl_camera<double> *
  clone() const
  {
    return nullptr;
  }

  virtual void
  project(const double x, const double y, const double z, double & u, double & v) const
  {
    // first, convert local to global geographic coordinates
    double lon, lat, gz;
    lvcs_.local_to_global(x, y, z, vpgl_lvcs::wgs84, lon, lat, gz);
    // convert to radians
    double lon_rad = lon / vnl_math::deg_per_rad, lat_rad = lat / vnl_math::deg_per_rad;
    // then, project global to 2D
    RSM_cam_ptr_->project(lon_rad, lat_rad, gz, u, v);
  }

private:
  vpgl_lvcs lvcs_;
  // pointer will be deleted externally
  vpgl_RSM_camera<double> * RSM_cam_ptr_;
};
bool
vpgl_nitf_RSM_camera_extractor::process_igeolo(size_t image_subheader_index)
{
  if (rsm_meta_.count(image_subheader_index) == 0)
  {
    std::cout << "invalid subheader index in process_igeolo " << image_subheader_index << std::endl;
    return false;
  }
  rsm_metadata & meta = rsm_meta_[image_subheader_index];
  // extract corner coordinates from image_geolo field
  // example 324158N1171117W324506N1171031W324428N1170648W324120N1170734W
  // coordinates are encoded as:
  // first section: 32 deg 41 min 58 seconds North
  // extract and convert to decimal degrees
  // from the NITF2.1 spec the IGEOLO order is:
  // "(0,0), (0, MaxCol), (MaxRow, MaxCol), (MaxRow, 0)"
  // i.e., UL > UR > LR > LL
  std::vector<std::pair<double, double>> coords;
  vpgl_nitf_rational_camera::geostr_to_latlon_v2(meta.igeolo_, coords);
  if (coords.size() != 4)
  {
    std::cout << "Parsing geostring failed" << std::endl;
    meta.igeolo_valid = false;
    return false;
  }
  unsigned LON = vpgl_nitf_rational_camera::LON;
  unsigned LAT = vpgl_nitf_rational_camera::LAT;
  unsigned UL = vpgl_nitf_rational_camera::UL;
  unsigned UR = vpgl_nitf_rational_camera::UR;
  unsigned LR = vpgl_nitf_rational_camera::LR;
  unsigned LL = vpgl_nitf_rational_camera::LL;

  meta.upper_left_.set(coords[UL].first, coords[UL].second);
  meta.upper_right_.set(coords[UR].first, coords[UR].second);
  meta.lower_left_.set(coords[LL].first, coords[LL].second);
  meta.lower_right_.set(coords[LR].first, coords[LR].second);
  meta.xy_corners_valid = true;
  double min_lon = std::numeric_limits<double>::max(), max_lon = -min_lon, min_lat = min_lon, max_lat = max_lon;
  for (size_t c = 0; c < 4; c += 2)
  {
    if (coords[c].first < min_lon)
      min_lon = coords[c].first;
    if (coords[c].first > max_lon)
      max_lon = coords[c].first;
    if (coords[c].second < min_lat)
      min_lat = coords[c].second;
    if (coords[c].second > max_lat)
      max_lat = coords[c].second;
  }
  

  // footprint in counter-clockwise order from lower left
  vgl_point_2d<double> ll(coords[LL].first, coords[LL].second);
  vgl_point_2d<double> lr(coords[LR].first, coords[LR].second);
  vgl_point_2d<double> ur(coords[UR].first, coords[UR].second);
  vgl_point_2d<double> ul(coords[UL].first, coords[UL].second);
  std::vector<vgl_point_2d<double>> sheet;
  sheet.push_back(ll);
  sheet.push_back(lr);
  sheet.push_back(ur);
  sheet.push_back(ul);
  meta.footprint_ = vgl_polygon<double>(sheet);
  return true;
}
static double r2d(double vrad, bool zero_to_360){
  double ret = 57.2957795130823*vrad;
  if(zero_to_360 && ret > 180.0)
    ret -= 360.0;
  return ret;
}
// the polytope vertices are (radians, radians, meters)
bool vpgl_nitf_RSM_camera_extractor::process_polytope(size_t image_subheader_index){
  if (rsm_meta_.count(image_subheader_index) == 0)
  {
    std::cout << "invalid header index in process_polytope " << image_subheader_index << std::endl;
    return false;
  }
  rsm_metadata& meta = rsm_meta_[image_subheader_index];
  bool zero_to_360 = (meta.ground_domain_=="H");//avoid +-180 cut
  bool local = (meta.ground_domain_ == "R");
  std::map<size_t, vgl_point_3d<double> >& polyt =  meta.polytope_;

  // convert radians to decimal degrees.
  if (meta.polytope_.size() != 8)
      return false;

  // axis-aligned bounding box (deg, deg, meters)
  double xmin = std::numeric_limits<double>::max(), xmax = -xmin;
  double ymin = xmin, ymax = -xmin;
  double zmin = xmin, zmax = -xmin;
  if(!local){//geodetic coordinates{
    for (size_t i = 1; i <= 8; ++i) {
      vgl_point_3d<double>& p = polyt[i];
      double x_deg = r2d(p.x(),zero_to_360) , y_deg = r2d(p.y(),zero_to_360);
      p.set(x_deg, y_deg, p.z());
      if(x_deg<xmin) xmin = x_deg; if(x_deg>xmax) xmax = x_deg;
      if(y_deg<ymin) ymin = y_deg; if(y_deg>ymax) ymax = y_deg;
      if(p.z()<zmin) zmin = p.z(); if(p.z()>zmax) zmax = p.z();
    }
  }else{//local Cartesian coordinates
    for (size_t i = 1; i <= 8; ++i) {
      vgl_point_3d<double>& p = polyt[i];
      double x = p.x(), y = p.y(), z = p.z();
      if(x<xmin) xmin = x; if(x>xmax) xmax = x;
      if(y<ymin) ymin = y; if(y>ymax) ymax = y;
      if(z<zmin) zmin = z; if(z>zmax) zmax = z;
    }
  }
  meta.polytope_valid = true;
  vgl_point_3d<double> pmin(xmin, ymin, zmin), pmax(xmax, ymax, zmax);
  meta.bounding_box_.add(pmin);
  meta.bounding_box_.add(pmax);
  meta.bounding_box_valid = true;

  // polytope vertex indices at image corners
  // doesn't seem to have a fixed relation to
  // polytope vertices in the test examples.
  unsigned UL = 1, UR = 3, LL = 2, LR = 4;
  meta.upper_left_.set( polyt[UL].x(), polyt[UL].y() );
  meta.upper_right_.set(polyt[UR].x(), polyt[UR].y() );
  meta.lower_left_.set( polyt[LL].x(), polyt[LL].y() );
  meta.lower_right_.set(polyt[LR].x(), polyt[LR].y() );
  meta.xy_corners_valid = true;

  // footprint in counter-clockwise order from lower left
   std::vector<vgl_point_2d<double>> sheet;
  sheet.push_back(meta.lower_left_);
  sheet.push_back(meta.lower_right_);
  sheet.push_back(meta.upper_right_);
  sheet.push_back(meta.upper_left_);
  meta.footprint_ = vgl_polygon<double>(sheet);

  meta.any_valid = true;
  return true;
}
bool
vpgl_nitf_RSM_camera_extractor::determine_header_status(vil_nitf2_image_subheader * header_ptr,
                                                        size_t header_idx,
                                                        bool & header_has_tres,
                                                        bool & header_has_RSM,
                                                        int & ixsofl)
{
  int ixshdl;
  vil_nitf2_tagged_record_sequence::const_iterator tres_itr;
  header_has_tres = false;
  header_has_RSM = false;


  if (!header_ptr)
  {
    std::cout << "NULL header pointer" << std::endl;
    return false;
  }
  std::string type = header_ptr->get_image_type();
  if (type != "")
  {
    if (verbose_)
      std::cout << "Image " << header_idx << " is of type " << type << std::endl;
    header_has_tres = true;
  }

  header_ptr->get_property("IXSHDL", ixshdl);
  if (ixshdl > 3)
  {
    // search for RSM tres
    if (header_ptr->get_property("IXSHD", hdr_ixshd_tres_[header_idx]))
    {
      header_has_tres = true;
      bool found = false; // stop looking if found
      // iterate through the tres looking for RSM entries
      for (tres_itr = hdr_ixshd_tres_[header_idx].begin(); tres_itr != hdr_ixshd_tres_[header_idx].end() && !found;
           ++tres_itr)
      {
        std::string tre_name = (*tres_itr)->name();
        if (tre_name == "RSMPCA")
        { // looking for "RSMPCA..."
          if (verbose_)
            std::cout << "RSMPCA PRESENT IN IMAGE SUBHEADER " << header_idx << std::endl;
          found = true; // found RSM data
        }
      }
      header_has_RSM = found;
    }
  }
  if (header_ptr->get_property("IXSOFL", ixsofl))
  {
    if (verbose_)
      std::cout << "IXSOFL PRESENT " << ixsofl << std::endl;
  }
  else
  {
    ixsofl = -1;
  }
  return true;
}
// check in overflow
bool
vpgl_nitf_RSM_camera_extractor::determine_overflow_status(vil_nitf2_image * nitf_image,
                                                          size_t header_idx,
                                                          int ixsofl,
                                                          bool & overflow_has_RSM)
{
  if (!nitf_image)
  {
    std::cout << "null nitf image pointer" << std::endl;
    return false;
  }
  if (ixsofl <= 0)
  {
    std::cout << "invalid ixsofl" << std::endl;
    return false;
  }
  vil_nitf2_tagged_record_sequence::const_iterator tres_itr;
  // get the data extension
  vil_nitf2_des * des = (nitf_image->get_des())[ixsofl - 1]; // should agree with subheader index
  if (!des)
  {
    std::cout << "null des" << std::endl;
    return false;
  }
  std::string ovf, ish;
  des->get_property("DESID", ovf);
  if (ovf == "TRE_OVERFLOW") // insure that overflow has occured
  {
    des->get_property("DESOFLW", ish);
    if (ish == "IXSHD")
    { // is IXSHD in overflow?
      // extract the ISXHD data
      des->get_property("DESDATA", ovfl_ixshd_tres_[header_idx]);
      if (ovfl_ixshd_tres_[header_idx].size() > 0)
      { // is there a finite amount of data?
        // iterate through the tres looking for RSM entries
        bool found = false;
        for (tres_itr = ovfl_ixshd_tres_[header_idx].begin(); tres_itr != ovfl_ixshd_tres_[header_idx].end() && !found;
             ++tres_itr)
        {
          std::string type = (*tres_itr)->name();
          if (type == "RSMIDA")
          {
            if (verbose_)
              std::cout << "RSMPCA PRESENT IN OVERFLOW DES: NOT IN IMAGE SUBHEADER " << header_idx << std::endl;
            found = true;
          }
        }
        overflow_has_RSM = found;
      }
      else
      {
        if (verbose_)
          std::cout << "EMPTY IXSHD" << std::endl;
        return false;
      }
    }
    else
    {
      if (verbose_)
        std::cout << "DESDATA Not IXSHD" << std::endl;
      return false;
    }
  }
  else
  {
    if (verbose_)
      std::cout << "ovf not TRE_OVERFLOW " << ovf << std::endl;
    return false;
  }

  return true;
}

// cases:
// 0) No info in image subheader or overflow (abort)
// 1) image subheader TREs IGEOLO, ICHIPB etc only
// 2) image subheader TREs including RSM TREs
// 3) image subheader TREs, RSM TREs in OVERFLOW
bool
vpgl_nitf_RSM_camera_extractor::init(vil_nitf2_image * nitf_image, bool verbose)
{
  std::vector<vil_nitf2_image_subheader *> headers = nitf_image->get_image_headers();
  int ixsofl = -1;
  if (headers.size() == 0)
  {
    std::cout << "no image subheaders present" << std::endl;
    return false;
  }
  // Determine state of nitf headers
  for (unsigned header_idx = 0; header_idx < headers.size(); ++header_idx)
  {
    vil_nitf2_image_subheader * hdr = headers[header_idx];
    if (hdr == std::nullptr_t(0))
      return false;
    bool header_has_tres = false, header_has_RSM = false, overflow_has_RSM = false;
    int ixsofl = -1;
    if (!determine_header_status(hdr, header_idx, header_has_tres, header_has_RSM, ixsofl))
    {
      std::cout << "Image Subheader[" << header_idx << "] analysis failed" << std::endl;
      continue;
    }
    if (header_has_tres && header_has_RSM)
    {
      nitf_status_[header_idx] = IMAGE_SUBHEADER_TREs_RSM_TREs;
      continue;
    }
    if (header_has_tres && ixsofl <= 0)
    {
      nitf_status_[header_idx] = IMAGE_SUBHEADER_TREs_ONLY;
      continue;
    }

    // RSM info may be in the overflow section of the file header
    if (!determine_overflow_status(nitf_image, header_idx, ixsofl, overflow_has_RSM))
    {
      std::cout << "Overflow section analysis failed" << std::endl;
      continue;
    }

    if (header_has_tres && overflow_has_RSM)
      nitf_status_[header_idx] = IMAGE_SUBHEADER_TREs_RSM_TREs_OVRFL;

    if (!header_has_tres && !header_has_RSM && !overflow_has_RSM)
    {
      nitf_status_[header_idx] = INVALID;
    }
  }

  for (unsigned i = 0; i < headers.size(); ++i)
  {
    vil_nitf2_image_subheader * hdr = headers[i];
    if (nitf_status_[i] == INVALID)
      continue;
    // Get standard metadata from the nitf2_image and image subheader
    if (!hdr->get_property("IID1", rsm_meta_[i].image_iid1_))
    {
      if (verbose_)
        std::cout << "IID2 Property not present vil_nitf2_image_subheader\n";
    }
    else
    {
      rsm_meta_[i].image_iid1_valid = true;
      rsm_meta_[i].any_valid = true;
    }

    if (!hdr->get_property("IID2", rsm_meta_[i].image_iid2_))
    {
      if (verbose_)
        std::cout << "IID2 Property not present vil_nitf2_image_subheader\n";
    }
    else
      rsm_meta_[i].image_iid2_valid = true;


    rsm_meta_[i].platform_name_ = hdr->get_image_source();
    rsm_meta_[i].platform_name_valid = true;

    std::string igeolo;
    if (!hdr->get_property("IGEOLO", igeolo))
    {
      if (verbose_)
        std::cout << "IGEOLO Property not specified in vil_nitf2_image_subheader " << i << "\n";
    }else{
      rsm_meta_[i].igeolo_ = igeolo;
      rsm_meta_[i].igeolo_valid = true;
#if 0 // ground geometry extracted from RSM instead (4/18/2025)
      if (!process_igeolo(i))
      {
        std::cout << "process IGEOLO in vil_nitf2_image_subheader " << i << "failed\n";
      }
#endif
    }
    std::vector<int> t(6, 0);
    if (!hdr->get_date_time(t[0], t[1], t[2], t[3], t[4], t[5]))
    {
      std::cout << "get_date_time failed in vil_nitf2_image_subheader\n";
    }
    else
      rsm_meta_[i].acquisition_time_valid = true;
    if (rsm_meta_[i].acquisition_time_valid)
      rsm_meta_[i].acquisition_time_ = t;

    if (verbose_)
      print_file_header_summary();
    // return true;
  }
  return true;
}
vpgl_nitf_RSM_camera_extractor::vpgl_nitf_RSM_camera_extractor(const std::string & nitf_image_path, bool verbose)
  : verbose_(verbose)
{
  // first open the nitf image
  vil_image_resource_sptr image = vil_load_image_resource(nitf_image_path.c_str());
  if (!image)
  {
    std::cout << "Image load failed in vpgl_nitf_RSM_camera_extractor_constructor\n";
    return;
  }
  std::string format = image->file_format();
  std::string prefix = format.substr(0, 4);
  if (prefix != "nitf")
  {
    std::cout << "not a nitf image in vpgl_nitf_RSM_camera_extractor_constructor\n";
    return;
  }
  RSM_defined_ = false;
  // cast to an nitf2_image
  auto * nitf_image = (vil_nitf2_image *)image.ptr();
  // read information
  if (!this->init(nitf_image, verbose_))
    throw std::runtime_error("NITF HEADER EXTRACTION FAILED");
  if (this->nitf_header_contains_RSM_tres().size() > 0)
    RSM_defined_ = true;
  if (RSM_defined_)
  {
    for (auto itr = nitf_status_.begin(); itr != nitf_status_.end(); ++itr)
    {
      if (itr->second != INVALID && itr->second != IMAGE_SUBHEADER_TREs_ONLY)
        RSM_cams_[itr->first] = vpgl_RSM_camera<double>();
    }
  }
}

vpgl_nitf_RSM_camera_extractor::vpgl_nitf_RSM_camera_extractor(vil_nitf2_image * nitf_image, bool verbose)
  : verbose_(verbose)
{
  RSM_defined_ = false;
  if (!this->init(nitf_image, verbose_))
    throw std::runtime_error("NITF HEADER EXTRACTION FAILED");
  if (this->nitf_header_contains_RSM_tres().size() > 0)
    RSM_defined_ = true;
  if (RSM_defined_)
  {
    for (auto itr = nitf_status_.begin(); itr != nitf_status_.end(); ++itr)
    {
      if (itr->second != INVALID && itr->second != IMAGE_SUBHEADER_TREs_ONLY)
        RSM_cams_[itr->first] = vpgl_RSM_camera<double>();
    }
  }
}

bool
vpgl_nitf_RSM_camera_extractor::scan_for_RSM_data(bool verbose)
{
  ss_.clear();
  std::stringstream & tre_str = ss_;
  // Now get the sub-header TRE parameters
  vil_nitf2_tagged_record_sequence::const_iterator tres_itr;
  // Check through the TREs to find ""
  bool v = verbose;
  // first look through the header tres
  for (auto sitr = nitf_status_.begin(); sitr != nitf_status_.end(); ++sitr)
  {
    if (sitr->second == IMAGE_SUBHEADER_TREs_ONLY)
      continue;
    size_t head_idx = sitr->first;
    vil_nitf2_tagged_record_sequence & ixshd_tres = hdr_ixshd_tres_[sitr->first];
    if (sitr->second == IMAGE_SUBHEADER_TREs_RSM_TREs_OVRFL)
      ixshd_tres = ovfl_ixshd_tres_[sitr->first];
    else if (sitr->second == IMAGE_SUBHEADER_TREs_RSM_TREs)
    {
      // avoid self assignment hdr_ixshd_tres_ = hdr_ixshd_tres_;
    }
    else
    {
      tre_str << "INVALID or missing TREs for IMAGE SUBHEADER " << sitr->first << std::endl;
      continue;
    }
    RSMIDA = false;
    RSMPIA = false;
    RSMPCA = false;
    RSMECA = false;
    RSMECB = false;
    RSMGIA = false;
    RSMDCA = false;
    RSMDCB = false;
    RSMAPA = false;
    RSMAPB = false;
    RSMGGA = false;
    tre_str << "======RSM DATA FOR IMAGE SUBHEADER " << sitr->first << std::endl;
    for (tres_itr = ixshd_tres.begin(); tres_itr != ixshd_tres.end(); ++tres_itr)
      {
        std::string type = (*tres_itr)->name();
        if (type == "RSMIDA") // looking for "RSMIDA..."
          {
            if (!tre_str)
              std::cout << "bad stream" << std::endl;
       // Start TRE section =====================
        nitf_tre<std::string> st("RSMIDA", tre_str);
        //=======================================
        rsm_metadata& mdata = rsm_meta_[sitr->first];
        // RSMIDA TREs
        nitf_tre<std::string> nt0("IID", false, true);
        nt0.get_append(tres_itr, tre_str, v);

        nitf_tre<std::string> nt1("EDITION", false, false);
        nt1.get_append(tres_itr, tre_str, v);

        nitf_tre<std::string> nt2("ISID", false, true);
        nt2.get_append(tres_itr, tre_str, v);

        nitf_tre<std::string> nt3("SID", false, true);
        nt3.get_append(tres_itr, tre_str, v);
        if(nt3.get(tres_itr, mdata.sid_))
           mdata.sid_valid = true;

        nitf_tre<std::string> nt4("STID", false, true);
        nt4.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt5("YEAR", false, true);
        nt5.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt6("MONTH", false, true);
        nt6.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt7("DAY", false, true);
        nt7.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt8("HOUR", false, true);
        nt8.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt9("MINUTE", false, true);
        nt9.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt10("SECOND", false, true);
        nt10.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt11("NRG", false, true);
        nt11.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt12("NCG", false, true);
        nt12.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt13("TRG", false, true);
        nt13.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt14("TCG", false, true);
        nt14.get_append(tres_itr, tre_str, v);

        nitf_tre<std::string> nt15("GRNDD", false, false);
        nt15.get_append(tres_itr, tre_str, v);
        mdata.ground_domain_valid = true;
        mdata.ground_domain_ = nt15.value_;
        bool opt = (nt15.value_ == "G") || (nt15.value_ == "H");
        std::string s;
        double xuor, yuor, zuor;
        vnl_matrix_fixed<double, 3, 3> & m = mdata.rotation_;

        nitf_tre<double> nt16("XUOR", opt, false);
        nt16.get_append(tres_itr, tre_str, v);
        nt16.get(tres_itr, xuor);
        
        nitf_tre<double> nt17("YUOR", opt, false);
        nt17.get_append(tres_itr, tre_str, v);
        nt17.get(tres_itr, yuor);
        
        nitf_tre<double> nt18("ZUOR", opt, false);
        nt18.get_append(tres_itr, tre_str, v);
        nt18.get(tres_itr, zuor);
        
        if (!opt)
          {
            mdata.translation_[0] = xuor;
            mdata.translation_[1] = yuor;
            mdata.translation_[2] = zuor;
          }
        nitf_tre<double> nt19("XUXR", opt, false);
        nt19.get_append(tres_itr, tre_str, v);
        if (!opt)
          {
            nt19.get(tres_itr, m[0][0]);
          }

        nitf_tre<double> nt20("XUYR", opt, false);
        nt20.get_append(tres_itr, tre_str, v);
        if (!opt)
          {
            nt20.get(tres_itr, m[1][0]);
          }

        nitf_tre<double> nt21("XUZR", opt, false);
        nt21.get_append(tres_itr, tre_str, v);
        if (!opt)
          {
            nt21.get(tres_itr, m[2][0]);
          }
        nitf_tre<double> nt22("YUXR", opt, false);
        nt22.get_append(tres_itr, tre_str, v);
        if (!opt)
          {
            nt22.get(tres_itr, m[0][1]);
          }
        nitf_tre<double> nt23("YUYR", opt, false);
        nt23.get_append(tres_itr, tre_str, v);
        if (!opt)
          {
            nt23.get(tres_itr, m[1][1]);
          }

        nitf_tre<double> nt24("YUZR", opt, false);
        nt24.get_append(tres_itr, tre_str, v);
        if (!opt)
          {
            nt24.get(tres_itr, m[2][1]);
          }

        nitf_tre<double> nt25("ZUXR", opt, false);
        nt25.get_append(tres_itr, tre_str, v);
        if (!opt)
          {
            nt25.get(tres_itr, m[0][2]);
          }

        nitf_tre<double> nt26("ZUYR", opt, false);
        nt26.get_append(tres_itr, tre_str, v);
        if (!opt)
          {
            nt26.get(tres_itr, m[1][2]);
          }
        
        nitf_tre<double> nt27("ZUZR", opt, false);
        nt27.get_append(tres_itr, tre_str, v);
        if (!opt)
          {
            nt27.get(tres_itr, m[2][2]);
            mdata.local_transform_valid = true;
          }
        
        double x, y, z;
        nitf_tre<double> nt28("V1X", false, false);
        nt28.get_append(tres_itr, tre_str, v);
        nt28.get(tres_itr, x);
        


        nitf_tre<double> nt29("V1Y", false, false);
        nt29.get_append(tres_itr, tre_str, v);
        nt29.get(tres_itr, y);
        

        nitf_tre<double> nt30("V1Z", false, false);
        nt30.get_append(tres_itr, tre_str, v);
        nt30.get(tres_itr, z);
        
        mdata.polytope_[1] = vgl_point_3d<double>(x, y, z);
        
        nitf_tre<double> nt31("V2X", false, false);
        nt31.get_append(tres_itr, tre_str, v);
        nt31.get(tres_itr, x);
        

        nitf_tre<double> nt32("V2Y", false, false);
        nt32.get_append(tres_itr, tre_str, v);
        nt32.get(tres_itr, y);
       

        nitf_tre<double> nt33("V2Z", false, false);
        nt33.get_append(tres_itr, tre_str, v);
        nt33.get(tres_itr, z);
        mdata.polytope_[2] = vgl_point_3d<double>(x, y, z);

        nitf_tre<double> nt34("V3X", false, false);
        nt34.get_append(tres_itr, tre_str, v);
        nt34.get(tres_itr, x);
       
        
        nitf_tre<double> nt35("V3Y", false, false);
        nt35.get_append(tres_itr, tre_str, v);
        nt35.get(tres_itr, y);
        

        nitf_tre<double> nt36("V3Z", false, false);
        nt36.get_append(tres_itr, tre_str, v);
        nt36.get(tres_itr, z);
       
        mdata.polytope_[3] = vgl_point_3d<double>(x, y, z);
        
        nitf_tre<double> nt37("V4X", false, false);
        nt37.get_append(tres_itr, tre_str, v);
        nt37.get(tres_itr, x);
       
        
        nitf_tre<double> nt38("V4Y", false, false);
        nt38.get_append(tres_itr, tre_str, v);
        nt38.get(tres_itr, y);
        
        
        nitf_tre<double> nt39("V4Z", false, false);
        nt39.get_append(tres_itr, tre_str, v);
        nt39.get(tres_itr, z);
       
        mdata.polytope_[4] = vgl_point_3d<double>(x, y, z);
        
        nitf_tre<double> nt40("V5X", false, false);
        nt40.get_append(tres_itr, tre_str, v);
        nt40.get(tres_itr, x);
       
        
        nitf_tre<double> nt41("V5Y", false, false);
        nt41.get_append(tres_itr, tre_str, v);
        nt41.get(tres_itr, y);
        
        
        nitf_tre<double> nt42("V5Z", false, false);
        nt42.get_append(tres_itr, tre_str, v);
        nt42.get(tres_itr, z);
       
        mdata.polytope_[5] = vgl_point_3d<double>(x, y, z);
        
        nitf_tre<double> nt43("V6X", false, false);
        nt43.get_append(tres_itr, tre_str, v);
        nt43.get(tres_itr, x);
     
        
        nitf_tre<double> nt44("V6Y", false, false);
        nt44.get_append(tres_itr, tre_str, v);
        nt44.get(tres_itr, y);
        
        
        nitf_tre<double> nt45("V6Z", false, false);
        nt45.get_append(tres_itr, tre_str, v);
        nt45.get(tres_itr, z);
        
        mdata.polytope_[6] = vgl_point_3d<double>(x, y, z);
        
        nitf_tre<double> nt46("V7X", false, false);
        nt46.get_append(tres_itr, tre_str, v);
        nt46.get(tres_itr, x);
       
                
        nitf_tre<double> nt47("V7Y", false, false);
        nt47.get_append(tres_itr, tre_str, v);
        nt47.get(tres_itr, y);
       
        
        nitf_tre<double> nt48("V7Z", false, false);
        nt48.get_append(tres_itr, tre_str, v);
        nt48.get(tres_itr, z);
        
        mdata.polytope_[7] = vgl_point_3d<double>(x, y, z);
        
        nitf_tre<double> nt49("V8X", false, false);
        nt49.get_append(tres_itr, tre_str, v);
        nt49.get(tres_itr, x);
        
        
        nitf_tre<double> nt50("V8Y", false, false);
        nt50.get_append(tres_itr, tre_str, v);
        nt50.get(tres_itr, y);
     
        nitf_tre<double> nt51("V8Z", false, false);
        nt51.get_append(tres_itr, tre_str, v);
        nt51.get(tres_itr, z);
      
        mdata.polytope_[8] = vgl_point_3d<double>(x, y, z);

        opt = true;

        nitf_tre<double> nt52("GRPX", opt, false);
        nt52.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt53("GRPY", opt, false);
        nt53.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt54("GRPZ", opt, false);
        nt54.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt55("FULLR", opt, true);
        nt55.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt56("FULLC", opt, true);
        nt56.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt57("MINR", false, false);
        nt57.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt58("MAXR", false, false);
        nt58.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt59("MINC", false, false);
        nt59.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt60("MAXC", false, false);
        nt60.get_append(tres_itr, tre_str, v);

        opt = true;
        nitf_tre<double> nt61("IE0", opt, false);
        nt61.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt62("IER", opt, false);
        nt62.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt63("IEC", opt, false);
        nt63.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt64("IERR", opt, false);
        nt64.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt65("IERC", opt, false);
        nt65.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt66("IECC", opt, false);
        nt66.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt67("IA0", opt, false);
        nt67.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt68("IAR", opt, false);
        nt68.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt69("IAC", opt, false);
        nt69.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt70("IARR", opt, false);
        nt70.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt71("IARC", opt, false);
        nt71.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt72("IACC", opt, false);
        nt72.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt73("SPX", opt, false);
        nt73.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt74("SVX", opt, false);
        nt74.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt75("SAX", opt, false);
        nt75.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt76("SPY", opt, false);
        nt76.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt77("SVY", opt, false);
        nt77.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt78("SAY", opt, false);
        nt78.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt79("SPZ", opt, false);
        nt79.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt80("SVZ", opt, false);
        nt80.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt81("SAZ", opt, false);
        nt81.get_append(tres_itr, tre_str, v);

        RSMIDA = true;
      }
      if (type == "RSMPCA")
      { // looking for "RSMPCA..."
        // =======================================
        nitf_tre<std::string> nt82("RSMPCA", tre_str);
        // =========================================
        bool opt = false;
        nitf_tre<std::string> nt83("IID", true, false);
        nt83.get_append(tres_itr, tre_str, v);

        nitf_tre<std::string> nt84("EDITION", false, false);
        nt84.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt85("RSN", false, false);
        nt85.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt86("CSN", false, false);
        nt86.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt87("RFEP", true, false);
        nt87.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt88("CFEP", true, false);
        nt88.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt89("RNRMO", false, false);
        nt89.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt90("CNRMO", false, false);
        nt90.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt91("XNRMO", false, false);
        nt91.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt92("YNRMO", false, false);
        nt92.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt93("ZNRMO", false, false);
        nt93.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt94("RNRMSF", false, false);
        nt94.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt95("CNRMSF", false, false);
        nt95.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt96("XNRMSF", false, false);
        nt96.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt97("YNRMSF", false, false);
        nt97.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt98("ZNRMSF", false, false);
        nt98.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt99("RNPWRX", false, false);
        nt99.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt100("RNPWRY", false, false);
        nt100.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt101("RNPWRZ", false, false);
        nt101.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt102("RNTRMS", false, false);
        nt102.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt103("RNPCF", "vector", false, false);
        nt103.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt104("RDPWRX", false, false);
        nt104.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt105("RDPWRY", false, false);
        nt105.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt106("RDPWRZ", false, false);
        nt106.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt107("RDTRMS", false, false);
        nt107.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt108("RDPCF", "vector", false, false);
        nt108.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt109("CNPWRX", false, false);
        nt109.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt110("CNPWRY", false, false);
        nt110.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt111("CNPWRZ", false, false);
        nt111.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt112("CNTRMS", false, false);
        nt112.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt113("CNPCF", "vector", false, false);
        nt113.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt114("CDPWRX", false, false);
        nt114.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt115("CDPWRY", false, false);
        nt115.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt116("CDPWRZ", false, false);
        nt116.get_append(tres_itr, tre_str, v);

        nitf_tre<int> nt117("CDTRMS", false, false);
        nt117.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt118("CDPCF", "vector", false, false);
        nt118.get_append(tres_itr, tre_str, v);
        RSMPCA = true;
      }
    }
    for (tres_itr = ixshd_tres.begin(); tres_itr != ixshd_tres.end(); ++tres_itr)
    {
      std::string type = (*tres_itr)->name();

      if (type == "RSMPIA")
      { // looking for "RSMPIA..."
        // =======================================
        nitf_tre<std::string> nt("RSMPIA", tre_str);
        // =========================================
        rsm_metadata & meta = rsm_meta_[sitr->first];
        nitf_tre<std::string> nt1("EDITION", false, false);
        nt1.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt2("R0", false, false);
        nt2.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt3("RX", false, false);
        nt3.get_append(tres_itr, tre_str, v);
          
        nitf_tre<double> nt4("RY", false, false);
        nt4.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt5("RZ", false, false);
        nt5.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt6("RXX", false, false);
        nt6.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt7("RXY", false, false);
        nt7.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt8("RXZ", false, false);
        nt8.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt9("RYY", false, false);
        nt9.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt10("RYZ", false, false);
        nt10.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt11("RZZ", false, false);
        nt11.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt12("C0", false, false);
        nt12.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt13("CX", false, false);
        nt13.get_append(tres_itr, tre_str, v);

        nitf_tre<double> nt14("CY", false, false);
        nt14.get_append(tres_itr, tre_str, v);
        
        nitf_tre<double> nt15("CZ", false, false);
        nt15.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt16("CXX", false, false);
        nt16.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt17("CXY", false, false);
        nt17.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt18("CXZ", false, false);
        nt18.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt19("CYY", false, false);
        nt19.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt20("CYZ", false, false);
        nt20.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt21("CZZ", false, false);
        nt21.get_append(tres_itr, tre_str, v);
        nitf_tre<std::string> nt22("RNIS", false, false);
        nt22.get_append(tres_itr, tre_str, v);
        nitf_tre<std::string> nt23("CNIS", false, false);
        nt23.get_append(tres_itr, tre_str, v);
        nitf_tre<std::string> nt24("TNIS", false, false);
        nt24.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt25("RSSIZ", false, false);
        nt25.get_append(tres_itr, tre_str, v);
        nitf_tre<double> nt26("CSSIZ", false, false);
        nt26.get_append(tres_itr, tre_str, v);
        RSMPIA = true;
      }

      if (type == "RSMGIA")
      { // looking for "RSMGIA..."
        // =======================================
        nitf_tre<std::string> nt("RSMGIA", tre_str);
        // =========================================
        nitf_tre<std::string> nt1("EDITION", false, false);
        nt1.get_append(tres_itr, tre_str, v);
        RSMGIA = true;
      }
      if (type == "RSMDCA")
      { // looking for "RSMDCA..."
        // =======================================
        nitf_tre<std::string> nt("RSMDCA", tre_str);
        // =========================================
        nitf_tre<std::string> nt1("EDITION", false, false);
        nt1.get_append(tres_itr, tre_str, v);
        RSMDCA = true;
      }

      if (type == "RSMDCB")
      { // looking for "RSMDCB..."
        // =======================================
        nitf_tre<std::string> nt("RSMDCB", tre_str);
        // =========================================
        nitf_tre<std::string> nt1("EDITION", false, false);
        nt1.get_append(tres_itr, tre_str, v);
        RSMDCB = true;
      }
      if (type == "RSMECA")
        { // looking for "RSMECA..."
          // =======================================
          nitf_tre<std::string> nt("RSMECA", tre_str);
          // =========================================
          RSM_ECA_adjustable_parameter_metadata & apdata = RSM_ECA_adj_param_data_[head_idx];
          
          nitf_tre<std::string> nt0("IID", false, false);
          nt0.get_append(tres_itr, tre_str, v);
          
          nitf_tre<std::string> nt1("EDITION", false, false);
          nt1.get_append(tres_itr, tre_str, v);
          
          nitf_tre<std::string> nt2("TID", false, false);
          nt2.get_append(tres_itr, tre_str, v);
          
          nitf_tre<std::string> nt3("INCLIC", false, false);
          nt3.get_append(tres_itr, tre_str, v);
          bool opt = (nt3.value_ != "Y");
          
          nitf_tre<std::string> nt4("INCLUC", true, false);
          nt4.get_append(tres_itr, tre_str, v);
          bool UCreq = nt4.value_ == "Y";
          
          nitf_tre<int> nt5("NPAR", opt, false);
          nt5.get_append(tres_itr, tre_str, v);
          int npar = 0;
          nt5.get(tres_itr, npar);
          
          nitf_tre<int> nt6("NPARO", opt, false);
          nt6.get_append(tres_itr, tre_str, v);
          int nparo = 0;
          nt6.get(tres_itr, nparo);
          
          nitf_tre<int> nt7("IGN", opt, false);
          nt7.get_append(tres_itr, tre_str, v);
          int n_indp = 0;
          
          
          if (!opt)
            {
              nt7.get(tres_itr, n_indp);
              apdata.num_adj_params_ = npar;
              apdata.num_original_adj_params_ = nparo;
              apdata.num_independent_subgroups_ = n_indp;
              apdata.defined_ = true;
            }
          
          nitf_tre<std::string> nt8("CVDATE", opt, false);
          nt8.get_append(tres_itr, tre_str, v);
          
          double xuol, yuol, zuol;
          std::string s;
          nitf_tre<std::string> nt9("XUOL", opt, false);
          nt9.get_append(tres_itr, tre_str, v);
          nt9.get(tres_itr, s);
          ASC_double(s, xuol);
          
          nitf_tre<std::string> nt10("YUOL", opt, false);
          nt10.get_append(tres_itr, tre_str, v);
          nt10.get(tres_itr, s);
          ASC_double(s, yuol);
          
          nitf_tre<std::string> nt11("ZUOL", opt, false);
          nt11.get_append(tres_itr, tre_str, v);
          nt11.get(tres_itr, s);
          ASC_double(s, zuol);
          if (!opt)
            {
              apdata.translation_[0] = xuol;
              apdata.translation_[1] = yuol;
              apdata.translation_[2] = zuol;
            }
          vnl_matrix_fixed<double, 3, 3> & m = apdata.rotation_;
          
          nitf_tre<std::string> nt12("XUXL", opt, false);
          nt12.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt12.get(tres_itr, s);
              ASC_double(s, m[0][0]);
            }
          
          nitf_tre<std::string> nt13("XUYL", opt, false);
          nt13.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt13.get(tres_itr, s);
              ASC_double(s, m[1][0]);
            }
          
          nitf_tre<std::string> nt14("XUZL", opt, false);
          nt14.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt14.get(tres_itr, s);
              ASC_double(s, m[2][0]);
            }
          
          nitf_tre<std::string> nt15("YUXL", opt, false);
          nt15.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt15.get(tres_itr, s);
              ASC_double(s, m[0][1]);
            }
          
          nitf_tre<std::string> nt16("YUYL", opt, false);
          nt16.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt16.get(tres_itr, s);
              ASC_double(s, m[1][1]);
            }
          
          nitf_tre<std::string> nt17("YUZL", opt, false);
          nt17.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt17.get(tres_itr, s);
              ASC_double(s, m[2][1]);
            }
          
          nitf_tre<std::string> nt18("ZUXL", opt, false);
          nt18.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt18.get(tres_itr, s);
              ASC_double(s, m[0][2]);
            }
          
          nitf_tre<std::string> nt19("ZUYL", opt, false);
          nt19.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt19.get(tres_itr, s);
              ASC_double(s, m[1][2]);
            }
          
          nitf_tre<std::string> nt19a("ZUZL", opt, false);
          nt19a.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt19a.get(tres_itr, s);
              ASC_double(s, m[2][2]);
            }
          
          std::map<std::string, int> & idx = apdata.covar_index_;
          
          nitf_tre<std::string> nt20("IRO", opt, false);
          nt20.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt20.get(tres_itr, s);
              ASC_int(s, idx["IRO"]);
            }
          
          nitf_tre<std::string> nt21("IRX", opt, false);
          nt21.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt21.get(tres_itr, s);
              ASC_int(s, idx["IRX"]);
            }
          
          nitf_tre<std::string> nt22("IRY", opt, false);
          nt22.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt22.get(tres_itr, s);
              ASC_int(s, idx["IRY"]);
            }
          
          nitf_tre<std::string> nt23("IRZ", opt, false);
          nt23.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt23.get(tres_itr, s);
              ASC_int(s, idx["IRZ"]);
            }
          
          nitf_tre<std::string> nt24("IRXX", opt, false);
          nt24.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt24.get(tres_itr, s);
              ASC_int(s, idx["IRXX"]);
            }
          
          nitf_tre<std::string> nt25("IRXY", opt, false);
          nt25.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt25.get(tres_itr, s);
              ASC_int(s, idx["IRXY"]);
            }
          
          nitf_tre<std::string> nt26("IRXZ", opt, false);
          nt26.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt26.get(tres_itr, s);
              ASC_int(s, idx["IRXZ"]);
            }
          
          nitf_tre<std::string> nt27("IRYY", opt, false);
          nt27.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt27.get(tres_itr, s);
              ASC_int(s, idx["IRYY"]);
            }
          
          nitf_tre<std::string> nt28("IRYZ", opt, false);
          nt28.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt28.get(tres_itr, s);
              ASC_int(s, idx["IRYZ"]);
            }
          
          nitf_tre<std::string> nt29("IRZZ", opt, false);
          nt29.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt29.get(tres_itr, s);
              ASC_int(s, idx["IRZZ"]);
            }
          
          nitf_tre<std::string> nt30("ICO", opt, false);
          nt30.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt30.get(tres_itr, s);
              ASC_int(s, idx["ICO"]);
            }
          
          nitf_tre<std::string> nt31("ICX", opt, false);
          nt31.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt31.get(tres_itr, s);
              ASC_int(s, idx["ICX"]);
            }
          
          nitf_tre<std::string> nt32("ICY", opt, false);
          nt32.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt32.get(tres_itr, s);
              ASC_int(s, idx["ICY"]);
            }
          
          nitf_tre<std::string> nt33("ICZ", opt, false);
          nt33.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt33.get(tres_itr, s);
              ASC_int(s, idx["ICZ"]);
            }
          
          nitf_tre<std::string> nt34("ICXX", opt, false);
          nt34.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt34.get(tres_itr, s);
              ASC_int(s, idx["ICXX"]);
            }
          
          nitf_tre<std::string> nt35("ICXY", opt, false);
          nt35.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt35.get(tres_itr, s);
              ASC_int(s, idx["ICXY"]);
            }
          
          nitf_tre<std::string> nt36("ICXZ", opt, false);
          nt36.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt36.get(tres_itr, s);
              ASC_int(s, idx["ICXZ"]);
            }
          
          nitf_tre<std::string> nt37("ICYY", opt, false);
          nt37.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt37.get(tres_itr, s);
              ASC_int(s, idx["ICYY"]);
            }
          
          nitf_tre<std::string> nt38("ICYZ", opt, false);
          nt38.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt38.get(tres_itr, s);
              ASC_int(s, idx["ICYZ"]);
            }
          
          nitf_tre<std::string> nt39("ICZZ", opt, false);
          nt39.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt39.get(tres_itr, s);
              ASC_int(s, idx["ICZZ"]);
            }
          
          nitf_tre<std::string> nt40("GXO", opt, false);
          nt40.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt40.get(tres_itr, s);
              ASC_int(s, idx["GXO"]);
            }
          
          nitf_tre<std::string> nt41("GYO", opt, false);
          nt41.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt41.get(tres_itr, s);
              ASC_int(s, idx["GYO"]);
            }
          
          nitf_tre<std::string> nt42("GZO", opt, false);
          nt42.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt42.get(tres_itr, s);
              ASC_int(s, idx["GZO"]);
            }
          
          nitf_tre<std::string> nt43("GXR", opt, false);
          nt43.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt43.get(tres_itr, s);
              ASC_int(s, idx["GXR"]);
            }
          
          nitf_tre<std::string> nt44("GYR", opt, false);
          nt44.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt44.get(tres_itr, s);
              ASC_int(s, idx["GYR"]);
            }
          
          nitf_tre<std::string> nt45("GZR", opt, false);
          nt45.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt45.get(tres_itr, s);
              ASC_int(s, idx["GZR"]);
            }
          
          nitf_tre<std::string> nt46("GS", opt, false);
          nt46.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt46.get(tres_itr, s);
              ASC_int(s, idx["GS"]);
            }
          
          nitf_tre<std::string> nt47("GXX", opt, false);
          nt47.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt47.get(tres_itr, s);
              ASC_int(s, idx["GXX"]);
            }
          
          nitf_tre<std::string> nt48("GXY", opt, false);
          nt48.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt48.get(tres_itr, s);
              ASC_int(s, idx["GXY"]);
            }
          
          nitf_tre<std::string> nt49("GXZ", opt, false);
          nt49.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt49.get(tres_itr, s);
              ASC_int(s, idx["GXZ"]);
            }
          
          nitf_tre<std::string> nt50("GYX", opt, false);
          nt50.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt50.get(tres_itr, s);
              ASC_int(s, idx["GYX"]);
            }
          
          nitf_tre<std::string> nt51("GYY", opt, false);
          nt51.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt51.get(tres_itr, s);
              ASC_int(s, idx["GYY"]);
            }
          
          nitf_tre<std::string> nt52("GYZ", opt, false);
          nt52.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt52.get(tres_itr, s);
              ASC_int(s, idx["GYZ"]);
            }
          
          nitf_tre<std::string> nt53("GZX", opt, false);
          nt53.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt53.get(tres_itr, s);
              ASC_int(s, idx["GZX"]);
            }
          
          nitf_tre<std::string> nt54("GZY", opt, false);
          nt54.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt54.get(tres_itr, s);
              ASC_int(s, idx["GZY"]);
            }
          
          nitf_tre<std::string> nt55("GZZ", opt, false);
          nt55.get_append(tres_itr, tre_str, v);
          if (!opt)
            {
              nt55.get(tres_itr, s);
              ASC_int(s, idx["GZZ"]);
            }
          
          // the following are segregated into independent subgroups
          nitf_tre<int> nt56("NUMOPG", "vector", opt, false);
          std::vector<int> numopg;
          nt56.get(tres_itr, numopg);
          
          nitf_tre<double> nt57("ERRCVG", "vector", opt, false);
          std::vector<double> errcvg;
          nt57.get(tres_itr, errcvg);
          
          nitf_tre<int> nt58("TCDF", "vector", opt, false);
          std::vector<int> tcdf;
          nt58.get(tres_itr, tcdf);
          
          nitf_tre<int> nt59("NCSEG", "vector", opt, false);
          std::vector<int> ncseg;
          nt59.get(tres_itr, ncseg);
          
          nitf_tre<double> nt60("CORSEG", "vector", opt, false);
          std::vector<double> corseg;
          nt60.get(tres_itr, corseg);
          
          nitf_tre<double> nt61("TAUSEG", "vector", opt, false);
          std::vector<double> tauseg;
          nt61.get(tres_itr, tauseg);
          //  end of subgroups
          // export subgroup info.
          if (opt)
            n_indp = 0;
          std::vector<vnl_matrix<double>> & cov = apdata.independent_subgroup_covariance_;
          cov.resize(n_indp);
          std::vector<size_t> & flgs = apdata.correlation_flags_;
          flgs.resize(n_indp);
          std::vector<std::vector<std::pair<double, double>>> & corsegs = apdata.correlation_segments_;
          corsegs.resize(n_indp);
          
          tre_str << " === indepenent subgroups ===" << std::endl;
          int offset = 0, corr_offset = 0;
          for (size_t i = 0; i < n_indp; ++i)
            {
              int n_vars = numopg[i];
              tre_str << "Ind.Group Id    Adj. Variable Size" << std::endl;
              tre_str << i << ' ' << n_vars << std::endl;
              size_t n_upper_diag = n_vars * (n_vars + 1) / 2;
              std::vector<double> err(n_upper_diag);
              for (size_t k = 0; k < n_upper_diag; ++k)
                err[k] = errcvg[offset + k];
              vnl_matrix<double> m(n_vars, n_vars, 0.0);
              // fill independent covar block
              size_t k = 0;
              for (size_t r = 0; r < n_vars; ++r)
                for (size_t c = r; c < n_vars; ++c, ++k)
                  {
                    m[r][c] = err[k];
                    if (r != c)
                      m[c][r] = m[r][c];
                  }
              cov[i] = m;
              tre_str << "independent correlation block" << std::endl;
              tre_str << m << std::endl;
              tre_str << "time correlation flag " << tcdf[i] << std::endl;
              tre_str << "time correlation params" << std::endl;
              std::vector<std::pair<double, double>> segments;
              for (size_t t = corr_offset; t < (corr_offset + ncseg[i]); ++t)
                {
                  tre_str << "corseg " << corseg[t] << " tauseg " << tauseg[t] << std::endl;
                  std::pair<double, double> pr(corseg[t], tauseg[t]);
                  segments.push_back(pr);
                }
              corsegs[i] = segments;
              offset += n_upper_diag;
              corr_offset += ncseg[i];
            }
          nitf_tre<double> nt62("MAP", "vector", opt, false);
          std::vector<double> map;
          if (!opt)
            {
              nt62.get(tres_itr, map);
              
              vnl_matrix<double> phi(npar, nparo, 0.0);
              size_t k = 0;
              for (size_t r = 0; r < npar; ++r)
                for (size_t c = 0; c < nparo; ++c, ++k)
                  {
                    phi[r][c] = map[k];
                  }
              tre_str << "mapping matrix, phi" << std::endl;
              tre_str << phi << std::endl;
              apdata.phi_ = phi;
            }
          opt = !UCreq;
          double urr;
          nitf_tre<std::string> nt63("URR",  opt, false);
          nt63.get_append(tres_itr, tre_str, v);
          if (!opt) {
            std::string urrs;
            nt63.get(tres_itr, urrs);
            ASC_double(urrs, urr);
          }
          double ucc;
          nitf_tre<std::string> nt64("UCC", opt, false);
          nt64.get_append(tres_itr, tre_str, v);
          if (!opt) {
            std::string uccs;
            nt64.get(tres_itr, uccs);
            ASC_double(uccs, ucc);
          }
          double urc;
          nitf_tre<std::string> nt65("URC", opt, false);
          nt65.get_append(tres_itr, tre_str, v);
          if (!opt) {
            std::string urcs;
            nt65.get(tres_itr, urcs);
            ASC_double(urcs, urc);
          }
          if (!opt) {
            apdata.unmodeled_error_ = true;
            apdata.unmodeled_row_variance_ = urr;
            apdata.unmodeled_col_variance_ = ucc;
            apdata.unmodeled_row_col_variance_ = urc;
          }
          int n_row_seg_u = 0 ;
          nitf_tre<int> nt66("UNCSR", opt, false);
          nt66.get_append(tres_itr, tre_str, v);
          if (!opt) {
            nt66.get(tres_itr, n_row_seg_u);
          }
          std::vector<double> ucorsr;
          std::vector<std::string> ucorsrs;
          nitf_tre<std::string> nt67("UCORSR", "vector", opt, false);
          nt67.get_append(tres_itr, tre_str, v);
          if(!opt){
            nt67.get(tres_itr, ucorsrs);
            for (size_t r = 0; r < ucorsrs.size(); ++r) {
              double val;
              ASC_double(ucorsrs[r], val);
              ucorsr.push_back(val);
            }
          }
          std::vector<double> utausr;
          std::vector<std::string> utausrs;
          nitf_tre<std::string> nt68("UTAUSR", "vector", opt, false);
          nt68.get_append(tres_itr, tre_str, v);
          if (!opt) {
            nt68.get(tres_itr, utausrs);
            for (size_t r = 0; r < utausrs.size(); ++r) {
              double val;
              ASC_double(utausrs[r], val);
              utausr.push_back(val);
            }
          }
          int n_col_seg_u = 0 ;
          nitf_tre<int> nt69("UNCSC", opt, false);
          nt69.get_append(tres_itr, tre_str, v);
          if (!opt) {
            nt69.get(tres_itr, n_col_seg_u);
          }
          std::vector<double> ucorsc;
          std::vector<std::string> ucorscs;
          nitf_tre<std::string> nt70("UCORSC", "vector", opt, false);
          nt70.get_append(tres_itr, tre_str, v);
          if(!opt){
            nt70.get(tres_itr, ucorscs);
            for (size_t c = 0; c < ucorscs.size(); ++c) {
              double val;
              ASC_double(ucorscs[c], val);
              ucorsc.push_back(val);
            }
          }
          std::vector<double> utausc;
          std::vector<std::string> utauscs;
          nitf_tre<std::string> nt71("UTAUSC", "vector", opt, false);
          nt71.get_append(tres_itr, tre_str, v);
          if (!opt) {
            nt71.get(tres_itr, utauscs);
            for (size_t c = 0; c < utauscs.size(); ++c) {
              double val;
              ASC_double(utauscs[c], val);
              utausc.push_back(val);
            }
          }
          if (!opt) {
            std::vector < std::pair<double, double> > temp_r, temp_c;
            for (size_t s = 0; s < n_row_seg_u; ++s)
              temp_r.emplace_back(ucorsr[s], utausr[s]);
            for (size_t s = 0; s < n_col_seg_u; ++s)
              temp_c.emplace_back(ucorsc[s], utausc[s]);
            
            std::tuple < size_t, std::vector<std::pair<double, double> > > row_func(n_row_seg_u, temp_r);
            std::tuple < size_t, std::vector<std::pair<double, double> > > col_func(n_col_seg_u, temp_c);
            apdata.unmodeled_row_correlation_ = row_func;
            apdata.unmodeled_col_correlation_ = col_func;
          }
          RSMECA = true;
        }

      if (type == "RSMECB")
      { // looking for "RSMDCB..."
        // =======================================
        nitf_tre<std::string> nt("RSMECB", tre_str);
        // =========================================
        RSM_ECB_adjustable_parameter_metadata & apdata = RSM_ECB_adj_param_data_[head_idx];
        nitf_tre<std::string> nt0("IID", false, false);
        nt0.get_append(tres_itr, tre_str, v);

        nitf_tre<std::string> nt1("EDITION", false, false);
        nt1.get_append(tres_itr, tre_str, v);

        nitf_tre<std::string> nt2("TID", false, false);
        nt2.get_append(tres_itr, tre_str, v);

        nitf_tre<std::string> nt3("INCLIC", false, false);
        nt3.get_append(tres_itr, tre_str, v);
        // Indirect Error Covariance Values Required
        bool ICreq = (nt3.value_ == "Y");

        nitf_tre<std::string> nt4("INCLUC", false, false);
        nt4.get_append(tres_itr, tre_str, v);
        // Unmodeled Indirect Error Covariance Values Required
        bool UCreq = (nt4.value_ == "Y");
        apdata.unmodeled_error_ = UCreq;

        // not optional for IC required values, i.e. optional = false;
        bool opt = !ICreq;
        int nparob = 0;
        nitf_tre<int> nt5("NPARO", opt, false);
        nt5.get_append(tres_itr, tre_str, v);
        nt5.get(tres_itr, nparob);
        apdata.num_original_adj_params_ = nparob;

        int n_indpb = 0;
        nitf_tre<int> nt5a("IGN", opt, false);
        nt5a.get_append(tres_itr, tre_str, v);
        if (!opt) {
            nt5a.get(tres_itr, n_indpb);
            apdata.num_independent_subgroups_ = n_indpb;
        }
        
        nitf_tre<std::string> nt6("CVDATE", opt, false);
        nt6.get_append(tres_itr, tre_str, v);
        
        int npar = 0;
        nitf_tre<int> nt7("NPAR", opt, false);
        nt7.get_append(tres_itr, tre_str, v);
        if(!opt){
          npar = nt7.value_;
          apdata.num_active_adj_params_ = npar;   
        }
        nitf_tre<std::string> nt8("APTYP", opt, false);
        nt8.get_append(tres_itr, tre_str, v);
        bool Iadj = (nt8.value_ == "I");
        bool Gadj = (nt8.value_ == "G");
        if(!opt){
          apdata.image_adjustable_params_  = Iadj;
          apdata.ground_adjustable_params_ = Gadj;
        }      
        nitf_tre<std::string> nt9("LOCTYP", opt, false);
        nt9.get_append(tres_itr, tre_str, v);
        if(!opt)
          apdata.rect_local_coordinate_system_ = (nt9.value_ == "R");

        // the scale offsets apply to both R and not R coordinate systems
        double noffx, noffy, noffz, nsfx, nsfy, nsfz;
        nitf_tre<std::string> nt10("NSFX", opt, false);
        nt10.get_append(tres_itr, tre_str, v);
        if(!opt)
          ASC_double(nt10.value_, nsfx);
          
        nitf_tre<std::string> nt11("NSFY", opt, false);
        nt11.get_append(tres_itr, tre_str, v);
        if(!opt)
          ASC_double(nt11.value_, nsfy);
        
        nitf_tre<std::string> nt12("NSFZ", opt, false);
        nt12.get_append(tres_itr, tre_str, v);
        if(!opt)
          ASC_double(nt12.value_, nsfz);

        nitf_tre<std::string> nt13("NOFFX", opt, false);
        nt13.get_append(tres_itr, tre_str, v);
        if(!opt)
          ASC_double(nt13.value_, noffx);

        nitf_tre<std::string> nt14("NOFFY", opt, false);
        nt14.get_append(tres_itr, tre_str, v);
        if(!opt)
          ASC_double(nt14.value_, noffy);

        nitf_tre<std::string> nt15("NOFFZ", opt, false);
        nt15.get_append(tres_itr, tre_str, v);
        if(!opt){
          ASC_double(nt14.value_, noffz);
          apdata.xyz_norm_[0] = std::pair<double, double> (noffx, nsfx);
          apdata.xyz_norm_[1] = std::pair<double, double>(noffy, nsfy);
          apdata.xyz_norm_[2] = std::pair<double, double>(noffz, nsfz);
        }
        bool optR = (nt9.value_ != "R");
        opt = optR || !ICreq;
        double xuol = 0, yuol = 0 , zuol = 0;
        std::string s;
        nitf_tre<std::string> nt16("XUOL", opt, false);
        nt16.get_append(tres_itr, tre_str, v);
        nt16.get(tres_itr, s);
        if(!opt)
          ASC_double(s, xuol);

        nitf_tre<std::string> nt17("YUOL", opt, false);
        nt17.get_append(tres_itr, tre_str, v);
        nt17.get(tres_itr, s);
        if(!opt)
          ASC_double(s, yuol);

        nitf_tre<std::string> nt18("ZUOL", opt, false);
        nt18.get_append(tres_itr, tre_str, v);
        nt18.get(tres_itr, s);
        if(!opt){
          ASC_double(s, zuol);
          apdata.rect_translation_[0]=xuol;
          apdata.rect_translation_[1] = yuol;
          apdata.rect_translation_[2] = zuol;
        }
        vnl_matrix_fixed<double, 3, 3> m ;

        nitf_tre<std::string> nt19("XUXL", opt, false);
        nt19.get_append(tres_itr, tre_str, v);
        if (!opt)
        {
            nt19.get(tres_itr, s);
            ASC_double(s, m[0][0]);
        }

        nitf_tre<std::string> nt20("XUYL", opt, false);
        nt13.get_append(tres_itr, tre_str, v);
        if (!opt)
        {
            nt20.get(tres_itr, s);
            ASC_double(s, m[1][0]);
        }

        nitf_tre<std::string> nt21("XUZL", opt, false);
        nt14.get_append(tres_itr, tre_str, v);
        if (!opt)
        {
            nt21.get(tres_itr, s);
            ASC_double(s, m[2][0]);
        }

        nitf_tre<std::string> nt22("YUXL", opt, false);
        nt15.get_append(tres_itr, tre_str, v);
        if (!opt)
        {
            nt22.get(tres_itr, s);
            ASC_double(s, m[0][1]);
        }

        nitf_tre<std::string> nt23("YUYL", opt, false);
        nt16.get_append(tres_itr, tre_str, v);
        if (!opt)
        {
            nt23.get(tres_itr, s);
            ASC_double(s, m[1][1]);
        }

        nitf_tre<std::string> nt24("YUZL", opt, false);
        nt17.get_append(tres_itr, tre_str, v);
        if (!opt)
        {
            nt24.get(tres_itr, s);
            ASC_double(s, m[2][1]);
        }

        nitf_tre<std::string> nt25("ZUXL", opt, false);
        nt18.get_append(tres_itr, tre_str, v);
        if (!opt)
        {
            nt25.get(tres_itr, s);
            ASC_double(s, m[0][2]);
        }

        nitf_tre<std::string> nt26("ZUYL", opt, false);
        nt19.get_append(tres_itr, tre_str, v);
        if (!opt)
        {
            nt26.get(tres_itr, s);
            ASC_double(s, m[1][2]);
        }

        nitf_tre<std::string> nt27("ZUZL", opt, false);
        nt27.get_append(tres_itr, tre_str, v);
        if (!opt)
        {
            nt27.get(tres_itr, s);
            ASC_double(s, m[2][2]);
        }
        if(!opt)
        apdata.rect_rotation_ = m;

        nitf_tre<std::string> nt28("APBASE", !Iadj, false);
        nt28.get_append(tres_itr, tre_str, v);
        bool Breq = nt28.value_ == "Y";
        if(Iadj)
          apdata.basis_option_ = Breq;

        opt = !Iadj || !ICreq;
        nitf_tre<int> nt29("NISAP", opt, false);
        nt29.get_append(tres_itr, tre_str, v);
        if (!opt) {
            int nipar= nt29.value_;
            apdata.n_image_adjustable_params_ = nipar;
        }

        int nrowp =0, ncolp =0;
        nitf_tre<int> nt30("NISAPR", opt, false);
        nt30.get_append(tres_itr, tre_str, v);
        if (!opt) {
            nt30.get(tres_itr,nrowp);
            apdata.n_image_row_adjustable_params_ = nrowp;
        }
        std::vector<int> xpr, ypr, zpr;        
        nitf_tre<int> nt31("XPWRR", "vector", opt, false);
        nt31.get_append(tres_itr, tre_str, v);
        if (!opt) {
          nt31.get(tres_itr, xpr);
        }
        nitf_tre<int> nt32("YPWRR", "vector", opt, false);
        nt32.get_append(tres_itr, tre_str, v);
        if (!opt) {
          nt32.get(tres_itr, ypr);
        }
        nitf_tre<int> nt33("ZPWRR", "vector", opt, false);
        nt33.get_append(tres_itr, tre_str, v);
        if (!opt) {
          nt33.get(tres_itr, zpr);
        }

        if(!opt){
          for (size_t r = 0; r < nrowp; ++r) {
            std::tuple<size_t, size_t, size_t> pows(xpr[r], ypr[r], zpr[r]);
            apdata.image_row_xyz_powers_[r] = pows;
          }
        }

        nitf_tre<int> nt34("NISAPC", opt, false);
        nt34.get_append(tres_itr, tre_str, v);
        if (!opt) {
            nt34.get(tres_itr, ncolp);
            apdata.n_image_col_adjustable_params_ = ncolp;
        }

        std::vector<int> xpc, ypc, zpc;        
        nitf_tre<int> nt35("XPWRC", "vector", opt, false);
        nt35.get_append(tres_itr, tre_str, v);
        if (!opt) {
          nt35.get(tres_itr, xpc);
        }
        nitf_tre<int> nt36("YPWRC", "vector", opt, false);
        nt36.get_append(tres_itr, tre_str, v);
        if (!opt) {
          nt36.get(tres_itr, ypc);
        }
        nitf_tre<int> nt37("ZPWRC", "vector", opt, false);
        nt37.get_append(tres_itr, tre_str, v);
        if (!opt) {
          nt37.get(tres_itr, zpc);
        }

        if(!opt){
          for (size_t c = 0; c < ncolp; ++c) {
            std::tuple<size_t, size_t, size_t> pows(xpc[c], ypc[c], zpc[c]);
            apdata.image_col_xyz_powers_[c] = pows;
          }
        }

        // ground adjustable parameters
        opt = !Gadj || !ICreq;
        int n_ground = 0;
        nitf_tre<int> nt38("NGSAP", opt, false);
        nt38.get_append(tres_itr, tre_str, v);
        if (!opt) {
            n_ground = nt38.value_;
            apdata.n_ground_adjustable_params_ = n_ground;
        }
        std::vector<int> gsids;
        nitf_tre<int> nt39("GSAPID", opt, false);
        nt39.get_append(tres_itr, tre_str, v);
        if(!opt){
          nt39.get(tres_itr, gsids);
          if (gsids.size() != n_ground)
            return false;
          for(size_t i = 0; i<n_ground; ++i)
            apdata.ground_adjust_param_idx_[i] = gsids[i];
        }
        opt = !ICreq || !Breq;
        int n_basis = 0;
        nitf_tre<int> nt40("NBASIS", opt, false);
        nt40.get_append(tres_itr, tre_str, v);
        if(!opt){
          n_basis = nt40.value_;
          apdata.n_basis_adjustable_params_ = n_basis;
          apdata.A_matrix_.set_size(npar, n_basis);
        }
        
        std::vector<std::string> Avals;
        nitf_tre<std::string> nt41("AEL", "vector", opt, false);
        nt41.get_append(tres_itr, tre_str, v);
        if(!opt){
            nt41.get(tres_itr, Avals);
          int idx = 0;
          for(size_t r = 0; r<npar; ++r)
            for(size_t c = 0; c<n_basis; ++c){
              double val;
              ASC_double(Avals[idx], val);
              apdata.A_matrix_[r][c] = val;
              idx++;
            }
        }

        opt = !ICreq;
        std::vector<int> n_opg;
        nitf_tre<int> nt42("NUMOPG", "vector",opt, false);
        nt42.get_append(tres_itr, tre_str, v);
        if(!opt){
            nt42.get(tres_itr, n_opg);
            }
        

        std::vector<std::string> errcvg_svals;
        nitf_tre<std::string> nt43("ERRCVG", "vector",opt, false);
        nt43.get_append(tres_itr, tre_str, v);
        if(!opt){
          nt43.get(tres_itr, errcvg_svals);
          size_t idx = 0;
          for (size_t ig = 0; ig < n_indpb; ++ig) {
              size_t covar_dim = n_opg[ig];
              vnl_matrix<double> cvar(covar_dim, covar_dim, 0.0);
              for (size_t r = 0; r < covar_dim; ++r)
                for( size_t c = r; c< covar_dim; ++c){
                  double val;
                  ASC_double(errcvg_svals[idx], val);
                  cvar[r][c] = val;
                  if (r != c)
                      cvar[c][r] = val;
                  idx++;
                }//r,c
              apdata.independent_covar_[ig]=cvar;
          }// ig
        }//opt

        
        std::vector<int> domain_flags;
        nitf_tre<int> nt44("TCDF", "vector", opt, false);
        nt44.get_append(tres_itr, tre_str, v);
        if (!opt) {
            nt44.get(tres_itr, domain_flags);
            for (size_t ig = 0; ig < n_indpb; ++ig)
             apdata.correlation_domain_flags_[ig] = domain_flags[ig];
        }

        std::vector<std::string> acsmc;
        std::vector<bool> use_function;
        nitf_tre<std::string> nt45("ACSMC", "vector", opt, false);
        nt45.get_append(tres_itr, tre_str, v);
        if (!opt) {
            nt45.get(tres_itr, acsmc);
            use_function.resize(n_indpb);
            for (size_t ig = 0; ig < n_indpb; ++ig)
                use_function[ig] = (acsmc[ig] == "Y");
        }

        
        std::vector<int> ncsegb;
        nitf_tre<int> nt46("NCSEG", "vector", opt, false);
        nt46.get_append(tres_itr, tre_str, v);
        if (!opt) {
            nt46.get(tres_itr, ncsegb);
        }

        std::vector<std::string> corsegs;
        std::vector<double> corseg_vals;
        nitf_tre<std::string> nt47("CORSEG", "vector", opt, false);
        nt47.get_append(tres_itr, tre_str, v);
        if (!opt) {
            nt47.get(tres_itr, corsegs);
            for (size_t s = 0; s < corsegs.size(); ++s) {
                double val;
                ASC_double(corsegs[s], val);
                corseg_vals.push_back(val);
            }
        }

        std::vector<std::string> tausegs;
        std::vector<double> tauseg_vals;
        nitf_tre<std::string> nt48("TAUSEG", "vector", opt, false);
        nt48.get_append(tres_itr, tre_str, v);
        if (!opt) {
            nt48.get(tres_itr, tausegs);
            for (size_t s = 0; s < tausegs.size(); ++s) {
                double val;
                ASC_double(tausegs[s], val);
                tauseg_vals.push_back(val);
            }
        }
        std::vector<std::string> Acs;
        std::vector<double> Acoef_vals;
        nitf_tre<std::string> nt49("AC", "vector", opt, false);
        nt49.get_append(tres_itr, tre_str, v);
        if (!opt) {
            nt49.get(tres_itr, Acs);
            for (size_t s = 0; s < Acs.size(); ++s) {
                double val;
                ASC_double(Acs[s], val);
                Acoef_vals.push_back(val);
            }
        }
        std::vector<std::string> alphs;
        std::vector<double> alpha_vals;
        nitf_tre<std::string> nt50("ALPC", "vector", opt, false);
        nt50.get_append(tres_itr, tre_str, v);
        if (!opt) {
            nt50.get(tres_itr, alphs);
            for (size_t s = 0; s < alphs.size(); ++s) {
                double val;
                ASC_double(alphs[s], val);
                alpha_vals.push_back(val);
            }
        }

        std::vector<std::string> bets;
        std::vector<double> beta_vals;
        nitf_tre<std::string> nt51("BETC", "vector", opt, false);
        nt51.get_append(tres_itr, tre_str, v);
        if (!opt) {
            nt51.get(tres_itr, bets);
            for (size_t s = 0; s < bets.size(); ++s) {
                double val;
                ASC_double(bets[s], val);
                beta_vals.push_back(val);
            }
        }

        std::vector<std::string> Tcs;
        std::vector<double> T_vals;
        nitf_tre<std::string> nt52("TC", "vector", opt, false);
        nt52.get_append(tres_itr, tre_str, v);
        if (!opt) {
            nt52.get(tres_itr, Tcs);
            for (size_t s = 0; s < Tcs.size(); ++s) {
                double val;
                ASC_double(Tcs[s], val);
                T_vals.push_back(val);
            }
        }
        // Encode time correlation functions for each independent group
        for (size_t ig = 0; ig < n_indpb; ++ig) {
            // use functional form of time correlation
            int func_idx = 0, piecewise_idx = 0;
            if (use_function[ig]) {
                double Ac = Acoef_vals[func_idx];
                double alpha = alpha_vals[func_idx];
                double beta = beta_vals[func_idx];
                double Tc = T_vals[func_idx];
                std::tuple<double, double, double, double> func(Ac, alpha, beta, Tc);
                apdata.corr_analytic_functions_[ig] = func;
                func_idx++;
                continue;
            }
            // else use piecewise linear form for correlation function
            int n_segments = ncsegb[piecewise_idx];
            std::vector<std::pair<double, double> > segments;
            for(size_t s = 0; s<n_segments; ++s)
                segments.emplace_back(corseg_vals[s], tauseg_vals[s]);
            
            std::tuple<size_t, std::vector<std::pair<double, double> > > temp(n_segments, segments);
            apdata.corr_piecewise_functions_[ig] = temp;
        }
        
        std::vector<std::string> Mvals;
        apdata.mapping_matrix_.set_size(npar, nparob);
        nitf_tre<std::string> nt53("MAP", "vector", opt, false);
        nt53.get_append(tres_itr, tre_str, v);
        if(!opt){
            nt53.get(tres_itr, Mvals);
          int idx = 0;
          for(size_t r = 0; r<npar; ++r)
            for(size_t c = 0; c<nparob; ++c){
              double val;
              ASC_double(Mvals[idx], val);
              apdata.mapping_matrix_[r][c] = val;
              idx++;
            }
        }
        // Unmodeled Error 
        opt = !UCreq;
        double urr;
        nitf_tre<std::string> nt54("URR",  opt, false);
        nt54.get_append(tres_itr, tre_str, v);
        if (!opt) {
            std::string urrs;
            nt54.get(tres_itr, urrs);
            ASC_double(urrs, urr);
        }
        double ucc;
        nitf_tre<std::string> nt55("UCC", opt, false);
        nt55.get_append(tres_itr, tre_str, v);
        if (!opt) {
            std::string uccs;
            nt55.get(tres_itr, uccs);
            ASC_double(uccs, ucc);
        }
        double urc;
        nitf_tre<std::string> nt56("URC", opt, false);
        nt56.get_append(tres_itr, tre_str, v);
        if (!opt) {
            std::string urcs;
            nt56.get(tres_itr, urcs);
            ASC_double(urcs, urc);
        }
        apdata.unmodeled_row_variance_ = urr;
        apdata.unmodeled_col_variance_ = ucc;
        apdata.unmodeled_row_col_variance_ = urc;

        std::string usamc;
        apdata.unmodeled_analytic_ = false;
        nitf_tre<std::string> nt57("UACSMC", opt, false);
        nt57.get_append(tres_itr, tre_str, v);
        if (!opt) {
            nt57.get(tres_itr, usamc);
            apdata.unmodeled_analytic_ = (usamc == "Y");
        }
        if (apdata.unmodeled_analytic_) {
            double uA_r;
            nitf_tre<std::string> nt58("UACR","vector", opt, false);
            nt58.get_append(tres_itr, tre_str, v);
            if (!opt) {
                std::vector<std::string> uacrs;
                nt58.get(tres_itr, uacrs);
                ASC_double(uacrs[0], uA_r);
            }
            double alpha_r;
            nitf_tre<std::string> nt59("UALPCR", "vector", opt, false);
            nt59.get_append(tres_itr, tre_str, v);
            if (!opt) {
                std::vector<std::string> ualpcrs;
                nt59.get(tres_itr, ualpcrs);
                ASC_double(ualpcrs[0], alpha_r);
            }
            double beta_r;
            nitf_tre<std::string> nt60("UBETCR", "vector", opt, false);
            nt60.get_append(tres_itr, tre_str, v);
            if (!opt) {
                std::vector<std::string> ubetcrs;
                nt60.get(tres_itr, ubetcrs);
                ASC_double(ubetcrs[0], beta_r);
            }
            double T_r;
            nitf_tre<std::string> nt61("UTCR", "vector", opt, false);
            nt61.get_append(tres_itr, tre_str, v);
            if (!opt) {
                std::vector<std::string> utcrs;
                nt61.get(tres_itr, utcrs);
                ASC_double(utcrs[0], T_r);
            }
            double uA_c;
            nitf_tre<std::string> nt62("UACC","vector", opt, false);
            nt62.get_append(tres_itr, tre_str, v);
            if (!opt) {
                std::vector<std::string> uaccs;
                nt62.get(tres_itr, uaccs);
                ASC_double(uaccs[0], uA_c);
            }
            double alpha_c;
            nitf_tre<std::string> nt63("UALPCC", "vector", opt, false);
            nt63.get_append(tres_itr, tre_str, v);
            if (!opt) {
                std::vector<std::string> ualpccs;
                nt63.get(tres_itr, ualpccs);
                ASC_double(ualpccs[0], alpha_c);
            }
            double beta_c;
            nitf_tre<std::string> nt64("UBETCC", "vector", opt, false);
            nt64.get_append(tres_itr, tre_str, v);
            if (!opt) {
                std::vector<std::string> ubetccs;
                nt64.get(tres_itr, ubetccs);
                ASC_double(ubetccs[0], beta_c);
            }
            double T_c;
            nitf_tre<std::string> nt65("UTCC", "vector", opt, false);
            nt65.get_append(tres_itr, tre_str, v);
            if (!opt) {
                std::vector<std::string> utccs;
                nt65.get(tres_itr, utccs);
                ASC_double(utccs[0], T_c);
            }
            std::tuple<double, double, double, double> row_func(uA_r, alpha_r, beta_r, T_r);
            std::tuple<double, double, double, double> col_func(uA_c, alpha_c, beta_c, T_c);
            apdata.unmodeled_row_analytic_function_ = row_func;
            apdata.unmodeled_col_analytic_function_ = col_func;
        }else {// piecewise correlation functions
          int n_row_seg_u = 0 ;
          nitf_tre<int> nt66("UNCSR", "vector", opt, false);
          nt66.get_append(tres_itr, tre_str, v);
          if (!opt) {
            std::vector<int> uncsr;
            nt66.get(tres_itr, uncsr);
            n_row_seg_u = uncsr[0];
          }
          std::vector<double> ucorsr;
          std::vector<std::string> ucorsrs;
          nitf_tre<std::string> nt67("UCORSR", "vector", opt, false);
          nt67.get_append(tres_itr, tre_str, v);
          if(!opt){
            nt67.get(tres_itr, ucorsrs);
            for (size_t r = 0; r < ucorsrs.size(); ++r) {
              double val;
              ASC_double(ucorsrs[r], val);
              ucorsr.push_back(val);
            }
          }
          std::vector<double> utausr;
          std::vector<std::string> utausrs;
          nitf_tre<std::string> nt68("UTAUSR", "vector", opt, false);
          nt68.get_append(tres_itr, tre_str, v);
          if (!opt) {
            nt68.get(tres_itr, utausrs);
            for (size_t r = 0; r < utausrs.size(); ++r) {
              double val;
              ASC_double(utausrs[r], val);
              utausr.push_back(val);
            }
          }
          int n_col_seg_u = 0 ;
          nitf_tre<int> nt69("UNCSC", "vector", opt, false);
          nt69.get_append(tres_itr, tre_str, v);
          if (!opt) {
            std::vector<int> uncsc;
            nt69.get(tres_itr, uncsc);
            n_col_seg_u = uncsc[0];
          }
          std::vector<double> ucorsc;
          std::vector<std::string> ucorscs;
          nitf_tre<std::string> nt70("UCORSC", "vector", opt, false);
          nt70.get_append(tres_itr, tre_str, v);
          if(!opt){
            nt70.get(tres_itr, ucorscs);
            for (size_t c = 0; c < ucorscs.size(); ++c) {
              double val;
              ASC_double(ucorscs[c], val);
              ucorsc.push_back(val);
            }
          }
          std::vector<double> utausc;
          std::vector<std::string> utauscs;
          nitf_tre<std::string> nt71("UTAUSC", "vector", opt, false);
          nt71.get_append(tres_itr, tre_str, v);
          if (!opt) {
            nt71.get(tres_itr, utauscs);
            for (size_t c = 0; c < utauscs.size(); ++c) {
              double val;
              ASC_double(utauscs[c], val);
              utausc.push_back(val);
            }
          }
          std::vector < std::pair<double, double> > temp_r, temp_c;
          for (size_t s = 0; s < n_row_seg_u; ++s)
              temp_r.emplace_back(ucorsr[s], utausr[s]);
          for (size_t s = 0; s < n_col_seg_u; ++s)
              temp_c.emplace_back(ucorsc[s], utausc[s]);

          std::tuple < size_t, std::vector<std::pair<double, double> > > row_func(n_row_seg_u, temp_r);
          std::tuple < size_t, std::vector<std::pair<double, double> > > col_func(n_col_seg_u, temp_c);
          apdata.unmodeled_row_piecewise_function_ = row_func;
          apdata.unmodeled_row_piecewise_function_ = col_func;
        }//end piecewise correlation function
            
        RSMECB = true;
        apdata.defined_ = true;
      }
      
      if (type == "RSMAPA")
      { // looking for "RSMAPA..."
        // =======================================
        nitf_tre<std::string> nt("RSMAPA", tre_str);
        // =========================================
        nitf_tre<std::string> nt1("EDITION", false, false);
        nt1.get_append(tres_itr, tre_str, v);
        RSMAPA = true;
      }

      if (type == "RSMAPB")
      { // looking for "RSMAPB..."
        // =======================================
        nitf_tre<std::string> nt("RSMAPB", tre_str);
        // =========================================
        nitf_tre<std::string> nt1("EDITION", false, false);
        nt1.get_append(tres_itr, tre_str, v);
        RSMAPB = true;
      }

      if (type == "RSMGGA")
      { // looking for "RSMGGA..."
        // =======================================
        nitf_tre<std::string> nt("RSMGGA", tre_str);
        // =========================================
        nitf_tre<std::string> nt1("EDITION", false, false);
        nt1.get_append(tres_itr, tre_str, v);
        RSMGGA = true;
      }
    }

    tre_str << "\n===========  TREs not present in NITF2.1 Image Header ===========" << std::endl;
    if (!RSMPIA)
      tre_str << "RSMPIA" << std::endl;
    if (!RSMGIA)
      tre_str << "RSMGIA" << std::endl;
    if (!RSMDCA)
      tre_str << "RSMDCA" << std::endl;
    if (!RSMDCB)
      tre_str << "RSMDCB" << std::endl;
    if (!RSMECA)
      tre_str << "RSMECA" << std::endl;
    if (!RSMECB)
      tre_str << "RSMECB" << std::endl;
    if (!RSMAPA)
      tre_str << "RSMAPA" << std::endl;
    if (!RSMAPB)
      tre_str << "RSMAPB" << std::endl;
    if (!RSMGGA)
      tre_str << "RSMGGA" << std::endl;
  }
  return true;
}


bool
vpgl_nitf_RSM_camera_extractor::set_RSM_camera_params()
{
  for (auto itr = RSM_cams_.begin(); itr != RSM_cams_.end(); ++itr)
  {
    size_t image_subheader_index = itr->first;
    vil_nitf2_tagged_record_sequence & ixshd_tres = hdr_ixshd_tres_[image_subheader_index];
    if (nitf_status_[image_subheader_index] == IMAGE_SUBHEADER_TREs_RSM_TREs_OVRFL)
      ixshd_tres = ovfl_ixshd_tres_[image_subheader_index];
    vil_nitf2_tagged_record_sequence::const_iterator tres_itr;
    vpgl_region_selector<double> rsel;
    bool good = false, aux_good = false;
    if (RSMIDA)
    {
      // scan for RSMPIA in case of multiple polynomial camera regions
      for (tres_itr = ixshd_tres.begin(); tres_itr != ixshd_tres.end(); ++tres_itr)
      {
        std::string type = (*tres_itr)->name();
        if (type == "RSMIDA")
        {
          nitf_tre<std::string> nt("STID", false, true);
          aux_good = nt.get(tres_itr, rsm_meta_[image_subheader_index].platform_name_);
          if (!aux_good)
            return false;
          rsm_meta_[image_subheader_index].platform_name_valid = aux_good;
          rsm_meta_[image_subheader_index].any_valid = true;

          // boundary metadata
          //  Obtain elevation bounds from the ground domain vertices
          //  The primary purpose of the RSM ground domain is to define
          //  the region of validity of the RSM TRE Set representation in ground space.
          double min_lon = std::numeric_limits<double>::max(), min_lat = min_lon;
          double max_lon = -min_lon, max_lat = -min_lat;
          double min_z, max_z;
          nitf_tre<double> nt1("V1Z", false, false);
          bool min_good = nt1.get(tres_itr, min_z);
          nitf_tre<double> nt2("V8Z", false, false);
          bool max_good = nt2.get(tres_itr, max_z);
          std::vector<std::pair<double, double>> coords;

          double az, el;
          nitf_tre<double> illa("IA0", false, false);
          bool az_good = illa.get(tres_itr, az);
          rsm_meta_[image_subheader_index].sun_azimuth_radians_ = az;
          rsm_meta_[image_subheader_index].sun_azimuth_valid = az_good;
          nitf_tre<double> ille("IE0", false, false);
          bool el_good = ille.get(tres_itr, el);
          rsm_meta_[image_subheader_index].sun_elevation_radians_ = el;
          rsm_meta_[image_subheader_index].sun_elevation_valid = el_good;

          // determine corners of image
          int min_r, max_r, min_c, max_c;
          nitf_tre<int> nt3("MINR", false, false);
          bool row_good_min = nt3.get(tres_itr, min_r);
          nitf_tre<int> nt4("MAXR", false, false);
          bool row_good_max = nt4.get(tres_itr, max_r);
          nitf_tre<int> nt5("MINC", false, false);
          bool col_good_min = nt5.get(tres_itr, min_c);
          nitf_tre<int> nt6("MAXC", false, false);
          bool col_good_max = nt6.get(tres_itr, max_c);
          bool img_corners_valid = row_good_min && row_good_max && col_good_min && col_good_max;
          if (!img_corners_valid)
          {
            std::cout << "Extracting image corners failed" << std::endl;
          }
          else
          {
            rsm_meta_[image_subheader_index].image_corners_valid = img_corners_valid;
            rsm_meta_[image_subheader_index].min_image_corner_.set(min_c, min_r);
            rsm_meta_[image_subheader_index].max_image_corner_.set(max_c, max_r);
          }
        }
      }
      if (!aux_good)
      {
        std::cout << "Auxillary data access failed for subheader " << image_subheader_index << std::endl;
        return false;
      }
      if (rsm_meta_[image_subheader_index].image_corners_valid)
      {
        rsel.minc_ = rsm_meta_[image_subheader_index].min_image_corner_.x();
        rsel.minr_ = rsm_meta_[image_subheader_index].min_image_corner_.y();
        rsel.maxc_ = rsm_meta_[image_subheader_index].max_image_corner_.x();
        rsel.maxr_ = rsm_meta_[image_subheader_index].max_image_corner_.y();
      }
    }
    else
    {
      std::cout << "RSMIDA not present - shouldn't happen at this point" << std::endl;
      return false;
    }

    if (RSMPIA)
    {
      // scan for RSMPIA in case of multiple polynomial camera regions
      for (tres_itr = ixshd_tres.begin(); tres_itr != ixshd_tres.end(); ++tres_itr)
      {
        std::string type = (*tres_itr)->name();
        if (type == "RSMPIA")
        {
#if 1
          nitf_tre<double> nt0("R0");
          good = nt0.get(tres_itr, rsel.row_coefs_[0]);
          if (!good)
            return false;
          nitf_tre<double> nt1("RX");
          good = nt1.get(tres_itr, rsel.row_coefs_[1]);
          if (!good)
            return false;
          nitf_tre<double> nt2("RY");
          good = nt2.get(tres_itr, rsel.row_coefs_[2]);
          if (!good)
            return false;
          nitf_tre<double> nt3("RZ");
          good = nt3.get(tres_itr, rsel.row_coefs_[3]);
          if (!good)
            return false;
          nitf_tre<double> nt4("RXX");
          good = nt4.get(tres_itr, rsel.row_coefs_[4]);
          if (!good)
            return false;
          nitf_tre<double> nt5("RXY");
          good = nt5.get(tres_itr, rsel.row_coefs_[5]);
          if (!good)
            return false;
          nitf_tre<double> nt6("RXZ");
          good = nt6.get(tres_itr, rsel.row_coefs_[6]);
          if (!good)
            return false;
          nitf_tre<double> nt7("RYY");
          good = nt7.get(tres_itr, rsel.row_coefs_[7]);
          if (!good)
            return false;
          nitf_tre<double> nt8("RYZ");
          good = nt8.get(tres_itr, rsel.row_coefs_[8]);
          if (!good)
            return false;
          nitf_tre<double> nt9("RZZ");
          good = nt9.get(tres_itr, rsel.row_coefs_[9]);
          nitf_tre<double> nt10("C0");
          good = nt10.get(tres_itr, rsel.col_coefs_[0]);
          if (!good)
            return false;
          nitf_tre<double> nt11("CX");
          good = nt11.get(tres_itr, rsel.col_coefs_[1]);
          if (!good)
            return false;
          nitf_tre<double> nt12("CY");
          good = nt12.get(tres_itr, rsel.col_coefs_[2]);
          if (!good)
            return false;
          nitf_tre<double> nt13("CZ");
          good = nt13.get(tres_itr, rsel.col_coefs_[3]);
          if (!good)
            return false;
          nitf_tre<double> nt14("CXX");
          good = nt14.get(tres_itr, rsel.col_coefs_[4]);
          if (!good)
            return false;
          nitf_tre<double> nt15("CXY");
          good = nt15.get(tres_itr, rsel.col_coefs_[5]);
          if (!good)
            return false;
          nitf_tre<double> nt16("CXZ");
          good = nt16.get(tres_itr, rsel.col_coefs_[6]);
          if (!good)
            return false;
          nitf_tre<double> nt17("CYY");
          good = nt17.get(tres_itr, rsel.col_coefs_[7]);
          if (!good)
            return false;
          nitf_tre<double> nt18("CYZ");
          good = nt18.get(tres_itr, rsel.col_coefs_[8]);
          if (!good)
            return false;
          nitf_tre<double> nt19("CZZ");
          good = nt19.get(tres_itr, rsel.col_coefs_[9]);
          if (!good)
            return false;
          nitf_tre<std::string> nt20("RNIS");
          std::string s; // convert string to int only
          int v;
          good = nt20.get(tres_itr, s);
          ASC_int(s, v);
          rsel.rnis_ = v;
          if (!good)
            return false;
          nitf_tre<std::string> nt21("CNIS");
          good = nt21.get(tres_itr, s);
          ASC_int(s, v);
          rsel.cnis_ = v;
          if (!good)
            return false;
          nitf_tre<std::string> nt22("TNIS");
          good = nt22.get(tres_itr, s);
          ASC_int(s, v);
          rsel.tnis_ = v;
          if (!good)
            return false;
          nitf_tre<double> nt23("RSSIZ");
          good = nt23.get(tres_itr, rsel.rssiz_);
          if (!good)
            return false;
          nitf_tre<double> nt24("CSSIZ");
          good = nt24.get(tres_itr, rsel.cssiz_);
          if (!good)
            return false;
#else // had to read a string value so convert all string to val
          std::string s;
          nitf_tre<std::string> nt0("R0");
          good = nt0.get(tres_itr, s);
          ASC_double(s, rsel.row_coefs_[0]);
          if (!good)
            return false;
          nitf_tre<std::string> nt1("RX");
          good = nt1.get(tres_itr, s);
          ASC_double(s, rsel.row_coefs_[1]);
          if (!good)
            return false;
          nitf_tre<std::string> nt2("RY");
          good = nt2.get(tres_itr, s);
          ASC_double(s, rsel.row_coefs_[2]);
          if (!good)
            return false;
          nitf_tre<std::string> nt3("RZ");
          good = nt3.get(tres_itr, s);
          ASC_double(s, rsel.row_coefs_[3]);
          if (!good)
            return false;
          nitf_tre<std::string> nt4("RXX");
          good = nt4.get(tres_itr, s);
          ASC_double(s, rsel.row_coefs_[4]);
          if (!good)
            return false;
          nitf_tre<std::string> nt5("RXY");
          good = nt5.get(tres_itr, s);
          ASC_double(s, rsel.row_coefs_[5]);
          if (!good)
            return false;
          nitf_tre<std::string> nt6("RXZ");
          good = nt6.get(tres_itr, s);
          ASC_double(s, rsel.row_coefs_[6]);
          if (!good)
            return false;
          nitf_tre<std::string> nt7("RYY");
          good = nt7.get(tres_itr, s);
          ASC_double(s, rsel.row_coefs_[7]);
          if (!good)
            return false;
          nitf_tre<std::string> nt8("RYZ");
          good = nt8.get(tres_itr, s);
          ASC_double(s, rsel.row_coefs_[8]);
          if (!good)
            return false;
          nitf_tre<std::string> nt9("RZZ");
          good = nt9.get(tres_itr, s);
          ASC_double(s, rsel.row_coefs_[9]);
          nitf_tre<std::string> nt10("C0");
          good = nt10.get(tres_itr, s);
          ASC_double(s, rsel.col_coefs_[0]);
          if (!good)
            return false;
          nitf_tre<std::string> nt11("CX");
          good = nt11.get(tres_itr, s);
          ASC_double(s, rsel.col_coefs_[1]);
          if (!good)
            return false;
          nitf_tre<std::string> nt12("CY");
          good = nt12.get(tres_itr, s);
          ASC_double(s, rsel.col_coefs_[2]);
          if (!good)
            return false;
          nitf_tre<std::string> nt13("CZ");
          good = nt13.get(tres_itr, s);
          ASC_double(s, rsel.col_coefs_[3]);
          if (!good)
            return false;
          nitf_tre<std::string> nt14("CXX");
          good = nt14.get(tres_itr, s);
          ASC_double(s, rsel.col_coefs_[4]);
          if (!good)
            return false;
          nitf_tre<std::string> nt15("CXY");
          good = nt15.get(tres_itr, s);
          ASC_double(s, rsel.col_coefs_[5]);
          if (!good)
            return false;
          nitf_tre<std::string> nt16("CXZ");
          good = nt16.get(tres_itr, s);
          ASC_double(s, rsel.col_coefs_[6]);
          if (!good)
            return false;
          nitf_tre<std::string> nt17("CYY");
          good = nt17.get(tres_itr, s);
          ASC_double(s, rsel.col_coefs_[7]);
          if (!good)
            return false;
          nitf_tre<std::string> nt18("CYZ");
          good = nt18.get(tres_itr, s);
          ASC_double(s, rsel.col_coefs_[8]);
          if (!good)
            return false;
          nitf_tre<std::string> nt19("CZZ");
          good = nt19.get(tres_itr, s);
          ASC_double(s, rsel.col_coefs_[9]);
          if (!good)
            return false;
          int v;
          nitf_tre<std::string> nt20("RNIS");
          good = nt20.get(tres_itr, s);
          ASC_int(s, v);
          rsel.rnis_ = v;
          if (!good)
            return false;
          nitf_tre<std::string> nt21("CNIS");
          good = nt21.get(tres_itr, s);
          ASC_int(s, v);
          rsel.cnis_ = v;
          if (!good)
            return false;
          nitf_tre<std::string> nt22("TNIS");
          good = nt22.get(tres_itr, s);
          ASC_int(s, v);
          rsel.tnis_ = v;
          if (!good)
            return false;
          nitf_tre<std::string> nt23("RSSIZ");
          good = nt23.get(tres_itr, s);
          ASC_double(s, rsel.rssiz_);
          if (!good)
            return false;
          nitf_tre<std::string> nt24("CSSIZ");
          good = nt24.get(tres_itr, s);
          ASC_double(s, rsel.cssiz_);
          if (!good)
            return false;
#endif
        } // RSMPIA
      } // tres_itr
    }
    else
    { //! has RSMPIA
      // only one set of polynomials
      //  RSMPCA should only occur once with row and col == 1
      manditory_PCA_row_ = 1;
      manditory_PCA_col_ = 1;
      rsel.tnis_ = 1;
    }
    RSM_cams_[image_subheader_index] = vpgl_RSM_camera<double>(rsel);
    if (RSMPCA)
    {
      for (tres_itr = ixshd_tres.begin(); tres_itr != ixshd_tres.end(); ++tres_itr)
      {
        std::string type = (*tres_itr)->name();
        if (type == "RSMPCA")
        {
          std::vector<std::vector<int>> powers;
          std::vector<std::vector<double>> coeffs;
          std::vector<vpgl_scale_offset<double>> scale_offsets;
          bool aux_good = false;
          double x_scale, x_off, y_scale, y_off, z_scale, z_off;
          double u_scale, u_off, v_scale, v_off;
          int x_pow, y_pow, z_pow, rsn, csn;


          nitf_tre<int> ntr("RSN");
          good = ntr.get(tres_itr, rsn);
          if (!good)
            return false;
          if (manditory_PCA_row_ == 1 && rsn != 1)
            return false;
          nitf_tre<int> ntc("CSN");
          good = ntc.get(tres_itr, csn);
          if (!good)
            return false;
          if (manditory_PCA_col_ == 1 && csn != 1)
            return false;

          vpgl_polycam<double> pcam(rsn, csn);

          nitf_tre<double> nt0("RNRMO");
          good = nt0.get(tres_itr, v_off);
          if (!good)
            return false;

          nitf_tre<double> nt1("CNRMO");
          good = nt1.get(tres_itr, u_off);
          if (!good)
            return false;

          nitf_tre<double> nt2("XNRMO");
          good = nt2.get(tres_itr, x_off);
          if (!good)
            return false;

          nitf_tre<double> nt3("YNRMO");
          good = nt3.get(tres_itr, y_off);
          if (!good)
            return false;

          nitf_tre<double> nt4("ZNRMO");
          good = nt4.get(tres_itr, z_off);
          if (!good)
            return false;

          nitf_tre<double> nt5("RNRMSF");
          good = nt5.get(tres_itr, v_scale);
          if (!good)
            return false;

          nitf_tre<double> nt6("CNRMSF");
          good = nt6.get(tres_itr, u_scale);
          if (!good)
            return false;

          nitf_tre<double> nt7("XNRMSF");
          good = nt7.get(tres_itr, x_scale);
          if (!good)
            return false;

          nitf_tre<double> nt8("YNRMSF");
          good = nt8.get(tres_itr, y_scale);
          if (!good)
            return false;

          nitf_tre<double> nt9("ZNRMSF");
          good = nt9.get(tres_itr, z_scale);
          if (!good)
            return false;

          scale_offsets.emplace_back(x_scale, x_off);
          scale_offsets.emplace_back(y_scale, y_off);
          scale_offsets.emplace_back(z_scale, z_off);
          scale_offsets.emplace_back(u_scale, u_off);
          scale_offsets.emplace_back(v_scale, v_off);

          int rn_nterms;
          std::vector<int> rnpows;

          nitf_tre<int> nt10("RNPWRX");
          good = nt10.get(tres_itr, x_pow);
          if (!good)
            return false;

          nitf_tre<int> nt11("RNPWRY");
          good = nt11.get(tres_itr, y_pow);
          if (!good)
            return false;

          nitf_tre<int> nt12("RNPWRZ");
          good = nt12.get(tres_itr, z_pow);
          if (!good)
            return false;

          nitf_tre<int> nt13("RNTRMS");
          good = nt13.get(tres_itr, rn_nterms);
          if (!good)
            return false;

          rnpows.push_back(x_pow);
          rnpows.push_back(y_pow);
          rnpows.push_back(z_pow);

          std::vector<double> rnpcf;
          nitf_tre<double> nt14("RNPCF");
          good = nt14.get(tres_itr, rnpcf);
          if (!good)
            return false;

          int rd_nterms;
          std::vector<int> rdpows;
          nitf_tre<int> nt15("RDPWRX");
          good = nt15.get(tres_itr, x_pow);
          if (!good)
            return false;

          nitf_tre<int> nt16("RDPWRY");
          good = nt16.get(tres_itr, y_pow);
          if (!good)
            return false;

          nitf_tre<int> nt17("RDPWRZ");
          good = nt17.get(tres_itr, z_pow);
          if (!good)
            return false;

          nitf_tre<int> nt18("RDTRMS");
          good = nt18.get(tres_itr, rd_nterms);
          if (!good)
            return false;

          rdpows.push_back(x_pow);
          rdpows.push_back(y_pow);
          rdpows.push_back(z_pow);

          std::vector<double> rdpcf;
          nitf_tre<double> nt19("RDPCF");
          good = nt19.get(tres_itr, rdpcf);
          if (!good)
            return false;
          int cn_nterms;
          std::vector<int> cnpows;
          nitf_tre<int> nt20("CNPWRX");
          good = nt20.get(tres_itr, x_pow);
          if (!good)
            return false;

          nitf_tre<int> nt21("CNPWRY");
          good = nt21.get(tres_itr, y_pow);
          if (!good)
            return false;

          nitf_tre<int> nt22("CNPWRZ");
          good = nt22.get(tres_itr, z_pow);
          if (!good)
            return false;

          nitf_tre<int> nt23("CNTRMS");
          good = nt23.get(tres_itr, cn_nterms);
          if (!good)
            return false;

          cnpows.push_back(x_pow);
          cnpows.push_back(y_pow);
          cnpows.push_back(z_pow);

          std::vector<double> cnpcf;
          nitf_tre<double> nt24("CNPCF");
          good = nt24.get(tres_itr, cnpcf);
          if (!good)
            return false;

          int cd_nterms;
          std::vector<int> cdpows;
          nitf_tre<int> nt25("CDPWRX");
          good = nt25.get(tres_itr, x_pow);
          if (!good)
            return false;

          nitf_tre<int> nt26("CDPWRY");
          good = nt26.get(tres_itr, y_pow);
          if (!good)
            return false;

          nitf_tre<int> nt27("CDPWRZ");
          good = nt27.get(tres_itr, z_pow);
          if (!good)
            return false;

          nitf_tre<int> nt28("CDTRMS");
          good = nt28.get(tres_itr, cd_nterms);
          if (!good)
            return false;

          cdpows.push_back(x_pow);
          cdpows.push_back(y_pow);
          cdpows.push_back(z_pow);

          std::vector<double> cdpcf;
          nitf_tre<double> nt29("CDPCF");
          good = nt29.get(tres_itr, cdpcf);
          if (!good)
            return false;

          powers.push_back(cnpows);
          powers.push_back(cdpows);
          powers.push_back(rnpows);
          powers.push_back(rdpows);
          coeffs.push_back(cnpcf);
          coeffs.push_back(cdpcf);
          coeffs.push_back(rnpcf);
          coeffs.push_back(rdpcf);
          good = (cnpcf.size() == cn_nterms) && (cdpcf.size() == cd_nterms) && (rnpcf.size() == rn_nterms) &&
                 (rdpcf.size() == rd_nterms);
          if (!good)
            return false;

          pcam.set_powers(powers);
          pcam.set_coefficients(coeffs);
          pcam.set_scale_offsets(scale_offsets);
          RSM_cams_[image_subheader_index].set_polycam(rsn, csn, pcam);
        }
      }
    } // RSMPCA
    else
    { // RSMPCA
      std::cout << "RSMPCA not found - shouldn't happen at this point" << std::endl;
      return false;
    }
      process_polytope(image_subheader_index);

    // ground domain
    const auto& meta = rsm_meta_[image_subheader_index];
    auto gd = vpgl_ground_domain<double>(meta.ground_domain_);
    if (meta.ground_domain_ == "R") {
      gd.translation_ = meta.translation_;
      gd.rotation_ = meta.rotation_;
    }
    RSM_cams_[image_subheader_index].set_ground_domain(gd);

  } // image headers
  return true;
}


void
vpgl_nitf_RSM_camera_extractor::print_file_header_summary()
{
  size_t n = nitf_status_.size();
  if (n == 0)
    std::cout << "NITF2.1 File has no image subheaders" << std::endl;

  else if (n == 1)
    std::cout << "NITF2.1 File has one image subheader" << std::endl;
  else
    std::cout << "NITF2.1 File has " << n << " image subheaders" << std::endl;

  std::vector<int> inv;
  for (auto itr = nitf_status_.begin(); itr != nitf_status_.end(); ++itr)
    if (itr->second == INVALID)
      inv.push_back(itr->first);
  int ni = inv.size();
  if (ni > 0)
  {
    if (ni == 1)
      std::cout << "subheader " << inv[0] << " could not be examined" << std::endl;
    else
    {
      std::cout << "subheaders ";
      for (int idx : inv)
        std::cout << idx << ' ';
      std::cout << " could not be examined" << std::endl;
    }
  }

  for (auto itr = nitf_status_.begin(); itr != nitf_status_.end(); ++itr)
  {
    if (itr->second == INVALID)
      continue;
    if (itr->second == IMAGE_SUBHEADER_TREs_ONLY)
    {
      std::cout << "Image " << itr->first << " has image TREs in the subheader but no RSM information" << std::endl;
      continue;
    }
    else if (itr->second == IMAGE_SUBHEADER_TREs_RSM_TREs)
    {
      std::cout << "Image " << itr->first << " has both image TREs in the subheader as well as RSM information"
                << std::endl;
      continue;
    }
    else if (itr->second == IMAGE_SUBHEADER_TREs_RSM_TREs_OVRFL)
    {
      std::cout << "Image " << itr->first
                << " has image TREs in the subheader and RSM information is present in the overflow section"
                << std::endl;
      continue;
    }
  }
}
