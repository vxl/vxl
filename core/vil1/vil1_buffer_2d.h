#ifndef vil_buffer_2d_h_
#define vil_buffer_2d_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_buffer_2d
// .LIBRARY vil
// .INCLUDE vil/vil_buffer_2d.h
// .FILE    vil_buffer_2d.txx
// .SECTION Author
//  fsm@robots.ox.ac.uk
//
// .SECTION Description
// an exception-safe way to allocate a contiguous array
// of memory of a given type. The ISO vector<> is not suitable as
// there is no guarantee of contiguity.

template <class T>
struct vil_buffer_2d
{  
  unsigned rows() const { return x_; }
  unsigned cols() const { return y_; }

  T       *data() { return data_; }
  // called something other than 'begin', to make it easy to grep for 'begin'.
  T const *data() const { return data_; }

  T *get_ptr( unsigned x, unsigned y) { return data_+(y*y_+x); }

  T       &operator()(unsigned x, unsigned y) { return data_[y*y_+x]; }
  T const &operator()(unsigned x, unsigned y) const { return data_[y*y_+x]; }
  T const &get(unsigned x, unsigned y) const { return data_[y*y_+x]; }
  void set( unsigned x, unsigned y, T const &d) { data_[y*y_+x]= d; }

  vil_buffer_2d(unsigned x, unsigned y) : data_(new T[x*y]), x_(x), y_(y) { }
  vil_buffer_2d(unsigned x, unsigned y, T const &d) : data_(new T[x*y]), x_(x), y_(y) { 
    for (unsigned i=0; i< (x*y); ++i)
      data_[i] = d;
  }
  ~vil_buffer_2d() { delete [] data_; data_ = 0; }
  
private:
  T       *data_;
  unsigned x_;
  unsigned y_;
  // disallow:
  vil_buffer_2d() { }
  void operator=(vil_buffer_2d<T> const &) { }
};

#endif // vil_buffer_2d_h_
