### find path to C++ headers, if possible
AC_DEFUN(AC_CXX_HEADERS,[
AC_CACHE_CHECK(path to C++ headers,[
#AC_PROG_CXXCPP
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
],,ac_cxx_headers=$CXX_HDRDIR,[])
])
dnl


### Check whether the compiler understands `bool'
AC_DEFUN(AC_CXX_HAS_BOOL,[
AC_CACHE_CHECK(whether the C++ compiler supports the keyword 'bool',ac_cxx_has_bool,
[AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([
void function(int i, void *ptr, bool v) { }
],,ac_cxx_has_bool=yes,ac_cxx_has_bool=no)
AC_LANG_RESTORE
])
if test "$ac_cxx_has_bool" = "yes" ; then
  VCL_HAS_BOOL=1;
else
  VCL_HAS_BOOL=0;
fi
export VCL_STD_BOOL
])
dnl


### Check whether the compiler supports dynamic_cast
AC_DEFUN(AC_CXX_HAS_DYNAMIC_CAST,[
AC_CACHE_CHECK(whether the C++ compiler supports dynamic_cast,ac_cxx_has_dynamic_cast,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE(
[
struct foo { foo(); virtual ~foo(); virtual void f() =0; };
struct boo : public foo { void f() { *(int*)0 = 1; } };	
boo *try_dynamic_cast() { boo *b = 0; foo *f = b; return dynamic_cast<boo*>(f); }
],,ac_cxx_has_dynamic_cast=yes,ac_cxx_has_dynamic_cast=no)
AC_LANG_RESTORE
 ])
if test "$ac_cxx_has_dynamic_cast" = "yes" ; then
  VCL_HAS_DYNAMIC_CAST=1;
else
  VCL_HAS_DYNAMIC_CAST=0;
fi;
export VCL_HAS_DYNAMIC_CAST
])


### Check whether the compiler supports "typename"
AC_DEFUN(AC_CXX_HAS_TYPENAME,[
AC_CACHE_CHECK(whether the C++ compiler supports the keyword 'typename',ac_cxx_has_typename,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE(
[
template <typename T>
class bingo {
public:
  void bongo(T **);
};
],,ac_cxx_has_typename=yes,ac_cxx_has_typename=no)
AC_LANG_RESTORE
 ])
if test "$ac_cxx_has_typename" = "yes" ; then
  VCL_HAS_TYPENAME=1;
else
  VCL_HAS_TYPENAME=0;
fi;
export VCL_HAS_TYPENAME
])




### Check whether the compiler supports "mutable"
AC_DEFUN(AC_CXX_HAS_MUTABLE,[
AC_CACHE_CHECK(whether the C++ compiler supports the keyword 'mutable',ac_cxx_has_mutable,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE(
[
class X {
public:
  mutable int const *p;
};
],,ac_cxx_has_mutable=yes,ac_cxx_has_mutable=no)
AC_LANG_RESTORE
 ])
if test "$ac_cxx_has_mutable" = "yes" ; then
  VCL_HAS_MUTABLE=1;
else
  VCL_HAS_MUTABLE=0;
fi;
export VCL_HAS_MUTABLE
])




### Check whether the compiler supports "explicit"
AC_DEFUN(AC_CXX_HAS_EXPLICIT,[
AC_CACHE_CHECK(whether the C++ compiler supports the keyword 'explicit',ac_cxx_has_explicit,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE(
[
class X {
public:
  explicit X(int ) { }
};
],,ac_cxx_has_explicit=yes,ac_cxx_has_explicit=no)
AC_LANG_RESTORE
 ])
if test "$ac_cxx_has_explicit" = "yes" ; then
  VCL_HAS_EXPLICIT=1;
else
  VCL_HAS_EXPLICIT=0;
fi;
export VCL_HAS_EXPLICIT
])




### Check whether the compiler supports namespaces
AC_DEFUN(AC_CXX_HAS_NAMESPACES,[
AC_CACHE_CHECK(whether the C++ compiler has working namespaces,ac_cxx_has_namespaces,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
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
],,ac_cxx_has_namespaces=yes,ac_cxx_has_namespaces=no)
AC_LANG_RESTORE
])
if test "$ac_cxx_has_namespaces" = "yes" ; then
  VCL_HAS_NAMESPACES=1;
else
  VCL_HAS_NAMESPACES=0;
fi;
export VCL_HAS_NAMESPACES
])
dnl


### Check whether the compiler allows std:: for the standard library
AC_DEFUN(AC_CXX_ALLOWS_NAMESPACE_STD,[
AC_CACHE_CHECK(whether the C++ compiler allows std:: for the standard library,ac_cxx_allows_namespace_std,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
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
],,ac_cxx_allows_namespace_std=yes,ac_cxx_allows_namespace_std=no)
AC_LANG_RESTORE
])
if test "$ac_cxx_allows_namespace_std" = "yes" ; then
  VCL_ALLOWS_NAMESPACE_STD=1;
else
  VCL_ALLOWS_NAMESPACE_STD=0;
fi;
export VCL_ALLOWS_NAMESPACE_STD
])
dnl


### Check whether the compiler needs std:: for the standard library
AC_DEFUN(AC_CXX_NEEDS_NAMESPACE_STD,[
AC_CACHE_CHECK(whether the C++ compiler needs std:: for the standard library,ac_cxx_needs_namespace_std,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
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
],,ac_cxx_needs_namespace_std=no,ac_cxx_needs_namespace_std=yes)
AC_LANG_RESTORE
])
if test "$ac_cxx_needs_namespace_std" = "yes" ; then
  VCL_NEEDS_NAMESPACE_STD=1;
else
  VCL_NEEDS_NAMESPACE_STD=0;
fi;
export VCL_NEEDS_NAMESPACE_STD
])
dnl


###
AC_DEFUN(AC_CXX_CHECK_FOR_SCOPE,[
AC_CACHE_CHECK(whether the C++ compiler supports ISO for scope,ac_cxx_check_for_scope,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([
void fn() {
  for (int i=0; i<100; ++i) { }
  for (long i=0; i<1000; ++i) { }
  double i = 3.141;
}
],,ac_cxx_check_for_scope=yes,ac_cxx_check_for_scope=no)
AC_LANG_RESTORE
])
if test "$ac_cxx_check_for_scope" = "yes" ; then
  VCL_FOR_SCOPE_HACK=0;
else
  VCL_FOR_SCOPE_HACK=1;
fi;
export VCL_FOR_SCOPE_HACK
])
dnl


### Check whether the compiler supports member templates
AC_DEFUN(AC_CXX_HAS_MEMBER_TEMPLATES,[
AC_CACHE_CHECK(whether the C++ compiler supports member templates,ac_cxx_has_member_templates,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
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
],,ac_cxx_has_member_templates=yes,ac_cxx_has_member_templates=no)
AC_LANG_RESTORE
])
if test "$ac_cxx_has_member_templates" = "yes" ; then
  VCL_HAS_MEMBER_TEMPLATES=1;
else
  VCL_HAS_MEMBER_TEMPLATES=0;
fi;
export VCL_HAS_MEMBER_TEMPLATES
])
dnl



### Check whether the compiler supports partial specialization
AC_DEFUN(AC_CXX_CAN_DO_PARTIAL_SPECIALIZATION,[
AC_CACHE_CHECK(whether the C++ compiler supports partial specialization,ac_cxx_can_do_partial_specialization,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([
template <class T>
class victor 
{
  T data[256];
public:
  victor() { }
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
  void bar() { }
};

template <class T>
struct foo<int *, T> {
  void baz() { }
};

],,ac_cxx_can_do_partial_specialization=yes,ac_cxx_can_do_partial_specialization=no)
AC_LANG_RESTORE
])
if test "$ac_cxx_can_do_partial_specialization" = "yes" ; then
  VCL_CAN_DO_PARTIAL_SPECIALIZATION=1;
else
  VCL_CAN_DO_PARTIAL_SPECIALIZATION=0;
fi;
export VCL_CAN_DO_PARTIAL_SPECIALIZATION
])
dnl


### Check whether the compiler has a header <blah>.
# AC_CXX_HAS_HEADER(blah, VCL_CXX_HAS_HEADER_BLAH)
AC_DEFUN(AC_CXX_HAS_HEADER,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_CHECK_HEADER($1,[$2=1],[$2=0])
AC_LANG_RESTORE
])
dnl



### Check whether the compiler needs values in definitions 
# of functions taking default values.
AC_DEFUN(AC_CXX_DEFAULT_VALUE,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING(whether the C++ compiler needs default values in second definition)
AC_TRY_COMPILE([
// declaration
void function(int x, char *ptr = "foo");

// definition
void function(int x, char *ptr) { ++ ptr[x]; }
],,[
VCL_DEFAULT_VALUE="/* no need */";
AC_MSG_RESULT(no)
],[
VCL_DEFAULT_VALUE=" = x";
AC_MSG_RESULT(yes)
])
AC_LANG_RESTORE
export VCL_DEFAULT_VALUE
])
dnl



###
AC_DEFUN(AC_CXX_DEFINE_SPECIALIZATION,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING(whether the C++ compiler allows template <> for (complete) specializations)
AC_TRY_COMPILE([
// declaration
template <class T>
class traits {
public:
};

// specialization
template <>
class traits<double> {
public:
  typedef double abs_t;
  typedef double float_t;
};
],,[
VCL_DEFINE_SPECIALIZATION="template <>";
AC_MSG_RESULT(yes)
],[
VCL_DEFINE_SPECIALIZATION="/* template <> */";
AC_MSG_RESULT(no)
])
AC_LANG_RESTORE
export VCL_DEFINE_SPECIALIZATION
])
dnl


###
AC_DEFUN(AC_CXX_STATIC_CONST_INIT_INT,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING(whether the C++ compiler allows initialization of static const int)
AC_TRY_COMPILE([
class A {
public:
  static const int x = 27;
};
//int main(int, char **)
//{
//  return A::x;
//}
],,[
VCL_STATIC_CONST_INIT_INT=" = x";
AC_MSG_RESULT(yes)
],[
VCL_STATIC_CONST_INIT_INT="/* = x */";
AC_MSG_RESULT(no)
])
AC_LANG_RESTORE
export VCL_STATIC_CONST_INIT_INT
])
dnl


###
AC_DEFUN(AC_CXX_STATIC_CONST_INIT_FLOAT,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING(whether the C++ compiler allows initialization of static const floats)
AC_TRY_COMPILE([
class A {
public:
  static const float x = 27.0f;
  static const double y = 27.0;
};
],,[
VCL_STATIC_CONST_INIT_FLOAT=" = x";
AC_MSG_RESULT(yes)
],[
VCL_STATIC_CONST_INIT_FLOAT="/* = x */";
AC_MSG_RESULT(no)
])
AC_LANG_RESTORE
export VCL_STATIC_CONST_INIT_FLOAT
])
dnl


###
AC_DEFUN(AC_CXX_IMPLEMENT_STATIC_CONSTS,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING(whether the C++ compiler requires implementation of static consts)
AC_TRY_RUN([
class A {
public:
  static const int x = 27;
};
int function() {
  return A::x * A::x;
}
int main(int, char**) { function(); return 0; }
],[
VCL_IMPLEMENT_STATIC_CONSTS="0";
AC_MSG_RESULT(no)
],[
VCL_IMPLEMENT_STATIC_CONSTS="1";
AC_MSG_RESULT(yes)
])
AC_LANG_RESTORE
export VCL_IMPLEMENT_STATIC_CONSTS
])
dnl


###
AC_DEFUN(AC_CXX_OVERLOAD_CAST,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING(whether the C++ compiler requires explicit casts where it should not)
AC_TRY_COMPILE([
//
template <class T>
class vnl_vector {
public:
  unsigned size;
  T *data;
  vnl_vector(unsigned n, T *ptr) : size(n), data(ptr) { }
};

template <class T>
bool operator==(vnl_vector<T> const &a, vnl_vector<T> const &);

//
template <unsigned n, class T>
class vnl_vector_fixed : public vnl_vector<T> {
public:
  T data_fixed[n];
  vnl_vector_fixed() : vnl_vector<T>(n, data_fixed) { }
};

//
void print_it(vnl_vector<double> const &);

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
void    copy_image(S const * const *src, T * const *dst, int, int);

typedef unsigned char byte;

void do_vision(int w, int h, byte **image_i, float **image_f) {
  // SGI CC 7.21 fails here.
  copy_image(image_i, image_f, w, h);
}
],,[
VCL_OVERLOAD_CAST="(x)";
AC_MSG_RESULT(no)
],[
VCL_OVERLOAD_CAST="((T)(x))";
AC_MSG_RESULT(yes)
])
AC_LANG_RESTORE
export VCL_OVERLOAD_CAST
])
dnl





###
AC_DEFUN(AC_CXX_NULL_TMPL_ARGS,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING(whether the C++ compiler requires <> in templated forward/friend declarations)
AC_TRY_COMPILE([
template <class T> class victor;

template <class T> T dot(victor<T> const &u, victor<T> const &v);

template <class T> class victor {
public:
  // Without -fguiding-decls, egcs and 2.95 will rightly think
  // this declares a non-template and so the program will fail
  // due to access violation below and missing symbols at link time.
  friend T dot /* <> */ (victor<T> const &, victor<T> const &);

private:
  T data[3];
};

template <class T> T dot(victor<T> const &u, victor<T> const &v)
{
  return 
    u.data[0] * v.data[0] +
    u.data[1] * v.data[1] +
    u.data[2] * v.data[2];
}

#include <math.h>

double function(victor<double> const &u,
		victor<double> const &v)
{
  return dot(u, v)/sqrt(dot(u, u) * dot(v, v));
}
],,[
VCL_NULL_TMPL_ARGS="/* <> */";
AC_MSG_RESULT(no)
],[
VCL_NULL_TMPL_ARGS="<>";
AC_MSG_RESULT(yes)
])
AC_LANG_RESTORE
export VCL_NULL_TMPL_ARGS
])
dnl





###
AC_DEFUN(AC_CXX_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING(whether the C++ compiler needs friend declarations for proper template function overloading)
AC_TRY_COMPILE([
template <class T>
class victor_base {
public:
  T &operator[](unsigned i) { return data[i]; }

protected:
  victor_base(T *p, unsigned n) : data(p), size(n) { }
  
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

  victor_fixed() : victor_base<T>(data_fixed, n) { }
};

int function(victor_fixed<double, 3> const &a, 
	     victor_fixed<double, 3> const &b)
{
  if (a == b) // 2.7 fails to resolve this.
    return 3141;
  else
    return 2718;
}
],,[
VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD="0";
AC_MSG_RESULT(no)
],[
VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD="1";
AC_MSG_RESULT(yes)
])
AC_LANG_RESTORE
export VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD
])
dnl



### Check whether the compiler mangles function templates differently
# from function non-templates.
AC_DEFUN(AC_CXX_HAS_TEMPLATE_SYMBOLS,[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING(whether the C++ compiler distinguishes templated and non-templated functions)
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
void function(double *, int) { }

// If the program links, the compiler didn't make a distinction.
],,[
VCL_HAS_TEMPLATE_SYMBOLS="0";
AC_MSG_RESULT(no)
],[
VCL_HAS_TEMPLATE_SYMBOLS="1";
AC_MSG_RESULT(yes)
])
AC_LANG_RESTORE
export VCL_HAS_TEMPLATE_SYMBOLS
])
dnl



### Check whether the compiler allows explicit instantiation of inline templates.
AC_DEFUN(AC_CXX_ALLOWS_INLINE_INSTANTIATION,[
AC_MSG_CHECKING(whether the C++ compiler allows explicit instantiation of inline templates)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS

fsm_save_CXXFLAGS=$CXXFLAGS
CXXFLAGS=$TXXFLAGS

AC_TRY_COMPILE([
template <class T>
inline
T dot(T const *a, T const *b)
{
  return a[0]*b[0];
}

template double dot(double const *, double const *);
],,[
VCL_ALLOWS_INLINE_INSTANTIATION="1";
AC_MSG_RESULT(yes)
],[
VCL_ALLOWS_INLINE_INSTANTIATION="0";
AC_MSG_RESULT(no)
])

CXXFLAGS=$fsm_save_CXXFLAGS

AC_LANG_RESTORE
export VCL_ALLOWS_INLINE_INSTANTIATION
])
dnl



### Check whether the compiler needs explicit instantiation of inline function templates.
AC_DEFUN(AC_CXX_NEEDS_INLINE_INSTANTIATION,[
AC_MSG_CHECKING(whether the C++ compiler needs explicit instantiation of inline function templates)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS

fsm_save_CXXFLAGS=$CXXFLAGS
CXXFLAGS=$TXXFLAGS

AC_TRY_RUN([
#ifdef __GNUC__
#pragma interface
#endif
template <class T>
inline
T dot(T const *a, T const *b)
{
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}
//template double dot(double const *, double const *);
int function();
int main(int, char **) { function(); return 0; }
int function()
{
  double a[3] = {1.0, 2.0, 3.0};
  double b[3] = {4.0, 5.0, 6.0};
  double a_b = dot(a, b);
  return int(a_b);
}
// If the program links, the compiler inlined the function template.
],[
VCL_NEEDS_INLINE_INSTANTIATION="0";
AC_MSG_RESULT(no)
],[
VCL_NEEDS_INLINE_INSTANTIATION="1";
AC_MSG_RESULT(yes)
])

CXXFLAGS=$fsm_save_CXXFLAGS

AC_LANG_RESTORE
export VCL_NEEDS_INLINE_INSTANTIATION
])
dnl





### Check whether the compiler needs the SunPro allocator hack.
AC_DEFUN(AC_CXX_SUNPRO_ALLOCATOR_HACK,[
AC_MSG_CHECKING(whether the C++ compiler needs the SunPro allocator hack)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([
template < class T > 
struct allocator 
{ 
  allocator ( ) ;
  allocator ( const allocator < T > & ) ;
} ; 

template < class T , class Allocator = allocator < T > > 
struct vector 
{ 
  vector ();
  ~ vector ( ) ;
} ; 

template < class T > 
struct spoof 
{ 
  void set_row ( unsigned , vector < T /*, allocator<T>*/ > const & ) ; 
} ; 

template < class T > 
void spoof < T > :: set_row ( unsigned , vector < T > const & ) 
{ 
} 

template class spoof < double > ; 

// If the program compiles, we don't need the hack
],,[
VCL_SUNPRO_ALLOCATOR_HACK="/* no need */";
AC_MSG_RESULT(no)
],[
VCL_SUNPRO_ALLOCATOR_HACK="T, allocator<T >";
AC_MSG_RESULT(yes)
])
AC_LANG_RESTORE
export VCL_SUNPRO_ALLOCATOR_HACK
])
dnl




### Check whether the compiler needs default template arguments repeated.
AC_DEFUN(AC_CXX_DEFAULT_TMPL_ARG,[
AC_MSG_CHECKING(whether the C++ compiler needs default template arguments repeated)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([
struct alloc1 { };

struct alloc2 { };

template <class T, class A = alloc1> class X;

template <class T, class A>
class X { 
public:
  T data[3];
  A a;
};

template class X<short>;

// If the program compiles, we don't need to repeat them
],,[
VCL_DEFAULT_TMPL_ARG="/* no need */";
AC_MSG_RESULT(no)
],[
VCL_DEFAULT_TMPL_ARG="arg";
AC_MSG_RESULT(yes)
])
AC_LANG_RESTORE
export VCL_DEFAULT_TMPL_ARG
])
dnl
