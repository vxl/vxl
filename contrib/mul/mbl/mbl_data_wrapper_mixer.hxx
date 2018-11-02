// This is mul/mbl/mbl_data_wrapper_mixer.hxx
#ifndef mbl_data_wrapper_mixer_hxx_
#define mbl_data_wrapper_mixer_hxx_
//:
// \file

#include <iostream>
#include <cstdlib>
#include "mbl_data_wrapper_mixer.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Default constructor
template<class T>
mbl_data_wrapper_mixer<T>::mbl_data_wrapper_mixer()
  : n_(0),index_(0),current_wrapper_(0)
{
}

//: Constructor
template<class T>
mbl_data_wrapper_mixer<T>::mbl_data_wrapper_mixer(mbl_data_wrapper<T> **wrapper, unsigned long n_wrappers)
{
  set(wrapper,n_wrappers);
}

//: Constructor
template<class T>
mbl_data_wrapper_mixer<T>::mbl_data_wrapper_mixer(std::vector<mbl_data_wrapper<T> *> wrapper)
{
  set(&wrapper[0],wrapper.size());
}

//: Constructor
template<class T>
mbl_data_wrapper_mixer<T>::mbl_data_wrapper_mixer(mbl_data_wrapper<T>& wrapper1,
                                                  mbl_data_wrapper<T>& wrapper2)
{
  std::vector<mbl_data_wrapper<T>*> w(2);
  w[0] = &wrapper1;
  w[1] = &wrapper2;
  set(&w[0],w.size());
}

//: Initialise to return elements from data[i]
template<class T>
void mbl_data_wrapper_mixer<T>::set(mbl_data_wrapper<T> **wrapper, unsigned long n_wrappers)
{
  assert (n_wrappers != ((unsigned long)-1)); // a common mistake
  wrapper_.resize(n_wrappers);
  n_       = 0;
  for (unsigned long i=0;i<n_wrappers;++i)
  {
    wrapper_[i] = wrapper[i];
    n_ += wrapper[i]->size();
  }
  current_wrapper_ = 0;
  wrapper_[0]->reset();
  set_index(0);
}

//: Default destructor
template<class T>
mbl_data_wrapper_mixer<T>::~mbl_data_wrapper_mixer() = default;

//: Number of objects available
template<class T>
unsigned long mbl_data_wrapper_mixer<T>::size() const
{
  return n_;
}

//: Reset so that current() returns first object
template<class T>
void mbl_data_wrapper_mixer<T>::reset()
{
  index_=0;
  current_wrapper_=0;
  if (wrapper_.size()>0) wrapper_[0]->reset();
}

//: Return current object
template<class T>
const T& mbl_data_wrapper_mixer<T>::current()
{
  return wrapper_[current_wrapper_]->current();
}

//: Move to next object, returning true if is valid
template<class T>
bool mbl_data_wrapper_mixer<T>::next()
{
  index_++;

  // Increment current and check for end of data in current wrapper
  if (index_<n_ && !wrapper_[current_wrapper_]->next())
  {
    // Find next non-empty wrapper
    if (current_wrapper_<(wrapper_.size()-1)) current_wrapper_++;

    while (current_wrapper_<(wrapper_.size()-1) && wrapper_[current_wrapper_]->size()==0)
      current_wrapper_++;

    wrapper_[current_wrapper_]->reset();
  }

  return index_<n_;
}

//: Return current index
template<class T>
unsigned long mbl_data_wrapper_mixer<T>::index() const
{
  return index_;
}

//: Create copy on heap and return base pointer
template<class T>
mbl_data_wrapper< T >* mbl_data_wrapper_mixer<T>::clone() const
{
  return new mbl_data_wrapper_mixer<T>(*this);
}

//: Move to element n
//  First example has index 0
template<class T>
void mbl_data_wrapper_mixer<T>::set_index(unsigned long n)
{
  assert(n != ((unsigned)-1));
  if (n>=n_)
  {
    std::cerr<<"mbl_data_wrapper_mixer<T>::set_index(n) :\n"
            <<"  n = "<<n<<" not in range 0<=n<"<<size()<<std::endl;
    std::abort();
  }

  index_=n;

  // Determine which wrapper to use
  current_wrapper_=0;
  unsigned long sum_n = wrapper_[0]->size();
  while (index_>=sum_n)
    { current_wrapper_++; sum_n += wrapper_[current_wrapper_]->size(); }
  unsigned long offset = index_ - (sum_n - wrapper_[current_wrapper_]->size());
  wrapper_[current_wrapper_]->set_index(offset);
}

template <class T>
bool mbl_data_wrapper_mixer<T>::is_class(std::string const& s) const
{
  return s==is_a(); // no ref to parent's is_class() since that is pure virtual
}


#undef MBL_DATA_WRAPPER_MIXER_INSTANTIATE
#define MBL_DATA_WRAPPER_MIXER_INSTANTIATE(T) \
template <> std::string mbl_data_wrapper_mixer<T >::is_a() const \
{ return std::string("mbl_data_wrapper_mixer<" #T ">"); } \
template class mbl_data_wrapper_mixer< T >

#endif // mbl_data_wrapper_mixer_hxx_
