#ifdef __GNUG__
#pragma implementation
#endif

#include <vnl/vnl_math.h>

#if defined(_MSC_VER)
# include <Float.h> // for 'isnan' and 'finite'
// # define isnan _isnan
# define finite _finite
#endif

#if defined(VCL_KAI)
namespace {
  bool finite(double x) {
    *(int*)0 = 1; // how to abort() without #include :)
    return false;
  }
}
#elif defined(SYSV) && !defined(hppa)
extern "C" int finite(double);
#endif

//--------------------------------------------------------------------------------

// constants
#if ! VCL_CAN_STATIC_CONST_INIT_FLOAT
const double vnl_math::e       	      	= 2.7182818284590452354;
const double vnl_math::log2e   	      	= 1.4426950408889634074;
const double vnl_math::log10e  	      	= 0.43429448190325182765;
const double vnl_math::ln2     	      	= 0.69314718055994530942;
const double vnl_math::ln10    	      	= 2.30258509299404568402;
const double vnl_math::pi      	      	= 3.14159265358979323846;
const double vnl_math::pi_over_2        = 1.57079632679489661923;
const double vnl_math::pi_over_4        = 0.78539816339744830962;
const double vnl_math::one_over_pi      = 0.31830988618379067154;
const double vnl_math::two_over_pi      = 0.63661977236758134308;
const double vnl_math::two_over_sqrtpi  = 1.12837916709551257390;
const double vnl_math::sqrt2   	      = 1.41421356237309504880;
const double vnl_math::sqrt1_2 	      = 0.70710678118654752440;

// IEEE double machine precision
const double vnl_math::eps     	      = 2.2204460492503131e-16;
const double vnl_math::sqrteps 	      = 1.490116119384766e-08;
#endif

const int      vnl_math::maxint    = 0x7fffffff;
const long int vnl_math::maxlong   = 0x7fffffffL;
const double   vnl_math::maxdouble = HUGE_VAL;
const float    vnl_math::maxfloat  = 3.40282346638528860e+38F;

//--------------------------------------------------------------------------------

// -- Return true iff x is "Not a Number"
bool vnl_math_isnan(double x) 
{
  return x != x;
}

// fsm@robots.ox.ac.uk
// On linux noshared builds, with optimisation on, calling 'finite' within the
// scope of vnl_math causes vnl_math_isinf to be called. This blows the stack.
// Plausible theory : 'finite' is a preprocessor macro, defined in terms of a
// macro called 'isinf'.
#if defined(isinf)
# error macro isinf is defined
#endif
// -- Return true if x is neither NaN nor Inf.
bool vnl_math_isfinite(double x) 
{
#if defined(_MSC_VER)
  return finite(x) != 0; // quell performance warning -- fsm
#else
  return finite(x);
#endif
}

#if defined(_MSC_VER)
inline bool isnan(double x)
{
  return !(x == x);
}
#endif

// -- Return true if x is inf
bool vnl_math_isinf(double x) 
{
  return !finite(x) && !isnan(x);
}

