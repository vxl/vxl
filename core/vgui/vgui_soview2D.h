// This is core/vgui/vgui_soview2D.h
#ifndef vgui_soview2D_h_
#define vgui_soview2D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   24 Mar 99
// \brief  2-dimensional spatial object view.
//
//  Contains classes:  vgui_soview2D  vgui_soview2D_point
//                     vgui_soview2D_lineseg  vgui_soview2D_group
//                     vgui_soview2D_infinite_line  vgui_soview2D_circle
//                     vgui_soview2D_ellipse  vgui_soview2D_linestrip
//                     vgui_soview2D_polygon
//
// Notes:  We use floats instead of doubles as size is a speed issue (sic.)
//
// \verbatim
//  Modifications
//   10 Feb 2000 fsm - removed dependency on MultiView
//   04 Jul 2000 Marko Bacic - Fixed vgui_soview2D_circle
// \endverbatim

#include "vgui_soview.h"
#include <vcl_iosfwd.h>

#include "vgui_gl.h"

class vil1_image;
class vil_image_view_base;
class vgui_section_buffer;

//-----------------------------------------------------------------------------
//: 2-dimensional spatial object view.
class vgui_soview2D : public vgui_soview
{
 public:
  //: Constructor - create a default soview2D.
  vgui_soview2D() {}

  //: Destructor - delete this soview2D.
  virtual ~vgui_soview2D() {}

  //: Returns the distance squared of this soview2D from the given position.
  virtual float distance_squared(float x, float y) const = 0;

  //: Returns the centroid of this soview2D.
  virtual void get_centroid(float* x, float* y) const = 0;

  //: Translate this soview2D by the given x and y distances.
  virtual void translate(float x, float y) = 0;
};

//-----------------------------------------------------------------------------
//: 2-dimensional point.
class vgui_soview2D_point : public vgui_soview2D
{
 public:
  //: Constructor - create a default 2D-point.
  vgui_soview2D_point() : x(0), y(0) {}

  //: Constructor - create a 2D-point with the given coordinates.
  vgui_soview2D_point(float x_, float y_) : x(x_), y(y_) {}

  //: Render this 2D-point on the display.
  virtual void draw() const;

  // inherit documentation from base class
  virtual void draw_select() const;

  //: Print details about this 2D-point to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Distance of this 2D-point from the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the type of this class ('vgui_soview2D_point').
  vcl_string type_name() const { return "vgui_soview2D_point"; }

  //: Returns the centroid of this 2D-point.
  void get_centroid(float* x, float* y) const;

  //: Translates this 2D-point by the given x and y distances.
  void translate(float x, float y);

  //: x-coordinate of this 2D-point.
  float x;
  //: y-coordinate of this 2D-point.
  float y;
};

//-----------------------------------------------------------------------------
//: 2-dimensional line segment (finite line).
class vgui_soview2D_lineseg : public vgui_soview2D
{
 public:
  //: Constructor - create a default 2D line segment.
  vgui_soview2D_lineseg() : x0(0), y0(0), x1(0), y1(0) {}

  //: Constructor - create a 2D line segment with given start and end points.
  vgui_soview2D_lineseg(float x0_, float y0_, float x1_, float y1_)
    : x0(x0_), y0(y0_), x1(x1_), y1(y1_) {}

  //: Constructor - create a 2D line segment same as the given 2D line segment.
  vgui_soview2D_lineseg( vgui_soview2D_lineseg &l_) : vgui_soview2D(), x0(l_.x0), y0(l_.y0), x1(l_.x1), y1(l_.y1) {}

  //: Render this 2D line segment on the display.
  virtual void draw() const;

  //: Print details about this 2D line segment to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the distance squared to this 2D line segment.
  virtual float distance_squared(float x, float y) const;

  //: Returns the type of this class ('vgui_soview2D_lineseg').
  vcl_string type_name() const { return "vgui_soview2D_lineseg"; }

  //: Returns the centroid of this 2D line segment.
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D line segment by the given x and y distances.
  void translate(float x, float y);

  //: Start point x coordinate.
  float x0;
  //: Start point y coordinate.
  float y0;
  //: End point x coordinate.
  float x1;
  //: End point y coordinate.
  float y1;
};

//-----------------------------------------------------------------------------
//: Group of vgui_soview2D's.
class vgui_soview2D_group : public vgui_soview2D
{
 public:
  //: Constructor - creates an empty 2D soview group.
  vgui_soview2D_group() {}

  //: Constructor - creates a 2D soview group containing the given 2D soviews.
  vgui_soview2D_group( vcl_vector<vgui_soview2D *> ls_) : ls(ls_) {}

  //: Destructor - responsible for deleting 2D soview objects
  //  In current design, Easy2D tableau is responsible for cleaning up
  //  soview objects. To avoid memory leak when using this group class, 
  //  clean up the soview objects in the descructor.
  //  It is hard to be nice & clean, unless smart ptr is introduced. 
  //  GY
  virtual ~vgui_soview2D_group();
  
  //: Set the style (colour, line width, etc) for this 2D soview group.
  virtual void set_style(const vgui_style_sptr&);

  //: Render this 2D soview group on the display.
  virtual void draw() const;

  //: for selection purpose
  //  see comments in base class
  virtual void draw_select() const;

  //: Print details about this 2D soview group to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns distance squared of this 2D soview group from the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the type of this class ('vgui_soview2D_group').
  vcl_string type_name() const { return "vgui_soview2D_group"; }

  //: Returns the centroid of this 2D soview group.
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D soview group by the given x and y distances.
  void translate(float x, float y);

  //: List of 2D soviews in this group.
  vcl_vector<vgui_soview2D *> ls;
};

//-----------------------------------------------------------------------------
//: 2-dimensional infinite line.
class vgui_soview2D_infinite_line : public vgui_soview2D
{
 public:
  //: Constructor - create a default 2D infinite line.
  vgui_soview2D_infinite_line() {}

  //: Constructor - create a 2D infinite line ax + by + c = 0.
  vgui_soview2D_infinite_line( float a_, float b_, float c_)
    : a(a_), b(b_), c(c_) {}

  //: Render this 2D infinite line on the display.
  virtual void draw() const;

  //: Print details about this 2D infinite line to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns distance squared of this 2D infinite line from the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the type of this class ('vgui_soview2D_infinite_line').
  vcl_string type_name() const { return "vgui_soview2D_infinite_line"; }

  //: Returns (0,0) - centroid does not make sense for 2D infinite line.
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D infinite line by the given x and y distances.
  void translate(float x, float y);

  //: Parameters of the 2D infinite line ax + by + c = 0.
  float a,b,c;
};


//-----------------------------------------------------------------------------
//: 2-dimensional circle.
class vgui_soview2D_circle : public vgui_soview2D
{
 public:
  //: Constructor - creates a default 2D circle.
  vgui_soview2D_circle() {}

  //: Constructor - creates a circle with radius r, centered at (x,y)
  vgui_soview2D_circle( float x_, float y_, float r_ )
    : r(r_), x(x_), y(y_)  {  }
  
  //: Render this 2D circle on the display.
  virtual void draw() const;

  //: Print details about this 2D circle to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the distance squared of this 2D circle from the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the type of this class ('vgui_soview2D_circle').
  vcl_string type_name() const { return "vgui_soview2D_circle"; }

  //: Returns the centroid of this 2D circle (same as centre).
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D circle by the given x and y distances.
  void translate(float x, float y);

  //: Radius of circle.
  float r;
  //: x-coordinate of the centre of the circle.
  float x;
  //: y-coordinate of the centre of the circle.
  float y;

  //: Compile the vcl_list
  static void compile();
};


//-----------------------------------------------------------------------------
//: 2-dimensional ellipse.
class vgui_soview2D_ellipse : public vgui_soview2D
{
 public:
  //: Constructor - create a default 2D ellipse.
  vgui_soview2D_ellipse() {}

  //: Render this 2D ellipse on the display.
  virtual void draw() const;

  //: Print details about this 2D ellipse to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the distance squared of this 2D ellipse from the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the type of this class ('vgui_soview2D_ellipse').
  vcl_string type_name() const {return "vgui_soview2D_ellipse"; }

  //: Returns the centroid of this 2D ellipse.
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D ellipse by the given x and y distances.
  void translate(float x, float y);

  //: Centre, width, height and angle of this 2D ellipse.
  float x, y, w, h, phi;

  //: Compile the vcl_list
  static void compile();
};


//-----------------------------------------------------------------------------
//: 2-dimensional linestrip.
class vgui_soview2D_linestrip : public vgui_soview2D
{
 public:
  //: Constructor - create a 2D linestrip from the given coordinate list.
  vgui_soview2D_linestrip(unsigned, float const *, float const *);

  //: Constructor - create a default 2D linestrip.
  vgui_soview2D_linestrip() : n(0), x(0), y(0) {}

  //: Destructor - delete this 2D linestrip.
  ~vgui_soview2D_linestrip();

  //: Render this 2D linestrip on the display.
  virtual void draw() const;

  //: Print information about this 2D linestrip to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the distance squared from this 2D linestrip to the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the type of this class ('vgui_soview2D_linestrip').
  vcl_string type_name() const { return "vgui_soview2D_linestrip"; }

  //: Returns the centroid of this 2D linestrip.
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D linestrip by the given x and y distances.
  void translate(float x, float y);

  unsigned n;
  float *x, *y;
  void set_size(unsigned );

  //static void compile();
};


//-----------------------------------------------------------------------------
//: 2-dimensional polygon.
class vgui_soview2D_polygon : public vgui_soview2D
{
 public:
  //: Constructor - create a 2D polygon with the given vertices.
  vgui_soview2D_polygon(unsigned, float const *, float const *);

  //: Constructor - create a default 2D polygon.
  vgui_soview2D_polygon() : n(0), x(0), y(0) {}

  //: Destructor - delete this 2D polygon.
  ~vgui_soview2D_polygon();

  //: Render this 2D polygon on the display.
  virtual void draw() const;

  //: Print details about this 2D polygon to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the distance squared from this 2D polygon to the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the type of this class ('vgui_soview2D_polygon').
  vcl_string type_name() const { return "vgui_soview2D_polygon"; }

  //: Returns the centroid of this 2D polygon.
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D polygon by the given x and y distances.
  void translate(float x, float y);

  unsigned n;
  float *x, *y;
  void set_size(unsigned );

  //static void compile();
};

//-----------------------------------------------------------------------------
//: 2-dimensional image.
//
// This will store a GL pixel buffer of the input image and display
// it, on request, with the top left corner at (x,y). It does not keep
// a reference to the image. Rather, it will create the GL pixel
// buffer at construction time and store that.
//
class vgui_soview2D_image : public vgui_soview2D
{
 public:
  //: Create the sprite from a vil1_image.
  //
  // If format and type are not specified, the "best" one will be
  // automatically chosen.  \a blend will set the blend state for
  // rendering. See draw().
  //
  vgui_soview2D_image( float x, float y,
                       vil1_image const& img,
                       bool blend = false,
                       GLenum format = GL_NONE,
                       GLenum type = GL_NONE );

  //: Create the sprite from a vil_image_view.
  //
  // If format and type are not specified, the "best" one will be
  // automatically chosen.  \a blend will set the blend state for
  // rendering. See draw().
  //
  vgui_soview2D_image( float x, float y,
                       vil_image_view_base const& img,
                       bool blend = false,
                       GLenum format = GL_NONE,
                       GLenum type = GL_NONE );

  //: Destructor - delete this image.
  ~vgui_soview2D_image();

  //: Render this image on the display.
  //
  // If the blend state is on, then the image will be rendered with
  // GL_BLEND enabled, and with glBlendFunc(GL_SRC_ALPHA,
  // GL_ONE_MINUS_SRC_ALPHA)
  //
  virtual void draw() const;

  //: Print details about this image to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the distance squared from the centroid
  virtual float distance_squared(float x, float y) const;

  //: Returns the type of this class ('vgui_soview2D_image').
  vcl_string type_name() const { return "vgui_soview2D_image"; }

  //: Returns the centroid of this 2D image.
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D image by the given x and y distances.
  void translate(float x, float y);

 private:
  //: Coordinates of the upper lefthand corner of the image
  float x_, y_;

  //: Width and height of the image
  unsigned w_, h_;

  //: Render with or without blending?
  bool blend_;

  //: The OpenGL buffer corresponding to the image
  vgui_section_buffer* buffer_;
};

#endif // vgui_soview2D_h_
