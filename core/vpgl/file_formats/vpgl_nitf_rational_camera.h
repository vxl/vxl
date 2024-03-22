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
  // note: to be consistent with point coords (x, y)
  enum geopt_coord { LON, LAT };
  enum igeolo_order {UL, UR, LR, LL};
  vpgl_nitf_rational_camera() = default;

  //: Construct from a nitf image file
  vpgl_nitf_rational_camera(std::string const& nitf_image_path,
                            bool verbose = false);

  //: Construct from a nitf image
  vpgl_nitf_rational_camera(vil_nitf2_image* nift_image,
                            bool verbose = false);

  //: Read from nitf
  bool read(std::string const& nitf_image, bool verbose = false);
  bool read(vil_nitf2_image* nitf_image, bool verbose = false);

  std::string rational_extension_type() const {return nitf_rational_type_;}

  std::string image_id() const {return image_id_;}

  //: Geographic coordinates of image corners
  vnl_double_2 upper_left() const {return ul_;}
  vnl_double_2 upper_right() const {return ur_;}
  vnl_double_2 lower_left() const {return ll_;}
  vnl_double_2 lower_right() const {return lr_;}

  //: print all camera information
  void print(std::ostream &ostr = std::cout,
             vpgl_rational_order output_order =
                 vpgl_rational_order::VXL) const override;

  // extract lat-lon pairs in decimal degreesfrom the igeolo string
  static int geostr_to_latlon(const char * str, double * lat, double * lon);

  // eliminate pointer-based interface
  //                                               igeolo                        lon      lat
  static void geostr_to_latlon_v2(std::string const& str, std::vector<std::pair<double, double> >& coords);

 private:

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
