// This is prip/vdtop/vdtop_well_composed_lower_leveling_kernel.h
#ifndef vdtop_well_composed_lower_leveling_kernel_h_
#define vdtop_well_composed_lower_leveling_kernel_h_

//:
// \file
// \brief Provides a function for computing a well composed connected lower leveling kernel of Bertrand and al
// ( G. Bertrand, J. C. Everat and M. Couprie, "Image segmentation through operators based upon topology",
//   Journal of Electronic Imaging, Vol. 6, N. 4, 395-405, 1997).
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim
#include <vdtop/vdtop_4_lower_leveling_kernel.h>
#include <vdtop/vdtop_construct_well_composed.h>

//: computes the 4 lower leveling kernel of arg.  Points having at least 1 lower neighbor can be viewed as a 4-connected watershed.
template <class T>
void vdtop_well_composed_lower_leveling_kernel(vil_image_view<T> & arg)
{
  vdtop_construct_well_composed(arg) ;
  vdtop_4_lower_leveling_kernel(arg) ;
}

#endif
