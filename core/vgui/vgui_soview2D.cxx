//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   24 Mar 1999
// \brief  See vgui_soview2D.h for a description of this file.

#include <cmath>
#include <iostream>
#include "vgui_soview2D.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_distance.h>
#include <vnl/vnl_math.h> // for twopi
#include <vil/vil_image_view_base.h>
#include <vil/vil_new.h>
#include <vil1/vil1_image.h>
#include <vgui/vgui_range_map_params.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_section_buffer.h>
#include <vgui/internals/vgui_draw_line.h>

//--------------------------------------------------------------------------//

std::ostream& vgui_soview2D_point::print(std::ostream& s) const
{
  s << "[ vgui_soview2D_point " << x << ',' << y << ' ';
  return vgui_soview2D::print(s) << " ]";
}

void vgui_soview2D_point::draw() const
{
#if 0
  style->apply_point_size();
#endif // 0
  glBegin(GL_POINTS);
    glVertex2f(x,y);
  glEnd();
}

void vgui_soview2D_point::draw_select() const
{
  // It's much faster to draw a polygon than a point. (At least, it is
  // on Win2000 OpenGL.) For selection, we just need to draw some
  // little area in the vicinity of the point. We make the area really
  // small to account for large zoom factors. In principle, we should
  // take the zoom factor into account when determining the radius,
  // but that's too much trouble. The radius should be large enough
  // that it doesn't get swallowed up when added to x and to y.

  // This will allow a 10000x zoom before the "circle" gets bigger
  // than one pixel. Should be good enough!
  //
  float const rad = 0.0001f;
  glBegin(GL_POLYGON);
    glVertex2f( x - rad, y - rad );
    glVertex2f( x + rad, y - rad );
    glVertex2f( x + rad, y + rad );
    glVertex2f( x - rad, y + rad );
  glEnd();
}

float vgui_soview2D_point::distance_squared(float vx, float vy) const
{
  float dx = x - vx;
  float dy = y - vy;
  return dx*dx + dy*dy;
}

void vgui_soview2D_point::get_centroid(float* vx, float* vy) const
{
  *vx = x;
  *vy = y;
}

void vgui_soview2D_point::translate(float tx, float ty)
{
  x += tx;
  y += ty;
}

//--------------------------------------------------------------------------//

std::ostream& vgui_soview2D_lineseg::print(std::ostream& s) const
{
  s << "[ vgui_soview2D_lineseg " << x0 << ',' << y0 << " -- " << x1 << ',' << y1 << ' ';
  return vgui_soview2D::print(s) << " ]";
}

void vgui_soview2D_lineseg::draw() const
{
#ifdef DEBUG
  std::cerr << "vgui_soview2D_lineseg::draw() line id=" << id << '\n';
#endif

#if 0
  glLineWidth(style->line_width);
#endif // 0
  glBegin(GL_LINES);
  glVertex2f(x0,y0);
  glVertex2f(x1,y1);
  glEnd();
}

float vgui_soview2D_lineseg::distance_squared(float vx, float vy) const
{
  // Here we explicitly cast some parameters to type float to help
  // the Borland compiler, which otherwise tries to use
  // vgl_distance2_to_linesegment<const float>, presumably because
  // this is a const member function so some of the parameters passed
  // to vgl_distance2_to_linesegment are effectively of type const
  // float.
  return (float)vgl_distance2_to_linesegment(x0, y0, x1, y1, vx, vy);
}

void vgui_soview2D_lineseg::get_centroid(float* vx, float* vy) const
{
  *vx = (x0 + x1) / 2;
  *vy = (y0 + y1) / 2;
}

void vgui_soview2D_lineseg::translate(float tx, float ty)
{
  x0 += tx;
  y0 += ty;
  x1 += tx;
  y1 += ty;
}

//--------------------------------------------------------------------------//

vgui_soview2D_group::~vgui_soview2D_group()
{
  for (unsigned int i=0; i< ls.size(); i++)
    if ( ls[i] )
      delete ls[i];

  // clear vector
  ls.clear();
}

void vgui_soview2D_group::set_style(const vgui_style_sptr& s)
{
  for (unsigned int i=0; i< ls.size(); i++)
    if (!ls[i]->get_style())
      ls[i]->set_style(s);

  vgui_soview::set_style( s);
}

std::ostream& vgui_soview2D_group::print(std::ostream& s) const
{
  s << "[ vgui_soview2D_group ";

  for (unsigned int i=0; i< ls.size(); i++)
    ls[i]->print(s);

  return vgui_soview2D::print(s) << " ]";
}

void vgui_soview2D_group::draw() const
{
  for (unsigned int i=0; i< ls.size(); i++)
    ls[i]->draw();
}

void vgui_soview2D_group::draw_select() const
{
  for (unsigned int i=0; i< ls.size(); i++)
    ls[i]->draw_select();
}

float vgui_soview2D_group::distance_squared(float vx, float vy) const
{
  if (ls.size() == 0)
    return -1e30f;

  float min= ls[0]->distance_squared( vx, vy);

  for (unsigned int i=1; i< ls.size(); i++)
  {
    float d= ls[i]->distance_squared( vx, vy);
    if ( d< min ) min= d;
  }

  return min;
}

void vgui_soview2D_group::get_centroid(float* vx, float* vy) const
{
  *vx = 0;
  *vy = 0;
  const int n = ls.size();

  for (int i=0; i < n; i++)
  {
    float cx, cy;
    ls[i]->get_centroid(&cx, &cy);
    *vx += cx;
    *vy += cy;
  }

  float s = 1.0f/n;
  *vx *= s;
  *vy *= s;
}

void vgui_soview2D_group::translate(float tx, float ty)
{
  for (unsigned int i=0; i < ls.size(); i++)
    ls[i]->translate(tx, ty);
}

//--------------------------------------------------------------------------//

std::ostream& vgui_soview2D_infinite_line::print(std::ostream& s) const
{
  s << "[ vgui_soview2D_infinite_line " << a << ',' << b << ',' << c << ' ';
  return vgui_soview2D::print(s) << " ]";
}

void vgui_soview2D_infinite_line::draw() const
{
  vgui_draw_line(a, b, c);
}

float vgui_soview2D_infinite_line::distance_squared(float vx, float vy) const
{
  float tmp = a*vx + b*vy + c;
  return tmp*tmp/(a*a + b*b);
}

void vgui_soview2D_infinite_line::get_centroid(float* vx, float* vy) const
{
  *vx = 0;
  *vy = 0;
}

void vgui_soview2D_infinite_line::translate(float tx, float ty)
{
  c += a * tx + b * ty;
}

//--------------------------------------------------------------------------//

constexpr int vgui__CIRCLE2D_LIST = 1;

void vgui_soview2D_circle::compile()
{
  glNewList(vgui__CIRCLE2D_LIST, GL_COMPILE);
  glBegin(GL_LINE_LOOP);
  for (unsigned int i=0;i<100;i++)
  {
    double angle = vnl_math::twopi*0.01*i;
    glVertex2d(std::cos(angle), std::sin(angle));
  }
  glEnd();
  glEndList();
}


std::ostream& vgui_soview2D_circle::print(std::ostream& s) const
{
  s << "[ vgui_soview2D_circle " << x << ',' << y << " r" << r << ' ';
  return vgui_soview2D::print(s) << " ]";
}

void vgui_soview2D_circle::draw() const
{
  glBegin(GL_LINE_LOOP);
  for (unsigned int i=0;i<100;i++)
  {
    double angle = vnl_math::twopi*0.01*i;
    glVertex2d(x+r*std::cos(angle), y+r*std::sin(angle));
  }
  glEnd();
}

float vgui_soview2D_circle::distance_squared(float vx, float vy) const
{
  float dx = x - vx;
  float dy = y - vy;

  // distance from point to centre
  float dcentre = std::sqrt(dx*dx + dy*dy);

  // signed distance from point to circumference
  float dcircum = dcentre - this->r;

  return dcircum * dcircum;
}

void vgui_soview2D_circle::get_centroid(float* vx, float* vy) const
{
  *vx = x;
  *vy = y;
}

void vgui_soview2D_circle::translate(float tx, float ty)
{
  x += tx;
  y += ty;
}

//--------------------------------------------------------------------------------//

std::ostream& vgui_soview2D_ellipse::print(std::ostream& s) const
{
  s << "[ vgui_soview2D_ellipse " << x << ',' << y
    << " w" << w << " h" << h << " phi" << phi << ' ';
  return vgui_soview2D::print(s) << " ]";
}

void vgui_soview2D_ellipse::draw() const
{
  double px, py;

  glBegin(GL_LINE_LOOP);
  for (unsigned int i=0;i<100;i++)
  {
    double angle = vnl_math::twopi*0.01*i;
    px = w*std::cos(this->phi)*std::cos(angle) + h*std::sin(this->phi)*std::sin(angle);
    py = h*std::cos(this->phi)*std::sin(angle) - w*std::sin(this->phi)*std::cos(angle);
    glVertex2d(x+px, y+py);
  }
  glEnd();
}

//:
// \todo not correctly implemented (assumes a circle)
float vgui_soview2D_ellipse::distance_squared(float vx, float vy) const
{
  return (vx - x)*(vx - x) + (vy - y)*(vy - y);
}

void vgui_soview2D_ellipse::get_centroid(float* vx, float* vy) const
{
  *vx = x;
  *vy = y;
}

void vgui_soview2D_ellipse::translate(float tx, float ty)
{
  x += tx;
  y += ty;
}


//--------------------------------------------------------------------------------//

vgui_soview2D_linestrip::vgui_soview2D_linestrip(unsigned n_, float const *x_, float const *y_)
  : n(n_), x(new float[n]), y(new float[n])
{
  for (unsigned i=0; i<n; ++i)
  {
    x[i] = x_[i];
    y[i] = y_[i];
  }
}

vgui_soview2D_linestrip::~vgui_soview2D_linestrip()
{
  n=0;
  delete [] x; x=nullptr;
  delete [] y; y=nullptr;
}

void vgui_soview2D_linestrip::draw() const
{
  glBegin(GL_LINE_STRIP);
  for (unsigned i=0; i<n; ++i)
    glVertex2f(x[i], y[i]);
  glEnd();
}

std::ostream& vgui_soview2D_linestrip::print(std::ostream&s) const { return s << "[ a linestrip. FIXME ]"; }

float vgui_soview2D_linestrip::distance_squared(float vx, float vy) const
{
  double tmp = vgl_distance_to_non_closed_polygon(x, y, this->n, vx, vy);
  return static_cast<float>(tmp*tmp);
}

void vgui_soview2D_linestrip::get_centroid(float* vx, float* vy) const
{
  *vx = 0;
  *vy = 0;
  for (unsigned i=0; i<n; ++i)
  {
    *vx += x[i];
    *vy += y[i];
  }
  float s = 1.0f / float(n);
  *vx *= s;
  *vy *= s;
}

void vgui_soview2D_linestrip::translate(float tx, float ty)
{
  for (unsigned i=0; i<n; ++i)
  {
    x[i] += tx;
    y[i] += ty;
  }
}

void vgui_soview2D_linestrip::set_size(unsigned nn)
{
  if (nn < n) { n = nn; return; }

  // we know that n <= nn
  float *nx = new float[nn];
  float *ny = new float[nn];
  for (unsigned i=0; i<n; ++i)
  {
    nx[i] = x[i];
    ny[i] = y[i];
  }

  n = nn;
  delete [] x; x = nx;
  delete [] y; y = ny;
}

//--------------------------------------------------------------------------------//

vgui_soview2D_polygon::vgui_soview2D_polygon(unsigned n_, float const *x_, float const *y_, bool fill)
  : n(n_), x(new float[n]), y(new float[n]), filled(fill)
{
  for (unsigned i=0; i<n; ++i)
  {
    x[i] = x_[i];
    y[i] = y_[i];
  }
}

vgui_soview2D_polygon::~vgui_soview2D_polygon()
{
  n=0;
  delete [] x; x=nullptr;
  delete [] y; y=nullptr;
}

void vgui_soview2D_polygon::draw() const
{
  if(filled){
    glBegin(GL_POLYGON);
    for (unsigned i=0; i<n; ++i)
      glVertex2f(x[i], y[i]);
    glEnd();
  }else{
    glBegin(GL_LINE_LOOP);
    for (unsigned i=0; i<n; ++i)
      glVertex2f(x[i], y[i]);
    glEnd();
  }
}

std::ostream& vgui_soview2D_polygon::print(std::ostream&s) const { return s << "[ a polygon. FIXME ]"; }

float vgui_soview2D_polygon::distance_squared(float vx, float vy) const
{
  double tmp = vgl_distance_to_closed_polygon(x, y, this->n, vx, vy);
  return static_cast<float>(tmp*tmp);
}

void vgui_soview2D_polygon::get_centroid(float* vx, float* vy) const
{
  *vx = 0;
  *vy = 0;
  for (unsigned i=0; i<n; ++i)
  {
    *vx += x[i];
    *vy += y[i];
  }
  float s = 1.0f / float(n);
  *vx *= s;
  *vy *= s;
}

void vgui_soview2D_polygon::translate(float tx, float ty)
{
  for (unsigned i=0; i<n; ++i)
  {
    x[i] += tx;
    y[i] += ty;
  }
}

void vgui_soview2D_polygon::set_size(unsigned nn)
{
  if (nn < n) { n = nn; return; }

  // we know that n <= nn
  float *nx = new float[nn];
  float *ny = new float[nn];
  for (unsigned i=0; i<n; ++i)
  {
    nx[i] = x[i];
    ny[i] = y[i];
  }

  n = nn;
  delete [] x; x = nx;
  delete [] y; y = ny;
}


//-----------------------------------------------------------

vgui_soview2D_image::vgui_soview2D_image( float in_x, float in_y,
                                          vil1_image const& img,
                                          bool in_blend,
                                          GLenum format,
                                          GLenum type )
  : x_( in_x ),
    y_( in_y ),
    w_( img.width() ),
    h_( img.height() ),
    blend_( in_blend ),
    buffer_( new vgui_section_buffer( 0, 0, w_, h_, format, type ) )
{
  buffer_->apply( img , (vgui_range_map_params*) nullptr);
}

vgui_soview2D_image::vgui_soview2D_image( float in_x, float in_y,
                                          vil_image_view_base const& img,
                                          bool in_blend,
                                          GLenum format,
                                          GLenum type )
  : x_( in_x ),
    y_( in_y ),
    w_( img.ni() ),
    h_( img.nj() ),
    blend_( in_blend ),
    buffer_( new vgui_section_buffer( 0, 0, w_, h_, format, type ) )
{
  buffer_->apply( vil_new_image_resource_of_view( img ), (vgui_range_map_params*) nullptr);
}

vgui_soview2D_image::~vgui_soview2D_image()
{
  delete buffer_;
}

void vgui_soview2D_image::draw() const
{
  // Get the current blend state so we can restore it later
  GLboolean blend_on;
  glGetBooleanv( GL_BLEND, &blend_on );

  if ( blend_ ) {
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  }
  else
    glDisable( GL_BLEND );

  glTranslatef(  x_,  y_, 0.0f );
  buffer_->draw_as_image() || buffer_->draw_as_rectangle();
  glTranslatef( -x_, -y_, 0.0f );

  if ( blend_on )
    glEnable( GL_BLEND );
  else
    glDisable( GL_BLEND );
}

std::ostream& vgui_soview2D_image::print(std::ostream&s) const
{
  return s << "[ vgui_soview2D_image "<<w_<<'x'<<h_<<", blend="<<blend_<<" ]";
}

float vgui_soview2D_image::distance_squared(float vx, float vy) const
{
  float dx = (x_ + (w_ / 2.0f)) - vx;
  float dy = (y_ + (h_ / 2.0f)) - vy;
  return dx*dx + dy*dy;
}

void vgui_soview2D_image::get_centroid(float* vx, float* vy) const
{
  float x1 = x_ + (w_ / 2.0f);
  float y1 = y_ + (h_ / 2.0f);

  *vx = x1;
  *vy = y1;
}

void vgui_soview2D_image::translate(float tx, float ty)
{
    x_ += tx;
    y_ += ty;
}
