#ifndef vepl_erode_disk_h_
#define vepl_erode_disk_h_

// .NAME vepl_erode_disk - morphological erosion with circular element
// .LIBRARY vepl
// .HEADER vxl package
// .INCLUDE vepl/vepl_erode_disk.h
// .FILE vepl_erode_disk.txx
//
// .SECTION Description
//   Erosion is a morphological operation that replaces a pixel with the
//   minimum value of its surrounding pixels, in a certain neighbourhood.
//   Here, the neighbourhood is circular, with an arbitrary (float) radius,
//   which is to be be passed to the constructor.
//
//   Note that the function min(DataIn,DataIn) is being used; for
//   non-scalar data types (like colour pixels) an appropriate min() function
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

vil_image vepl_erode_disk(vil_image const& , float radius);

#endif // vepl_erode_disk_h_
