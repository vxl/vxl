#include <mbl/mbl_data_array_wrapper.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

//: Default constructor
template<class T>
mbl_data_array_wrapper<T>::mbl_data_array_wrapper()
  : data_(0),n_(0),index_(-1)
{
}

//: Constructor
template<class T>
mbl_data_array_wrapper<T>::mbl_data_array_wrapper(const T* data, int n)
{
  set(data,n);
}

//: Initialise to return elements from data[i]
template<class T>
void mbl_data_array_wrapper<T>::set(const T* data, int n)
{
  data_    = data;
  n_       = n;
  index_ = 0;
}

//: Default destructor
template<class T>
mbl_data_array_wrapper<T>::~mbl_data_array_wrapper()
{
}

//: Number of objects available
template<class T>
int mbl_data_array_wrapper<T>::size() const
{
  return n_;
}

//: Reset so that current() returns first object
template<class T>
void mbl_data_array_wrapper<T>::reset()
{
  index_=0;
}

//: Return current object
template<class T>
const T& mbl_data_array_wrapper<T>::current()
{
  return data_[index_];
}

//: Move to next object, returning true if is valid
template<class T>
bool mbl_data_array_wrapper<T>::next()
{
  index_++;
  return (index_<n_);
}

//: Return current index
template<class T>
int mbl_data_array_wrapper<T>::index() const
{
  return index_;
}


//: Create copy on heap and return base pointer
template<class T>
mbl_data_array_wrapper< T >* mbl_data_array_wrapper<T>::clone() const
{
	return new mbl_data_array_wrapper<T>(*this);
}

//: Move to element n
//  First example has index 0
template<class T>
void mbl_data_array_wrapper<T>::set_index(int n)
{
  if ((n<0) || (n>=size()))
  {
    vcl_cerr<<"TC_VectorDataBase::set_index(n) ";
    vcl_cerr<<"n = "<<n<<" not in range 0 .. "<<size()-1<<vcl_endl;
    vcl_abort();
  }

  index_=n;
}

#define MBL_DATA_ARRAY_WRAPPER_INSTANTIATE(T) \
template class mbl_data_array_wrapper< T >;
