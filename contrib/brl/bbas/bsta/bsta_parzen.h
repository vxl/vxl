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
#include <vnl/vnl_math.h>
//#include <vcl_set.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
//: forward declare vnl_vector_fixed
template<class T, unsigned n> class vnl_vector_fixed;

//: forward declare parzen
template<class T, unsigned n> class bsta_parzen;

//: functor for sorting samples in descending order of likelyhood
template<class T, unsigned n>
class vless
{
public:
  vless():parz_ptr_(0){}
  vless(bsta_parzen<T, n>* parz_ptr): parz_ptr_(parz_ptr){}
  bool operator()(vnl_vector_fixed<T,n> const& va,
                  vnl_vector_fixed<T,n> const& vb) const
    { 
      if(!parz_ptr_) return false;
	  if(!parz_ptr_->size()) return false;
    T pda = parz_ptr_->prob_density(va);
    T pdb = parz_ptr_->prob_density(vb);
    return pdb < pda; // descending order
    }
  bool operator()(T const& va, T const& vb) const
    { 
      if(!parz_ptr_) return false;
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
  typedef typename bsta_distribution<T,n>::vector_type vector_;

 public:
  
  bsta_parzen(){}

  bsta_parzen(vcl_vector<vector_> const& samples):
  samples_(samples){}


  //: The mean of the distribution
  virtual vector_ mean() const = 0;

  //: Insert a new sample into the distribution
  void insert_sample(const vector_& sample)
    {samples_.push_back(sample);}

  //: Insert a vector of samples
  void insert_samples(const vcl_vector<vector_> & samples)
   {for(vcl_vector<vector_>::const_iterator sit = samples.begin();
        sit != samples.end(); ++sit) samples_.push_back(*sit);}

  vcl_vector<vector_> samples() const 
    {	return samples_;}

  vector_ sample(unsigned index) const 
    {if(index>=samples_.size())
      {vector_ v; v.fill(T(0)); return v;}
    return samples_[index];
    }

  bool remove_sample(unsigned index)
    {if(index>=samples_.size())return false; 
    samples_.erase(samples_.begin()+index); return true;
    }


  //: clear the sample set
  void clear() { samples_.clear();}

  //: number of samples
  unsigned size() const {return samples_.size();}

  //: The probability density at sample pt
  virtual T prob_density(const vnl_vector_fixed<T,n>& pt) const = 0;

  //: The probability density integrated over a box (returns a probability)
  virtual T probability(const vnl_vector_fixed<T,n>& min_pt,
                        const vnl_vector_fixed<T,n>& max_pt) const = 0;

 protected:

  //: The samples
  vcl_vector<vector_> samples_;
};

#endif // bsta_parzen_h_
