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


class vpgl_nitf_RSM_camera : public vpgl_RSM_camera<double>
{
 public:

  vpgl_nitf_RSM_camera(std::string const& nitf_image_path,
                            bool verbose = false);

  //: Construct from a nitf image
  vpgl_nitf_RSM_camera(vil_nitf2_image* nift_image,
                            bool verbose = false);

  std::string image_id() const {return image_id_;}

  //: Geographic coordinates of image corners
  vnl_double_2 upper_left() const {return ul_;}
  vnl_double_2 upper_right() const {return ur_;}
  vnl_double_2 lower_left() const {return ll_;}
  vnl_double_2 lower_right() const {return lr_;}

  //: print all camera information
  
  void print(std::ostream& ostr = std::cout) const;
  bool test_rsm_params() const;
  bool get_rsm_camera_params(std::vector<std::vector<int> >& powers,
                             std::vector<std::vector<double> >& coeffs,
                             std::vector<vpgl_scale_offset<double> >& scale_offsets);

 private:
  // internal functions
  bool init(vil_nitf2_image* nitf_image, bool verbose);
  vil_nitf2_tagged_record_sequence  isxhd_tres_;
  // data members
  std::string image_id_;
  std::string image_igeolo_;
  //: geo-coordinates of image corners
  vnl_double_2 ul_;
  vnl_double_2 ur_;
  vnl_double_2 ll_;
  vnl_double_2 lr_;
};

#endif // vpgl_nitf_RSM_camera_h_
