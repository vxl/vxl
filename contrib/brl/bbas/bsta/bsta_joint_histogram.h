// This is brl/bbas/bsta/bsta_joint_histogram.h
#ifndef bsta_joint_histogram_h_
#define bsta_joint_histogram_h_
//:
// \file
// \brief A simple joint_histogram class
// \author Joseph L. Mundy
// \date 5/19/04
//
// A templated joint_histogram class.  Supports entropy calculations
// 
//
// \verbatim
//  Modifications
// \endverbatim
#include <vbl/vbl_array_2d.h>
template <class T> class bsta_joint_histogram
{
 public:
  bsta_joint_histogram(const T range = 360, const unsigned int nbins = 8,
                  const T min_prob = 0.0);//0.005
 ~bsta_joint_histogram() {}
  unsigned int nbins() const { return nbins_; }
  void upcount(T a, T mag_a,
               T b, T mag_b);
  void parzen(const T sigma);

  T p(unsigned int a, unsigned int b) const;
  T volume() const;
  T entropy() const;
  T renyi_entropy() const;
  void print() const;
 private:
  void compute_volume() const; // mutable const
  mutable bool volume_valid_;
  mutable T volume_;
  unsigned int nbins_;
  T range_;
  T delta_;
  T min_prob_;
  vbl_array_2d<T> counts_;
};

#define BSTA_JOINT_HISTOGRAM_INSTANTIATE(T) extern "Please #include <bsta/bsta_joint_histogram.txx>"

#endif // bsta_joint_histogram_h_
