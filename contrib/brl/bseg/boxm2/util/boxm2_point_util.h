#ifndef boxm2_point_util_h
#define boxm2_point_util_h
//:
// \file
#include <vpgl/vpgl_perspective_camera.h>
#include <bwm/video/bwm_video_corr_sptr.h>
#include <bwm/video/bwm_video_corr.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_algorithm.h>
//#include <vcl_cstdlib.h>

typedef vpgl_perspective_camera<double> CamType;

//: Utility class to manipulate sceen points/cameras,
class boxm2_point_util
{
  public :
    //take 3d points and fit plane
    static bool fit_plane_ransac( vcl_vector<vgl_homg_point_3d<double> > & points,
                                  vgl_homg_plane_3d<double>              & plane);


    //given a list of video correspondance points and cameras, axis align scene
    static bool axis_align_scene( vcl_vector<bwm_video_corr_sptr>        & corrs,
                                  vcl_vector<CamType> & cams);

    //simple stddev calculation given a vector of points
    static vnl_double_3 stddev( vcl_vector<vgl_point_3d<double> > const& v);

    // Calc projection error
    static void calc_projection_error(vcl_vector<CamType>                & cams,
                                      vcl_set<int>                       & bad_cams,
                                      vcl_vector<bwm_video_corr_sptr>    & corrs,
                                      vcl_map<unsigned,double>           & view_error_map,
                                      vcl_map<unsigned,unsigned>         & view_count_map );

    // takes view error map and view count map, and sets bad cams and reports error
    static void report_error( vcl_map<unsigned,double>    & view_error_map,
                              vcl_map<unsigned,unsigned>  & view_count_map,
                              vcl_set<int>                & bad_cams,
                              float                       filter_thresh = 1.5f);
};

#endif // boxm2_point_util_h
