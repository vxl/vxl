// This is mul/mbl/mbl_selected_data_wrapper.txx
#ifndef mbl_selected_data_wrapper_txx_
#define mbl_selected_data_wrapper_txx_
//:
// \file

#include "mbl_selected_data_wrapper.h"

#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>

//: Default constructor.
template<class T>
mbl_selected_data_wrapper<T>::mbl_selected_data_wrapper():
  data_(0), index_(0)
{
}

//: Constructor.
template<class T>
mbl_selected_data_wrapper<T>::mbl_selected_data_wrapper(
  const mbl_data_wrapper<T>& data, const vcl_vector<unsigned> &selection)
  : data_(0)
{
  set(data, selection);
}

//: Copy constructor.
template<class T>
mbl_selected_data_wrapper<T>::mbl_selected_data_wrapper(
  const mbl_selected_data_wrapper<T>& p)
  : data_(0), index_(0)
{
  *this = p;
}

template<class T>
void mbl_selected_data_wrapper<T>::set(const mbl_data_wrapper<T>& data,
                                       const vcl_vector<unsigned> &selection)
{
  delete data_;
  data_ = data.clone();
  selection_ = selection;
  reset();
}

//: Default destructor
template<class T>
mbl_selected_data_wrapper<T>::~mbl_selected_data_wrapper()
{
  delete data_;
}

//: Copy operator
template<class T>
mbl_selected_data_wrapper<T>& mbl_selected_data_wrapper<T>::operator=(
                                       const mbl_selected_data_wrapper<T>& b)
{
  if (this==&b) return *this;

  delete data_; data_=0;
  if (b.data_!=0)
    data_=b.data_->clone();

  index_ = b.index_;
  selection_ = b.selection_;

  return *this;
}


//: Number of objects available
template<class T>
unsigned long mbl_selected_data_wrapper<T>::size() const
{
  return selection_.size();
}

//: Reset so that current() returns first object
template<class T>
void mbl_selected_data_wrapper<T>::reset()
{
  index_ = 0;
  if (size() > 0)
    data_->set_index(selection_[index_]);
}

//: Return current object
template<class T>
const T& mbl_selected_data_wrapper<T>::current()
{
  return data_->current();
}

//: Move to next object, returning true if is valid
template<class T>
bool mbl_selected_data_wrapper<T>::next()
{
  if (index_ < size()-1)
  {
    index_++;
    data_->set_index(selection_[index_]);
    return true;
  }
  else
    return false;
}

//: Return current index
template<class T>
unsigned long mbl_selected_data_wrapper<T>::index() const
{
  return index_;
}

//: Create copy on heap and return base pointer
template<class T>
mbl_data_wrapper<T>* mbl_selected_data_wrapper<T>::clone() const
{
  return (mbl_data_wrapper<T> *)
    new mbl_selected_data_wrapper<T>(*this);
}

//: Move to element n
//  First example has index 0
template<class T>
void mbl_selected_data_wrapper<T>::set_index(unsigned long n)
{
  assert(n != ((unsigned)-1));
  if (n>=size())
  {
    vcl_cerr<<"mbl_selected_data_wrapper<T>::set_index(n) :\n"
            <<"  n = "<<n<<" not in range 0<=n<"<<size()<<vcl_endl;
    vcl_abort();
  }
  index_=n;
  data_->set_index(selection_[index_]);
}

template <class T>
bool mbl_selected_data_wrapper<T>::is_class(vcl_string const& s) const
{
  return s==is_a(); // no ref to parent's is_class() since that is pure virtual
}


#define MBL_SELECTED_DATA_WRAPPER_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string mbl_selected_data_wrapper<T >::is_a() const \
{ return vcl_string("mbl_selected_data_wrapper<" #T ">"); } \
template class mbl_selected_data_wrapper< T >

#endif // mbl_selected_data_wrapper_txx_
