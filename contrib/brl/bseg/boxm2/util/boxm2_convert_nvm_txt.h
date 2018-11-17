#ifndef boxm2_convert_nvm_txt_h
#define boxm2_convert_nvm_txt_h
//:
// \file
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vpgl/vpgl_perspective_camera.h>

#include <bwm/video/bwm_video_corr_sptr.h>
#include <bwm/video/bwm_video_corr.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_3d.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

typedef vpgl_perspective_camera<double> CamType;

//: Main boxm2_convert_nvm_txt function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_convert_nvm_txt (std::string nvm_file,
                             std::string img_dir,
                             std::map<std::string, CamType*>& cams,
                             std::map<std::string, std::string >& img_name_mapping);

//: Utility class with static methods
class boxm2_convert_nvm_txt
{
  public:
    boxm2_convert_nvm_txt(const std::string& nvm_file, const std::string& img_dir);
    std::map<std::string, CamType*>&       get_cams() { return final_cams_; }
    std::map<std::string, std::string>&     get_img_name_mapping() { return img_name_map_; }

    vgl_box_3d<double>                   get_bbox() const { return bbox_; }
   private:

    //final cams (map from image file name to camera
    std::map<std::string, CamType*>                 final_cams_;
    std::map<std::string, std::string>               img_name_map_;


    std::vector<CamType>                           cams_;
    std::vector<std::string>                        names_;
    std::vector<std::string>                        old_names_;
    vgl_box_3d<double>                            bbox_;
    std::string                                    img_dir_;
    std::string                                    nvm_file_;
    //-------------------------------------------------------------------------
    // Helpers
    //-------------------------------------------------------------------------
    bool read_cameras(std::ifstream& bfile);
};

#endif // boxm2_convert_nvm_txt_h
