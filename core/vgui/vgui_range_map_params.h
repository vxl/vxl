// This is core/vgui/vgui_range_map_params.h
#ifndef vgui_range_map_params_h_
#define vgui_range_map_params_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author J. L. Mundy
// \date   December 26, 2004
// \brief  Parameters to define pixel range mapping
//
// \verbatim
//  Modifications <none>
// \endverbatim
#include <vbl/vbl_ref_count.h>
class vgui_range_map_params : public vbl_ref_count
{
 public:
  long double min_val_;  //!< domain minimum for computed mapping 
  long double max_val_;  //!< domain maximum for computed mapping 
  float gamma_;//!< photometric non-linear gamma correction
  bool invert_;//!< invert the mapping to a negative image

  vgui_range_map_params(const long double min,
                        const long double max,
                        const float gamma,
                        const bool invert) :
  min_val_(min), max_val_(max), gamma_(gamma), invert_(invert){};
};
#include <vgui/vgui_range_map_params_sptr.h>
#endif // vgui_range_map_params_h_
