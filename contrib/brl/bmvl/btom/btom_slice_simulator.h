//this-sets-emacs-to-*-c++-*-mode
#ifndef btom_slice_simulator_h_
#define btom_slice_simulator_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief A simulator for tomographic reconstruction using Gaussian cylinders
//
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy March 04, 2003   Initial version.
// \endverbatim
//-----------------------------------------------------------------------------

#include <vil1/vil1_memory_image_of.h>
#include <btom/btom_slice_simulator_params.h>

class btom_slice_simulator : public btom_slice_simulator_params
{
 public:
  btom_slice_simulator(btom_slice_simulator_params& ssp);
  ~btom_slice_simulator();

  void gaussian_sinogram(vil1_memory_image_of<float> & sinogram,
                         vil1_memory_image_of<float> & reconst);

 protected:
  btom_slice_simulator();
};

#endif // btom_slice_simulator_h_
