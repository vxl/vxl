#ifndef sdet_region_proc_params_h_
#define sdet_region_proc_params_h_
//:
// \file
// \brief parameter mixin for vdgl_region_proc
//
// \author
//             Joseph L. Mundy - Apr. 11, 2001
//             GE Corporate Research and Development
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <sdet/sdet_detector_params.h>
#include <vcl_iostream.h>

class sdet_region_proc_params : public gevd_param_mixin
{
public :
  sdet_region_proc_params(bool debug = false,
                          bool verbose = false,
                          const sdet_detector_params& dp = sdet_detector_params()
                         );

  sdet_region_proc_params(const sdet_region_proc_params& old_params);
  ~sdet_region_proc_params(){}

  bool SanityCheck();
 friend
  vcl_ostream& operator<<(vcl_ostream& os, const sdet_region_proc_params& rpp);
protected:
  void InitParams(bool debug,
                  bool verbose,
                  const sdet_detector_params& dp
                 );
public:
  //
  // Parameter blocks and parameters
  //
  bool debug_;               //!< Carry out debug processing
  bool verbose_;             //!< Print detailed output
  sdet_detector_params dp_;  //!< parameters associated with step and fold detection
};


#endif // sdet_region_proc_params_h_
