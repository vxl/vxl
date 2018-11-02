// This is core/vgui/vgui_soview3D.h
#ifndef vgui_soview3D_h_
#define vgui_soview3D_h_
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   24 Mar 99
// \brief  3-dimensional spatial object view.
//
// Contains classes:  vgui_soview3D  vgui_point3D  vgui_linseg3D
// Notes:  We use floats instead of doubles as size is a speed issue (sic.)
//

#include <iosfwd>
#include "vgui_soview.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: 3-dimensional spatial object view.
class vgui_soview3D : public vgui_soview
{
 public:
  vgui_soview3D() {}
  virtual ~vgui_soview3D() {}
};


//: 3-dimensional point.
class vgui_point3D : public vgui_soview3D
{
 public:
  vgui_point3D() {}
  vgui_point3D(float x_, float y_, float z_) : vgui_soview3D(), x(x_), y(y_), z(z_) {}

  virtual void draw() const;
  virtual std::ostream& print(std::ostream&) const;
  std::string type_name() const { return "vgui_point3D"; }

  float x,y,z;
};


//: 3-dimensional line segment.
class vgui_lineseg3D : public vgui_soview3D
{
 public:
  vgui_lineseg3D() {}
  vgui_lineseg3D( float x0_, float y0_, float z0_,
                  float x1_, float y1_, float z1_ )
    : x0(x0_), y0(y0_), z0(z0_), x1(x1_), y1(y1_), z1(z1_) {}

  virtual void draw() const;
  virtual std::ostream& print(std::ostream&) const;
  std::string type_name() const { return "vgui_lineseg3D"; }

  float x0, y0, z0, x1, y1, z1;
};


//: 3-dimensional line segment.
class vgui_triangle3D : public vgui_soview3D
{
 public:
  vgui_triangle3D() {}
  vgui_triangle3D( float x0_, float y0_, float z0_,
                   float x1_, float y1_, float z1_,
                   float x2_, float y2_, float z2_ )
    : x0(x0_), y0(y0_), z0(z0_),
      x1(x1_), y1(y1_), z1(z1_),
      x2(x2_), y2(y2_), z2(z2_) {}

  virtual void draw() const;
  virtual std::ostream& print(std::ostream&) const;
  std::string type_name() const { return "vgui_triangle3D"; }

  float x0, y0, z0, x1, y1, z1, x2, y2, z2;
};


#endif // vgui_soview3D_h_
