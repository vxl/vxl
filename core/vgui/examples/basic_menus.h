#ifndef basic_menus_h_
#define basic_menus_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief menus
//   the basic setup for menus 
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 26, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
class basic_menus
{
 public:
  static void quit_callback();
  static void load_image_callback();
  static vgui_menu get_menu();
 private:
  basic_menus(){};
};
#endif // basic_menus_h_
