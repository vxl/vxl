#include <vcsl/vcsl_axis.h>
// Once the compiler sees the primary template
// definition of vbl_smart_ptr<T>::ref() and unref()
// in vbl_smart_ptr.txx, it is no longer allowed to
// use a smart pointer of incomplete class as a data
// member in another class. So the above #include
// is not redundant, but necessary to satisfy the
// container requirements for std::vector<>. fsm.

#include <vcsl/vcsl_cartesian.h>
#include <vbl/vbl_smart_ptr.txx>

VBL_SMART_PTR_INSTANTIATE(vcsl_cartesian);

#if 0
template <typename T>
struct wrap
{
  wrap () : ptr_(0) { }
  wrap (wrap<T> const &p) : ptr_(p.ptr_) { if (ptr_) f(); }
  wrap (T *p) : ptr_(p) { }
  wrap<T> &operator = (wrap<T> const &r) { ptr_ = r.ptr_; return *this; }

  void f();

private:
  T *ptr_;
};

#include <vcl_vector.h>

class X;

void g(X *x)
{
  vcl_vector<wrap<X> > v;
  v.push_back(x);
};

#if 1
template <typename T>
void wrap<T>::f()
{
  ptr_->f();
}
#endif // 1
#endif // 0
