// This is tbl/vipl/filter/vipl_filter_abs.h
#ifndef vipl_filter_abs_h_
#define vipl_filter_abs_h_
//:
// \file
//   This class provides a non-templated abstract class for all filters.  It
//   has no data and little functionality, providing only axis naming and
//   composition methods.
// \author
//   Terry Boult, Lehigh University 1997, 1998.
//
// \verbatim
// Modifications
//   Peter Vanroose - Aug.2000 - converted to vxl
// \endverbatim

#ifndef FILTER_IMPTR_DEC_REFCOUNT
#define FILTER_IMPTR_DEC_REFCOUNT(v) ((v)->dec_refcount(),(v)=0)
#endif
#ifndef FILTER_IMPTR_INC_REFCOUNT
#define FILTER_IMPTR_INC_REFCOUNT(v) ((v)->inc_refcount())
#endif
#ifndef FILTER_KERNPTR_DEC_REFCOUNT
#define FILTER_KERNPTR_DEC_REFCOUNT(v) ((v)->dec_refcount())
#endif

class vipl_filter_abs
{
 public:

#ifdef USE_COMPOSE_WITH
  // Try to set the output of this filter to be the input of ``to''. So far
  // this function does nothing. Arg should be a nonconst ref. Return value is
  // true if the filter was composed. If its not ``composable'', the filter
  // state is not changed.
  virtual bool compose_with(vipl_filter_abs& to) = 0;
#endif

  //: Constant for accessing X_axis when appropriate for the filter. Currently==0, but please use the function...
  inline static int X_Axis() { return 0; }
  //: Constant for accessing Y_axis when appropriate for the filter. Currently==1, but please use the function...
  inline static int Y_Axis() { return 1; }
  //: Constant for accessing Z_axis when appropriate for the filter. Currently==2, but please use the function...
  inline static int Z_Axis() { return 2; }
  //: Constant for accessing T_axis when appropriate for the filter. Currently==3, but please use the function...
  inline static int T_Axis() { return 3; }
};

#endif // vipl_filter_abs_h_
