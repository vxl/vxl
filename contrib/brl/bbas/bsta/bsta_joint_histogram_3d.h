// This is brl/bbas/bsta/bsta_joint_histogram_3d.h
#ifndef bsta_joint_histogram_3d_h_
#define bsta_joint_histogram_3d_h_
//:
// \file
// \brief A simple joint_histogram_3d class
// \author Joseph L. Mundy
// \date   May 19, 2004
//
// A templated joint_histogram_3d class.  Supports entropy calculations
//
// \verbatim
//  Modifications
//  none
// \endverbatim

#include <vector>
#include <iostream>
#include <vbl/vbl_array_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <bsta/bsta_joint_histogram_3d_base.h>
template <class T> class bsta_joint_histogram_3d :
public bsta_joint_histogram_3d_base
{
 public:
  bsta_joint_histogram_3d();

  bsta_joint_histogram_3d(const T range, const unsigned nbins,
                    const T min_prob = 0.0);

  bsta_joint_histogram_3d(const T min, const T max, const unsigned nbins,
                    const T min_prob = 0.0);

  bsta_joint_histogram_3d(const T range_a, const unsigned nbins_a,
                    const T range_b, const unsigned nbins_b,
                    const T range_c, const unsigned nbins_c,
                    const T min_prob = 0.0);

  //:More general constructor defining a signed value range
  bsta_joint_histogram_3d(const T min_a, const T max_a,
                    const unsigned nbins_a,
                    const T min_b, const T max_b,
                    const unsigned nbins_b,
                    const T min_c, const T max_c,
                    const unsigned nbins_c,
                    const T min_prob = 0.0);

 ~bsta_joint_histogram_3d() override = default;

 //: legacy use where a, b and c, have the same bin granularity
 unsigned nbins() const {
   assert(nbins_a_==nbins_b_&&nbins_a_==nbins_c_);
   return nbins_a_;}

 //: number of bins for variable a
  unsigned nbins_a() const {return nbins_a_;}

 //: number of bins for variable b
  unsigned nbins_b() const {return nbins_b_;}

 //: number of bins for variable b
  unsigned nbins_c() const {return nbins_c_;}

 //: legacy use where a and b have the same range
  T range() const {
    assert(range_a_==range_b_&&range_a_==range_c_);
    return range_a_;}

  //: range for variable a
  T range_a() const {return range_a_;}
 //: range for variable b
  T range_b() const {return range_b_;}
 //: range for variable c
  T range_c() const {return range_c_;}

 //: min value for variable a
  T min_a() const {return min_a_;}
 //: max value for variable a
  T max_a() const {return max_a_;}

 //: min value for variable b
  T min_b() const {return min_b_;}
 //: max value for variable b
  T max_b() const {return max_b_;}

 //: min value for variable b
  T min_c() const {return min_c_;}
 //: max value for variable b
  T max_c() const {return max_c_;}

  //: delta value for variable a
  T delta_a() const { return delta_a_; }
  //: delta value for variable b
  T delta_b() const { return delta_b_; }
  //: delta value for variable c
  T delta_c() const { return delta_c_; }

  T min_prob() const {return min_prob_;}

  vbl_array_3d<T> counts() const {return counts_;}

  //: update the count of the cell corresponding to the input values
  void upcount(T a, T mag_a,
               T b, T mag_b,
               T c, T mag_c);

  //: smooth histogram with a spherical Gaussian kernel
  void parzen(const T sigma);

  //: access by bin index
  T p(unsigned ia, unsigned ib, unsigned ic) const;

  //: access by value
  T p(T a, T b, T c) const;

  T volume() const;
  T entropy() const;

#if 0 // not implemented
  T mutual_information() const;
  T renyi_entropy() const;
  T entropy_marginal_a() const;
  //: The average and variance bin value for row a using counts to compute probs
  bool avg_and_variance_bin_for_row_a(const unsigned a, T & avg, T & var) const;
#endif
  void set_count(unsigned ia, unsigned ib, unsigned ic, T cnt)
  {if(ia<static_cast<unsigned>(counts_.get_row1_count())&&
      ib<static_cast<unsigned>(counts_.get_row2_count())&&
      ic<static_cast<unsigned>(counts_.get_row3_count()))
    counts_[ia][ib][ic]=cnt;
  }

  //:access by index
  T get_count(unsigned ia, unsigned ib, unsigned ic) const
  { if (ia<static_cast<unsigned>(counts_.get_row1_count())&&
        ib<static_cast<unsigned>(counts_.get_row2_count())&&
        ic<static_cast<unsigned>(counts_.get_row3_count()))
      return counts_[ia][ib][ic];
  else
    return T(0);
  }
  //:access by value
  T get_count(T a, T b, T c) const;

  //:return false if bin not found
  bool bin_at_val(
    const T a, const T b, const T c,
    int& ia, int &ib, int& ic) const;


  //: Find bin indices with largest count.
  //: Will return the first (lowest bin indices) in case of ties
  void bin_max_count(unsigned& ia, unsigned& ib, unsigned& ic) const;

  // prints only cells with non-zero probability
  void print(std::ostream& os = std::cout) const;

  //: If relative_probability_scale = true, then the largest probability sphere will occupy one histogram cell. Otherwise only p=1 will occupy a full cell.
  // bin axis lines: a = red, b = green, c = blue
  // color arguments define color of bin probability spheres
  void print_to_vrml(std::ostream& os, bool relative_prob_scale = true,
                     T red = T(1), T green = T(0), T blue = T(0)) const;
  void print_to_text(std::ostream& os) const;

  //:restore to default constructor state
  void clear();

 private:
  void compute_volume() const; // mutable const
  mutable bool volume_valid_;
  mutable T volume_;
  unsigned nbins_a_, nbins_b_, nbins_c_;
  T range_a_, range_b_, range_c_;
  T delta_a_, delta_b_, delta_c_;
  T min_a_, max_a_;
  T min_b_, max_b_;
  T min_c_, max_c_;
  T min_prob_;
  vbl_array_3d<T> counts_;
};
#include <bsta/bsta_joint_histogram_3d_sptr.h>
#define BSTA_JOINT_HISTOGRAM_3D_INSTANTIATE(T) extern "Please #include <bsta/bsta_joint_histogram_3d.hxx>"

#endif // bsta_joint_histogram_3d_h_
