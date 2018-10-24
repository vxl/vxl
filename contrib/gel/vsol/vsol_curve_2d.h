#ifndef vsol_curve_2d_h_
#define vsol_curve_2d_h_
//*****************************************************************************
//:
// \file
// \brief Abstract curve in a 2D space
//
// \author
//  Francois BERTEL
//
// \verbatim
//  Modifications
//   2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
//   2000/04/27 Francois BERTEL Creation
//   2002/04/22 Amir Tamrakar   Added POLYLINE type
//   2004/09/10 Peter Vanroose  Inlined all 1-line methods in class decl
//   2004/09/23 MingChing Chang fix name of cast_to functions and added endpoints_equal
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
class vdgl_digital_curve;
class vsol_line_2d;
class vsol_conic_2d;
class vsol_polyline_2d;
class vsol_digital_curve_2d;
class bsold_circ_arc_2d;

class vsol_curve_2d : public vsol_spatial_object_2d
{
 protected:
  enum vsol_curve_2d_type
  { CURVE_NO_TYPE=0,
    LINE,
    CIRCULAR_ARC,
    CONIC,
    POLYLINE,
    DIGITAL_CURVE,
    NUM_CURVE_TYPES
  };
 private: // has been superseded by is_a()
  //: return the curve type
  virtual vsol_curve_2d_type curve_type()const{return vsol_curve_2d::CURVE_NO_TYPE;}

  //***************************************************************************
  // Initialization
  //***************************************************************************
 public:
  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_curve_2d() override = default;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: return the spatial type
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d_type spatial_type()const override{return vsol_spatial_object_2d::CURVE;}

 public:
  //---------------------------------------------------------------------------
  //: Return the first point of `this'; pure virtual function
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr p0() const=0;

  //---------------------------------------------------------------------------
  //: Return the last point of `this'; pure virtual function
  //---------------------------------------------------------------------------
  virtual vsol_point_2d_sptr p1() const=0;

  //***************************************************************************
  // Replaces dynamic_cast<T>
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a curve, 0 otherwise
  //---------------------------------------------------------------------------
  vsol_curve_2d *cast_to_curve() override {return this;}
  const vsol_curve_2d *cast_to_curve() const override {return this;}

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a line, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vsol_line_2d const*cast_to_line()const{return nullptr;}
  virtual vsol_line_2d *cast_to_line() {return nullptr;}

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a conic, 0 otherwise
  //---------------------------------------------------------------------------
  virtual bsold_circ_arc_2d const*cast_to_circ_arc()const{return nullptr;}
  virtual bsold_circ_arc_2d *cast_to_circ_arc() {return nullptr;}


  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a conic, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vsol_conic_2d const*cast_to_conic()const{return nullptr;}
  virtual vsol_conic_2d *cast_to_conic() {return nullptr;}

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a polyline, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vsol_polyline_2d const*cast_to_polyline()const{return nullptr;}
  virtual vsol_polyline_2d *cast_to_polyline() {return nullptr;}

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a digital_curve_2d, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vsol_digital_curve_2d const*cast_to_digital_curve()const{return nullptr;}
  virtual vsol_digital_curve_2d *cast_to_digital_curve() {return nullptr;}

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a vdgl_digital_curve, 0 otherwise
  //---------------------------------------------------------------------------
  virtual vdgl_digital_curve const*cast_to_vdgl_digital_curve()const{return nullptr;}
  virtual vdgl_digital_curve *cast_to_vdgl_digital_curve() {return nullptr;}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the length of `this'
  //---------------------------------------------------------------------------
  virtual double length() const=0;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the first point of the curve
  //---------------------------------------------------------------------------
  virtual void set_p0(const vsol_point_2d_sptr &new_p0)=0;

  //---------------------------------------------------------------------------
  //: Set the last point of the curve
  //---------------------------------------------------------------------------
  virtual void set_p1(const vsol_point_2d_sptr &new_p1)=0;

 protected:
  //: Helper function to determine if curve endpoints are equal (in any order).
  // Useful for curve equality tests.
  bool endpoints_equal(const vsol_curve_2d &other) const;
};

#endif // vsol_curve_2d_h_
