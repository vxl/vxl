#include <vcl_vector.h>
#include <rrel/rrel_util.txx>

// Trigger this implicitly, because on many compilers, the
// vector::iterator conflicts with a pointer. With GCC 3.x (only?)
// vector::iterator _is not_ a pointer*. The implicit instantiation
// should make the symbols weak. Therefore, if
// vector::iterator==pointer, the symbols here will be discarded in
// favour of the symbols in the pointer instantiation.

// not static to avoid compiler warnings about unused functions.
void
rrel_util_vector_double_iterator_instantiation_tickler()
{
  typedef vcl_vector<double>::iterator Iter;
  Iter itr;
  double val;

  rrel_util_weighted_scale( itr, itr, itr, 1, &val );
  rrel_util_weighted_scale( itr, itr, itr, 1 );
  rrel_util_median_and_scale_copy( itr, itr, val, val, 1 );
  rrel_util_intercept_adjustment_copy( itr, itr, val, val, 1 );
  rrel_util_intercept_adjust_stats_copy( itr, itr, val, val, val, 1 );
}
