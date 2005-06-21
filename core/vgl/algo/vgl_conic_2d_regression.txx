// This is core/vgl/algo/vgl_conic_2d_regression.txx
#ifndef vgl_conic_2d_regression_txx_
#define vgl_conic_2d_regression_txx_
//:
// \file
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x1.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_transpose.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_distance.h>
#include <vgl/algo/vgl_norm_trans_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include "vgl_conic_2d_regression.h"
template <class T>
void vgl_conic_2d_regression<T>::init()
{
  partial_sums_.resize(14, 0.0);
  npts_ = 0;
  Dinv_.fill(0.0);
  Dinv_.put(0,0,0.5);  Dinv_.put(1,1,1.0);  Dinv_.put(2,2,0.5);
  cost_ = 0;
  sampson_error_ = 0;
}


//--------------------------------------------------------------
//: Constructors
template <class T>
vgl_conic_2d_regression<T>::vgl_conic_2d_regression()
{
  this->init();
}

// == OPERATIONS ==

template <class T>
void vgl_conic_2d_regression<T>::clear_points()
{
  points_.clear();
  npts_ = 0;
}

// get the estimated error with respect to the fitted conic segment
template <class T>
T vgl_conic_2d_regression<T>::get_rms_error_est(vgl_point_2d<T> const& p) const
{
  vgl_homg_point_2d<T> hp(p);
  vgl_homg_point_2d<T> hc = vgl_homg_operators_2d<T>::closest_point(conic_, p);
  return static_cast<T>(vgl_distance(hp, hc));
}

// The Sampson approximation to mean Euclidean distance
// The conic coefficients are for the original frame
template <class T>
void vgl_conic_2d_regression<T>::set_sampson_error(const double a,
                                                   const double b,
                                                   const double c,
                                                   const double d,
                                                   const double e,
                                                   const double f
                                                   ) 
{
  double sum = 0;
  for(vcl_vector<vgl_point_2d<T> >::iterator pit = points_.begin();
      pit != points_.end(); ++pit)
    {
      double x = static_cast<double>(pit->x()),
        y = static_cast<double>(pit->y());
      double alg_dist = (a*x + b*y + d)*x + (c*y + e)*y + f;
      double dx = 2.0*a*x + b*y + d;
      double dy = 2.0*c*y + b*x + e;
      double grad_mag_sqrd = dx*dx+dy*dy;

      sum += (alg_dist*alg_dist)/grad_mag_sqrd;
    }
  if(npts_)
    {
      sampson_error_ = static_cast<T>(vcl_sqrt(sum/npts_));  
      return;
    }
  sampson_error_ = vnl_numeric_traits<T>::maxval;  
}

template <class T>
void vgl_conic_2d_regression<T>::add_point(vgl_point_2d<T> const& p)
{
  points_.push_back(p);
  ++npts_;
}

template <class T>
void vgl_conic_2d_regression<T>::remove_point(vgl_point_2d<T> const& p)
{
  vcl_vector<vgl_point_2d<T> >::iterator result;
  result = vcl_find(points_.begin(), points_.end(), p);
  if(result != points_.end())
    points_.erase(result);
  if(npts_>0)
    --npts_;
}

// get the current rms algebraic fitting error
template <class T>
T vgl_conic_2d_regression<T>::get_rms_algebraic_error() const
{
  return cost_;
}

template <class T>
void vgl_conic_2d_regression<T>::compute_partial_sums()
{
  hnorm_points_.clear();
  //Compute the normalizing transformation
  vcl_vector<vgl_homg_point_2d<double> > hpoints;
  for(vcl_vector<vgl_point_2d<T> >::iterator pit = points_.begin();
      pit != points_.end(); ++pit)
    {
      vgl_homg_point_2d<double> hp(static_cast<double>((*pit).x()),
                                   static_cast<double>((*pit).y()));
      hpoints.push_back(hp);
    }
  trans_.compute_from_points(hpoints, false); 
  //Transform the input pointset
  //  vcl_vector<vgl_homg_point_2d<double> > hnorm_points;
  for(vcl_vector<vgl_homg_point_2d<double> >::iterator pit = hpoints.begin();
      pit != hpoints.end(); ++pit)
    hnorm_points_.push_back(trans_(*pit));
      
  for(vcl_vector<double>::iterator dit = partial_sums_.begin();
      dit != partial_sums_.end(); ++dit)
  (*dit)=0;

  double x2,y2,x3,y3;
  for(vcl_vector<vgl_homg_point_2d<double> >::iterator pit = hnorm_points_.begin();          pit != hnorm_points_.end(); ++pit)
    {
      double x = static_cast<double>(pit->x()/pit->w()),
        y = static_cast<double>(pit->y()/pit->w());
      
      x2 = x*x;  x3 = x2*x;  y2 = y*y;  y3 = y2*y;
      partial_sums_[0] += x3*x;   partial_sums_[1] += x3*y;
      partial_sums_[2] += x2*y2;  partial_sums_[3] += x*y3;
      partial_sums_[4] += y3*y;   partial_sums_[5] += x3;
      partial_sums_[6] += x2*y;   partial_sums_[7] += x*y2;
      partial_sums_[8] += y3;     partial_sums_[9] += x2;
      partial_sums_[10] += x*y;   partial_sums_[11] += y2;
      partial_sums_[12] += x;     partial_sums_[13] += y;
    }
}

template <class T>
void vgl_conic_2d_regression<T>::fill_scatter_matrix()
{
  S11_.put(0,0,partial_sums_[0]); // x3*x
  S11_.put(0,1,partial_sums_[1]); // x3*y
  S11_.put(0,2,partial_sums_[2]); // x2*y2
  S11_.put(1,0,partial_sums_[1]); // x3*y
  S11_.put(1,1,partial_sums_[2]); // x2*y2
  S11_.put(1,2,partial_sums_[3]); // x*y3
  S11_.put(2,0,partial_sums_[2]); // x2*y2
  S11_.put(2,1,partial_sums_[3]); // x*y3
  S11_.put(2,2,partial_sums_[4]); // y3*y
  
  S12_.put(0,0,partial_sums_[5]); // x3
  S12_.put(0,1,partial_sums_[6]); // x2*y
  S12_.put(0,2,partial_sums_[9]); // x2
  S12_.put(1,0,partial_sums_[6]); // x2*y
  S12_.put(1,1,partial_sums_[7]); // x*y2
  S12_.put(1,2,partial_sums_[10]);// x*y
  S12_.put(2,0,partial_sums_[7]); // x*y2
  S12_.put(2,1,partial_sums_[8]); // y3
  S12_.put(2,2,partial_sums_[11]);// y2
  
  S22_.put(0,0,partial_sums_[9]);  // x2
  S22_.put(0,1,partial_sums_[10]); // x*y
  S22_.put(0,2,partial_sums_[12]); // x
  S22_.put(1,0,partial_sums_[10]); // x*y
  S22_.put(1,1,partial_sums_[11]); // y2
  S22_.put(1,2,partial_sums_[13]); // y
  S22_.put(2,0,partial_sums_[12]); // x
  S22_.put(2,1,partial_sums_[13]); // y
  S22_.put(2,2,npts_); // 1
}

template <class T>
bool vgl_conic_2d_regression<T>::fit()
{
  //Can't fit an conic with fewer than 5 points
  if(this->get_n_pts()<5)
    return false;

  //Compute the elements of the scatter matrix from the points
  this->compute_partial_sums();

  //Fill the scatter matrices
  this->fill_scatter_matrix();

  //The Bookstein solution.
  vnl_double_3x3 S_lambda =
    Dinv_*(S11_- S12_*(vnl_inverse(S22_)*vnl_transpose(S12_)));

  vnl_svd<double> svd(S_lambda);
  cost_ = static_cast<T>(svd.sigma_min());
  vnl_double_3 v1 = svd.nullvector();
  vnl_double_3 v2 = - vnl_inverse(S22_)*vnl_transpose(S12_)*v1;
  vgl_conic<double> nc(v1[0], v1[1], v1[2], v2[0], v2[1], v2[2]);

  //Transform back to original frame
  // We have xn^t nc xn = 0 in the normalized frame
  // But xn = trans_  x;
  // So (trans_ x )^t  nc  (trans_ x) = 0
  // Thus x^t ( (trans_)^t nc trans_ ) x = 0;
  // so c = trans_(nc);

  vgl_conic<double> c =   trans_(nc);
  
  //Set the Sampson approximation to fitting error
  this->set_sampson_error(c.a(), c.b(), c.c(), c.d(), c.e(), c.f());

  //Cast to original type
  conic_ = vgl_conic<T>(static_cast<T>(c.a()), static_cast<T>(c.b()), 
                        static_cast<T>(c.c()), static_cast<T>(c.d()), 
                        static_cast<T>(c.e()), static_cast<T>(c.f())); 
  return true;
}

template <class T>
void vgl_conic_2d_regression<T>::print_pointset(vcl_ostream& str)
{
  str << "Current Pointset has " << npts_ << " points\n";
  for(vcl_vector<vgl_point_2d<T> >::iterator pit = points_.begin();
      pit != points_.end(); ++pit)
    str << *pit << '\n';
}

//--------------------------------------------------------------------------
#undef VGL_CONIC_2D_REGRESSION_INSTANTIATE
#define VGL_CONIC_2D_REGRESSION_INSTANTIATE(T) \
template class vgl_conic_2d_regression<T >

#endif // vgl_conic_2d_regression_txx_
