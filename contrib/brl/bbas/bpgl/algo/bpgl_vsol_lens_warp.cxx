// This is bbas/bpgl/algo/bpgl_vsol_lens_warp.cxx
#include <list>
#include <iostream>
#include "bpgl_vsol_lens_warp.h"
//:
// \file

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Apply lens distortion to this vsol_spatial_object_2d and produce a new spatial object
vsol_spatial_object_2d_sptr
bpgl_vsol_lens_warp(const vsol_spatial_object_2d_sptr& obj,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert,
                    double midpt_thresh)
{
  if (vsol_point_2d_sptr pt = obj->cast_to_point())
  {
    return bpgl_vsol_lens_warp(pt, lens, invert, midpt_thresh).ptr();
  }
  else if (vsol_curve_2d* curve = obj->cast_to_curve())
  {
    if (vsol_line_2d_sptr line = curve->cast_to_line())
    {
      return bpgl_vsol_lens_warp(line, lens, invert, midpt_thresh).ptr();
    }
    else if (vsol_polyline_2d_sptr pline = curve->cast_to_polyline())
    {
      return bpgl_vsol_lens_warp(pline, lens, invert, midpt_thresh).ptr();
    }
    else if (vsol_digital_curve_2d_sptr dc = curve->cast_to_digital_curve())
    {
      return bpgl_vsol_lens_warp(dc, lens, invert, midpt_thresh).ptr();
    }
  }
  else if (vsol_region_2d* region = obj->cast_to_region())
  {
    if (vsol_polygon_2d_sptr pg = region->cast_to_polygon())
    {
      return bpgl_vsol_lens_warp(pg, lens, invert, midpt_thresh).ptr();
    }
  }
  return nullptr;
}

//: Apply lens distortion to this vsol_point_2d and produce a new point
vsol_point_2d_sptr
bpgl_vsol_lens_warp(const vsol_point_2d_sptr& pt,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert,
                    double midpt_thresh)
{
  if ( midpt_thresh <= 0 )
    return nullptr;
  if (invert)
    return new vsol_point_2d( lens.undistort( vgl_homg_point_2d<double>(pt->get_p()) ) );
  else
    return new vsol_point_2d( lens.distort( vgl_homg_point_2d<double>(pt->get_p()) ) );
}


//: Recursively divide the line segment in half as long as the midpoint moves more than \p midpt_thresh
static bool
bpgl_vsol_warp_divide(const vsol_line_2d_sptr& ln,
                      const vsol_line_2d_sptr& wln,
                      const vpgl_lens_distortion<double>& lens,
                      bool invert,
                      double midpt_thresh,
                      std::list<vsol_point_2d_sptr>& pts)
{
  if ( midpt_thresh <= 0 )
    return false;
  vsol_point_2d_sptr mp = ln->middle();
  vsol_point_2d_sptr dmp = bpgl_vsol_lens_warp(mp,lens,invert,midpt_thresh);
  vsol_point_2d_sptr wmp = wln->middle();
  if (vgl_distance(wmp->get_p(), dmp->get_p()) < midpt_thresh)
    return false;

  std::list<vsol_point_2d_sptr> pts1, pts2;
  bpgl_vsol_warp_divide(new vsol_line_2d(ln->p0(),mp),
                         new vsol_line_2d(wln->p0(),dmp),
                         lens, invert, midpt_thresh, pts1);
  bpgl_vsol_warp_divide(new vsol_line_2d(mp,ln->p1()),
                         new vsol_line_2d(dmp,wln->p1()),
                         lens, invert, midpt_thresh, pts2);

  pts.splice(pts.end(),pts1);
  pts.push_back(dmp);
  pts.splice(pts.end(),pts2);
  return true;
}


//: Apply lens distortion to this vsol_line_2d and produce a new line
vsol_curve_2d_sptr
bpgl_vsol_lens_warp(const vsol_line_2d_sptr& ln,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert,
                    double midpt_thresh)
{
  vsol_point_2d_sptr p0 = bpgl_vsol_lens_warp(ln->p0(), lens, invert, midpt_thresh);
  vsol_point_2d_sptr p1 = bpgl_vsol_lens_warp(ln->p1(), lens, invert, midpt_thresh);
  if (!p0 || !p1)
    return nullptr;
  vsol_line_2d_sptr line = new vsol_line_2d(p0,p1);
  std::list<vsol_point_2d_sptr> pts;
  bpgl_vsol_warp_divide(ln,line,lens,invert,midpt_thresh,pts);
  if (pts.empty())
    return line.ptr();
  pts.push_front(p0);
  pts.push_back(p1);
  std::vector<vsol_point_2d_sptr> v_pts;
  for (auto & pt : pts)
    v_pts.push_back(pt);
  return new vsol_polyline_2d(v_pts);
}


//: Apply lens distortion to this vsol_polyline_2d and produce a new polyline
vsol_polyline_2d_sptr
bpgl_vsol_lens_warp(const vsol_polyline_2d_sptr& pln,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert,
                    double midpt_thresh)
{
  std::list<vsol_point_2d_sptr> pts;
  vsol_point_2d_sptr last_p = pln->p0();
  vsol_point_2d_sptr last_dp = bpgl_vsol_lens_warp(last_p, lens, invert, midpt_thresh);
  pts.push_back(last_dp);
  for (unsigned int i=1; i<pln->size(); ++i)
  {
    vsol_point_2d_sptr p = pln->vertex(i);
    vsol_point_2d_sptr dp = bpgl_vsol_lens_warp(p, lens, invert, midpt_thresh);
    if (!dp)
      return nullptr;
    vsol_line_2d_sptr ln = new vsol_line_2d(last_p,p);
    vsol_line_2d_sptr wln = new vsol_line_2d(last_dp,dp);
    std::list<vsol_point_2d_sptr> new_pts;
    bpgl_vsol_warp_divide(ln,wln,lens,invert,midpt_thresh,new_pts);
    pts.splice(pts.end(),new_pts);
    pts.push_back(dp);
    last_p = p;
    last_dp = dp;
  }
  std::vector<vsol_point_2d_sptr> v_pts;
  for (auto & pt : pts)
    v_pts.push_back(pt);
  return new vsol_polyline_2d(v_pts);
}


//: Apply lens distortion to this vsol_digital_curve_2d and produce a new digital curve
vsol_digital_curve_2d_sptr
bpgl_vsol_lens_warp(const vsol_digital_curve_2d_sptr& dc,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert,
                    double midpt_thresh)
{
  std::list<vsol_point_2d_sptr> pts;
  vsol_point_2d_sptr last_p = dc->p0();
  vsol_point_2d_sptr last_dp = bpgl_vsol_lens_warp(last_p, lens, invert, midpt_thresh);
  pts.push_back(last_dp);
  for (unsigned int i=1; i<dc->size(); ++i){
    vsol_point_2d_sptr p = dc->point(i);
    vsol_point_2d_sptr dp = bpgl_vsol_lens_warp(p, lens, invert, midpt_thresh);
    if (!dp)
      return nullptr;
    vsol_line_2d_sptr ln = new vsol_line_2d(last_p,p);
    vsol_line_2d_sptr wln = new vsol_line_2d(last_dp,dp);
    std::list<vsol_point_2d_sptr> new_pts;
    bpgl_vsol_warp_divide(ln,wln,lens,invert,midpt_thresh,new_pts);
    pts.splice(pts.end(),new_pts);
    pts.push_back(dp);
    last_p = p;
    last_dp = dp;
  }
  std::vector<vsol_point_2d_sptr> v_pts;
  for (auto & pt : pts)
    v_pts.push_back(pt);
  return new vsol_digital_curve_2d(v_pts);
}


//: Apply lens distortion to this vsol_polygon_2d and produce a new polygon
vsol_polygon_2d_sptr
bpgl_vsol_lens_warp(const vsol_polygon_2d_sptr& pg,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert,
                    double midpt_thresh)
{
  std::list<vsol_point_2d_sptr> pts;
  vsol_point_2d_sptr last_p = pg->vertex(pg->size()-1);
  vsol_point_2d_sptr last_dp = bpgl_vsol_lens_warp(last_p, lens, invert, midpt_thresh);
  for (unsigned int i=0; i<pg->size(); ++i){
    vsol_point_2d_sptr p = pg->vertex(i);
    vsol_point_2d_sptr dp = bpgl_vsol_lens_warp(p, lens, invert, midpt_thresh);
    if (!dp)
      return nullptr;
    vsol_line_2d_sptr ln = new vsol_line_2d(last_p,p);
    vsol_line_2d_sptr wln = new vsol_line_2d(last_dp,dp);
    std::list<vsol_point_2d_sptr> new_pts;
    bpgl_vsol_warp_divide(ln,wln,lens,invert,midpt_thresh,new_pts);
    pts.splice(pts.end(),new_pts);
    pts.push_back(dp);
    last_p = p;
    last_dp = dp;
  }
  std::vector<vsol_point_2d_sptr> v_pts;
  for (auto & pt : pts)
    v_pts.push_back(pt);
  return new vsol_polygon_2d(v_pts);
}

//================================================================

//: Apply lens distortion to this vsol_spatial_object_2d in place
// \return true if successful
bool
bpgl_vsol_lens_warp(vsol_spatial_object_2d_sptr& obj,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert)
{
  if (invert)
    std::cerr << "Warning: invert not yet implemented in bpgl_vsol_lens_warp\n";

  if (vsol_point_2d_sptr pt = obj->cast_to_point())
  {
    return bpgl_vsol_lens_warp(pt, lens);
  }
  else if (vsol_curve_2d* curve = obj->cast_to_curve())
  {
    if (vsol_line_2d_sptr line = curve->cast_to_line())
    {
      return bpgl_vsol_lens_warp(line, lens);
    }
    else if (vsol_polyline_2d_sptr pline = curve->cast_to_polyline())
    {
      return bpgl_vsol_lens_warp(pline, lens);
    }
    else if (vsol_digital_curve_2d_sptr dc = curve->cast_to_digital_curve())
    {
      return bpgl_vsol_lens_warp(dc, lens);
    }
  }
  else if (vsol_region_2d* region = obj->cast_to_region())
  {
    if (vsol_polygon_2d_sptr pg = region->cast_to_polygon())
    {
      return bpgl_vsol_lens_warp(pg, lens);
    }
  }
  return false;
}


//: Apply lens distortion to this vsol_point_2d in place
// \return true if successful
bool
bpgl_vsol_lens_warp(vsol_point_2d_sptr& pt,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert)
{
  vgl_point_2d<double> new_pt;
  if (invert)
    new_pt = lens.undistort( vgl_homg_point_2d<double>(pt->get_p()) );
  else
    new_pt = lens.distort( vgl_homg_point_2d<double>(pt->get_p()) );
  pt->set_x(new_pt.x());
  pt->set_y(new_pt.y());
  return true;
}


//: Apply lens distortion to this vsol_line_2d in place
// \return true if successful
// \note only the end points are warped
bool
bpgl_vsol_lens_warp(vsol_line_2d_sptr& ln,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert)
{
  if (invert)
    std::cerr << "Warning: line invert not yet implemented in bpgl_vsol_lens_warp\n";

  vsol_point_2d_sptr p0=ln->p0(), p1=ln->p1();
  return bpgl_vsol_lens_warp(p0, lens) &&
         bpgl_vsol_lens_warp(p1, lens);
}

//: Apply lens distortion to this vsol_polyline_2d in place
// \return true if successful
// \note only the segment end points are warped
bool
bpgl_vsol_lens_warp(vsol_polyline_2d_sptr& pln,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert)
{
  if (invert)
    std::cerr << "Warning: polyline invert not yet implemented in bpgl_vsol_lens_warp\n";

  for (unsigned int i=0; i<pln->size(); ++i){
    vsol_point_2d_sptr p=pln->vertex(i);
    if (!bpgl_vsol_lens_warp(p, lens))
      return false;
  }
  return true;
}


//: Apply lens distortion to this vsol_digital_curve_2d in place
// \return true if successful
// \note only the sample points are warped
bool
bpgl_vsol_lens_warp(vsol_digital_curve_2d_sptr& dc,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert)
{
  if (invert)
    std::cerr << "Warning: curve invert not yet implemented in bpgl_vsol_lens_warp\n";

  for (unsigned int i=0; i<dc->size(); ++i) {
    vsol_point_2d_sptr p=dc->point(i);
    if (!bpgl_vsol_lens_warp(p, lens))
      return false;
  }
  return true;
}


//: Apply lens distortion to this vsol_polygon_2d in place
// \return true if successful
// \note only the segment end points are warped
bool
bpgl_vsol_lens_warp(vsol_polygon_2d_sptr& pg,
                    const vpgl_lens_distortion<double>& lens,
                    bool invert)
{
  if (invert)
    std::cerr << "Warning: polygon invert not yet implemented in bpgl_vsol_lens_warp\n";

  for (unsigned int i=0; i<pg->size(); ++i){
    vsol_point_2d_sptr p=pg->vertex(i);
    if (!bpgl_vsol_lens_warp(p, lens))
      return false;
  }
  return true;
}
