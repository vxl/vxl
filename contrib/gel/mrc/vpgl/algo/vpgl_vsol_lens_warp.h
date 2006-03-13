// This is gel/mrc/vpgl/algo/vpgl_vsol_lens_warp.h
#ifndef vpgl_vsol_lens_warp_h_
#define vpgl_vsol_lens_warp_h_
//:
// \file
// \brief Apply lens distortion to vsol objects
// \author Matt Leotta
// \date 8/24/05
//

#include <vpgl/vpgl_lens_distortion.h>
#include <vpgl/algo/vpgl_lens_warp_mapper.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_curve_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>


//: Apply lens distortion to this vsol_spatial_object_2d and produce a new spatial object
// \param undistort applies the inverse of the distortion
// \param midpt_thresh is the threshold in the error of midpoints of line segments
// \note if \p midpt_thresh is negative it is ignored
vsol_spatial_object_2d_sptr
vpgl_vsol_lens_warp(const vsol_spatial_object_2d_sptr& obj,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert = false,
                    double midpt_thresh = -1.0);

//: Apply lens distortion to this vsol_point_2d and produce a new point
vsol_point_2d_sptr
vpgl_vsol_lens_warp(const vsol_point_2d_sptr& pt,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert = false,
                    double midpt_thresh = -1.0);


//: Apply lens distortion to this vsol_line_2d and produce a new curve (polyline)
vsol_curve_2d_sptr
vpgl_vsol_lens_warp(const vsol_line_2d_sptr& ln,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert = false,
                    double midpt_thresh = -1.0);


//: Apply lens distortion to this vsol_polyline_2d and produce a new polyline
vsol_polyline_2d_sptr
vpgl_vsol_lens_warp(const vsol_polyline_2d_sptr& pln,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert = false,
                    double midpt_thresh = -1.0);


//: Apply lens distortion to this vsol_digital_curve_2d and produce a new digital curve
vsol_digital_curve_2d_sptr
vpgl_vsol_lens_warp(const vsol_digital_curve_2d_sptr& dc,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert = false,
                    double midpt_thresh = -1.0);


//: Apply lens distortion to this vsol_polygon_2d and produce a new polygon
vsol_polygon_2d_sptr
vpgl_vsol_lens_warp(const vsol_polygon_2d_sptr& pg,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert = false,
                    double midpt_thresh = -1.0);

//===============================================================

//: Apply lens distortion to this vsol_spatial_object_2d in place
// \return true if successful
bool
vpgl_vsol_lens_warp(vsol_spatial_object_2d_sptr& obj,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert = false);


//: Apply lens distortion to this vsol_point_2d in place
// \return true if successful
bool
vpgl_vsol_lens_warp(vsol_point_2d_sptr& pt,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert = false);


//: Apply lens distortion to this vsol_line_2d in place
// \return true if successful
// \note only the end points are warped
bool
vpgl_vsol_lens_warp(vsol_line_2d_sptr& ln,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert = false);


//: Apply lens distortion to this vsol_polyline_2d in place
// \return true if successful
// \note only the segment end points are warped
bool
vpgl_vsol_lens_warp(vsol_polyline_2d_sptr& pln,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert = false);


//: Apply lens distortion to this vsol_digital_curve_2d in place
// \return true if successful
// \note only the sample points are warped
bool
vpgl_vsol_lens_warp(vsol_digital_curve_2d_sptr& dc,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert = false);


//: Apply lens distortion to this vsol_polygon_2d in place
// \return true if successful
// \note only the segment end points are warped
bool
vpgl_vsol_lens_warp(vsol_polygon_2d_sptr& pg,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert = false);

#endif // vpgl_vsol_lens_warp_h_
