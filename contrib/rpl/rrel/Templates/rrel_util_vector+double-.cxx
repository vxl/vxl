#include <vcl_compiler.h>
#include <vcl_vector.h>
#include <rrel/rrel_util.txx>

#if defined(VCL_GCC_30) || defined(VCL_ICC)

// Apply explicit instantiation
// ICC 8.1 has problems with the following 
// implicit trigger function. There are weak 
// symbols(V type) generated. But they did not
// get through linking stage. 
//
typedef vcl_vector<double>::iterator Iter;
RREL_UTIL_INSTANTIATE_RAN_ITER(double, Iter);
 
#else

// Trigger this implicitly, because on many compilers, the
// vector::iterator conflicts with a pointer. With GCC 3.x (only?)
// vector::iterator _is not_ a pointer. The implicit instantiation
// should make the symbols weak. Therefore, if
// vector::iterator==pointer, the symbols here will be discarded in
// favour of the symbols in the pointer instantiation.
//
// This function must have external linkage (i.e. should not be static);
// otherwise, an aggressive optimizer (VC7) will throw it away and never
// emit the instantiations.

void
rrel_util_vector_double_iterator_instantiation_tickler()
{
  vcl_vector<double> v;
  typedef vcl_vector<double>::iterator Iter;
  Iter itr = v.begin(); // to avoid compiler warning on uninitialised variable
  double val = 0.0;

  rrel_util_median_abs_dev_scale( itr, itr, 1, &val );
  rrel_util_median_abs_dev_scale( itr, itr, 1 );
  rrel_util_weighted_scale( itr, itr, itr, 1, &val );
#if !VCL_TEMPLATE_MATCHES_TOO_OFTEN // not for compilers with overload problems
  rrel_util_weighted_scale( itr, itr, itr, 1 );
#endif
  rrel_util_median_and_scale_copy( itr, itr, val, val, 1 );
  rrel_util_intercept_adjustment_copy( itr, itr, val, val, 1 );
  rrel_util_intercept_adjust_stats_copy( itr, itr, val, val, val, 1 );
}
#endif

