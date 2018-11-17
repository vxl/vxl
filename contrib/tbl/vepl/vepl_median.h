#ifndef vepl_median_h_
#define vepl_median_h_
//:
// \file
// \brief median filter with circular element
//
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
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   7 October 2002
//
// \verbatim
//  Modifications
//   Peter Vanroose - 20 aug 2003 - changed parameter and return types from vil_image_view_base_sptr to vil_image_resource_sptr
// \endverbatim

#include <vil/vil_image_resource.h>

//: median filter with circular element of supplied radius
vil_image_resource_sptr vepl_median(const vil_image_resource_sptr& , float radius=1.0);

#endif // vepl_median_h_
