// This is core/vgui/examples/basic_manager.h
#ifndef basic_manager_h_
#define basic_manager_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief An example of a GUI manager class
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications
//   J.L. Mundy December 26, 2002    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vil1/vil1_image.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>

class basic_manager : public vgui_wrapper_tableau
{
  vil1_image img_;
  vgui_image_tableau_sptr itab_;
 public:
  basic_manager() : vgui_wrapper_tableau() {}
  ~basic_manager() {}
  static basic_manager *instance();
  void quit();
  void load_image();
  void init();
  virtual bool handle(vgui_event const &);
};

#endif // basic_manager_h_
