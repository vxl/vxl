//:
// \file
#include "bgui_bmrf_soview2D.h"
#include <bmrf/bmrf_epi_seg.h>
#include <bmrf/bmrf_epi_point_sptr.h>
#include <bmrf/bmrf_epi_point.h>

#include <vcl_iostream.h>
#include <vcl_limits.h> 
#include <vcl_vector.h>

#include <vgl/vgl_distance.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_soview2D.h>


// Constructor
bgui_bmrf_epi_seg_soview2D::bgui_bmrf_epi_seg_soview2D( bmrf_epi_seg_sptr const & seg)
 : seg_sptr_(seg)
{}


//: Render the bmrf_epi_seg on the display.
void
bgui_bmrf_epi_seg_soview2D::draw() const
{
  glBegin(GL_LINE_STRIP);
  vcl_vector<bmrf_epi_point_sptr>::const_iterator p_itr;
  for(p_itr = seg_sptr_->begin(); p_itr != seg_sptr_->end(); ++p_itr)
    glVertex2f((*p_itr)->x(), (*p_itr)->y());
  glEnd();
}


//: Print details about this bmrf_epi_seg to the given stream.
vcl_ostream&
bgui_bmrf_epi_seg_soview2D::print(vcl_ostream&s) const
{
  return s << "[ a bmrf_epi_seg FIXME ]";
}

//: Returns the distance squared from this bmrf_epi_seg to the given position.
float
bgui_bmrf_epi_seg_soview2D::distance_squared(float x, float y) const
{
  if(seg_sptr_->n_pts() == 0) return vcl_numeric_limits<float>::infinity();
  if(seg_sptr_->n_pts() == 1){
    bmrf_epi_point_sptr pt = *(seg_sptr_->begin());
    float dx = x-pt->x();
    float dy = y-pt->y();
    return dx*dx + dy*dy;
  }
  
  float dd = -1.0f;
  vcl_vector<bmrf_epi_point_sptr>::const_iterator p_itr;
  for(p_itr = seg_sptr_->begin(); (p_itr+1) != seg_sptr_->end(); ++p_itr){
    float nd = vgl_distance2_to_linesegment(float((*p_itr)->x()),     float((*p_itr)->y()),
                                            float((*(p_itr+1))->x()), float((*(p_itr+1))->y()),
                                            x, y);
    if (dd<0 || nd<dd)
      dd = nd;
  }
  return dd;
}

//: Returns the centroid of this bmrf_epi_seg.
void
bgui_bmrf_epi_seg_soview2D::get_centroid(float* x, float* y) const
{
  *x = 0;
  *y = 0;
  int n=0;
  vcl_vector<bmrf_epi_point_sptr>::const_iterator p_itr;
  for(p_itr = seg_sptr_->begin(); p_itr != seg_sptr_->end(); ++p_itr, ++n){
    *x += (*p_itr)->x();
    *y += (*p_itr)->y();
  }
  float s = 1.0f / float(n);
  *x *= s;
  *y *= s;
}


