// This is gel/mrc/vpgl/file_formats/vpgl_nitf_rational_camera.h
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
#include <vpgl/vpgl_rational_camera.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_double_2.h>
#include <vil/file_formats/vil_nitf2_image.h>


class vpgl_nitf_rational_camera : public vpgl_rational_camera<double>
{
 public:

  enum geopt_coord { LAT, LON };
  vpgl_nitf_rational_camera();

  //: Construct from a nitf image file
  vpgl_nitf_rational_camera(vcl_string const& nitf_image_path,
                            bool verbose = false);

  //: Construct from a nitf image
  vpgl_nitf_rational_camera(vil_nitf2_image* nift_image,
                            bool verbose = false);

  vcl_string rational_extension_type() {return nitf_rational_type_;}

  vcl_string image_id() {return image_id_;}

  //: Geographic coordinates of image corners
  vnl_double_2 upper_left(){return ul_;}
  vnl_double_2 upper_right(){return ur_;}
  vnl_double_2 lower_left(){return ll_;}
  vnl_double_2 lower_right(){return lr_;}

 private:
  //internal functions
  // NITF_RATIONAL00B - commercial + airborne
  void set_order_b(int*);
  bool init(vil_nitf2_image* nitf_image, bool verbose);
  // data members
  vcl_string nitf_rational_type_;
  vcl_string image_id_;
  vcl_string image_igeolo_;
  //: geo-coordinates of image corners
  vnl_double_2 ul_;
  vnl_double_2 ur_;
  vnl_double_2 ll_;
  vnl_double_2 lr_;
};

#endif // vpgl_nitf_rational_camera_h_
