#ifndef VCSL_SPATIAL_TRANSFORMATION_H
#define VCSL_SPATIAL_TRANSFORMATION_H
//*****************************************************************************
//
// .NAME vcsl_spatial_transformation - Transformation between 2 spatial
//                                     coordinate systems
// .LIBRARY vcsl
// .HEADER  vxl Package
// .INCLUDE vcsl/vcsl_spatial_transformation.h
// .FILE    vcsl/vcsl_spatial_transformation.cxx
//
// .SECTION Description 
// A spatial transformation can be static or dynamic
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/07/19 François BERTEL Creation.
//*****************************************************************************

#include <vcsl/vcsl_spatial_transformation_ref.h>

#include <vbl/vbl_ref_count.h>
#include <vcl/vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_quaternion.h>

typedef vcl_vector<double> list_of_scalars;
typedef vcl_vector<vnl_vector<double> *> list_of_vectors;

enum vcsl_interpolator
{
  vcsl_linear,
  vcsl_cubic,
  vcsl_spline
};

class vcsl_spatial_transformation
  : public vbl_ref_count
{
public:

  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor. Do nothing
  explicit vcsl_spatial_transformation(void);

  //: Destructor. Do nothing
  virtual ~vcsl_spatial_transformation();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Return the list of time clocks
  virtual vcl_vector<double> *beat(void) const;

  //: Return the list of interpolators
  virtual vcl_vector<vcsl_interpolator> *interpolators(void) const;

  //: Is `time' between the two time bounds ?
  virtual bool valid_time(const double time) const;

  //: Is `this' invertible at time `time'?
  //: REQUIRE: valid_time(time)
  virtual bool is_invertible(const double time) const=0;

  //: Is `this' correctly set ?
  virtual bool is_valid(void) const;

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Return the index of the beat inferior or equal to `time'
  //: REQUIRE: valid_time(time)
  virtual int matching_interval(const double time) const;


  //: Image of `v' by `this'
  //: REQUIRE: is_valid()
  virtual vnl_vector<double> *execute(const vnl_vector<double> &v,
                                      const double time) const=0;

  //: Image of `v' by the inverse of `this'
  //: REQUIRE: is_invertible(time)
  //: REQUIRE: is_valid()
  virtual vnl_vector<double> *inverse(const vnl_vector<double> &v,
                                      const double time) const=0;

  //***************************************************************************
  // Status setting
  //***************************************************************************
 
  //: Set the list of time clocks
  virtual void set_beat(vcl_vector<double> &new_beat);

  //: Set the list of interpolators
  virtual
  void set_interpolators(vcl_vector<vcsl_interpolator> &new_interpolators);

  //***************************************************************************
  // Interpolators
  //***************************************************************************

  //: Linear interpolation on scalar values
  virtual double lsi(const double v0,
                     const double v1,
                     const int index,
                     const double time) const;

  //: Linear interpolation on vnl_vectors
  virtual vnl_vector<double> *lvi(const vnl_vector<double> &v0,
                                  const vnl_vector<double> &v1,
                                  const int index,
                                  const double time) const;

  //: Linear interpolation on quaternions
  virtual vnl_quaternion<double> *lqi(const vnl_quaternion<double> &v0,
                                      const vnl_quaternion<double> &v1,
                                      const int index,
                                      const double time) const;

protected:
  //: List of time clocks
  vcl_vector<double> *beat_;
  vcl_vector<vcsl_interpolator> *interpolator_;
};

#endif // #ifndef VCSL_SPATIAL_TRANSFORMATION_H
