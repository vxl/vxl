#===========================================================================
# from vcl/aclocal.m4

### find path to C++ headers, if possible
AC_DEFUN(VCL_CXX_HEADERS,[
AC_CACHE_CHECK(path to C++ headers,[
AC_MSG_CHECKING( standard C++ headers )
CXX_HDRDIR=""; export CXX_HDRDIR
cat > conftest.cc <<!
#include <iostream.h>
!
CXX_HDRDIR=`$CXXCPP conftest.cc \
            | egrep '^\#[ \t]*[0-9]+[ \t].*iostream' \
            | sed -e 's/\/iostream.*$//g' \
            | sed -e 's/^.*\"//g'`
for i in $CXX_HDRDIR; do
  CXX_HDRDIR=$i;
  break;
done
rm -f conftest.cc
AC_MSG_RESULT( $CXX_HDRDIR )
],,vcl_cv_cxx_headers=$CXX_HDRDIR,[])
])


###
AC_DEFUN(VCL_CXX_HAS_BOOL,[
AC_MSG_CHECKING(whether the C++ compiler supports the keyword 'bool')
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
void function(int i, void *ptr, bool v) {}
],,VCL_HAS_BOOL=1;AC_MSG_RESULT(yes),VCL_HAS_BOOL=0;AC_MSG_RESULT(no))
export VCL_HAS_BOOL
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_HAS_DYNAMIC_CAST,[
AC_MSG_CHECKING(whether the C++ compiler supports the keyword 'dynamic_cast')
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
struct foo { foo(); virtual ~foo(); virtual void f() =0; };
struct boo : public foo { void f() { *(int*)0 = 1; } };
boo *try_dynamic_cast() { boo *b = 0; foo *f = b; return dynamic_cast<boo*>(f); }
],,[VCL_HAS_DYNAMIC_CAST=1;AC_MSG_RESULT(yes)],[VCL_HAS_DYNAMIC_CAST=0;AC_MSG_RESULT(no)])
export VCL_HAS_DYNAMIC_CAST
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_HAS_RTTI,[
AC_MSG_CHECKING(whether the C++ compiler supports RTTI, viz the 'typeid' function)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
#include <typeinfo>
class A { public: virtual ~A() {} virtual void f() {} };
class B : public A { public: void f() {} };
bool try_rtti() { B*b=0; A*a1=b,*a2=b; return typeid(a1)==typeid(a2); }
],,[VCL_HAS_RTTI=1;AC_MSG_RESULT(yes)],[VCL_HAS_RTTI=0;AC_MSG_RESULT(no)])
export VCL_HAS_RTTI
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_HAS_TYPENAME,[
AC_MSG_CHECKING(whether the C++ compiler supports the keyword 'typename')
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE(
[
template <typename T>
class bingo { public: void bongo(T **); };
],,[VCL_HAS_TYPENAME=1;AC_MSG_RESULT(yes)],[VCL_HAS_TYPENAME=0;AC_MSG_RESULT(no)])
export VCL_HAS_TYPENAME
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_HAS_EXPORT,[
AC_MSG_CHECKING(whether the C++ compiler accepts the keyword 'export')
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE(
[
export
template <class T, int N>
struct plither
{
  plither();
  ~plither();
  void f(T *, int);
};

void g()
{
  double x;
  int y;
  plither<double, 3> obj;
  obj.f(&x, y);
}
],,[VCL_HAS_EXPORT=1;AC_MSG_RESULT(yes)],[VCL_HAS_EXPORT=0;AC_MSG_RESULT(no)])
export VCL_HAS_EXPORT
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_HAS_MUTABLE,[
AC_MSG_CHECKING(whether the C++ compiler supports the keyword 'mutable')
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE(
[
class X { public: mutable int const *p; };
],,[VCL_HAS_MUTABLE=1;AC_MSG_RESULT(yes)],[VCL_HAS_MUTABLE=0;AC_MSG_RESULT(no)])
export VCL_HAS_MUTABLE
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_HAS_EXPLICIT,[
AC_MSG_CHECKING(whether the C++ compiler supports the keyword 'explicit')
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE(
[
class X { public: explicit X(int ) {} };
],,[VCL_HAS_EXPLICIT=1;AC_MSG_RESULT(yes)],[VCL_HAS_EXPLICIT=0;AC_MSG_RESULT(no)])
export VCL_HAS_EXPLICIT
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_HAS_EXCEPTIONS,[
AC_MSG_CHECKING(whether the C++ compiler has working exceptions)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
struct bizop {};

int functionella(char const *a, char const *b)
{
  if (!a &&  b) throw "a is no good";
  if ( a && !b) throw "b is no better";
  if (!a && !b) throw bizop();
  return *a - *b;
}

void monkeylette()
{
  try {
    functionella(  0,   0);
    functionella("a",   0);
    functionella(  0, "b");
    functionella("a", "b");
  }
  catch (char const *s) {
    // oops.
  }
  catch (bizop b) {
    // outch
  }
  catch (...) {
    // phew
  }
}
],,[VCL_HAS_EXCEPTIONS=1;AC_MSG_RESULT(yes)],[VCL_HAS_EXCEPTIONS=0;AC_MSG_RESULT(no)])
export VCL_HAS_EXCEPTIONS
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_HAS_NAMESPACES,[
AC_MSG_CHECKING(whether the C++ compiler has working namespaces)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
namespace foo {
  int hello() { return 10; }
};

// 7.3.1
namespace Outer {
  int i;
  namespace Inner {
    void f() { i++; } // Outer::i
    int i;
    void g() { i++; } // Inner::i
  }
}

// 7.3.1.1
namespace { int i; } // unique::i
void f() { i++; }    // unique::i  (gcc 2.7.2 fails here).

namespace A {
  namespace {
    int i;           // A::unique::i
    int j;           // A::unique::j
  }
  void g() { i++; }  // A::unique::i
}

using namespace A;
void h() {
  //i++;               // error: unique::i or A::unique::i
  A::i++;              // A::unique::i
  j++;                 // A::unique::j
}

extern "C" double sqrt(double);

namespace foo {
  template <class T> struct complex { T re, im; };
  template <class T> T abs(complex<T> const &z) { return T(::sqrt(double(z.re*z.re + z.im+z.im))); }
}

namespace bar {
  int abs(int);
  long abs(long);
  float abs(float);
  double abs(double);
}

namespace diced {
  using foo::complex; // <-- I'm told vc60 fails here.
  using foo::abs;
  using bar::abs;
}

extern "C" int printf(char const *, ...);

void flegg() {
  int a = -1;
  long b = -2;
  float c = -3;
  double d = -4;
  diced::complex<double> e = { 3, 4 };
  printf("%d\n",  diced::abs(a)); // 1
  printf("%ld\n", diced::abs(b)); // 2
  printf("%f\n",  diced::abs(c)); // 3
  printf("%lf\n", diced::abs(d)); // 4
  printf("%lf\n", diced::abs(e)); // 5
}
],,[VCL_HAS_NAMESPACES=1;AC_MSG_RESULT(yes)],[VCL_HAS_NAMESPACES=0;AC_MSG_RESULT(no)])
export VCL_HAS_NAMESPACES
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_ALLOWS_NAMESPACE_STD,[
AC_MSG_CHECKING(whether the C++ compiler allows std:: for the standard library)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
#include <cmath>
#include <vector>
#include <iostream>
void function() {
  std::vector<double> flaz;
  flaz.push_back(std::sqrt(2.0));
  flaz.push_back(std::fabs(1.0f));
  std::cerr << "hello, std::world" << std::endl;
}
],,[VCL_ALLOWS_NAMESPACE_STD=1;AC_MSG_RESULT(yes)],[VCL_ALLOWS_NAMESPACE_STD=0;AC_MSG_RESULT(no)])
export VCL_ALLOWS_NAMESPACE_STD
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_NEEDS_NAMESPACE_STD,[
AC_MSG_CHECKING(whether the C++ compiler needs std:: for the standard library)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
#include <cmath>
#include <vector>
//#include <iostream>
void function() {
  vector<double> flaz;
  flaz.push_back(sqrt(2.0));
  flaz.push_back(fabs(1.0f));
  //cerr << "hello, world" << endl;
}
],,[VCL_NEEDS_NAMESPACE_STD=0;AC_MSG_RESULT(no)],[VCL_NEEDS_NAMESPACE_STD=1;AC_MSG_RESULT(yes)])
export VCL_NEEDS_NAMESPACE_STD
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_CHECK_FOR_SCOPE,[
AC_MSG_CHECKING(whether the C++ compiler supports ISO for scope)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
class A { public: void f() {} };
void fn() {
  for (int i=0; i<100; ++i) {}
  for (long i=0; i<1000; ++i) {}
  for (double i = 3.141; i<100.0; i += 1.0) {}
  // VC7 only raises warnings for previous tests
  A i; i.f();
}
],,[VCL_FOR_SCOPE_HACK=0;AC_MSG_RESULT(yes)],[VCL_FOR_SCOPE_HACK=1;AC_MSG_RESULT(no)])
export VCL_FOR_SCOPE_HACK
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_HAS_MEMBER_TEMPLATES,[
AC_MSG_CHECKING(whether the C++ compiler supports member templates)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
template <class S>
class blip {
 public:
  S *ptr;
  template <class T>
  void klor(T *p) { *ptr = *p; }
};
void function()
{
  blip<double> b;
  int s;
  b.klor(&s);
}
],,[VCL_HAS_MEMBER_TEMPLATES=1;AC_MSG_RESULT(yes)],[VCL_HAS_MEMBER_TEMPLATES=0;AC_MSG_RESULT(no)])
export VCL_HAS_MEMBER_TEMPLATES
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_CAN_DO_PARTIAL_SPECIALIZATION,[
AC_MSG_CHECKING(whether the C++ compiler supports partial specialization)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
template <class T>
class victor
{
  T data[256];
 public:
  victor() {}
  T &operator[](unsigned i) { return data[i]; }
};

template <class T>
class victor<T *>
{
  T *data[256];
 public:
  T * &operator[](unsigned i) { return data[i]; }
  void slarf() { data[0] += (data[2] - data[1]); }
};

template <class A, class R>
struct foo {
  typedef A a;
  typedef R r;
};

template <class T>
struct foo<T *, T *> {
  void bar() {}
};

template <class T>
struct foo<int *, T> {
  void baz() {}
};
],,[VCL_CAN_DO_PARTIAL_SPECIALIZATION=1;AC_MSG_RESULT(yes)],[VCL_CAN_DO_PARTIAL_SPECIALIZATION=0;AC_MSG_RESULT(no)])
export VCL_CAN_DO_PARTIAL_SPECIALIZATION
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_COMPLEX_POW_WORKS,[
AC_MSG_CHECKING([whether the C++ compiler has a fully functional pow(complex,complex)])
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_RUN([
// It appears several programmers have (independently)
// not realised their lack of knowledge of complex numbers.
// pow(complex(-1,0),0.5) should return (0,1) not (Nan,0), etc.
#include <complex>
int main()
{
  const std::complex<double> neg1(-1.0,0.0);
  const std::complex<double> half(0.5,0.0);
  std::complex<double> sqrt_neg1 = std::pow(neg1, 0.5);
  if ( std::abs(sqrt_neg1.real()) > 1e-6 || std::abs(sqrt_neg1-1.0) < 1e-6 ) return 1;
  sqrt_neg1 = std::pow(neg1, half);
  if ( std::abs(sqrt_neg1.real()) > 1e-6 || std::abs(sqrt_neg1-1.0) < 1e-6 ) return 1;
  sqrt_neg1 = std::pow(-1.0, half);
  if ( std::abs(sqrt_neg1.real()) > 1e-6 || std::abs(sqrt_neg1-1.0) < 1e-6 ) return 1;
  return 0; // success
}
],,[VCL_COMPLEX_POW_WORKS=1;AC_MSG_RESULT(yes)],[VCL_COMPLEX_POW_WORKS=0;AC_MSG_RESULT(no)])
export VCL_COMPLEX_POW_WORKS
AC_LANG_RESTORE])



###
AC_DEFUN(VCL_CXX_DEFAULT_VALUE,[
AC_MSG_CHECKING(whether the C++ compiler needs default values in definition of function taking default values in declaration)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
// declaration
void function(int x, char *ptr = "foo");
// definition
void function(int x, char *ptr) { ++ ptr[x]; }
],,[VCL_DEFAULT_VALUE="/* no need */";AC_MSG_RESULT(no)],[VCL_DEFAULT_VALUE=" = x";AC_MSG_RESULT(yes)])
export VCL_DEFAULT_VALUE
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_DEFINE_SPECIALIZATION,[
AC_MSG_CHECKING(whether the C++ compiler understands the 'template <>' specialization syntax)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
// declaration
template <class T>
class traits { public: };

// specialization
template <>
class traits<double> {
 public:
  typedef double abs_t;
  typedef double float_t;
};
],,[VCL_DEFINE_SPECIALIZATION="template <>";VCL_DEF_SPEC=1;AC_MSG_RESULT(yes)],[VCL_DEFINE_SPECIALIZATION="/* template <> */";VCL_DEF_SPEC=0;AC_MSG_RESULT(no)])
export VCL_DEFINE_SPECIALIZATION
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_CANNOT_SPECIALIZE_CV,[
AC_MSG_CHECKING(whether the C++ compiler can distinguish cv-qualified specialization)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
given_CXXFLAGS="$CXXFLAGS -DVCL_DEF_SPEC=$VCL_DEF_SPEC"
VCL_COMPILE_CXX
AC_TRY_COMPILE([
template <class T> struct A;
#if VCL_DEF_SPEC
#define VCL_DEFINE_SPECIALIZATION template <>
#else
#define VCL_DEFINE_SPECIALIZATION
#endif
VCL_DEFINE_SPECIALIZATION struct A<int> {};
VCL_DEFINE_SPECIALIZATION struct A<int const> {};
],,[VCL_CANNOT_SPECIALIZE_CV=0;AC_MSG_RESULT(yes)],[VCL_CANNOT_SPECIALIZE_CV=1;AC_MSG_RESULT(no)])
export VCL_CANNOT_SPECIALIZE_CV
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_STATIC_CONST_INIT_INT,[
AC_MSG_CHECKING(whether the C++ compiler allows initialization of static const int)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_RUN([
class A {
 public:
  static const int x = 27;
  static const bool y = false;
};
int main() { return A::x == 27 && !A::y ? 0 : 1; }
],,[VCL_STATIC_CONST_INIT_INT=1;AC_MSG_RESULT(yes)],[VCL_STATIC_CONST_INIT_INT=0;AC_MSG_RESULT(no)])
export VCL_STATIC_CONST_INIT_INT
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_STATIC_CONST_INIT_NO_DEFN,[
AC_MSG_CHECKING(whether the C++ compiler incorrectly allocates storage for a static const)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
class A { public: static const int x = 27; };
int f(const void* x) { return x?1:0; }
int not_main() { return f(&A::x); }
],,[VCL_STATIC_CONST_INIT_NO_DEFN=1;AC_MSG_RESULT(yes)],[VCL_STATIC_CONST_INIT_NO_DEFN=0;AC_MSG_RESULT(no)])
export VCL_STATIC_CONST_INIT_NO_DEFN
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_STATIC_CONST_INIT_FLOAT,[
AC_MSG_CHECKING(whether the C++ compiler allows initialization of static const floats)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_RUN([
class A {
 public:
  static const float x = 27.0f;
  static const double y = 27.0;
};
int main() { return A::x == 27.0f && A::y == 27.0 ? 0 : 1; }
],,[VCL_STATIC_CONST_INIT_FLOAT=1;AC_MSG_RESULT(yes)],[VCL_STATIC_CONST_INIT_FLOAT=0;AC_MSG_RESULT(no)])
export VCL_STATIC_CONST_INIT_FLOAT
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_OVERLOAD_CAST,[
AC_MSG_CHECKING(whether the C++ compiler requires explicit casts where it should not)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
template <class T>
class vnl_vector {
 public:
  unsigned size;
  T *data;
  vnl_vector(unsigned n, T *ptr) : size(n), data(ptr) {}
};
template <class T>
bool operator==(vnl_vector<T> const &a, vnl_vector<T> const &);
//
template <unsigned n, class T>
class vnl_vector_fixed : public vnl_vector<T> {
 public:
  T data_fixed[n];
  vnl_vector_fixed() : vnl_vector<T>(n, data_fixed) {}
};
//
void print_it(vnl_vector<double> const &);
//
void try_it(vnl_vector_fixed<3, double> const &u,
            vnl_vector_fixed<3, double> const &v)
{
  // gcc 2.7 fails in this function.
  if (u == v) print_it(u);
  else { print_it(u); print_it(v); }
}
//
template <class S, class T>
void    copy_image(S const * const *src, T * const *dst, int, int);

typedef unsigned char byte;

void do_vision(int w, int h, byte **image_i, float **image_f) {
  // SGI CC 7.21 fails here.
  copy_image(image_i, image_f, w, h);
}
],,[VCL_OVERLOAD_CAST="(x)";AC_MSG_RESULT(no)],[VCL_OVERLOAD_CAST="((T)(x))";AC_MSG_RESULT(yes)])
export VCL_OVERLOAD_CAST
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_NULL_TMPL_ARGS,[
AC_MSG_CHECKING(whether the C++ compiler requires <> in templated forward/friend declarations)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_LINK([
template <class T> class victor;
template <class T> T dot(victor<T> const &u, victor<T> const &v);

template <class T> class victor {
 public:
  // Without -fguiding-decls, egcs and 2.95 will rightly think
  // this declares a non-template and so the program will fail
  // due to access violation below (and missing symbols at link time).
  friend T dot /* <> */ (victor<T> const &, victor<T> const &);
 private:
  T data[3];
};

template <class T> T dot(victor<T> const &u, victor<T> const &v)
{
  return  // access violation here:
    u.data[0] * v.data[0] +
    u.data[1] * v.data[1] +
    u.data[2] * v.data[2];
}

template double dot(victor<double> const &, victor<double> const &);

double function(victor<double> const &u,
                victor<double> const &v)
{
  double uu = dot(u, u);
  double uv = dot(u, v);
  double vv = dot(v, v);
  return (uv*uv)/(uu*vv);
}
],,[VCL_NULL_TMPL_ARGS="/* <> */";AC_MSG_RESULT(no)],[VCL_NULL_TMPL_ARGS="<>";AC_MSG_RESULT(yes)])
export VCL_NULL_TMPL_ARGS
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD,[
AC_MSG_CHECKING(whether the C++ compiler needs friend declarations for proper template function overloading)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
template <class T>
class victor_base {
 public:
  T &operator[](unsigned i) { return data[i]; }
 protected:
  victor_base(T *p, unsigned n) : data(p), size(n) {}
 private:
  T *data;
  unsigned size;
};

template <class T>
bool operator==(victor_base<T> const &, victor_base<T> const &);

template <class T, int n>
class victor_fixed : public victor_base<T> {
 public:
  T data_fixed[n];
  victor_fixed() : victor_base<T>(data_fixed, n) {}
};

int function(victor_fixed<double, 3> const &a,
             victor_fixed<double, 3> const &b)
{
  if (a == b) // 2.7 fails to resolve this.
    return 3141;
  else
    return 2718;
}
],,[VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD=0;AC_MSG_RESULT(no)],[VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD=1;AC_MSG_RESULT(yes)])
export VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_HAS_TEMPLATE_SYMBOLS,[
AC_MSG_CHECKING(whether the C++ compiler mangles templated and non-templated functions differently)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_LINK([
// Declare a function template.
template <class T> void function(T *ptr, int n);

int caller()
{
  double array[3];
  function(array, 0); // This should call function<double>(double *, int);
  return 0;
}

// Define a non-template function with the same name and signature.
void function(double *, int) {}

// If the program links, the compiler didn't make a distinction.
],,[VCL_HAS_TEMPLATE_SYMBOLS=0;AC_MSG_RESULT(no)],[VCL_HAS_TEMPLATE_SYMBOLS=1;AC_MSG_RESULT(yes)])
export VCL_HAS_TEMPLATE_SYMBOLS
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_COMPILE_CXX,[
CXXFLAGS=$given_CXXFLAGS
export CXXFLAGS
])


###
AC_DEFUN(VCL_COMPILE_TXX,[
CXXFLAGS=$given_TXXFLAGS
export CXXFLAGS
])


###
AC_DEFUN(VCL_CXX_ALLOWS_INLINE_INSTANTIATION,[
AC_MSG_CHECKING(whether the C++ compiler allows explicit instantiation of inline templates)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_TXX
AC_TRY_COMPILE([
template <class T> inline T dot(T const *a, T const *b) { return a[0]*b[0]; }
template double dot(double const *, double const *);
],,[VCL_ALLOWS_INLINE_INSTANTIATION=1;AC_MSG_RESULT(yes)],[VCL_ALLOWS_INLINE_INSTANTIATION=0;AC_MSG_RESULT(no)])
export VCL_ALLOWS_INLINE_INSTANTIATION
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_NO_STATIC_DATA_MEMBERS,[
AC_MSG_CHECKING(whether the C++ compiler allows static data members)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_TXX
AC_TRY_COMPILE([
template <class T>
class vvv { static T xxx; };
template class vvv<int>;
],,[VCL_NO_STATIC_DATA_MEMBERS=0;AC_MSG_RESULT(yes)],[VCL_NO_STATIC_DATA_MEMBERS=1;AC_MSG_RESULT(no)])
export VCL_NO_STATIC_DATA_MEMBERS
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_NEEDS_INLINE_INSTANTIATION,[
AC_MSG_CHECKING(whether the C++ compiler needs explicit instantiation of inline function templates)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_LINK([
template <class T>
inline
T dot(T const *a, T const *b)
{
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}
//template double dot(double const *, double const *);
int function();
int call_this() { function(); return 0; }
int function()
{
  double a[3] = {1.0, 2.0, 3.0};
  double b[3] = {4.0, 5.0, 6.0};
  double a_b = dot(a, b);
  return int(a_b);
}
// If the program links, the compiler inlined the function template.
],,[VCL_NEEDS_INLINE_INSTANTIATION=0;AC_MSG_RESULT(no)],[VCL_NEEDS_INLINE_INSTANTIATION=1;AC_MSG_RESULT(yes)])
export VCL_NEEDS_INLINE_INSTANTIATION
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_SUNPRO_CLASS_SCOPE_HACK,[
AC_MSG_CHECKING(whether the C++ compiler needs the SunPro class scope hack)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
template < class T >
struct allocator
{
  allocator() {}
  allocator(const allocator<T>&) {}
};

template < class T , class Allocator = allocator < T > >
struct vector
{
  vector() {}
 ~vector() {}
};

template < class T >
struct spoof { void set_row(unsigned, vector< T /*, allocator<T>*/ > const&); };

template < class T >
void spoof < T > :: set_row ( unsigned , vector < T /*, allocator<T>*/ > const & ) {}

template class spoof<double>;

// If the program compiles, we don't need the hack
],,[VCL_SUNPRO_CLASS_SCOPE_HACK="/* , A */";AC_MSG_RESULT(no)],[VCL_SUNPRO_CLASS_SCOPE_HACK=", A";AC_MSG_RESULT(yes)])
export VCL_SUNPRO_CLASS_SCOPE_HACK
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_DEFAULT_TMPL_ARG,[
AC_MSG_CHECKING(whether the C++ compiler needs default template arguments repeated)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
struct alloc1 {};
struct alloc2 {};
template <class T, class A = alloc1> class X;
template <class T, class A> class X { public: T data[3]; A a; };
template class X<short>;
// If the program compiles, we don't need to repeat them
],,[VCL_DEFAULT_TMPL_ARG="/* no need */";AC_MSG_RESULT(no)],[VCL_DEFAULT_TMPL_ARG="arg";AC_MSG_RESULT(yes)])
export VCL_DEFAULT_TMPL_ARG
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER,[
AC_MSG_CHECKING(whether the C++ compiler accepts complete types as default template parameters)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
template <class T> struct less {};

template <class T, class C=less<int> >
struct X
{
  typedef X<T,C> self;
  self foo (self const & t) {
    if ( t.a == 0 ) return *this;
    else return t;
  }
 private:
  int a;
};

X<int> a;
X<int, less<short> > b;
],,[VCL_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER=1;AC_MSG_RESULT(yes)],[VCL_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER=0;AC_MSG_RESULT(no)])
export VCL_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER,[
AC_MSG_CHECKING(whether the C++ compiler accepts default template type parameters templated over earlier parameters)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
template <class T> struct less {};
template <class T, class C=less<T> >
struct X { C t1; };
X<int> a;
X<int, less<short> > b;
],,[VCL_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER=1;AC_MSG_RESULT(yes)],[VCL_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER=0;AC_MSG_RESULT(no)])
export VCL_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_CAN_DO_STATIC_TEMPLATE_MEMBER,[
AC_MSG_CHECKING(whether the C++ compiler accepts templated definitions of static class template members)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
template <class T>
struct A { A() {} static char *fmt; };
template <class T> char *A<T>::fmt = 0;
],,[VCL_CAN_DO_STATIC_TEMPLATE_MEMBER=1;AC_MSG_RESULT(yes)],[VCL_CAN_DO_STATIC_TEMPLATE_MEMBER=0;AC_MSG_RESULT(no)])
export VCL_CAN_DO_STATIC_TEMPLATE_MEMBER
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER,[
AC_MSG_CHECKING(whether the C++ compiler accepts non-type template parameters to function templates)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
template <class T, int n>
struct splek { T data[n]; };
template <class T, int n>
void splok_that_splek(splek<T, n> &s)
{
  for (int i=0; i<n; ++i)
    s.data[i] = T(27);
}
template struct splek<double, 3>;
template void splok_that_splek(splek<double, 3> &);
],,[VCL_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER=1;AC_MSG_RESULT(yes)],[VCL_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER=0;AC_MSG_RESULT(no)])
export VCL_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_CAN_DO_IMPLICIT_TEMPLATES,[
AC_MSG_CHECKING(whether the C++ compiler instantiates templates implicitly)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_LINK([
struct fsm_plap_normal {};
template <class I>
inline fsm_plap_normal fsm_plap(I) { return fsm_plap_normal(); }

template <class I, class T>
void fsm_plop(I b, I e, T x, fsm_plap_normal)
{
  for (I p=b; p!=e; ++p)
    *p = x;
}

struct fsm_plap_double_star {};
inline fsm_plap_double_star fsm_plap(double *) { return fsm_plap_double_star(); }

template <class T>
void fsm_plop(double *b, double *e, T x, fsm_plap_double_star)
{
  for (double *p=b; p<e; ++p)
    *p = x;
}

template <class I, class T>
inline void fsm_plip(I b, I e, T x)
{
  if (b != e)
    fsm_plop(b, e, x, fsm_plap(b));
}

void f()
{
  int iarray[20];
  fsm_plip(iarray, iarray+20, 3141);
  double darray[20];
  fsm_plip(darray, darray+20, 2718);
}
],,[VCL_CAN_DO_IMPLICIT_TEMPLATES=1;AC_MSG_RESULT(yes)],[VCL_CAN_DO_IMPLICIT_TEMPLATES=0;AC_MSG_RESULT(no)])
export VCL_CAN_DO_IMPLICIT_TEMPLATES
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_TEMPLATE_MATCHES_TOO_OFTEN,[
AC_MSG_CHECKING(whether the C++ compiler incorrectly chooses template over specialisation)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_COMPILE([
class A {};
template <class T> void f(T t) { t.compiler_selected_wrong_overload(); }
void f(const A&) {}
int not_main() { f(A()); return 0; }
],,[VCL_TEMPLATE_MATCHES_TOO_OFTEN=0;AC_MSG_RESULT(no)],[VCL_TEMPLATE_MATCHES_TOO_OFTEN=1;AC_MSG_RESULT(yes)])
export VCL_TEMPLATE_MATCHES_TOO_OFTEN
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_HAS_SLICED_DESTRUCTOR_BUG,[
AC_MSG_CHECKING(whether the C++ compiler forgets to destruct a temporary)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_RUN([
// Some compilers (at least Intel C++ 7) will create a B temporary on
// the f(c) line below and call both the A and B constructors, but
// then destroy the temporary by calling only ~A() and not calling
// ~B() first (or ever).  This program exits with 1 in such a case.
#include <stdlib.h>
struct A
{
  int mark;
  A() : mark(0) {}
  A(A const&): mark(0) {}
 ~A() { if (mark) { exit(1); } }
};
struct B: public A
{
  B(): A() {}
  B(B const& b): A(b) { mark = 1; }
 ~B() { mark = 0; }
};
struct C
{
  operator B () { return B(); }
};
void f(A) {}
int main() { C c; f(c); return 0; }
],,[VCL_HAS_SLICED_DESTRUCTOR_BUG=0;AC_MSG_RESULT(no)],[VCL_HAS_SLICED_DESTRUCTOR_BUG=1;AC_MSG_RESULT(yes)])
export VCL_HAS_SLICED_DESTRUCTOR_BUG
AC_LANG_RESTORE])

###
AC_DEFUN(VCL_CXX_CHAR_IS_SIGNED,[
AC_MSG_CHECKING(whether char is signed)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_RUN([
// Return 0 for char signed and 1 for char unsigned.
int main() {
  unsigned char uc = 255;
  return (*reinterpret_cast<char*>(&uc) < 0)?0:1;
}],,[VCL_CHAR_IS_SIGNED=1;AC_MSG_RESULT(yes)],[VCL_CHAR_IS_SIGNED=0;AC_MSG_RESULT(no)])
export VCL_CHAR_IS_SIGNED
AC_LANG_RESTORE])

###
AC_DEFUN(VCL_CXX_NUMERIC_LIMITS_HAS_INFINITY,[
AC_MSG_CHECKING(whether std::numeric_limits<float>::has_infinity == 1)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_RUN([
// Several versions of gcc (3.0, 3.1, and 3.2) come with a
// numeric_limits that reports that they have no infinity.
#include <limits>
int main() {
  return std::numeric_limits<double>::has_infinity &&
         std::numeric_limits<float>::has_infinity ? 0 : 1;
}],,[VCL_NUMERIC_LIMITS_HAS_INFINITY=1;AC_MSG_RESULT(yes)],[VCL_NUMERIC_LIMITS_HAS_INFINITY=0;AC_MSG_RESULT(no)])
export VCL_NUMERIC_LIMITS_HAS_INFINITY
AC_LANG_RESTORE])


###
AC_DEFUN(VCL_CXX_PROCESSOR_HAS_INFINITY,[
AC_MSG_CHECKING(whether the processor actually has an infinity)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
VCL_COMPILE_CXX
AC_TRY_RUN([
#include <cfloat>
union u {  double d;  unsigned char c[8]; };
int main()
{
  if (sizeof(double) != 8) return 1;
  // If you have an odd machine, then add your own construction of infinity.
  u v;
  // Can we generate an IEEE infinity artifically on a big-endian machine?
  v.c[0] = 0x7f; v.c[1] = 0xf0;
  v.c[2] = v.c[3] = v.c[4] = v.c[5] = v.c[6] = v.c[7] = 0x00;
  if (v.d > DBL_MAX) return 0;
  // Can we generate an IEEE infinity artifically on a little-endian machine?
  v.c[7] = 0x7f; v.c[6] = 0xf0;
  v.c[0] = v.c[1] = v.c[2] = v.c[3] = v.c[4] = v.c[5] = 0x00;
  if (v.d > DBL_MAX) return 0;
  return 1;
}],,[VCL_PROCESSOR_HAS_INFINITY=1;AC_MSG_RESULT(yes)],[VCL_PROCESSOR_HAS_INFINITY=0;AC_MSG_RESULT(no)])
export VCL_PROCESSOR_HAS_INFINITY
AC_LANG_RESTORE])


#===========================================================================
# from vxl/aclocal.m4

dnl ----------------------------------------------------------------------------
dnl  VXL_CXX_UNISTD
dnl ---------------------------------------------------------------------------

AC_DEFUN(VXL_CXX_UNISTD,[
echo "checking <unistd.h>..."

# first get preprocessed unistd.h :
cat > check_vxl_unistd.c <<EOF
#include <unistd.h>
EOF
eval "$ac_cpp check_vxl_unistd.c" 2>&5 > check_vxl_unistd.i;

# caveat: sometimes __useconds_t is defined, hence the space
if (egrep "typedef.* useconds_t;" check_vxl_unistd.i >/dev/null 2>&1); then
  VXL_UNISTD_HAS_USECONDS_T=1;
  echo "... for useconds_t... yes"
else
  VXL_UNISTD_HAS_USECONDS_T=0;
  echo "... for useconds_t... no"
fi; export VXL_UNISTD_HAS_USECONDS_T;

# caveat: sometimes __intptr_t is defined, hence the space
if (egrep "typedef.* intptr_t;" check_vxl_unistd.i >/dev/null 2>&1); then
  VXL_UNISTD_HAS_INTPTR_T=1;
  echo "... for intptr_t... yes"
else
  VXL_UNISTD_HAS_INTPTR_T=0;
  echo "... for intptr_t... no"
fi; export VXL_UNISTD_HAS_INTPTR_T;

echo "... if usleep() returns void"
AC_TRY_COMPILE(
[#include <unistd.h>
],[{ int x = usleep(0); }],[VXL_UNISTD_USLEEP_IS_VOID=0;],[VXL_UNISTD_USLEEP_IS_VOID=1;])
export VXL_UNISTD_USLEEP_IS_VOID;

rm -f check_vxl_unistd.c check_vxl_unistd.i
AC_CHECK_FUNC(ualarm,VXL_UNISTD_HAS_UALARM=1,VXL_UNISTD_HAS_UALARM=0)
AC_CHECK_FUNC(usleep,VXL_UNISTD_HAS_USLEEP=1,VXL_UNISTD_HAS_USLEEP=0)
AC_CHECK_FUNC(lchown,VXL_UNISTD_HAS_LCHOWN=1,VXL_UNISTD_HAS_LCHOWN=0)
AC_CHECK_FUNC(pread,VXL_UNISTD_HAS_PREAD=1,VXL_UNISTD_HAS_PREAD=0)
AC_CHECK_FUNC(pwrite,VXL_UNISTD_HAS_PWRITE=1,VXL_UNISTD_HAS_PWRITE=0)
AC_CHECK_FUNC(tell,VXL_UNISTD_HAS_TELL=1,VXL_UNISTD_HAS_TELL=0)
])


dnl ----------------------------------------------------------------------------
dnl  Usage: VXL_CXX_WORDS
dnl ----------------------------------------------------------------------------
AC_DEFUN(VXL_CXX_WORDS,[
AC_MSG_CHECKING( [for machine word sizes] )
cat > check_vxl_words.cc <<EOF
#include <stdio.h>
#include <limits.h>
#ifndef CHAR_BIT
# define CHAR_BIT 8
#endif

// this is a silly workaround. on most machines, the configure
// script will cat the 2-character sequence \" as a 2-character
// sequence. however, julia@robots.ox.ac.uk "expands it" to a
// single quote character first. the obvious solution, which is
// to add extra backslashes will fix it for julia, but break it
// for other machines. so to print a quote, we use its ascii
// value.
// note that the backslashes in the macro 'macro' are expanded
// by configure, but we dont care about that.
#define QUOTE 34

#define macro(NAME, n, cand, cnd2) \
  if (CHAR_BIT==8 && sizeof(cnd2)==n) \
    printf("VXL_" #NAME "=%c" #cnd2 "%c;\nVXL_HAS_" #NAME "=%c1%c;\n" , QUOTE, QUOTE, QUOTE, QUOTE); \
  else if (CHAR_BIT==8 && sizeof(cand)==n) \
    printf("VXL_" #NAME "=%c" #cand "%c;\nVXL_HAS_" #NAME "=%c1%c;\n" , QUOTE, QUOTE, QUOTE, QUOTE); \
  else \
    printf("VXL_" #NAME "=%c" "void" "%c;\nVXL_HAS_" #NAME "=%c0%c;\n" , QUOTE, QUOTE, QUOTE, QUOTE); \
  printf("export VXL_" #NAME ";\nexport VXL_HAS_" #NAME ";\n" )

int main(int, char **) {
  macro(BYTE, 1, char, char);
  macro(INT_8, 1, char, short);
  macro(INT_16, 2, short, int);
  macro(INT_32, 4, long, int);
  macro(INT_64, 8, long long, long);
  macro(IEEE_32, 4, double, float);
  macro(IEEE_64, 8, long double, double);
  macro(IEEE_96, 12, long double, double);  // x86
  macro(IEEE_128, 16, long double, double); // sparc, mips
  if (sizeof(long)==8)
    printf("VXL_INT_64_IS_LONG=1;\n");
  else
    printf("VXL_INT_64_IS_LONG=0;\n");
  return 0;
}
EOF
if eval "$CXX ./check_vxl_words.cc -o ./check_vxl_words.exe" 2>&5; then
  eval `./check_vxl_words.exe` 2>&5
  AC_MSG_RESULT( ok )
else
  AC_MSG_RESULT( error )
fi
rm -f ./check_vxl_words.*
])


dnl ----------------------------------------------------------------------------
dnl  Usage: VXL_HAS_QSORT : do we have a qsort() function?
dnl ---------------------------------------------------------------------------
AC_DEFUN(VXL_HAS_QSORT,[
AC_MSG_CHECKING(whether we have a working qsort)
AC_LANG_SAVE
AC_LANG_C
AC_TRY_COMPILE(
[
#include <stdlib.h>
int f(const void *a,const void *b) { return 1; }
/* configure provides its own main(), so putting one here makes the test fail even if qsort() is available */
int not_main(void) { int a[5]; qsort(a, 5, sizeof(int), f); return 0; }
],,VXL_STDLIB_HAS_QSORT=1,VXL_STDLIB_HAS_QSORT=0)
export VXL_STDLIB_HAS_QSORT
AC_LANG_RESTORE])


dnl ----------------------------------------------------------------------------
dnl Usage AC_TWO_ARG_TIMEOFDAY : check for one or two argument gettimeofday
dnl                              also sets TIME_WITH_SYS_TIME and HAVE_SYS_TIME_H
dnl ---------------------------------------------------------------------------
AC_DEFUN(AC_TWO_ARG_TIMEOFDAY,[
# these must go first or the msg_result will get clobbered.
AC_HEADER_TIME
AC_STRUCT_TM
AC_CACHE_CHECK(whether gettimeofday takes two arguments,ac_twoarg_timeofday,[
AC_LANG_SAVE
AC_LANG_C
AC_TRY_RUN([
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
int main()
{
  struct timeval real0;
  struct timezone tz;
  gettimeofday(&real0, &tz);
  return 0;
}],ac_twoarg_timeofday=yes,ac_twoarg_timeofday=no)
AC_LANG_RESTORE])
])


dnl ------------------------------------------------------------
dnl VXL_CXX_CHECK_PROVIDES([header],[function],[args],[variable])
dnl Check if a function is declared in a header and if it is implemented
dnl in some library. If so, set variable=1 dnl else set variable=0.
dnl
dnl NB: Avoid extra spaces! In particular, trailing spaces in "variable"
dnl will cause the set command to be
dnl    variable =1
dnl and will fail. (The Bourne shell does not allow spaces around the
dnl "=" sign.)
dnl ------------------------------------------------------------
AC_DEFUN([VXL_CXX_CHECK_PROVIDES],[
  AC_MSG_CHECKING([whether <$1> provides $2()])
  AC_TRY_LINK([
  #include <$1>
  ],[
  $2 ( $3 ) ;
  ],[
  $4=1
  AC_MSG_RESULT(yes)
  ],[
  $4=0
  AC_MSG_RESULT(no)
  ])
])


dnl ------------------------------------------------------------
AC_DEFUN(VXL_MATH_HAS_FINITEF,[
AC_LANG_SAVE
AC_LANG_C
VXL_CXX_CHECK_PROVIDES([math.h],[finitef],[4.0f],[VXL_C_MATH_HAS_FINITEF])
AC_LANG_RESTORE])


dnl ------------------------------------------------------------
AC_DEFUN(VXL_MATH_HAS_FINITE,[
AC_LANG_SAVE
AC_LANG_C
VXL_CXX_CHECK_PROVIDES([math.h],[finite],[4.0],[VXL_C_MATH_HAS_FINITE])
AC_LANG_RESTORE])


dnl ------------------------------------------------------------
AC_DEFUN(VXL_MATH_HAS_FINITEL,[
AC_LANG_SAVE
AC_LANG_C
VXL_CXX_CHECK_PROVIDES([math.h],[finitel],[(long double)4],[VXL_C_MATH_HAS_FINITEL])
AC_LANG_RESTORE])


dnl ------------------------------------------------------------
dnl Check whether <math.h> provides the sqrtf() function
dnl ------------------------------------------------------------
AC_DEFUN(VXL_MATH_HAS_SQRTF,[
AC_LANG_SAVE
AC_LANG_C
VXL_CXX_CHECK_PROVIDES([math.h],[sqrtf],[4.0],[VXL_C_MATH_HAS_SQRTF])
AC_LANG_RESTORE])


dnl ------------------------------------------------------------
AC_DEFUN(VXL_IEEEFP_FINITE,[
AC_LANG_SAVE
AC_LANG_C
VXL_CXX_CHECK_PROVIDES([ieeefp.h],[finite],[4.0],[VXL_IEEEFP_HAS_FINITE])
AC_LANG_RESTORE])


dnl ------------------------------------------------------------
AC_DEFUN(VXL_STDLIB_RAND48,[
AC_LANG_SAVE
AC_LANG_C
VXL_CXX_CHECK_PROVIDES([stdlib.h],[lrand48],[],[VXL_STDLIB_HAS_LRAND48])
VXL_CXX_CHECK_PROVIDES([stdlib.h],[drand48],[],[VXL_STDLIB_HAS_DRAND48])
VXL_CXX_CHECK_PROVIDES([stdlib.h],[srand48],[0L],[VXL_STDLIB_HAS_SRAND48])
AC_LANG_RESTORE])
