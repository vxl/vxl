// This is core/vcsl/vcsl_spatial_transformation.h
#ifndef vcsl_spatial_transformation_h_
#define vcsl_spatial_transformation_h_
//:
// \file
// \author François BERTEL
// \brief Transformation between 2 spatial coordinate systems
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2002/01/22 Peter Vanroose - added lmi() as it is used in vcsl_matrix.cxx
//   2002/01/22 Peter Vanroose - return type of lqi(), lvi(), execute() and inverse() changed to non-ptr
//   2002/01/28 Peter Vanroose - vcl_vector members beat_ and interpolator_ changed to non-ptr
//   2004/09/10 Peter Vanroose - Added explicit copy constructor (ref_count !)
//   2004/09/17 Peter Vanroose - made beat() and interpolators() non-virtual: they just return a member and should not be overloaded
// \endverbatim

#include <vcsl/vcsl_spatial_transformation_sptr.h>

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_quaternion.h>

typedef vcl_vector<double> list_of_scalars;
typedef vcl_vector<vnl_vector<double> > list_of_vectors;

enum vcsl_interpolator
{
  vcsl_linear,
  vcsl_cubic,
  vcsl_spline
};

//: Transformation between 2 spatial coordinate systems
// A spatial transformation can be static or dynamic
class vcsl_spatial_transformation : public vbl_ref_count
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

 protected:
  // Default constructor. Do nothing
  vcsl_spatial_transformation() {}

 public:
  // Copy constructor
  vcsl_spatial_transformation(vcsl_spatial_transformation const& x)
    : vbl_ref_count(), beat_(x.beat_), interpolator_(x.interpolator_) {}

  // Destructor. Do nothing
  virtual ~vcsl_spatial_transformation() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Return the list of time clocks
  vcl_vector<double> beat() const { return beat_; }

  //: Return the time duration
  unsigned int duration() const { return beat_.size(); }

  //: Return the list of interpolators
  vcl_vector<vcsl_interpolator> interpolators() const { return interpolator_; }

  //: Is `time' between the two time bounds ?
  bool valid_time(double time) const;

  //: Is `this' invertible at time `time'?
  //  REQUIRE: valid_time(time)
  virtual bool is_invertible(double time) const=0;

  //: Is `this' correctly set ?
  virtual bool is_valid() const
  { return (duration()==0&&interpolator_.size()==0) ||
           (duration()==interpolator_.size()+1); }

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Return the index of the beat inferior or equal to `time'
  //  REQUIRE: valid_time(time)
  int matching_interval(double time) const;

  //: Image of `v' by `this'
  //  REQUIRE: is_valid()
  virtual vnl_vector<double> execute(const vnl_vector<double> &v,
                                     double time) const=0;

  //: Image of `v' by the inverse of `this'
  //  REQUIRE: is_invertible(time)
  //  REQUIRE: is_valid()
  virtual vnl_vector<double> inverse(const vnl_vector<double> &v,
                                     double time) const=0;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //: Set the list of time clocks
  void set_beat(vcl_vector<double> const& new_beat) { beat_=new_beat; }

  //: Set the list of interpolators
  void set_interpolators(vcl_vector<vcsl_interpolator> const& i) { interpolator_=i; }

  //: Empty the time clock and interpolators, thereby making the transf static
  void set_static();

  //***************************************************************************
  // Interpolators
  //***************************************************************************

  //: Linear interpolation on scalar values
  double lsi(double v0,
             double v1,
             int index,
             double time) const;

  //: Linear interpolation on vnl_vectors
  vnl_vector<double> lvi(const vnl_vector<double> &v0,
                         const vnl_vector<double> &v1,
                         int index,
                         double time) const;

  //: Linear interpolation on vnl_matrices
  vnl_matrix<double> lmi(const vnl_matrix<double> &m0,
                         const vnl_matrix<double> &m1,
                         int index,
                         double time) const;

  //: Linear interpolation on quaternions
  vnl_quaternion<double> lqi(const vnl_quaternion<double> &v0,
                             const vnl_quaternion<double> &v1,
                             int index,
                             double time) const;

 protected:
  //: List of time clocks
  vcl_vector<double> beat_;
  vcl_vector<vcsl_interpolator> interpolator_;
};

#endif // vcsl_spatial_transformation_h_
