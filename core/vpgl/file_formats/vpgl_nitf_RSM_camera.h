// This is core/vpgl/file_formats/vpgl_nitf_RSM_camera.h
#ifndef vpgl_nitf_RSM_camera_h_
#define vpgl_nitf_RSM_camera_h_
//:
// \file
// \brief: instance a replacement sensor model (RSM) polynomial camera
// \author J.L. Mundy
// \date October 2023
//
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_RSM_camera.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vnl/vnl_double_2.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <iostream>
#include <vgl/vgl_polygon.h>
#include <vpgl_replacement_sensor_model_tres.h>
struct image_time {
  int year, month, day, hour, min, sec;
};
struct rsm_metadata{
  std::string catalog_id_;                     bool catalog_id_valid=false;
  std::string platform_name_;                  bool platform_name_valid=false;
  std::string image_name_;                     bool image_name_valid = false;
  std::vector<int> acquisition_time_;          bool acquisition_time_valid = false;
  unsigned effective_bits_per_pixel_;          bool effective_bits_per_pixel_valid = false;
  std::string image_type_;                     bool image_type_valid = false;
  bool corners_valid = false;
  vgl_point_3d<double> upper_left_; 
  vgl_point_3d<double> upper_right_;
  vgl_point_3d<double> lower_left_;
  vgl_point_3d<double> lower_right_;
  vgl_box_3d<double> bounding_box_; 
  vgl_polygon<double> footprint_;
  vgl_point_2d<double> image_offset_;           bool image_offset_valid = false;
  vgl_point_2d<double> rsm_image_offset_;       bool rsm_image_offset_valid = false;
  double cloud_percentage_;                     bool cloud_percentage_valid = false;
  double gsd_;                                  bool gsd_valid = false;
  //az, elev_ 
  vgl_point_2d<double> sun_angles_;             bool sun_angles_valid = false;
  vgl_point_2d<double> view_angles_;            bool view_angles_valid = false;
};
struct ichipb_data{
  bool ichipb_data_valid_ = false;
  std::pair<double, double> translation_;
  std::vector<std::pair<double, double> > F_grid_points_;
  std::vector<std::pair<double, double> > O_grid_points_;
  double scale_factor_;
  bool anamorphic_corr_;
};
  
  class vpgl_nitf_RSM_camera : public vpgl_RSM_camera<double>
{
 public:

  // path to a NITFV2.1 image file
  vpgl_nitf_RSM_camera(std::string const& nitf_image_path,
                            bool verbose = false);

  //: Construct from a nitf image
  vpgl_nitf_RSM_camera(vil_nitf2_image* nift_image,
                            bool verbose = false);

  //  === accessors from rational_camera for consistency==
  // rational type
  std::string rational_extension_type() const {return "RSM";}
  // image name 
  std::string image_id() const {if(rsm_meta_.image_name_valid)
      return rsm_meta_.image_name_;
    else return "";
  }
  //: 2D geographic coordinates of image corners (lon, lat in degrees)
  vnl_double_2 upper_left() const {return ul_;}
  vnl_double_2 upper_right() const {return ur_;}
  vnl_double_2 lower_left() const {return ll_;}
  vnl_double_2 lower_right() const {return lr_;}
  // ======================================================

  //: replacement sensor model tres present in header
  bool nitf_header_contains_RSM_tres()const{ return RSM_defined_;}

  //: read NITF2.1 tagged record extensions (tres) from header
  // a text file of tres present in header
  bool raw_tres(std::ostream& ostr, bool verbose = false) const;

  bool set_RSM_camera_params();

  // the parameters necessary to construct rational polynomials
  void RSM_camera_params(std::vector<std::vector<int> >& powers,
                         std::vector<std::vector<double> >& coeffs,
                         std::vector<vpgl_scale_offset<double> >& scale_offsets){
    powers = powers_; coeffs = coeffs_; scale_offsets = scale_offsets_;
  }
    
  //Extracted metadata. 
  void meta(rsm_metadata& rsm_meta, ichipb_data& ichipb) const {
      rsm_meta = rsm_meta_; ichipb = ichipb_;
  }
  
    private:
  // internal functions
  bool init(vil_nitf2_image* nitf_image, bool verbose);

  // data members
  vil_nitf2_tagged_record_sequence  isxhd_tres_;
  bool RSM_defined_ = false;
  bool igeolo_valid_ = false;
  std::string image_igeolo_;
  //: 2D geo-coordinates of image corners from igeolo
  vnl_double_2 ul_;
  vnl_double_2 ur_;
  vnl_double_2 ll_;
  vnl_double_2 lr_;
  rsm_metadata rsm_meta_;
  ichipb_data ichipb_;
};

#endif // vpgl_nitf_RSM_camera_h_
