// This is contrib/brl/bseg/bmrf/bmrf_epi_transform.h
#ifndef bmrf_epi_transform_h_
#define bmrf_epi_transform_h_
//:
// \file
// \brief Epipolar transformation functions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 02/24/04
//
// This file contains a base class for epipolar transformations
// and several specific functions derived from it.  All transformations
// are calculated as $ s(\alpha,t) = \frac{s_{0}(\alpha)}{1-\gamma(\alpha)*t} $,
// where (s0,alpha) is an initial point in epipolar space, (s,alpha) is the
// transformed point, t is time, and gamma is a function of alpha that defines
// the transformation.  
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <bmrf/bmrf_epi_point_sptr.h>
#include <bmrf/bmrf_epi_seg_sptr.h>

//: Epipolar transformation function base class
class bmrf_epi_transform : public vbl_ref_count
{
 public:
  //: Return the gamma value for any alpha
  virtual double gamma(double alpha) const = 0;

  //: Calculates the transformed value of \i s
  double apply(double s0, double alpha, double t = 1.0) const;

  //: Calculates a transformed epi_point
  // \param ep is the epipoint to trasform
  // \param t is the time step
  // \param update_all is a flag indicating whether or not to update all
  //        members of the bmrf_epi_point.  By default, only the \i s value is updated
  bmrf_epi_point_sptr apply(const bmrf_epi_point_sptr& ep, double t = 1.0, bool update_all = false) const;

  //: Calculates a transformed epi_seg
  // \param ep is the epipoint to trasform
  // \param t is the time step
  // \param update_all is a flag indicating whether or not to update all
  //        members of the bmrf_epi_seg.  By default, only the \i s values
  //        of each epi_point are updated
  bmrf_epi_seg_sptr apply(const bmrf_epi_seg_sptr& ep, double t = 1.0, bool update_all = false) const;
  

 protected:
  //: Constructor
  bmrf_epi_transform(){}

};


//: An epipolar trasformation function where gamma is a constant
class bmrf_const_epi_transform : public bmrf_epi_transform
{
 public:
  //: Constructor
  bmrf_const_epi_transform(double gamma = 0.0);

  //: Set the constant gamma value
  void set_gamma(double gamma);

  //: Returns the constant gamma value
  virtual double gamma(double alpha) const;
  
 private:
  //: The constant gamma value
  double gamma_;
};


//: An epipolar trasformation function where gamma is a linear
class bmrf_linear_epi_transform : public bmrf_epi_transform
{
 public:
  //: Constructor
  bmrf_linear_epi_transform(double m = 0.0, double b = 0.0);

  //: Set the linear function paramaters
  void set_params(double m, double b);

  //: Returns the gamma value
  virtual double gamma(double alpha) const;

 private:
  //: The function parameters
  double m_, b_;
};

#endif // bmrf_epi_transform_h_
