//-------------------------------------

#ifdef vcl_cv_cxx_has_bool
/* (vcl_cv_cxx_has_bool=yes)
   (vcl_cv_cxx_has_bool=no)
   (vcl_cv_cxx_has_bool)
*/

void function(int i, void *ptr, bool v) {}

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_has_typename
/* (vcl_cv_cxx_has_typename=yes)
   (vcl_cv_cxx_has_typename=no)
   (vcl_cv_cxx_has_typename)
*/

template <typename T>
class bingo { public: void bongo(T **); };

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_has_export
/* (vcl_cv_cxx_has_export=yes)
   (vcl_cv_cxx_has_export=no)
   (vcl_cv_cxx_has_export)
*/

export
template <class T, int N>
struct plither
{
  plither(){}
  ~plither(){}
  void f(T *, int){}
};

void g()
{
  double x;
  int y;
  plither<double, 3> obj;
  obj.f(&x, y);
}

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_has_mutable
/* (vcl_cv_cxx_has_mutable=yes)
   (vcl_cv_cxx_has_mutable=no)
   (vcl_cv_cxx_has_mutable)
*/

class X {
 public:
  mutable int const *p;
};

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_has_explicit
/* (vcl_cv_cxx_has_explicit=yes)
   (vcl_cv_cxx_has_explicit=no)
   (vcl_cv_cxx_has_explicit)
*/

class X { public: explicit X(int) {} };

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_has_dynamic_cast
/* (vcl_cv_cxx_has_dynamic_cast=yes)
   (vcl_cv_cxx_has_dynamic_cast=no)
   (vcl_cv_cxx_has_dynamic_cast)
*/

struct foo { foo(){} virtual ~foo(){} virtual void f()=0; };
struct boo : public foo { void f() { *(int*)0 = 1; } };
boo *try_dynamic_cast() { boo *b = 0; foo *f = b; return dynamic_cast<boo*>(f); }

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_check_for_scope
/* (vcl_cv_cxx_check_for_scope=yes)
   (vcl_cv_cxx_check_for_scope=no)
   (vcl_cv_cxx_check_for_scope)
*/

void fn() {
  for (int i=0; i<100; ++i) {}
  for (long i=0; i<1000; ++i) {}
  double i = 3.141; i *= -1.0;
}

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_default_value
/* (vcl_cv_cxx_default_value=no)
   (vcl_cv_cxx_default_value=yes)
   (vcl_cv_cxx_default_value)
*/

// declaration
void function(int x, char *ptr = "foo");

// definition
void function(int x, char *ptr) { ++ ptr[x]; }

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_has_member_templates
/* (vcl_cv_cxx_has_member_templates=yes)
   (vcl_cv_cxx_has_member_templates=no)
   (vcl_cv_cxx_has_member_templates)
*/

template <class S>
class blip {
 public:
  S *ptr;
  template <class T> void klor(T *p) { *ptr = *p; }
};
void function()
{
  blip<double> b;
  int s;
  b.klor(&s);
}

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_can_do_partial_specialization
/* (vcl_cv_cxx_can_do_partial_specialization=yes)
   (vcl_cv_cxx_can_do_partial_specialization=no)
   (vcl_cv_cxx_can_do_partial_specialization)
*/

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

template <class T> struct foo<T *, T *> { void bar() {} };
template <class T> struct foo<int *, T> { void baz() {} };

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_define_specialization
/* (vcl_cv_cxx_define_specialization=yes)
   (vcl_cv_cxx_define_specialization=no)
   (vcl_cv_cxx_define_specialization)
*/

// declaration
template <class T> class traits {};

// specialization
template <>
class traits<double> {
 public:
  typedef double abs_t;
  typedef double float_t;
};

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_ALLOWS_INLINE_INSTANTIATION
/* (VCL_ALLOWS_INLINE_INSTANTIATION="1"; echo """yes" 1>&6)
   (VCL_ALLOWS_INLINE_INSTANTIATION="0"; echo """no" 1>&6)
   (VCL_ALLOWS_INLINE_INSTANTIATION)
*/

template <class T>
inline
T dot(T const *a, T const *b)
{
  return a[0]*b[0];
}

template double dot(double const *, double const *);

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_NEEDS_INLINE_INSTANTIATION
/* (VCL_NEEDS_INLINE_INSTANTIATION="0"; echo """no" 1>&6)
   (VCL_NEEDS_INLINE_INSTANTIATION="1"; echo """yes" 1>&6)
   (VCL_NEEDS_INLINE_INSTANTIATION)
*/

template <class T>
inline T dot(T const *a, T const *b) { return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]; }
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

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_static_const_init_int
/* (vcl_cv_cxx_static_const_init_int=yes)
   (vcl_cv_cxx_static_const_init_int=no)
   (vcl_cv_cxx_static_const_init_int)
*/

class A {
 public:
  static const int x = 27;
  static const bool y = false;
};

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_STATIC_CONST_INIT_FLOAT
/* (VCL_STATIC_CONST_INIT_FLOAT="1"; echo """yes" 1>&6)
   (VCL_STATIC_CONST_INIT_FLOAT="0"; echo """no" 1>&6)
   (VCL_STATIC_CONST_INIT_FLOAT)
*/

class A {
 public:
  static const float x = 27.0f;
  static const double y = 27.0;
};

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_CAN_DO_STATIC_TEMPLATE_MEMBER
/* (VCL_CAN_DO_STATIC_TEMPLATE_MEMBER="1"; echo """yes" 1>&6)
   (VCL_CAN_DO_STATIC_TEMPLATE_MEMBER="0"; echo """no" 1>&6)
   (VCL_CAN_DO_STATIC_TEMPLATE_MEMBER)
*/

template <class T> struct A { A() {} static char *fmt; };
template <class T> char *A<T>::fmt = 0;

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER
/* (VCL_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER="1"; echo """yes" 1>&6)
   (VCL_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER="0"; echo """no" 1>&6)
   (VCL_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER)
*/

template <class T, int n> struct splek { T data[n]; };

template <class T, int n>
void splok_that_splek(splek<T, n> &s)
{
  for (int i=0; i<n; ++i)
    s.data[i] = T(27);
}

template struct splek<double, 3>;
template void splok_that_splek(splek<double, 3> &);

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD
/* (VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD="0"; echo """no" 1>&6)
   (VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD="1"; echo """yes" 1>&6)
   (VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD)
*/

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
bool operator==(victor_base<T> const&, victor_base<T> const&) { return false; }

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

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_OVERLOAD_CAST
/* (VCL_OVERLOAD_CAST="(x)"; echo """no" 1>&6)
   (VCL_OVERLOAD_CAST="((T)(x))"; echo """yes" 1>&6)
   (VCL_OVERLOAD_CAST)
*/

//
template <class T>
class vnl_vector {
 public:
  unsigned size;
  T *data;
  vnl_vector(unsigned n, T *ptr) : size(n), data(ptr) {}
};

template <class T>
bool operator==(vnl_vector<T> const&, vnl_vector<T> const&) { return false; }

//
template <unsigned n, class T>
class vnl_vector_fixed : public vnl_vector<T> {
 public:
  T data_fixedn;
  vnl_vector_fixed() : vnl_vector<T>(n, data_fixed) {}
};

//
void print_it(vnl_vector<double> const &){}

void try_it(vnl_vector_fixed<3, double> const &u,
            vnl_vector_fixed<3, double> const &v)
{
  // gcc 2.7 fails in this function.
  if (u == v)
    print_it(u);
  else {
    print_it(u);
    print_it(v);
  }
}

//
template <class S, class T>
void copy_image(S const * const *src, T * const *dst, int, int) {}

typedef unsigned char byte;

void do_vision(int w, int h, byte **image_i, float **image_f) {
  // SGI CC 7.21 fails here.
  copy_image(image_i, image_f, w, h);
}

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_NULL_TMPL_ARGS
/* (VCL_NULL_TMPL_ARGS=" / * <> * / "; echo """no" 1>&6)
   (VCL_NULL_TMPL_ARGS="<>"; echo """yes" 1>&6)
   (VCL_NULL_TMPL_ARGS)
*/

template <class T> class victor;
template <class T> T dot(victor<T> const &u, victor<T> const &v);

template <class T> class victor {
 public:
  // Without -fguiding-decls, egcs and 2.95 will rightly think
  // this declares a non-template and so the program will fail
  // due to access violation below (and missing symbols at link time).
  friend T dot(victor<T> const &, victor<T> const &);

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

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_NO_STATIC_DATA_MEMBERS
/* (VCL_NO_STATIC_DATA_MEMBERS="0"; echo """yes" 1>&6)
   (VCL_NO_STATIC_DATA_MEMBERS="1"; echo """no" 1>&6)
   (VCL_NO_STATIC_DATA_MEMBERS)
*/

template <class T> class vvv { static T xxx; };
template class vvv<int>;

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_HAS_TEMPLATE_SYMBOLS
/* (VCL_HAS_TEMPLATE_SYMBOLS="0"; echo """no" 1>&6)
   (VCL_HAS_TEMPLATE_SYMBOLS="1"; echo """yes" 1>&6)
   (VCL_HAS_TEMPLATE_SYMBOLS)
*/

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

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_CAN_DO_IMPLICIT_TEMPLATES
/* (VCL_CAN_DO_IMPLICIT_TEMPLATES="1"; echo """yes" 1>&6)
   (VCL_CAN_DO_IMPLICIT_TEMPLATES="0"; echo """no" 1>&6)
   (VCL_CAN_DO_IMPLICIT_TEMPLATES)
*/

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

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER
/* (VCL_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER="1"; echo """yes" 1>&6)
   (VCL_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER="0"; echo """no" 1>&6)
   (VCL_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER)
*/

template <class T> struct less {};

template <class T, class C=less<int> >
struct X
{
  typedef X<T,C> self;
  self foo(self const & t) {
    if ( t.a == 0 )
      return *this;
    else
      return t;
  }
 private:
  int a;
};

X<int> a;
X<int, less<short> > b;

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER
/* (VCL_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER="1"; echo """yes" 1>&6)
   (VCL_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER="0"; echo """no" 1>&6)
   (VCL_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER)
*/

template <class T> struct less {};
template <class T, class C=less<T> > struct X { C t1; };
X<int> a;
X<int, less<short> > b;

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VCL_SUNPRO_CLASS_SCOPE_HACK
/* (VCL_SUNPRO_CLASS_SCOPE_HACK=" / * , A * / "; echo """no" 1>&6)
   (VCL_SUNPRO_CLASS_SCOPE_HACK=", A"; echo """yes" 1>&6)
   (VCL_SUNPRO_CLASS_SCOPE_HACK)
*/

template < class T >
struct allocator
{
  allocator() {}
  allocator(const allocator<T>& ) {}
};

template < class T , class Allocator = allocator < T > >
struct vector
{
  vector() {}
  ~vector() {}
};

template < class T >
struct spoof
{
  void set_row( unsigned , vector < T /*, allocator<T>*/ > const & );
};

template < class T >
void spoof < T > :: set_row( unsigned , vector < T /*, allocator<T>*/ > const & )
{
}

template class spoof < double >;

// If the program compiles, we don't need the hack

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_has_exceptions
/* (vcl_cv_cxx_has_exceptions=yes)
   (vcl_cv_cxx_has_exceptions=no)
   (vcl_cv_cxx_has_exceptions)
*/

struct bizop {};

int functionella(char const *a, char const *b)
{
  if (!a &&  b)
    throw "a is no good";
  if ( a && !b)
    throw "b is no better";
  if (!a && !b)
    throw bizop();

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

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_has_namespaces
/* (vcl_cv_cxx_has_namespaces=yes)
   (vcl_cv_cxx_has_namespaces=no)
   (vcl_cv_cxx_has_namespaces)
*/

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

extern "C" double vxl_sqrt(double){return 0;}

namespace foo {
  template <class T> struct complex { T re, im; };
  template <class T> T abs(complex<T> const &z) { return T(::vxl_sqrt(double(z.re*z.re + z.im+z.im))); }
}

namespace bar {
  int abs(int){return 0;}
  long abs(long){return 0;}
  float abs(float){return 0;}
  double abs(double){return 0;}
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

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_allows_namespace_std
/* (vcl_cv_cxx_allows_namespace_std=yes)
   (vcl_cv_cxx_allows_namespace_std=no)
   (vcl_cv_cxx_allows_namespace_std)
*/

#include <cmath>
#include <vector>
#include <iostream>
void function() {
  std::vector<double> flaz;
  flaz.push_back(std::sqrt(2.0));
  flaz.push_back(std::fabs(1.0f));
  std::cerr << "hello, std::world" << std::endl;
}

int main() { return 0; }
#endif

//-------------------------------------

#ifdef vcl_cv_cxx_needs_namespace_std
/* (vcl_cv_cxx_needs_namespace_std=no)
   (vcl_cv_cxx_needs_namespace_std=yes)
   (vcl_cv_cxx_needs_namespace_std)
*/

#include <cmath>
#include <vector>
//#include <iostream>
void function() {
  vector<double> flaz; // correct should be:  std::vector<double> 
  flaz.push_back(sqrt(2.0));   // should be:  std::sqrt(2.0)
  flaz.push_back(fabs(1.0f));  // should be:  std::fabs(1.0)
  //cerr << "hello, world" << endl;
}

int main() { return 0; }
#endif

//-------------------------------------

#ifdef VXL_UNISTD_USLEEP_IS_VOID
/* (VXL_UNISTD_USLEEP_IS_VOID="0";)
   (VXL_UNISTD_USLEEP_IS_VOID="1";)
   (VXL_UNISTD_USLEEP_IS_VOID)
*/
#include <unistd.h>

int main() { int x = usleep(0); return x*0; }
#endif

//-------------------------------------

#ifdef ac_vxl_has_qsort
/* (ac_vxl_has_qsort=yes)
   (ac_vxl_has_qsort=no)
   (ac_vxl_has_qsort)
*/

/* This is not a C++ header, strictly speaking. */
/* Actually, it is normative but deprecated, strictly speaking :) */
#include <stdlib.h>
int f(const void *a,const void *b) { return 1; }
/* configure provides its own main(), so putting one here
   makes the test fail even if qsort() is available */
int not_main(void) { int a[5]; qsort(a, 5, sizeof(int), f); return 0; }

int main() { return 0; }
#endif

//-------------------------------------

#ifdef   VXL_MATH_HAS_FINITE
/* (VXL_MATH_HAS_FINITE=1; echo """yes" 1>&6)
   (VXL_MATH_HAS_FINITE=0; echo """no" 1>&6)
   (VXL_MATH_HAS_FINITE)
*/

#include <math.h>

int main() { finite(4.0); return 0; }
#endif

//-------------------------------------

#ifdef   VXL_MATH_HAS_SQRTF
/* (VXL_MATH_HAS_SQRTF=1; echo """yes" 1>&6)
   (VXL_MATH_HAS_SQRTF=0; echo """no" 1>&6)
   (VXL_MATH_HAS_SQRTF)
*/

#include <math.h>

int main() { sqrtf(4.0f); return 0; }
#endif

//-------------------------------------

#ifdef   VXL_IEEEFP_HAS_FINITE
/* (VXL_IEEEFP_HAS_FINITE=1; echo """yes" 1>&6)
   (VXL_IEEEFP_HAS_FINITE=0; echo """no" 1>&6)
   (VXL_IEEEFP_HAS_FINITE)
*/

#include <ieeefp.h>

int main() { finite(4.0); return 0; }
#endif

//-------------------------------------

#ifdef   VXL_STDLIB_HAS_LRAND48
/* (VXL_STDLIB_HAS_LRAND48=1; echo """yes" 1>&6)
   (VXL_STDLIB_HAS_LRAND48=0; echo """no" 1>&6)
   (VXL_STDLIB_HAS_LRAND48)
*/

#include <stdlib.h>

int main() { lrand48(); return 0; }
#endif

//-------------------------------------

#ifdef   VXL_STDLIB_HAS_DRAND48
/* (VXL_STDLIB_HAS_DRAND48=1; echo """yes" 1>&6)
   (VXL_STDLIB_HAS_DRAND48=0; echo """no" 1>&6)
   (VXL_STDLIB_HAS_DRAND48)
*/

#include <stdlib.h>

int main() { drand48(); return 0; }
#endif

//-------------------------------------
