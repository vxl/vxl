// This is oxl/ogui/examples/geoff/vgui_testbed/vgui_point_selector.h
#ifndef example_point_selector_h_
#define example_point_selector_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

#include "vgui_point_selector_sptr.h"
#include <vgui/vgui_wrapper_tableau.h>

class example_point_selector : public vgui_wrapper_tableau
{
 public:

  example_point_selector( vgui_tableau_sptr child);

  bool handle( vgui_event const &e);
};

#endif // example_point_selector_h_
