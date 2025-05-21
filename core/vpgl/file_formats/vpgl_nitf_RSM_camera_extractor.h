// This is core/vpgl/file_formats/vpgl_nitf_RSM_camera_extractor.h
#ifndef vpgl_nitf_RSM_camera_extractor_h_
#define vpgl_nitf_RSM_camera_extractor_h_
//:
// \file
// \brief: Extract a replacement sensor model (RSM) polynomial camera from NITF TREs
// \author J.L. Mundy
// \date October 2023
// \verbatim
//  Modifications
//   Add extraction of RSMECA metadata,  November 2024
// \endverbatim
//
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_RSM_camera.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vil/vil_load.h>
#include <vnl/vnl_double_2.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <iostream>
#include <vgl/vgl_polygon.h>
#include <vpgl_replacement_sensor_model_tres.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_math.h>
#include <fstream>
struct image_time
{
  int year, month, day, hour, min, sec;
};
// information extracted from NITF header
struct rsm_metadata
{
  bool any_valid = false;
  // sensor information
  std::string platform_name_;
  bool platform_name_valid = false;
  std::string image_iid1_;
  bool image_iid1_valid = false;
  std::string image_iid2_;
  bool image_iid2_valid = false;
  bool sid_valid = false;
  std::string sid_;
  std::vector<int> acquisition_time_;
  bool acquisition_time_valid = false;
  std::string image_type_;
  bool image_type_valid = false;

  bool ground_domain_valid = false;
  std::string ground_domain_;
  // local coordinate system (ground domain = "R")
  bool local_transform_valid = false;
  vnl_vector_fixed<double, 3> translation_;
  vnl_matrix_fixed<double, 3, 3> rotation_;

  // 3-d geographic bounds
  std::string igeolo_;
  bool igeolo_valid = false;
  // RSM validity volume
  bool polytope_valid = false;
  std::map<size_t, vgl_point_3d<double> > polytope_;
  bool bounding_box_valid = false;
  vgl_box_3d<double> bounding_box_;
  // image ground footprint
  bool xy_corners_valid = false;
  vgl_point_2d<double> upper_left_;
  vgl_point_2d<double> upper_right_;
  vgl_point_2d<double> lower_left_;
  vgl_point_2d<double> lower_right_;
  vgl_polygon<double> footprint_;
  // image coordinates
  //       x => column y => row
  vgl_point_2d<double> min_image_corner_;
  bool image_corners_valid = false;
  vgl_point_2d<double> max_image_corner_;
  // sun angles
  double sun_azimuth_radians_ = 0.0;
  bool sun_azimuth_valid = false;
  double sun_elevation_radians_ = 0.0;
  bool sun_elevation_valid = false;
};

struct RSM_ECA_adjustable_parameter_metadata
{
  void
  print(std::ostream & os);
  bool defined_ = false;
  size_t num_adj_params_ = 0;
  size_t num_original_adj_params_ = 0;
  size_t num_independent_subgroups_ = 0;
  bool unmodeled_error_ = false;
  // local coordinate system
  vnl_vector_fixed<double, 3> translation_;
  vnl_matrix_fixed<double, 3, 3> rotation_;

  // adjustable param covariance matrix index  (1-based)
  std::map<std::string, int> covar_index_;

  // indp. group id      covariance matrix
  std::vector<vnl_matrix<double>> independent_subgroup_covariance_;

  //== independent segment time correlation parameters ==
  //   flag indicating type of correlation
  // 0 - all time within and between images;  1 - time between images;  2 -time within image
  std::vector<size_t> correlation_flags_;
  //                 correlation piecewise segments
  //                           correlation    tau
  std::vector<std::vector<std::pair<double, double>>> correlation_segments_;
  // mapping matrix (phi)
  vnl_matrix<double> phi_;

  // unmodeled error
  double unmodeled_row_variance_;
  double unmodeled_col_variance_;
  double unmodeled_row_col_variance_;
  std::tuple<size_t, std::vector<std::pair<double, double> > > unmodeled_row_correlation_;
  std::tuple<size_t, std::vector<std::pair<double, double> > > unmodeled_col_correlation_;
};

struct RSM_ECB_adjustable_parameter_metadata
{
  void
  print(std::ostream & os);
  bool defined_ = false;
  bool unmodeled_error_ = false;
  size_t num_active_adj_params_ = 0;
  size_t num_original_adj_params_ = 0;
  size_t num_independent_subgroups_ = 0;
  bool image_adjustable_params_ = false;
  bool rect_local_coordinate_system_ = false;

  size_t n_image_adjustable_params_ = 0;
  size_t n_image_row_adjustable_params_ = 0;
  size_t n_image_col_adjustable_params_ = 0;

  //    param id               xpower ypower   zpower
  std::map<size_t, std::tuple<size_t, size_t, size_t> > image_row_xyz_powers_;
  std::map<size_t, std::tuple<size_t, size_t, size_t> > image_col_xyz_powers_;
  bool ground_adjustable_params_ = false;
  size_t n_ground_adjustable_params_ = 0;
  std::vector<size_t> ground_adjust_param_idx_;

  bool basis_option_ = false;
  size_t n_basis_adjustable_params_ = 0;
  vnl_matrix<double> A_matrix_;

  //    indep. id   covariance submatrix
  std::map<size_t, vnl_matrix<double> > independent_covar_;
  //    indep. id 0 - all time, 1 - between images, 2 - within image
  std::map<size_t, int> correlation_domain_flags_;
  //    indep. id               A      alpha    beta    T
  std::map<size_t, std::tuple<double, double, double, double > > corr_analytic_functions_;
  //    indep. id             n_segments                  cor_seg  tau_seg
  std::map<size_t, std::tuple<size_t, std::vector<std::pair<double, double> > > > corr_piecewise_functions_;

  vnl_matrix<double> mapping_matrix_;
    
  //coord index(x-0,y-1,z-2) offset scale
  std::map<size_t, std::pair<double, double> > xyz_norm_;

  // local rectanglular coordinate system
  vnl_vector_fixed<double, 3> rect_translation_;
  vnl_matrix_fixed<double, 3, 3> rect_rotation_;

  // unmodeled error
  double unmodeled_row_variance_;
  double unmodeled_col_variance_;
  double unmodeled_row_col_variance_;

  bool unmodeled_analytic_ = true;
  std::tuple<double, double, double, double > unmodeled_row_analytic_function_;
  std::tuple<double, double, double, double > unmodeled_col_analytic_function_;
  std::tuple<size_t, std::vector<std::pair<double, double> > > unmodeled_row_piecewise_function_;
  std::tuple<size_t, std::vector<std::pair<double, double> > >unmodeled_col_piecewise_function_;
};

class vpgl_nitf_RSM_camera_extractor
{
public:
  // possible outcomes depending on header layout
  enum tre_status
  {
    IMAGE_SUBHEADER_TREs_ONLY = 0,
    IMAGE_SUBHEADER_TREs_RSM_TREs,
    IMAGE_SUBHEADER_TREs_RSM_TREs_OVRFL,
    INVALID
  };
  vpgl_nitf_RSM_camera_extractor() = default;
  // path to a NITFV2.1 image file
  vpgl_nitf_RSM_camera_extractor(const std::string & nitf_image_path, bool verbose = false);

  //: Construct from a nitf image pointer
  vpgl_nitf_RSM_camera_extractor(vil_nitf2_image * nift_image, bool verbose = false);

  // image identification information
  std::string
  image_id(size_t image_subheader_index)
  {
    if (rsm_meta_.count(image_subheader_index) > 0)
    {
      rsm_metadata rm = rsm_meta_[image_subheader_index];
      if (rm.image_iid1_valid)
        return rm.image_iid1_;
    }
    return "";
  }


  //: indices of subheaders that contain RSM camera TREs
  //  a return vector of size 0 indicates no RSM data
  std::vector<size_t>
  nitf_header_contains_RSM_tres() const
  {
    std::vector<size_t> ret;
    for (auto itr = nitf_status_.begin(); itr != nitf_status_.end(); ++itr)
      if (itr->second != INVALID && itr->second != IMAGE_SUBHEADER_TREs_ONLY)
        ret.push_back(itr->first);
    return ret;
  }

  //: read NITF2.1 tagged record extensions (tres) from header
  // and output a text file of tres present in header. Also populates
  // some metadata information
  bool
  scan_for_RSM_data(bool verbose);
  // text stream of records found
  std::stringstream
  tre_stream()
  {
    std::stringstream s(ss_.str());
    return s;
  }
  //: set params for image subheaders that have RSM data
  // create the camera projection function (ground-to-image)
  // and camera instances for each image subheader with RSM metadata
  bool
  set_RSM_camera_params();

  // the overall extraction process
  bool
  process(bool verbose)
  {
    if (!scan_for_RSM_data(verbose))
      return false;

    if (!set_RSM_camera_params())
      return false;

    return true;
  }
  bool
  save_tre_values(std::string file)
  {
    std::ofstream os(file.c_str());
    if (!os)
      return false;
    std::stringstream ss = this->tre_stream();
    os << ss.str();
    os.close();
    return true;
  }

  //: return the RSM camera associated with the subheader index
  vpgl_RSM_camera<double>
  RSM_camera(size_t image_subheader_index = 0)
  {
    vpgl_RSM_camera<double> ret;
    if (RSM_cams_.count(image_subheader_index) > 0)
      return RSM_cams_[image_subheader_index];

    std::cout << "image_subheader index " << image_subheader_index << " has no RSM metadata" << std::endl;
    // note: ret.n_regions() == 0 to test invalidity
    // a RSM camera has 1 or more polynomial regions
    return ret;
  }
  // in case of multiple image subheaders associated with RSM cameras
  // the first index associated with a RSM camera definition is the default
  int
  first_index_with_RSM()
  {
    if (RSM_cams_.size() == 0)
      return -1;
    auto iter = RSM_cams_.begin();
    return iter->first;
  }
#if 0 // for debug purposes
  // which polytope verts project to image corners
  bool set_footprint_bounds(){
    if (RSM_cams_.size() == 0)
      return false;
    unsigned UL = 3, UR = 4, LL = 1, LR = 2;
    int idx = first_index_with_RSM();
    vpgl_RSM_camera<double>& rcam = RSM_camera(idx);
    std::map<size_t, vgl_point_2d<double> > img_pts;
    std::map<size_t, vgl_point_3d<double> >& ptp = rsm_meta_[idx].polytope_;
    if(!rsm_meta_[idx].local_transform_valid){
      //polytope vertices are in degrees
      //convert to radians
      double rad_per_deg = 1.0/vnl_math::deg_per_rad;
      for(int i = 1; i<=4; ++i){
        vgl_point_3d<double>& p = ptp[i];
        double x_rad = p.x()*rad_per_deg;
        double y_rad = p.y()*rad_per_deg;
        double z = p.z();
        double u, v;
        rcam.project(x_rad, y_rad, z, u, v);
        img_pts[i].x() = u; img_pts[i].y() = v;
      }
    }
  }
#endif
  //: extracted metadata contained in the image subheader including some RSM-related info
  // default header index 0
  rsm_metadata
  meta(size_t image_subheader_index = 0)
  {
    rsm_metadata ret;
    if (rsm_meta_.count(image_subheader_index) > 0)
      return rsm_meta_[image_subheader_index];

    std::cout << "image_subheader index " << image_subheader_index << " has no general RSM metadata" << std::endl;
    return ret;
  }
  // adjustable parameters defined for the RSM, e.g. image offset correction
  // the metadata also supplies apriori error covariance for the defined adjustable
  // parameters. If not available, meta.defined_ = false;
  RSM_ECA_adjustable_parameter_metadata
      RSM_ECA_adjustable_parameter_data(size_t image_subheader_index = 0)
  {
    RSM_ECA_adjustable_parameter_metadata ret;
    if (RSM_ECA_adj_param_data_.count(image_subheader_index) > 0)
      return RSM_ECA_adj_param_data_[image_subheader_index];
    return ret;
  }
  RSM_ECB_adjustable_parameter_metadata
      RSM_ECB_adjustable_parameter_data(size_t image_subheader_index = 0)
  {
    RSM_ECB_adjustable_parameter_metadata ret;
    if (RSM_ECB_adj_param_data_.count(image_subheader_index) > 0)
      return RSM_ECB_adj_param_data_[image_subheader_index];
    return ret;
  }
  // describe the layout of the file header in terms of number of image
  // subheaders and overflow conditions
  void
  print_file_header_summary();

private:
  // internal functions
  // convert a string to an integer
  void
  ASC_int(std::string str, int & ival)
  {
    std::stringstream ss(str);
    ss >> ival;
  }
  // convert a string to a double
  void
  ASC_double(std::string str, double & dval)
  {
    std::stringstream ss(str);
    ss >> dval;
  }
  // parse the image header tres for required information
  bool
  determine_header_status(vil_nitf2_image_subheader * header_ptr,
                          size_t header_idx,
                          bool & header_has_tres,
                          bool & header_has_RSM,
                          int & ixofl);

  // parse the overflow area for possible RSM information
  bool
  determine_overflow_status(vil_nitf2_image * nitf_image, size_t header_idx, int ixsofl, bool & overflow_has_RSM);

  // determine the layout of information in the file header
  bool
  init(vil_nitf2_image * nitf_image, bool verbose);

  // extract numerical geographic locations of image corners from
  // the concatenated string representation defined by the TRE
  bool
  process_igeolo(size_t image_subheader_index);

  // extract the ground polytope, 8 3-D vertices bounding RSM validity
  bool
    process_polytope(size_t image_subheader_index);
    
  // data members
  // tres extracted from potentially multiple image subheaders
  // a map is used since not all data segments are present for each subheader
  //   header index     TRE data
  std::map<size_t, vil_nitf2_tagged_record_sequence> hdr_ixshd_tres_;

  // tres extracted from the overflow area
  std::map<size_t, vil_nitf2_tagged_record_sequence> ovfl_ixshd_tres_;

  // do any headers define RSM cameras.
  bool RSM_defined_ = false;

  // overall layout status of the file header
  std::map<size_t, tre_status> nitf_status_;

  // useful general iamge info from the tres
  std::map<size_t, rsm_metadata> rsm_meta_;

  // RSM cameras associated with potentially multiple image subheaders
  std::map<size_t, vpgl_RSM_camera<double>> RSM_cams_;

  // storage of adjustable parameter metadata associated with each image subheader
  std::map<size_t, RSM_ECA_adjustable_parameter_metadata>  RSM_ECA_adj_param_data_;
  std::map<size_t, RSM_ECB_adjustable_parameter_metadata>  RSM_ECB_adj_param_data_;

  // Flags indicating if various required TRE groups are present
  bool RSMIDA = false, RSMPIA = false, RSMPCA = false, RSMECA = false, RSMECB = false;

  // presence is checked by attemping to read the EDITION field (40 bytes)
  bool RSMGIA = false, RSMDCA = false, RSMDCB = false;
  bool RSMAPA = false, RSMAPB = false, RSMGGA = false;

  // These variables must be defined by some TRE section
  // to have a valid RPC camera
  int manditory_PCA_row_ = -1;
  int manditory_PCA_col_ = -1;

  // storage for tre records to produce reports
  std::stringstream ss_;

  // print more detailed error messages
  bool verbose_;
};

#endif // vpgl_nitf_RSM_camera_extractor_h_
