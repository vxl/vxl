//This is brl/bbas/volm/volm_io.h
#ifndef volm_io_h_
#define volm_io_h_
//:
// \file
// \brief A class to hold xml file io methods for volumetric matchers
//
// \author Ozge C. Ozcanli
// \date September 18, 2012
// \verbatim
//  Modifications
//   Yi Dong - October 25 2012 - modify the read_labelme to construct depth_map_scene from xml file
// \endverbatim
// units are in meters

#include <vcl_string.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>


class volm_io 

{
 public:

  enum VOLM_ERROR_CODES {SUCCESS, EXE_ARGUMENT_ERROR, EXE_RUNNING, CAM_FILE_IO_ERROR, EXE_STARTED, LABELME_FILE_IO_ERROR};
  //: scale value is STRONG_POSITIVE-STRONG_NEGATIVE
  enum VOLM_IMAGE_CODES {UNEVALUATED = 0, STRONG_NEGATIVE = 1, UNKNOWN = 127, STRONG_POSITIVE = 255, SCALE_VALUE = 254};

  static bool write_status(vcl_string out_folder, int status_code, int percent=0);
  static bool read_camera(vcl_string kml_file, vpgl_perspective_camera<double>& cam, unsigned ni, unsigned nj);
  static bool read_labelme(vcl_string xml_file, vpgl_perspective_camera<double> const& cam, depth_map_scene_sptr& depth_scene);
  
};  



#endif // volm_io_h_
