#ifndef boxm2_convert_nvm_txt_h
#define boxm2_convert_nvm_txt_h
//:
// \file
#include <vpgl/vpgl_perspective_camera.h>

#include <bwm/video/bwm_video_corr_sptr.h>
#include <bwm/video/bwm_video_corr.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_algorithm.h>

typedef vpgl_perspective_camera<double> CamType;

//: Main boxm2_convert_nvm_txt function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_convert_nvm_txt (vcl_string nvm_file,
                             vcl_string img_dir,
                             vcl_map<vcl_string, CamType*>& cams,
                             vcl_map<vcl_string, vcl_string >& img_name_mapping);

//: Utility class with static methods
class boxm2_convert_nvm_txt
{
  public:
    boxm2_convert_nvm_txt(vcl_string nvm_file, vcl_string img_dir);
    vcl_map<vcl_string, CamType*>&       get_cams() { return final_cams_; }
    vcl_map<vcl_string, vcl_string>&     get_img_name_mapping() { return img_name_map_; }

    vgl_box_3d<double>                   get_bbox() const { return bbox_; }
   private:

    //final cams (map from image file name to camera
    vcl_map<vcl_string, CamType*>                 final_cams_;
    vcl_map<vcl_string, vcl_string>               img_name_map_;


    vcl_vector<CamType>                           cams_;
    vcl_vector<vcl_string>                        names_;
    vcl_vector<vcl_string>                        old_names_;
    vgl_box_3d<double>                            bbox_;
    vcl_string                                    img_dir_;
    vcl_string                                    nvm_file_;
    //-------------------------------------------------------------------------
    // Helpers
    //-------------------------------------------------------------------------
    bool read_cameras(vcl_ifstream& bfile);
};

#endif // boxm2_convert_nvm_txt_h
