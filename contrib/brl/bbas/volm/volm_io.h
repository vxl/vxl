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

  //: warning: always add to the end of this error code list, python script on the server has a hard copy of some of these values, they should not be changed
  enum VOLM_ERROR_CODES {SUCCESS, EXE_ARGUMENT_ERROR, EXE_RUNNING, CAM_FILE_IO_ERROR, MATCHER_EXE_STARTED, MATCHER_EXE_FINISHED, MATCHER_EXE_FAILED, COMPOSE_STARTED, LABELME_FILE_IO_ERROR, SCORE_FILE_MISSING};
  
  //: scale value is STRONG_POSITIVE-STRONG_NEGATIVE
  enum VOLM_IMAGE_CODES {UNEVALUATED = 0, STRONG_NEGATIVE = 1, UNKNOWN = 127, STRONG_POSITIVE = 255, SCALE_VALUE = 254};

  static bool write_status(vcl_string out_folder, int status_code, int percent=0, vcl_string log_message = "");
  static bool write_log(vcl_string out_folder, vcl_string log);
  static bool write_composer_log(vcl_string out_folder, vcl_string log);
  
  //: return true if MATCHER_EXE_FINISHED, otherwise return false
  static bool check_matcher_status(vcl_string out_folder);

  static bool read_camera(vcl_string kml_file, unsigned const& ni, unsigned const& nj,
                                               double& heading,   double& heading_dev,
                                               double& tilt,      double& tilt_dev,
                                               double& roll,      double& roll_dev,
                                               double& top_fov,   double& top_fov_dev);

  static bool read_labelme(vcl_string xml_file, depth_map_scene_sptr& depth_scene, vcl_string& img_category);
  
};  



#endif // volm_io_h_
