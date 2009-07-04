// This is brl/bbas/bsta/bsta_parzen.h
#ifndef bsta_parzen_h_
#define bsta_parzen_h_
//:
// \file
// \brief A distribution based on a number of samples (Parzen windows).
// \author Joseph L. Mundy
// \date October 12, 2008
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim
//
// Do not remove the following statement
// Approved for Public Release, Distribution Unlimited (DISTAR Case 12529)
//

#include "bsta_distribution.h"
#include <vcl_vector.h>
#include <vcl_iostream.h>

//: forward declare vnl_vector_fixed
template<class T, unsigned n> class vnl_vector_fixed;

//: forward declare parzen
template<class T, unsigned n> class bsta_parzen;

//: functor for sorting samples in descending order of likelihood
template<class T, unsigned n>
class vless
{
 public:
  vless():parz_ptr_(0){}
  vless(bsta_parzen<T, n>* parz_ptr): parz_ptr_(parz_ptr){}
  bool operator()(vnl_vector_fixed<T,n> const& va,
                  vnl_vector_fixed<T,n> const& vb) const
  {
    if (!parz_ptr_) return false;
      if (!parz_ptr_->size()) return false;
    T pda = parz_ptr_->prob_density(va);
    T pdb = parz_ptr_->prob_density(vb);
    return pdb < pda; // descending order
  }
  bool operator()(T const& va, T const& vb) const
  {
    if (!parz_ptr_) return false;
    bool comp = parz_ptr_->prob_density(vb)<parz_ptr_->prob_density(va);
    return comp;
  }
 private:
  //:pointer to the parzen distribtion for computing probability density
  bsta_parzen<T, n>* parz_ptr_;
};

//: A Parzen distribution
// used as a component of the mixture
template <class T, unsigned n>
class bsta_parzen : public bsta_distribution<T,n>
{
 public:
  typedef typename bsta_distribution<T,n>::vector_type vect_t;
  typedef vcl_vector<vect_t > sample_vector;
  typedef typename sample_vector::const_iterator sv_const_it;

 protected:
  //: The samples
  sample_vector samples_;

 public:

  bsta_parzen() {}
  virtual ~bsta_parzen() {}

  bsta_parzen(sample_vector const& samples)
  : samples_(samples) {}

  //: Insert a new sample into the distribution
  void insert_sample(const vect_t& sample)
  { samples_.push_back(sample); }

  //: Insert a vector of samples
  void insert_samples(const sample_vector & samples)
  { for (sv_const_it sit = samples.begin();
         sit != samples.end(); ++sit)
      samples_.push_back(*sit);
  }

  sample_vector samples() const
  { return samples_; }

  vect_t sample(unsigned index) const
  { if (index>=samples_.size())
    { vect_t v(T(0)); return v; }
    return samples_[index];
  }

  bool remove_sample(unsigned index)
  { if (index>=samples_.size()) return false;
    samples_.erase(samples_.begin()+index);
    return true;
  }

  //: clear the sample set
  void clear() { samples_.clear(); }

  //: number of samples
  unsigned size() const { return samples_.size(); }
};

#endif // bsta_parzen_h_
