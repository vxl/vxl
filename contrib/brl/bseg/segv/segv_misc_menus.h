#ifndef segv_vil_menus_h_
#define segv_vil_menus_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief the setup for menus for miscellanous tests and debugging
//  This is essentially a namespace for static methods;
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy January 5, 2007    Initial version.
// \endverbatim
//----------------------------------------------------------------------------
#include <segv/vil_dll.h>
class vgui_menu;

class segv_misc_menus
{
  //: inaccessible default constructor: all methods are static
  segv_misc_menus() {}
 public:
  SEGV_VIL_DLL_DATA static void quit_callback();
  SEGV_VIL_DLL_DATA static void load_image_callback();
  SEGV_VIL_DLL_DATA static void save_image_callback();
  SEGV_VIL_DLL_DATA static void project_on_subm_callback();
  SEGV_VIL_DLL_DATA static void project_on_articulation_callback();

  SEGV_VIL_DLL_DATA static vgui_menu get_menu();
};

#endif // segv_misc_menus_h_
