#ifndef boxm2_convert_bundle_h
#define boxm2_convert_bundle_h
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
//#include <vcl_cstdlib.h>

typedef vpgl_perspective_camera<double> CamType;

//: Main boxm2_convert_bundle function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_convert_bundle( vcl_string bundle_file,
                                vcl_string img_dir,
                                vcl_map<vcl_string, CamType*>& cams,
                                vgl_box_3d<double>& bbox,
                                double& resolution);

//: Utility class with static methods
class boxm2_convert_bundle
{
  public:
    boxm2_convert_bundle(vcl_string bundle_file, vcl_string img_dir);
    vcl_map<vcl_string, CamType*>&       get_cams() { return final_cams_; }
    vgl_box_3d<double>                   get_bbox() const { return bbox_; }
    double                               get_resolution() const { return resolution_; }

  private:
    //final cams (map from image file name to camera
    vcl_map<vcl_string, CamType*>                  final_cams_;

    //error map (image number to pixel-wise RMS error, and observation count)
    vcl_vector<bwm_video_corr_sptr>               corrs_;
    vcl_map<unsigned,double>                      view_error_map_;
    vcl_map<unsigned,unsigned>                    view_count_map_;
    vcl_vector<CamType>                           cams_;
    vcl_set<int>                                  bad_cams_;
    vgl_box_3d<double>                            bbox_;
    vcl_string                                    img_dir_;
    vcl_string                                    bundle_file_;
    double                                        resolution_;

    //-------------------------------------------------------------------------
    // Helpers
    //-------------------------------------------------------------------------
    bool read_nums(vcl_ifstream& bfile, unsigned& num_cams, unsigned& num_pts);
    bool read_cameras(vcl_ifstream& bfile, unsigned num_cams, vgl_point_2d<double> ppoint);
    bool read_points(vcl_ifstream& bfile, unsigned num_pts, vgl_point_2d<double> ppoint);
};

#endif // boxm2_convert_bundle_h
