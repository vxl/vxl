#ifndef boxm2_convert_bundle_h
#define boxm2_convert_bundle_h
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

//: Main boxm2_convert_bundle function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_convert_bundle( std::string bundle_file,
                                std::string img_dir,
                                std::map<std::string, CamType*>& cams,
                                vgl_box_3d<double>& bbox,
                                double& resolution);

//: Utility class with static methods
class boxm2_convert_bundle
{
  public:
    boxm2_convert_bundle(const std::string& bundle_file, const std::string& img_dir);
    std::map<std::string, CamType*>&       get_cams() { return final_cams_; }
    vgl_box_3d<double>                   get_bbox() const { return bbox_; }
    double                               get_resolution() const { return resolution_; }

  private:
    //final cams (map from image file name to camera
    std::map<std::string, CamType*>                  final_cams_;

    //error map (image number to pixel-wise RMS error, and observation count)
    std::vector<bwm_video_corr_sptr>               corrs_;
    std::map<unsigned,double>                      view_error_map_;
    std::map<unsigned,unsigned>                    view_count_map_;
    std::vector<CamType>                           cams_;
    std::set<int>                                  bad_cams_;
    vgl_box_3d<double>                            bbox_;
    std::string                                    img_dir_;
    std::string                                    bundle_file_;
    double                                        resolution_;

    //-------------------------------------------------------------------------
    // Helpers
    //-------------------------------------------------------------------------
    bool read_nums(std::ifstream& bfile, unsigned& num_cams, unsigned& num_pts);
    bool read_cameras(std::ifstream& bfile, unsigned num_cams, vgl_point_2d<double> ppoint);
    bool read_points(std::ifstream& bfile, unsigned num_pts, vgl_point_2d<double> ppoint);
};

#endif // boxm2_convert_bundle_h
