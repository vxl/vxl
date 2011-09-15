#ifndef boxm2_bundle_to_scene_h
#define boxm2_bundle_to_scene_h
//:
// \file
#include <bsta/bsta_histogram.h>

#include <bwm/bwm_observer_cam.h>
#include <bwm/video/bwm_video_cam_ostream.h>
#include <bwm/video/bwm_video_corr_sptr.h>
#include <bwm/video/bwm_video_corr.h>
#include <bwm/video/bwm_video_site_io.h>

#include <vcl_cstdlib.h> // for rand()
#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vnl/vnl_double_3.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_bounds.h>

#include <bwm/bwm_site_mgr.h>


//: Utility class with static methods
class boxm2_bundle_to_scene
{
  public:

    //: bundler file to vector of vpgl_perspective_cameras
    vcl_vector<vpgl_perspective_camera<double>* > bundler_to_cameras(vcl_string bundle);

  private:

    bool fit_plane_ransac(vcl_vector<vgl_homg_point_3d<double> > & points, vgl_homg_plane_3d<double>  & plane);
    bool axis_align_scene(vcl_vector<bwm_video_corr_sptr> & corrs,
                          vcl_vector<vpgl_perspective_camera<double> > & cams);

    vnl_double_3 stddev( vcl_vector<vgl_point_3d<double> > const& v);

    //------------------------------------------------------------------------
    // Calc projection error
    // read the correspondence and 3-d points
    //------------------------------------------------------------------------
    void calc_projection_error( vcl_vector<vpgl_perspective_camera<double> >& cams,
                                vcl_set<int>&                    bad_cams,
                                vcl_vector<bwm_video_corr_sptr>& corrs,
                                vcl_map<unsigned,double>&        view_error_map,
                                vcl_map<unsigned,unsigned>&      view_count_map );


    void report_error(vcl_map<unsigned,double>&   view_error_map,
                      vcl_map<unsigned,unsigned>& view_count_map,
                      vcl_set<int>&               bad_cams,
                      float                       filter_thresh);
};

#endif // boxm2_bundle_to_scene_h
