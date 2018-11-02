// This is brl/bbas/bsta/bsta_joint_histogram.h
#ifndef bsta_joint_histogram_h_
#define bsta_joint_histogram_h_
//:
// \file
// \brief A simple joint_histogram class
// \author Joseph L. Mundy
// \date   May 19, 2004
//
// A templated joint_histogram class.  Supports entropy calculations
//
// \verbatim
//  Modifications
//   06/01/2010  Brandon A. Mayer. Added clear() function so that a single joint histogram
//               instance may revert to the default constructor and be reused.
//   06/01/2010  Brandon A. Mayer. Added mutual_information() function
// \endverbatim

#include <vector>
#include <iostream>
#include <vbl/vbl_array_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bsta/bsta_joint_histogram_base.h>

template <class T> class bsta_joint_histogram : public bsta_joint_histogram_base
{
 public:
  bsta_joint_histogram();

  bsta_joint_histogram(const T range, const unsigned int nbins,
                       const T min_prob = 0.0);

  bsta_joint_histogram(const T range_a, const unsigned int nbins_a,
                       const T range_b, const unsigned int nbins_b,
                       const T min_prob = 0.0);
  //:More general constructor defining a signed value range
  bsta_joint_histogram(const T min_a, const T max_a,
                       const unsigned int nbins_a,
                       const T min_b, const T max_b,
                       const unsigned int nbins_b,
                       const T min_prob = 0.0);

  ~bsta_joint_histogram() override = default;

  //: legacy use where a and b have the same bin granularity
  unsigned int nbins() const { return nbins_a_; }

  //: number of bins for variable a
  unsigned int nbins_a() const {return nbins_a_;}
  //: number of bins for variable b
  unsigned int nbins_b() const {return nbins_b_;}

  //: legacy use where a and b have the same range
  T range() const {return range_a_;}

  //: range for variable a
  T range_a() const {return range_a_;}
  //: range for variable b
  T range_b() const {return range_b_;}
  //: min value for variable a
  T min_a() const {return min_a_;}
  //: max value for variable a
  T max_a() const {return max_a_;}
  //: min value for variable b
  T min_b() const {return min_b_;}
  //: max value for variable b
  T max_b() const {return max_b_;}
  //: delta value for variable a
  T delta_a() const { return delta_a_; }
  //: delta value for variable b
  T delta_b() const { return delta_b_; }

  T min_prob() const {return min_prob_;}

  vbl_array_2d<T> counts() const {return counts_;}

  //:Return read-only reference to count data
  const vbl_array_2d<T>& counts() {return counts_;}

  void upcount(T a, T mag_a,
               T b, T mag_b);
  void parzen(const T sigma);

  //: access by bin index
  T p(unsigned int a, unsigned int b) const;

  //: access by value
  T p(T a, T b) const;

  //: conditional probabilities
  T p_a_given_b(unsigned a, unsigned b) const;
  T p_a_given_b(T a, T b) const;

  T p_b_given_a(unsigned a, unsigned b) const;
  T p_b_given_a(T a, T b) const;

  //: bin index for a value
  bool index_a(T a, unsigned& indx) const;
  bool index_b(T b, unsigned& indx) const;

  T volume() const;
  T entropy() const;
  T mutual_information() const;
  T renyi_entropy() const;
  T entropy_marginal_a() const;

  void print(std::ostream& os = std::cout) const;

  //: The average and variance bin value for row a using counts to compute probs
  bool avg_and_variance_bin_for_row_a(const unsigned int a, T & avg, T & var) const;

  void set_count(unsigned r, unsigned c, T cnt)
    { if (r<static_cast<unsigned>(counts_.rows())&&
          c<static_cast<unsigned>(counts_.cols()))
      counts_[r][c]=cnt;
    }

  //:access by index
  T get_count(unsigned r, unsigned c) const
    { if (r<static_cast<unsigned>(counts_.rows())&&
          c<static_cast<unsigned>(counts_.cols()))
      return counts_[r][c];
    else
      return T(0);
    }
  //:access by value
  T get_count(T a, T b) const;

  //: green box indicates the a-b origin
  // the red bar designates the a axis of the histogram, cyan the b axis.
  void print_to_vrml(std::ostream& os) const;

  void print_to_m(std::ostream& os) const;
  void print_to_text(std::ostream& os) const;

  //: green box indicates the a-b origin
  // the red bar designates the a axis of the histogram, cyan the b axis.
  void print_cond_prob_to_vrml(std::ostream& os, bool a_given_b = true/*else b_given_a*/) const;

  //:restore to default constructor state
  void clear();

 private:
  void compute_volume() const; // mutable const
  void compute_cond_counts() const;
  mutable bool volume_valid_;
  mutable bool cond_counts_valid_;
  mutable T volume_;
  unsigned int nbins_a_, nbins_b_;
  T range_a_, range_b_;
  T delta_a_, delta_b_;
  T min_a_, max_a_;
  T min_b_, max_b_;
  T min_prob_;
  vbl_array_2d<T> counts_;
  std::vector<T> sum_counts_of_a_given_b_;
  std::vector<T> sum_counts_of_b_given_a_;
};
#include <bsta/bsta_joint_histogram_sptr.h>
#define BSTA_JOINT_HISTOGRAM_INSTANTIATE(T) extern "Please #include <bsta/bsta_joint_histogram.hxx>"

#endif // bsta_joint_histogram_h_
