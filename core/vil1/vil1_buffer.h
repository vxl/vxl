#ifndef vil_buffer_h_
#define vil_buffer_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

// Purpose: an exception-safe way to allocate a contiguous array
// of memory of a given type. The ISO vector<> is not suitable as
// there is no guarantee of contiguity.

template <class T>
struct vil_buffer
{  
  typedef T *iterator;
  typedef T *const_iterator;

  unsigned size() const { return size_; }

  T       *data() { return data_; }
  // called something other than 'begin', to make it easy to grep for 'begin'.
  T const *data() const { return data_; }

  iterator begin() { return data_; }
  iterator end() { return data_ + size_; }

  const_iterator begin() const { return data_; }
  const_iterator end() const { return data_ + size_; }

  T       &operator[](unsigned i) { return data_[i]; }
  T const &operator[](unsigned i) const { return data_[i]; }
  
  vil_buffer(unsigned n) : data_(new T[n]), size_(n) { }
  vil_buffer(unsigned n, T const &x) : data_(new T[n]), size_(n) { 
    for (unsigned i=0; i<n; ++i)
      data_[i] = x;
  }
  ~vil_buffer() { delete [] data_; data_ = 0; }
  
private:
  T       *data_;
  unsigned size_;
  // disallow:
  vil_buffer() { }
  void operator=(vil_buffer<T> const &) { }
};

#endif // vil_buffer_h_
