// This is brl/bbas/bsta/bsta_von_mises.h
#ifndef bsta_von_mises_h_
#define bsta_von_mises_h_
//:
// \file
// \brief A distribution over unit vectors (orientation on a d-sphere)
// \author Joseph L. Mundy
// \date July 6, 2009
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim
//
// do not remove the following text
// Approved for public release, distribution unlimited (DISTAR Case 14389)
//

#include "bsta_distribution.h"
#include <vnl/vnl_math.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

//: A Von_Mises distribution
template <class T, unsigned n>
class bsta_von_mises : public bsta_distribution<T,n>
{
 public:
  typedef typename bsta_distribution<T,n>::vector_type vector_type;
  typedef typename bsta_distribution<T,n>::math_type math_type;
  typedef typename bsta_distribution<T,n>::field_type field_type;
  enum { data_dimension = n };
  bsta_von_mises(): mean_(vector_type(T(0))), kappa_(T(1)){}
  bsta_von_mises(vector_type const& mean, T kappa): mean_(mean), kappa_(kappa){}
  virtual ~bsta_von_mises() {}
  //: The mean vector
  const vector_type& mean() const {return mean_;}
  void set_mean(const vector_type& mean){mean_ = mean;}

  //: the concentration parameter (kappa)
  T kappa() const {return kappa_;}
  void set_kappa(T kappa) {kappa_=kappa;}

  //: The probability density at a given unit vector
  T prob_density(vector_type const& v) const{vcl_cerr << "not implemented\n"; return 0;}

  //:Probability of an angular range of vectors about v, which defines a cone.
  T probability(vector_type const& v,const T theta_max) const {vcl_cerr << "not implemented\n"; return 0;}

 protected:
  //: The mean vector
  vector_type mean_;
  //: The concentration parameter (kappa)
  T kappa_;
};


#if VCL_CAN_DO_PARTIAL_SPECIALIZATION
template <class T>
class bsta_von_mises<T,3>
{
 public:
  typedef typename bsta_distribution<T,3>::math_type math_type;
  typedef typename bsta_distribution<T,3>::vector_type vector_type;
  typedef typename bsta_distribution<T,3>::field_type field_type;
  enum { data_dimension = 3 };
  bsta_von_mises(): mean_(vector_type(T(0))), kappa_(T(1)){mean_[2]=T(1);}
  bsta_von_mises(vector_type const& mean, T kappa): mean_(mean), kappa_(kappa){}
  virtual ~bsta_von_mises() {}
  //: The mean vector
  const vector_type& mean() const {return mean_;}
  void set_mean(const vector_type& mean){mean_ = mean;}

  //: the concentration parameter (kappa)
  T kappa() const {return kappa_;}
  void set_kappa(T kappa) {kappa_=kappa;}
  //: The probability density at a given unit vector
  T prob_density(vector_type const& v) const;

  //:Probability of an angular range of vectors about v, which defines a cone.
  // theta_max is in radians
  T probability(vector_type const& v,const T theta_max) const;

 protected:
  //: The mean vector
  vector_type mean_;
  //: The concentration parameter (kappa)
  T kappa_;
};


template <class T>
class bsta_von_mises<T,2>
{
 public:
  typedef typename bsta_distribution<T,2>::math_type math_type;
  typedef typename bsta_distribution<T,2>::vector_type vector_type;
  typedef typename bsta_distribution<T,2>::field_type field_type;
  enum { data_dimension = 2 };
  bsta_von_mises(): mean_(vector_type(T(0))), kappa_(T(1)){mean_[1]=T(1);}
  bsta_von_mises(vector_type const& mean, T kappa): mean_(mean), kappa_(kappa){}
  virtual ~bsta_von_mises() {}
  //: The mean vector
  const vector_type& mean() const {return mean_;}
  void set_mean(const vector_type& mean){mean_ = mean;}

  //: the concentration parameter (kappa)
  T kappa() const {return kappa_;}
  void set_kappa(T kappa) {kappa_=kappa;}

  //: The probability density at a given unit vector
  T prob_density(vector_type const& v) const;

  //:Probability of an angular range of vectors about v, which defines a cone.
  T probability(vector_type const& v,const T theta_max) const {vcl_cerr << "not implemented\n"; return 0;}

 protected:
  //: The mean vector
  vector_type mean_;
  //: The concentration parameter (kappa)
  T kappa_;
};

#endif //VCL_CAN_DO_PARTIAL_SPECIALIZATION

template <class T , unsigned n>
inline vcl_ostream& operator<< (vcl_ostream& os,
                                bsta_von_mises<T, n> const& vm)
{
  os << "von_mises:mean(" << vm.mean() << ")\n"
     << "von_mises:kappa(" << vm.kappa() << ")\n";
  return os;
}
#endif // bsta_von_mises_h_
