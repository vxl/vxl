#ifndef boxm2_bundle_to_scene_h
#define boxm2_bundle_to_scene_h
//:
// \file
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>

#include <bwm/bwm_observer_cam.h>
#include <bwm/video/bwm_video_cam_ostream.h>
#include <bwm/video/bwm_video_corr_sptr.h>
#include <bwm/video/bwm_video_corr.h>
#include <bwm/video/bwm_video_site_io.h>
#include <bwm/bwm_site_mgr.h>
#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <vul/vul_file.h>
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_convert.h>
#include <vcl_cstdlib.h> // for rand()

typedef vpgl_perspective_camera<double> CamType;

//: Main boxm2_bundle_to_scene function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_bundle_to_scene(vcl_string bundle_file,
                                vcl_string img_dir,
                                vcl_map<vcl_string, CamType*>& cams,
                                vgl_box_3d<double>& bbox,
                                double& resolution);

//: Utility class with static methods
class boxm2_bundle_to_scene
{
  public:
    boxm2_bundle_to_scene(vcl_string bundle_file, vcl_string img_dir);
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

    bool fit_plane_ransac( vcl_vector<vgl_homg_point_3d<double> > & points,
                           vgl_homg_plane_3d<double>              & plane);

    bool axis_align_scene(vcl_vector<bwm_video_corr_sptr>              & corrs,
                          vcl_vector<vpgl_perspective_camera<double> > & cams);

    vnl_double_3 stddev( vcl_vector<vgl_point_3d<double> > const& v);

    // Calc projection error
    void calc_projection_error( vcl_vector<vpgl_perspective_camera<double> > & cams,
                                vcl_set<int>                                 & bad_cams,
                                vcl_vector<bwm_video_corr_sptr>              & corrs,
                                vcl_map<unsigned,double>                     & view_error_map,
                                vcl_map<unsigned,unsigned>                   & view_count_map );

    void report_error(vcl_map<unsigned,double>&   view_error_map,
                      vcl_map<unsigned,unsigned>& view_count_map,
                      vcl_set<int>&               bad_cams,
                      float                       filter_thresh);
};

#endif // boxm2_bundle_to_scene_h
