// This is brl/bbas/bgui/bgui_vsol_soview2D.h
#ifndef bgui_vsol_soview2D_h_
#define bgui_vsol_soview2D_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A collection of vgui_soview2D objects
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy     June 2, 2003  Initial version.
//   Amir Tamrakar June 06, 2003  Added soview for vsol_polyline_2d.
//   M.R. Johnson  June 20, 2003  Initial version of linked soviews.
//   M.J. Leotta   April 3, 2004  Merged linked soviews back into this file
//   Sancar Adali    May 5, 2004  Created a base class bgui_vsol_soview2D, a base
//                                class for all bgui_vsol_soview2D_xxx objects
//                                It has the sptr to the vsol data object  
// \endverbatim
//--------------------------------------------------------------------------------

#include <vcl_iosfwd.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <vgui/vgui_soview2D.h>


class bgui_vsol_soview2D : public vgui_soview2D
{
public:
  //: Constructor - creates a default vsol_spatial_object_2d view
  bgui_vsol_soview2D( vsol_spatial_object_2d_sptr const & pt);

  //: Destructor
  ~bgui_vsol_soview2D() {}

  //: Print details about this vsol object to the given stream.
  vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class
  virtual vcl_string type_name() const=0;

  //: Render this 2D digital_curve on the display.
  virtual void draw() const=0;
  
  //: Translate this 2D digital_curve by the given x and y distances.
  virtual void translate(float x, float y)=0;
protected:

  //: Smart pointer to a vsol_spatial_object
  vsol_spatial_object_2d_sptr sptr_;

};


class bgui_vsol_soview2D_point : public bgui_vsol_soview2D
{
 public:
  //: Constructor - creates a default vsol_point_2d view
  bgui_vsol_soview2D_point( vsol_point_2d_sptr const & pt);      
         
  ~bgui_vsol_soview2D_point() {}

  //: Returns the type of this class ('bgui_vsol_soview2D_point').
  static vcl_string type_name_() { return "bgui_vsol_soview2D_point"; }
  vcl_string type_name() const { return type_name_(); }

  //: Render this 2D digital_curve on the display.
  void draw() const;

  // Returns vsol_point_2d_sptr of the vsol_spatial_object_2d_sptr
  vsol_point_2d_sptr sptr() const;   
  
  //: Returns the distance squared from this 2D digital_curve to the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the centroid of this 2D digital_curve.
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D digital_curve by the given x and y distances.
  void translate(float x, float y);

};


//: vsol_line_2d
class bgui_vsol_soview2D_line_seg : public bgui_vsol_soview2D
{
 public:
  //: Constructor - creates a view of a given vtol_edge_2d
  bgui_vsol_soview2D_line_seg( vsol_line_2d_sptr const & pt);

  //: Destructor - does nothing, smart pointers pass out of scope automatically
  ~bgui_vsol_soview2D_line_seg() {}

  //: Returns the type of this class ('bgui_vsol_soview2D_line_seg').
  static vcl_string type_name_() { return "bgui_vsol_soview2D_line_seg"; }
  vcl_string type_name() const { return type_name_(); }

  //: Render this 2D digital_curve on the display.
  virtual void draw() const;

  // Returns a smart pointer to the line
  // \note cast from a vsol_spatial_object_2d_sptr in the base class
  vsol_line_2d_sptr sptr() const; 

  //: Returns the distance squared from this 2D digital_curve to the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the centroid of this 2D digital_curve.
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D digital_curve by the given x and y distances.
  virtual void translate(float x, float y);
  
};


//: vsol_polyline_2d
class bgui_vsol_soview2D_polyline : public bgui_vsol_soview2D
{
 public:
  //: Constructor - creates a view of a given vsol_polyline_2d
  bgui_vsol_soview2D_polyline(vsol_polyline_2d_sptr const& pline);

  //: Deconstructor
  ~bgui_vsol_soview2D_polyline() {}

  //: Returns the type of this class ('bgui_vsol_soview2D_polyline').
  static vcl_string type_name_() { return "bgui_vsol_soview2D_polyline"; }
  vcl_string type_name() const { return type_name_(); }

  //: Render this 2D digital_curve on the display.
  void draw() const;

  // Returns a smart pointer to the polyline
  // \note cast from a vsol_spatial_object_2d_sptr in the base class
  vsol_polyline_2d_sptr sptr() const;

  //: Returns the distance squared from this 2D digital_curve to the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the centroid of this 2D digital_curve.
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D digital_curve by the given x and y distances.
  void translate(float x, float y);
  
};


//: vsol_polygon_2d
class bgui_vsol_soview2D_polygon : public bgui_vsol_soview2D
{
 public:
  //: Constructor - creates a view of a vdgl_polygon
  bgui_vsol_soview2D_polygon(vsol_polygon_2d_sptr const& e);

  //: Deconstructor
  ~bgui_vsol_soview2D_polygon() {}

  //: Returns the type of this class ('bgui_vsol_soview2D_polygon').
  static vcl_string type_name_() { return "bgui_vsol_soview2D_polygon"; }
  vcl_string type_name() const { return type_name_(); }

  // Returns a smart pointer to the polygon
  // \note cast from a vsol_spatial_object_2d_sptr in the base class
  vsol_polygon_2d_sptr sptr() const;     

  //: Render this 2D digital_curve on the display.
  void draw() const;

  //: Returns the distance squared from this 2D digital_curve to the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the centroid of this 2D digital_curve.
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D digital_curve by the given x and y distances.
  void translate(float x, float y);
 
};


//: vdgl_digital_curve
class bgui_vsol_soview2D_digital_curve : public vgui_soview2D
{
 public:
  //: Constructor - creates a view of a vdgl_digital_curve
  bgui_vsol_soview2D_digital_curve(vdgl_digital_curve_sptr const& e, bool dotted = false)
  : draw_dotted_(dotted)
  { sptr = e; }

  //: Destructor - does nothing, smart pointers pass out of scope automatically
  ~bgui_vsol_soview2D_digital_curve() {}

  //: Print details about this vdgl_digital_curve to the given stream.
  virtual vcl_ostream& print(vcl_ostream&) const;

  //: Returns the type of this class ('bgui_vsol_soview2D_digital_curve').
  static vcl_string type_name_() { return "bgui_vsol_soview2D_digital_curve"; }
  vcl_string type_name() const { return type_name_(); }

  //: Render this 2D digital_curve on the display.
  void draw() const;

  //: Returns the distance squared from this 2D digital_curve to the given position.
  virtual float distance_squared(float x, float y) const;

  //: Returns the centroid of this 2D digital_curve.
  void get_centroid(float* x, float* y) const;

  //: Translate this 2D digital_curve by the given x and y distances.
  void translate(float x, float y);

  //: Smart pointer to vdgl_digital_curve
  vdgl_digital_curve_sptr sptr;

 protected:
  bool draw_dotted_;
};


#endif // bgui_vsol_soview2D_h_
