#ifndef vepl_median_h_
#define vepl_median_h_

// .NAME vepl_median - median filter with circular element
// .LIBRARY vepl
// .HEADER vxl package
// .INCLUDE vepl/vepl_median.h
// .FILE vepl_median.txx
//
// .SECTION Description
//   vepl_median filtering is an operation that replaces a pixel with the
//   median value of its surrounding pixels, in a certain neighbourhood.
//   Here, the neighbourhood is circular, with an arbitrary (float) radius,
//   which is to be passed to the constructor.
//
//   Note that the function operator>(DataIn,DataIn) is being used; for
//   non-scalar data types (like colour pixels) an appropriate ">" function
//   must thus be supplied.
//
//   Note also the implicit use of DataOut::DataOut(DataIn),
//   which you probably will have to provide when DataIn and DataOut
//   are not the same type.  It could even be argued that these types should
//   always be the same!
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven (ESAT/PSI), 28 April 2001

#include <vil/vil_image.h>

vil_image vepl_median(vil_image const& , float radius=1.0);

#endif // vepl_median_h_
