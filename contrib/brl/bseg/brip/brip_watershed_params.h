#ifndef brip_watershed_params_h_
#define brip_watershed_params_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author  Joe Mundy July 8, 2004
// \brief brip_watershed_params - Parameter block
//
// \verbatim
//  Modifications:
//   Ported to vxl July 01, 2004
// \endverbatim
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iostream.h>

class brip_watershed_params : public gevd_param_mixin
{
 public:

  brip_watershed_params(float sigma  = 1.0,
                        float thresh  = 10.0,
                        bool eight_connected = true,
                        bool verbose = false);


   brip_watershed_params(const brip_watershed_params& old_params);
  bool SanityCheck();
  friend vcl_ostream& operator<<(vcl_ostream& os, const brip_watershed_params& wp);
 protected:
  void InitParams(float sigma, float thresh,
                  bool eight_connected,
                  bool verbose);

 public:
  //
  // Parameters for detecting edgel chains
  //
  float sigma_;            //!< Standard deviation of the smoothing kernel
  float thresh_;           //!< Threshold (min grad difference for a seed)
  bool eight_connected_;   //!< Neighborhood connectivity (8 vs 4)
  bool verbose_;           //!< output debug messages
};
#endif
