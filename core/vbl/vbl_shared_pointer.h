// This is core/vbl/vbl_shared_pointer.h
#ifndef vbl_shared_pointer_h_
#define vbl_shared_pointer_h_
//:
// \file
// \brief Non-intrusive smart pointers
// \author fsm
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
//   13 Feb. 2007 Amitha Perera   Change implementation to allow base class conversions.
// \endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define vbl_shared_pointer_zero(var) (var) = 0


struct vbl_shared_pointer_data
{
  int use_count; //!< number of shared_pointers using object.
  vbl_shared_pointer_data(int u) : use_count(u) { }
};

//: Non-intrusive smart pointers
//
// If your compiler supports member templates, these pointers will
// also work with base classes and derived classes, so that they work
// very much like raw pointers. If you do this, make sure your
// destructors are virtual (as you'd need to do for raw pointers
// anyway).
template <class T>
class vbl_shared_pointer
{
 public:
  typedef T element_type;
  typedef vbl_shared_pointer<T> self;

  typedef vbl_shared_pointer_data data_t;

  vbl_shared_pointer() : pointer(nullptr), count_data(nullptr) { }

  explicit
  vbl_shared_pointer(T *p) {
    if (p) {
      pointer = p;
      count_data = new data_t(1);
    } else {
      pointer = nullptr;
      count_data = nullptr;
    }
  }

  vbl_shared_pointer(self const &that)
    : pointer( that.pointer ),
      count_data( that.count_data )
  {
    up_ref();
  }

  template<class U> friend class vbl_shared_pointer;

  //: Construct using smart pointer to derived class.
  template <class U>
  vbl_shared_pointer( vbl_shared_pointer<U> const &that )
    : pointer( that.pointer ),
      count_data( that.count_data )
  {
    up_ref();
  }

  template <class U>
  self &operator=( vbl_shared_pointer<U> const &that) {
    that.up_ref();
    down_ref();
    pointer = that.pointer;
    count_data = that.count_data;
    return *this;
  }


#if 0
  // Remove these convenience methods because they conflict with the
  // base class conversion of the raw pointer. That is, we should be
  // able to do
  //    vbl_shared_pointer<base> p = new derived;
  // but these overloads prevent that.
  // if T has a constructor T::T(3, "foo") then it's nice
  // to be able to say
  //   vbl_shared_pointer<T> sp(3, "foo");
  // instead of
  //   vbl_shared_pointer<T> sp = new T(3, "foo");
  template </*typename*/ class V1>
  explicit vbl_shared_pointer(V1 const &v1)
    : data(new data_t(new T(v1), 1)) { }

  template <class V1, class V2>
  explicit vbl_shared_pointer(V1 const &v1, V2 const &v2)
    : data(new data_t(new T(v1, v2), 1)) { }

  template <class V1, class V2, class V3>
  explicit vbl_shared_pointer(V1 const &v1, V2 const &v2, V3 const &v3)
    : data(new data_t(new T(v1, v2, v3), 1)) { }

  template <class V1, class V2, class V3, class V4>
  explicit vbl_shared_pointer(V1 const &v1, V2 const &v2, V3 const &v3, V4 const &v4)
    : data(new data_t(new T(v1, v2, v3, v4), 1)) { }
#endif

  self &operator=(self const &that) {
    that.up_ref();
    down_ref();
    pointer = that.pointer;
    count_data = that.count_data;
    return *this;
  }

  ~vbl_shared_pointer() {
    down_ref();
  }

 private:

 public:
  // conversion to bool
  explicit operator bool () const
    { return (pointer != 0)? true : false; }

  // inverse conversion to bool
  bool operator!() const
    { return (pointer != 0)? false : true; }

  // conversion to pointer
#if !defined VBL_SHARED_POINTER_OF_NON_COMPOUND // Get rid of warning with vbl_shared_pointer<int>
  T const *operator->() const { return as_pointer(); }
  T       *operator->() { return as_pointer(); }
#endif

  // conversion to T
  T const &operator*() const { return *as_pointer(); }
  T       &operator*() { return *as_pointer(); }

  // relational
  bool operator!=(self const &that) const { return pointer != that.pointer; }
  bool operator==(self const &that) const { return pointer == that.pointer; }
  bool operator< (self const &that) const { return pointer <  that.pointer; }

  // use these if you like, but at your own risk.
  T *as_pointer() const {
    return pointer;
  }
  void up_ref() const {
    if (count_data)
      ++ count_data->use_count;
  }
  void down_ref() const {
    if (count_data && (-- count_data->use_count == 0)) {
      delete pointer;
      delete count_data;
    }
  }
 private:
  T *pointer;    //!< pointer to object.
  data_t *count_data;
};

#define VBL_SHARED_POINTER_INSTANTIATE(T) // template class vbl_shared_pointer<T >

#endif // vbl_shared_pointer_h_
