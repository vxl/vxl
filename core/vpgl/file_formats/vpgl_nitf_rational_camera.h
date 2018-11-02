// This is core/vpgl/file_formats/vpgl_nitf_rational_camera.h
#ifndef vpgl_nitf_rational_camera_h_
#define vpgl_nitf_rational_camera_h_
//:
// \file
// \brief: instance a nitf_rational camera from nitf header information.
// \author Jim Green
// \date Dec 2006
//
//  Note that there are several alternate "orderings" of the
//  polynomial coefficients in the nitf header that apply to different
//  versions of nitf images.  These variations were added over time
//  by different government agencies but all are in use in some circumstances.
//  This class defined the ordering for RPC00B which is used for
//  commercial satellite and for imagery taken by military aircraft.
//  Details of the ordering are contained in the document STDI-0002_V3.doc.
//  Additional coefficient permutations can be included by defining other
//  versions of the set_order_* function.
//
// Modified extensively - J.L. Mundy January 07, 2007
//
#include <string>
#include <vpgl/vpgl_rational_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
// not used? #include <vector>
#include <vnl/vnl_double_2.h>
#include <vil/file_formats/vil_nitf2_image.h>


class vpgl_nitf_rational_camera : public vpgl_rational_camera<double>
{
 public:

  enum geopt_coord { LAT, LON };
  vpgl_nitf_rational_camera();

  //: Construct from a nitf image file
  vpgl_nitf_rational_camera(std::string const& nitf_image_path,
                            bool verbose = false);

  //: Construct from a nitf image
  vpgl_nitf_rational_camera(vil_nitf2_image* nift_image,
                            bool verbose = false);

  std::string rational_extension_type() const {return nitf_rational_type_;}

  std::string image_id() const {return image_id_;}

  //: Geographic coordinates of image corners
  vnl_double_2 upper_left() const {return ul_;}
  vnl_double_2 upper_right() const {return ur_;}
  vnl_double_2 lower_left() const {return ll_;}
  vnl_double_2 lower_right() const {return lr_;}

 private:
  //internal functions
  // NITF_RATIONAL00B - commercial + airborne
  void set_order_b(int*);
  bool init(vil_nitf2_image* nitf_image, bool verbose);
  // data members
  std::string nitf_rational_type_;
  std::string image_id_;
  std::string image_igeolo_;
  //: geo-coordinates of image corners
  vnl_double_2 ul_;
  vnl_double_2 ur_;
  vnl_double_2 ll_;
  vnl_double_2 lr_;
};

#endif // vpgl_nitf_rational_camera_h_
