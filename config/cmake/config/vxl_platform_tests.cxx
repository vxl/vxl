//-------------------------------------

#ifdef VCL_HAS_LONG_LONG

void function(int i, void *ptr, long long v) {}

int main() { return 0; }
#endif // VCL_HAS_LONG_LONG

//-------------------------------------

#ifdef VCL_CAN_DO_PARTIAL_SPECIALIZATION

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
#endif // VCL_CAN_DO_PARTIAL_SPECIALIZATION

//-------------------------------------

#ifdef VCL_DEFINE_SPECIALIZATION
// VCL_DEFINE_SPECIALIZATION is set to "template <>" if this compiles, to "" otherwise

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
#endif // VCL_DEFINE_SPECIALIZATION

//-------------------------------------

#ifdef VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD
// VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD is set to 1 if this fails to compile

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
#endif // VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD

//-------------------------------------

#ifdef VCL_OVERLOAD_CAST
// VCL_OVERLOAD_CAST(x) is set to "(x)" if this compiles, to "((T)(x))" otherwise

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
  copy_image(image_i, image_f, w, h);
}

int main() { return 0; }
#endif // VCL_OVERLOAD_CAST

//-------------------------------------

#ifdef VCL_NULL_TMPL_ARGS
// VCL_NULL_TMPL_ARGS is set to "<>" if this fails to compile, to "" otherwise

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
#endif // VCL_NULL_TMPL_ARGS

//-------------------------------------

#ifdef VCL_NO_STATIC_DATA_MEMBERS
// VCL_NO_STATIC_DATA_MEMBERS is set to 1 if this fails to compile

template <class T> class vvv { static T xxx; };
template class vvv<int>;

int main() { return 0; }
#endif // VCL_NO_STATIC_DATA_MEMBERS

//-------------------------------------

#ifdef VCL_HAS_TEMPLATE_SYMBOLS
// VCL_HAS_TEMPLATE_SYMBOLS is set to 1 if this fails to link

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
#endif // VCL_HAS_TEMPLATE_SYMBOLS

//-------------------------------------

#ifdef VCL_CAN_DO_IMPLICIT_TEMPLATES

# ifdef _MSC_VER
// Use template typing to figure out correct method, because
// several MSVC versions can't cope with overloaded return types
template <class S> struct ims_what;

template <>
struct ims_what<double *> {
  typedef double type; };

template <class S>
struct ims_what {
  typedef int type; };


template <class I, class T>
void fsm_plop(I b, I e, T x, int)
{
  for (I p=b; p!=e; ++p)
    *p = x;
}

template <class T>
void fsm_plop(double *b, double *e, T x, double)
{
  for (double *p=b; p<e; ++p)
    *p = x;
}

template <class I, class T>
inline void fsm_plip(I b, I e, T x)
{
  if (b != e)
    fsm_plop(b, e, x, ims_what<I>::type());
}

# else
// FSM: The code is imitating the way the gcc STL chooses (or did choose, way
// back) between algorithms for different iterator types. A very brief look
// at the 3.2.2 <algorithm> header suggests they no longer use that mechanism
// so maybe it was deemed non-standard and abandoned.

struct fsm_plap_normal {};

template <class I>
inline fsm_plap_normal fsm_plap(I) { return fsm_plap_normal(); }

struct fsm_plap_double_star {};
inline fsm_plap_double_star fsm_plap(double *) { return fsm_plap_double_star(); }


template <class I, class T>
void fsm_plop(I b, I e, T x, fsm_plap_normal)
{
  for (I p=b; p!=e; ++p)
    *p = x;
}

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

# endif

void f()
{
  int iarray[20];
  fsm_plip(iarray, iarray+20, 3141);

  double darray[20];
  fsm_plip(darray, darray+20, 2718);
}

int main() { return 0; }
#endif // VCL_CAN_DO_IMPLICIT_TEMPLATES

//-------------------------------------

#ifdef VXL_UNISTD_USLEEP_IS_VOID
// VXL_UNISTD_USLEEP_IS_VOID is set to 1 if this test fails
#include <unistd.h>

int main() { int x = usleep(0); return x*0; }
#endif // VXL_UNISTD_USLEEP_IS_VOID
//-------------------------------------
#ifdef VCL_NUMERIC_LIMITS_HAS_INFINITY
// Does vcl_numeric_limits<float>::has_infinity == 1?

// Several versions of gcc (3.0, 3.1, and 3.2) come with a
// numeric_limits that reports that they have no infinity.
#include <limits>
int main() {
  return std::numeric_limits<double>::has_infinity &&
         std::numeric_limits<float>::has_infinity ? 0 : 1;
}
#endif // VCL_NUMERIC_LIMITS_HAS_INFINITY

//-------------------------------------

#ifdef VCL_CANNOT_SPECIALIZE_CV
// VCL_CANNOT_SPECIALIZE_CV is set to 1 if this fails to compile

// Some compilers do not distinguish between A<int> and A<int const>.

template <class T> struct A;
#if !defined(NOT_CONFORMING_SPECIALIZATION)
template <> struct A<int> {};
template <> struct A<int const> {};
#else
struct A<int> {};
struct A<int const> {};
#endif // VCL_CANNOT_SPECIALIZE_CV

int main() { return 0; }

#endif

//-------------------------------------

#ifdef VCL_TEMPLATE_MATCHES_TOO_OFTEN
// VCL_TEMPLATE_MATCHES_TOO_OFTEN is set to 1 if this fails to compile

// Some compilers will incorrectly choose the template over the
// non-template.  This will not compile if the template is chosen,
// which will reveal the bug.

class A {};
template <class T> void f(T t) { t.compiler_selected_wrong_overload(); }
void f(const A&) {}

int main()
{
  f(A());
  return 0;
}

#endif // VCL_TEMPLATE_MATCHES_TOO_OFTEN

//-------------------------------------

#ifdef VCL_HAS_SLICED_DESTRUCTOR_BUG
// VCL_HAS_SLICED_DESTRUCTOR_BUG is set to 1 if this program exist(1)s

// Some compilers (at least Intel C++ 7) will create a B temporary on
// the f(c) line below and call both the A and B constructors, but
// then destroy the temporary by calling only ~A() and not calling
// ~B() first (or ever).  This program will return 1 if the bug exists
// and 0 otherwise.

#include <cstdlib>

struct A
{
  A(): mark(0) {}
  A(const A&): mark(0) {}
  ~A() { if (mark) { exit(1); } }
  int mark;
};

struct B: public A
{
  B(): A() {}
  B(const B& b): A(b) { mark = 1; }
  ~B() { mark = 0; }
};

struct C
{
  operator B () { return B(); }
};

void f(A) {}

int main()
{
  C c;
  f(c);
  return 0;
}

#endif // VCL_HAS_SLICED_DESTRUCTOR_BUG

//-------------------------------------

#ifdef VCL_HAS_WORKING_STRINGSTREAM
// VCL_HAS_WORKING_STRINGSTREAM is set to 1 if a fully functional std::stringstream is found.

// Some compilers don't provide a fully functional std::stringstream.
// This program will return 0 whenever sufficient functionality is detected.

#include <sstream>

int main()
{
  std::istringstream s1("text"); char c;
  s1 >> c; if (c != 't') return 1;
  s1 >> c; if (c != 'e') return 1;
  s1 >> c; if (c != 'x') return 1;
  std::ostringstream s2; s2 << "text";
  if (s2.str() != "text") return 1;
  std::ostringstream s3;
  c = 't'; s3 << c;
  c = 'e'; s3 << c;
  c = 'x'; s3 << c;
  c = 't'; s3 << c;
  if (s3.str() != "text") return 1;
  return 0; // success
}

#endif // VCL_HAS_WORKING_STRINGSTREAM

//-------------------------------------

#ifdef VXL_HAS_TYPE_OF_SIZE
// This is used to check if (1) a type exists, (2) is has the required
// size in bytes, and (3) it is functional. The last requirement is
// driven by MSCV 6 which has __int64, but it is not fully
// functional. (It can't be cast to a double, for example.)

// CHAR_BIT is the number of bits per char.
#include <climits>
#ifndef CHAR_BIT
# define CHAR_BIT 8
#endif

#include "config.h"

#if INTEGRAL_TYPE
double cast( THE_TYPE a, unsigned THE_TYPE b, signed THE_TYPE c )
{
  return double( a ) + double( b ) + double( c );
}
#else // INTEGRAL_TYPE
double cast( THE_TYPE a )
{
  return double( a );
}
#endif // INTEGRAL_TYPE

// These declarations conflict unless the sizes match.
extern int (*verify_size)[sizeof(THE_TYPE) * CHAR_BIT];
extern int (*verify_size)[THE_SIZE];

int main()
{
  return 0;
}

#endif // VXL_HAS_TYPE_OF_SIZE

//-------------------------------------

#ifdef VCL_CHAR_IS_SIGNED

// Return 0 for char signed and 1 for char unsigned.
int main()
{
  unsigned char uc = 255;
  return (*reinterpret_cast<char*>(&uc) < 0)?0:1;
}

#endif // VCL_CHAR_IS_SIGNED

//-------------------------------------

#ifdef VXL_C_MATH_HAS_LROUND

// This C99 func is a much faster version of standard vnl_math_round
#include <cmath>

int main() { long c = lround(100.0); }

#endif // VXL_C_MATH_HAS_LROUND

//-------------------------------------

#ifdef VCL_HAS_LFS

// Return 1 if compiler has #define-switchable Large File Support
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64

#include <fstream>
#include <iostream>

int main(int argc, char * argv[])
{
 if( sizeof(std::streamoff)==8 )
   return 0 ;
 else
   return 1;
}

#endif // VCL_HAS_LFS

//-------------------------------------

#ifdef VXL_HAS_DBGHELP_H

// This is a Windows header, and needs windows.h included first to make it compile properly.
#include <Windows.h>
#include <DbgHelp.h>

int main() { MINIDUMP_EXCEPTION_INFORMATION dummy; return 0; }
#endif // VXL_HAS_DBGHELP_H

//-------------------------------------

//-------------------------------------
#ifdef VXL_HAS_WIN_WCHAR_T

#ifdef _WCHAR_T_DEFINED
#include <cwchar>
int main()
{
  wchar_t buf [10];
  buf[0] = L'1';
  buf[1] = L'\0';
  return 0;
}
#else
  int main() { return 1; }
#endif

#endif

//-------------------------------------

#ifdef VXL_HAS_MM_MALLOC
#include <emmintrin.h>
int main()
{
  void* x = _mm_malloc(4*sizeof(float),16);
  _mm_free(x);
  return 0;
}
#endif

//-------------------------------------

#ifdef VXL_HAS_ALIGNED_MALLOC
#include <malloc.h>
int main()
{
  void* x = _aligned_malloc(4*sizeof(float),16);
  _aligned_free(x);
  return 0;
}
#endif

//-------------------------------------

#ifdef VXL_HAS_MINGW_ALIGNED_MALLOC
#include <malloc.h>
int main()
{
  void* x = __mingw_aligned_malloc(4*sizeof(float),16);
  __mingw_aligned_free(x);
  return 0;
}
#endif

//-------------------------------------

#ifdef VXL_HAS_POSIX_MEMALIGN
#include <cstdlib>
int main()
{
  void* x = memalign(16,4*sizeof(float));
  free(x);
  return 0;
}
#endif

//-------------------------------------

#if defined(VXL_HAS_SSE2_HARDWARE_SUPPORT) || defined(VXL_SSE2_HARDWARE_SUPPORT_POSSIBLE)
#include <emmintrin.h>
int main()
{
  //try to do some sse2 calculations
  double d_a[]  = { 6.75, 3.42 };
  double d_b[]  = { 2.3, 9.2 };
  double res[2] = {0.0};

  __m128d z;
  z = _mm_mul_pd(_mm_loadu_pd(d_a),_mm_loadu_pd(d_b));

  _mm_storeu_pd(res,z);

  return 0;
}
#endif
