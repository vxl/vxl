// This is core/vgui/vgui_quit_tableau.h
#ifndef vgui_quit_tableau_h_
#define vgui_quit_tableau_h_
//:
// \file
// \brief  A tableau which quits the application on receiving 'q' or ESC
// \author fsm
//
//  Contains classes  vgui_quit_tableau  vgui_quit_tableau_new
//
// \verbatim
//  Modifications
//   08-Aug-2002 K.Y.McGaul - Changed to Doxygen style comments.
// \endverbatim

#include "vgui_quit_tableau_sptr.h"
#include <vgui/vgui_tableau.h>

//: A tableau which quits the application on receiving 'q' or ESC
//
//  This is provided by default if you use a vgui_shell_tableau.
class vgui_quit_tableau : public vgui_tableau
{
 public:

  //: Handle all events used by this tableau.
  //  In particular, this is interested in 'q' and ESC key-presses.
  bool handle(vgui_event const &);

  //: Return the type of this tableau ('vgui_quit_tableau').
  std::string type_name() const;

 protected:
  //: Destructor - called by vgui_quit_tableau_sptr.
  ~vgui_quit_tableau() { }
};

//: Create a smart-pointer to a vgui_quit_tableau.
struct vgui_quit_tableau_new : public vgui_quit_tableau_sptr {
  typedef vgui_quit_tableau_sptr base;
};

#endif // vgui_quit_tableau_h_
