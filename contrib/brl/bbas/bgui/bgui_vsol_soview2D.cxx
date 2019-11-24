//:
// \file
#include <iostream>
#include "bgui_vsol_soview2D.h"

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vnl/vnl_math.h"

#include "vgui/vgui_gl.h"
#include "vgui/vgui_soview2D.h"

#include "vgl/vgl_distance.h"
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_poly_set_2d.h>
#include <vsol/vsol_digital_curve_2d.h>

//--------------------------------------------------------------------------
//: vsol_spatial_object_2d view
//-------------------------------------------------------------------------
bgui_vsol_soview2D::bgui_vsol_soview2D( vsol_spatial_object_2d_sptr const & pt)
  : sptr_(pt)
{
}

std::ostream& bgui_vsol_soview2D::print(std::ostream& s) const
{
  this->sptr_->describe(s);
  return vgui_soview2D::print(s);
}


//--------------------------------------------------------------------------
//: vsol_point_2d view
//--------------------------------------------------------------------------

bgui_vsol_soview2D_point::bgui_vsol_soview2D_point( vsol_point_2d_sptr const & pt)
 : bgui_vsol_soview2D(pt.ptr())
{
}

vsol_point_2d_sptr bgui_vsol_soview2D_point::sptr() const
{
  return sptr_->cast_to_point();
}

void bgui_vsol_soview2D_point::draw() const
{
  glBegin(GL_POINTS);
  glVertex2f(static_cast<float>(sptr()->x()), static_cast<float>(sptr()->y()));
  glEnd();
}

float bgui_vsol_soview2D_point::distance_squared(float x, float y) const
{
  float dx = (float)sptr()->x() - x;
  float dy = (float)sptr()->y() - y;
  return dx*dx + dy*dy;
}

void bgui_vsol_soview2D_point::get_centroid(float* x, float* y) const
{
  *x = (float)sptr()->x();
  *y = (float)sptr()->y();
}

void bgui_vsol_soview2D_point::translate(float tx, float ty)
{
  sptr()->set_x( sptr()->x() + tx );
  sptr()->set_y( sptr()->y() + ty );
}

//--------------------------------------------------------------------------
//: vsol_line_2d view
//--------------------------------------------------------------------------

bgui_vsol_soview2D_line_seg::bgui_vsol_soview2D_line_seg( vsol_line_2d_sptr const & line)
  : bgui_vsol_soview2D(line.ptr())
{
}

vsol_line_2d_sptr bgui_vsol_soview2D_line_seg::sptr() const
{
  return sptr_->cast_to_curve()->cast_to_line();
}

void bgui_vsol_soview2D_line_seg::draw() const
{
  glBegin(GL_LINES);
  glVertex2f(static_cast<float>(sptr()->p0()->x()), static_cast<float>(sptr()->p0()->y()));
  glVertex2f(static_cast<float>(sptr()->p1()->x()), static_cast<float>(sptr()->p1()->y()));
  glEnd();
}

float bgui_vsol_soview2D_line_seg::distance_squared(float x, float y) const
{
  return (float)vgl_distance2_to_linesegment((float)sptr()->p0()->x(), (float)sptr()->p0()->y(),
                                             (float)sptr()->p1()->x(), (float)sptr()->p1()->y(),
                                             x, y);
}

void bgui_vsol_soview2D_line_seg::get_centroid(float* x, float* y) const
{
  *x = static_cast<float>(sptr()->p0()->x() + sptr()->p1()->x()) / 2;
  *y = static_cast<float>(sptr()->p0()->y() + sptr()->p1()->y()) / 2;
}

void bgui_vsol_soview2D_line_seg::translate(float tx, float ty)
{
  sptr()->p0()->set_x( sptr()->p0()->x() + tx );
  sptr()->p0()->set_y( sptr()->p0()->y() + ty );
  sptr()->p1()->set_x( sptr()->p1()->x() + tx );
  sptr()->p1()->set_y( sptr()->p1()->y() + ty );
}

//--------------------------------------------------------------------------
//: vsol_conic_2d view - currently restricted to type: real ellipse
//--------------------------------------------------------------------------

bgui_vsol_soview2D_conic_seg::bgui_vsol_soview2D_conic_seg( vsol_conic_2d_sptr const & conic)
  : bgui_vsol_soview2D(conic.ptr())
{
  if (!conic||!conic->is_real_ellipse())
  {
    xc_ = 0; yc_ =0;
    major_axis_ = 0; minor_axis_ = 0;
    angle_ = 0;
    start_angle_ = 0;
    end_angle_ = 0;
    return;
  }
  conic->ellipse_parameters(xc_, yc_, angle_, major_axis_, minor_axis_);

  // compute the angle at p0
  vsol_point_2d_sptr p0 = conic->p0();
  start_angle_ = conic->ellipse_angular_position(p0);

  // compute the angle at p1
  vsol_point_2d_sptr p1 = conic->p1();
  end_angle_ = conic->ellipse_angular_position(p1);
  if (end_angle_<=start_angle_)
    end_angle_ = vnl_math::twopi + end_angle_;
}

vsol_conic_2d_sptr bgui_vsol_soview2D_conic_seg::sptr() const
{
  return sptr_->cast_to_curve()->cast_to_conic();
}
// the convention is that the segment extends from p0 to p1 in a
// counter-clockwise angular sense, i.e. positive phi.
void bgui_vsol_soview2D_conic_seg::draw() const
{
  if (start_angle_==end_angle_)
    return;

  // Increments of 1 degree should be adequate
  double one_degree = vnl_math::pi_over_180;

  double px, py;
  glBegin(GL_LINE_STRIP);
  for (double phi = start_angle_; phi<=end_angle_; phi+=one_degree)
  {
    px = major_axis_*std::cos(angle_)*std::cos(phi)
      - minor_axis_*std::sin(angle_)*std::sin(phi);

    py = minor_axis_*std::cos(angle_)*std::sin(phi)
      + major_axis_*std::sin(angle_)*std::cos(phi);

    glVertex2d(xc_+px, yc_+py);
  }
  glEnd();
}

float bgui_vsol_soview2D_conic_seg::distance_squared(float x, float y) const
{
  vsol_point_2d_sptr p = new vsol_point_2d(x, y);
  double d = sptr()->distance(p);
  return static_cast<float>(d*d);
}

void bgui_vsol_soview2D_conic_seg::get_centroid(float* x, float* y) const
{
  *x = (float)xc_;
  *y = (float)yc_;
}

void bgui_vsol_soview2D_conic_seg::translate(float tx, float ty)
{
  double txd = static_cast<double>(tx), tyd = static_cast<double>(ty);
  //first translate the endpoints
  vsol_point_2d_sptr p0 = sptr()->p0();
  p0->set_x(p0->x()+tx);   p0->set_y(p0->y()+ty);
  vsol_point_2d_sptr p1 = sptr()->p1();
  p1->set_x(p1->x()+tx);   p1->set_y(p1->y()+ty);

  //compute new d, e, f coefficients for the conic
  double a = sptr()->a(), b = sptr()->b(), c = sptr()->c(),
         d = sptr()->d(), e = sptr()->e(), f = sptr()->f();
  double dp = ( d + 2.0*a*txd + b*ty );
  double ep = ( e + 2.0*c*tyd + b*tx );
  double fp = ( f + a*txd*txd + b*txd*tyd + c*tyd*tyd + d*tx + e*ty);
  sptr()->set(a, b, c, dp, ep, fp);
}

//--------------------------------------------------------------------------
//: vsol_polyline_2d view
//--------------------------------------------------------------------------

bgui_vsol_soview2D_polyline::bgui_vsol_soview2D_polyline(vsol_polyline_2d_sptr const& pline)
  : bgui_vsol_soview2D(pline.ptr())
{
}

vsol_polyline_2d_sptr bgui_vsol_soview2D_polyline::sptr() const
{
  return sptr_->cast_to_curve()->cast_to_polyline();
}

void bgui_vsol_soview2D_polyline::draw() const
{
  unsigned int n = sptr()->size();

  glBegin( GL_LINE_STRIP );
  for (unsigned int i=0; i<n;i++)
  {
    glVertex2f( static_cast<float>(sptr()->vertex(i)->x()), static_cast<float>(sptr()->vertex(i)->y()) );
  }
  glEnd();
}

float bgui_vsol_soview2D_polyline::distance_squared(float x, float y) const
{
  unsigned int n = sptr()->size();

  float* xptr = new float[n];
  float* yptr = new float[n];
  for (unsigned int i=0; i<n;i++)
  {
    xptr[i] = (float)sptr()->vertex(i)->x();
    yptr[i] = (float)sptr()->vertex(i)->y();
  }

  double tmp = vgl_distance_to_non_closed_polygon( xptr , yptr , n , x , y );

  delete [] xptr;
  delete [] yptr;

  return static_cast<float>(tmp * tmp);
}

void bgui_vsol_soview2D_polyline::get_centroid(float* x, float* y) const
{
  unsigned int n = sptr()->size();

  *x = 0;
  *y = 0;

  for (unsigned int i=0; i<n;i++)
  {
    *x += (float)sptr()->vertex(i)->x();
    *y += (float)sptr()->vertex(i)->y();
  }
  float s = 1.0f / n;
  *x *= s;
  *y *= s;
}

void bgui_vsol_soview2D_polyline::translate(float tx, float ty)
{
  unsigned int n = sptr()->size();

  for (unsigned int i=0; i<n;i++)
  {
    sptr()->vertex(i)->set_x( sptr()->vertex(i)->x() + tx );
    sptr()->vertex(i)->set_y( sptr()->vertex(i)->y() + ty );
  }
}

//--------------------------------------------------------------------------
//: vsol_digital_curve_2d view
//--------------------------------------------------------------------------

bgui_vsol_soview2D_digital_curve::bgui_vsol_soview2D_digital_curve(vsol_digital_curve_2d_sptr const& dc,
                                                                   bool dotted)
  : bgui_vsol_soview2D(dc.ptr()), draw_dotted_(dotted)
{
}

vsol_digital_curve_2d_sptr bgui_vsol_soview2D_digital_curve::sptr() const
{
  return sptr_->cast_to_curve()->cast_to_digital_curve();
}

void bgui_vsol_soview2D_digital_curve::draw() const
{
  unsigned int n = sptr()->size();

  glBegin( GL_LINE_STRIP );
  for (unsigned int i=0; i<n;i++)
  {
    glVertex2f( static_cast<float>(sptr()->point(i)->x()), static_cast<float>(sptr()->point(i)->y()) );
  }
  glEnd();

  if (draw_dotted_)
  {
    glBegin(GL_POINTS);
    for (unsigned int i=0; i<n;i++)
    {
      glVertex2f( static_cast<float>(sptr()->point(i)->x()), static_cast<float>(sptr()->point(i)->y()) );
    }
    glEnd();
  }
}

float bgui_vsol_soview2D_digital_curve::distance_squared(float x, float y) const
{
  unsigned int n = sptr()->size();

  float* xptr = new float[n];
  float* yptr = new float[n];
  for (unsigned int i=0; i<n;i++)
  {
    xptr[i] = (float)sptr()->point(i)->x();
    yptr[i] = (float)sptr()->point(i)->y();
  }

  double tmp = vgl_distance_to_non_closed_polygon( xptr , yptr , n , x , y );

  delete [] xptr;
  delete [] yptr;

  return static_cast<float>(tmp * tmp);
}

void bgui_vsol_soview2D_digital_curve::get_centroid(float* x, float* y) const
{
  unsigned int n = sptr()->size();

  *x = 0;
  *y = 0;

  for (unsigned int i=0; i<n;i++)
  {
    *x += (float)sptr()->point(i)->x();
    *y += (float)sptr()->point(i)->y();
  }
  float s = 1.0f / float( n );
  *x *= s;
  *y *= s;
}

void bgui_vsol_soview2D_digital_curve::translate(float tx, float ty)
{
  unsigned int n = sptr()->size();

  for (unsigned int i=0; i<n;i++)
  {
    vsol_point_2d_sptr pt = sptr()->point(i);
    pt->set_x( pt->x() + tx );
    pt->set_y( pt->y() + ty );
  }
}

//--------------------------------------------------------------------------
//: vdgl_digital_curve regular view
//--------------------------------------------------------------------------

bgui_vsol_soview2D_edgel_curve::bgui_vsol_soview2D_edgel_curve(vdgl_digital_curve_sptr const& e,
                                                               bool dotted)
  : bgui_vsol_soview2D(e.ptr()), draw_dotted_(dotted)
{
}

vdgl_digital_curve_sptr bgui_vsol_soview2D_edgel_curve::sptr() const
{
  return (vdgl_digital_curve*)(sptr_.ptr());
}

void bgui_vsol_soview2D_edgel_curve::draw() const
{
  //get the edgel chain
  vdgl_interpolator_sptr itrp = this->sptr()->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  unsigned int n = ech->size();

  glBegin( GL_LINE_STRIP );
  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    glVertex2f( static_cast<float>(ed.get_x()), static_cast<float>(ed.get_y()) );
  }
  glEnd();

  if (draw_dotted_)
  {
    glBegin(GL_POINTS);
    for (unsigned int i=0; i<n;i++)
    {
      vdgl_edgel ed = (*ech)[i];
      glVertex2f( static_cast<float>(ed.get_x()), static_cast<float>(ed.get_y()) );
    }
    glEnd();
  }
}

float bgui_vsol_soview2D_edgel_curve::distance_squared( float x , float y ) const
{
  //get the edgel chain
  vdgl_interpolator_sptr itrp = this->sptr()->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  unsigned int n = ech->size();

  float* xptr = new float[n];
  float* yptr = new float[n];
  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    xptr[i] = (float)ed.get_x();
    yptr[i] = (float)ed.get_y();
  }

  double tmp = vgl_distance_to_non_closed_polygon( xptr , yptr , n , x , y );

  delete [] xptr;
  delete [] yptr;

  return static_cast<float>(tmp * tmp);
}

void bgui_vsol_soview2D_edgel_curve::get_centroid( float* x, float* y ) const
{
  //get the edgel chain
  vdgl_interpolator_sptr itrp = this->sptr()->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  unsigned int n = ech->size();

  *x = 0;
  *y = 0;

  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    *x += (float)ed.get_x();
    *y += (float)ed.get_y();
  }
  float s = 1.0f / float( n );
  *x *= s;
  *y *= s;
}

void bgui_vsol_soview2D_edgel_curve::translate( float x , float y )
{
  //get the edgel chain
  vdgl_interpolator_sptr itrp = this->sptr()->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  unsigned int n = ech->size();

  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    ed.set_x( ed.get_x() + x );
    ed.set_y( ed.get_y() + y );
  }
}

//--------------------------------------------------------------------------
//: vsol_polygon_2d view
//--------------------------------------------------------------------------

bgui_vsol_soview2D_polygon::bgui_vsol_soview2D_polygon(vsol_polygon_2d_sptr const& e)
  : bgui_vsol_soview2D(e.ptr())
{
}

vsol_polygon_2d_sptr bgui_vsol_soview2D_polygon::sptr() const
{
  return sptr_->cast_to_region()->cast_to_polygon();
}

void bgui_vsol_soview2D_polygon::draw() const
{
  unsigned int n = sptr()->size();

  glBegin( GL_LINE_LOOP );
  for (unsigned int i=0; i<n;i++)
  {
    glVertex2f( static_cast<float>(sptr()->vertex(i)->x()), static_cast<float>(sptr()->vertex(i)->y()) );
  }
  glEnd();
}

float bgui_vsol_soview2D_polygon::distance_squared( float x , float y ) const
{
  unsigned int n = sptr()->size();

  float* xptr = new float[n];
  float* yptr = new float[n];
  for (unsigned int i=0; i<n;i++)
  {
    xptr[i] = (float)sptr()->vertex(i)->x();
    yptr[i] = (float)sptr()->vertex(i)->y();
  }

  double tmp = vgl_distance_to_closed_polygon( xptr , yptr , n , x , y );

  delete [] xptr;
  delete [] yptr;

  return static_cast<float>(tmp * tmp);
}

void bgui_vsol_soview2D_polygon::get_centroid( float* x, float* y ) const
{
  unsigned int n = sptr()->size();

  *x = 0;
  *y = 0;

  for (unsigned int i=0; i<n;i++)
  {
    *x += (float)sptr()->vertex(i)->x();
    *y += (float)sptr()->vertex(i)->y();
  }
  float s = 1.0f / n;
  *x *= s;
  *y *= s;
}

void bgui_vsol_soview2D_polygon::translate( float x , float y )
{
  unsigned int n = sptr()->size();

  for (unsigned int i=0; i<n;i++)
  {
    sptr()->vertex(i)->set_x( sptr()->vertex(i)->x() + x );
    sptr()->vertex(i)->set_y( sptr()->vertex(i)->y() + y );
  }
}

bgui_vsol_soview2D_polygon_set::bgui_vsol_soview2D_polygon_set(vsol_poly_set_2d_sptr const& e)
: bgui_vsol_soview2D(e.ptr())
{
}

vsol_poly_set_2d_sptr bgui_vsol_soview2D_polygon_set::sptr() const
{
  return sptr_->cast_to_region()->cast_to_poly_set();
}

void bgui_vsol_soview2D_polygon_set::draw() const
{
  unsigned int n = sptr()->size();
  for (unsigned int i=0; i<n;i++)
  {
    bgui_vsol_soview2D_polygon poly(sptr()->poly(i));
    poly.draw();
  }
}

float bgui_vsol_soview2D_polygon_set::distance_squared( float x , float y ) const
{
  unsigned int n = sptr()->size();
  float d=0;
  for (unsigned int i=0; i<n;i++)
  {
    bgui_vsol_soview2D_polygon poly(sptr()->poly(i));
    d += poly.distance_squared(x,y);
  }

  return d/n;
}

void bgui_vsol_soview2D_polygon_set::get_centroid( float* x, float* y ) const
{
  unsigned int n = sptr()->size();
  for (unsigned int i=0; i<n;i++)
  {
    bgui_vsol_soview2D_polygon poly(sptr()->poly(i));
    poly.get_centroid(x,y);
  }
}

void bgui_vsol_soview2D_polygon_set::translate(float x , float y)
{
  unsigned int n = sptr()->size();
  for (unsigned int i=0; i<n;i++)
  {
    bgui_vsol_soview2D_polygon poly(sptr()->poly(i));
    poly.translate(x,y);
  }
}
