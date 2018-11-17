#ifndef boxm2_convert_nvm_h
#define boxm2_convert_nvm_h
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
//#include <cstdlib>

typedef vpgl_perspective_camera<double> CamType;

//: Main boxm2_convert_nvm function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_convert_nvm (std::string nvm_file,
                             std::string img_dir,
                             std::map<std::string, CamType*>& cams,
                             std::vector<vgl_point_3d<double> > &  pts,
                             vgl_box_3d<double>& bbox,
                             double& resolution,bool axis_align = true  );

//: Utility class with static methods
class boxm2_convert_nvm
{
  public:
    boxm2_convert_nvm(const std::string& nvm_file, const std::string& img_dir,bool axis_align = true);
    std::map<std::string, CamType*>&       get_cams() { return final_cams_; }
    vgl_box_3d<double>                   get_bbox() const { return bbox_; }
    double                               get_resolution() const { return resolution_; }
    std::vector<vgl_point_3d<double> >   get_points() const { return pts_3d_;}
   private:

    //final cams (map from image file name to camera
    std::map<std::string, CamType*>                 final_cams_;

    //error map (image number to pixel-wise RMS error, and observation count)
    std::vector<bwm_video_corr_sptr>               corrs_;
    std::map<unsigned,double>                      view_error_map_;
    std::map<unsigned,unsigned>                    view_count_map_;
    std::vector<CamType>                           cams_;
    std::vector<std::string>                        names_;
    std::set<int>                                  bad_cams_;
    vgl_box_3d<double>                            bbox_;
    std::string                                    img_dir_;
    std::string                                    nvm_file_;
    double                                        resolution_;
    std::vector<vgl_point_3d<double> > pts_3d_;
    //-------------------------------------------------------------------------
    // Helpers
    //-------------------------------------------------------------------------
    bool read_cameras(std::ifstream& bfile, vgl_point_2d<double> ppoint);
    bool read_points(std::ifstream& bfile, vgl_point_2d<double> ppoint);
};

#endif // boxm2_convert_nvm_h
