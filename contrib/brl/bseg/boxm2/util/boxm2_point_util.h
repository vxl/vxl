#ifndef boxm2_point_util_h
#define boxm2_point_util_h
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
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//#include <cstdlib>

typedef vpgl_perspective_camera<double> CamType;

//: Utility class to manipulate sceen points/cameras,
class boxm2_point_util
{
  public :
    //take 3d points and fit plane
    static bool fit_plane_ransac( std::vector<vgl_homg_point_3d<double> > & points,
                                  vgl_homg_plane_3d<double>              & plane);


    //given a list of video correspondance points and cameras, axis align scene
    static bool axis_align_scene( std::vector<bwm_video_corr_sptr>        & corrs,
                                  std::vector<CamType> & cams);

    //simple stddev calculation given a vector of points
    static vnl_double_3 stddev( std::vector<vgl_point_3d<double> > const& v);

    // Calc projection error
    static void calc_projection_error(std::vector<CamType>                & cams,
                                      std::set<int>                       & bad_cams,
                                      std::vector<bwm_video_corr_sptr>    & corrs,
                                      std::map<unsigned,double>           & view_error_map,
                                      std::map<unsigned,unsigned>         & view_count_map );

    // takes view error map and view count map, and sets bad cams and reports error
    static void report_error( std::map<unsigned,double>    & view_error_map,
                              std::map<unsigned,unsigned>  & view_count_map,
                              std::set<int>                & bad_cams,
                              float                       filter_thresh = 1.5f);
};

#endif // boxm2_point_util_h
