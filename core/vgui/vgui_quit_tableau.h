#ifndef vgui_quit_tableau_h_
#define vgui_quit_tableau_h_
/*
  fsm@robots.ox.ac.uk
*/

// .NAME vgui_quit_tableau - A tableau which quits the application on receiving 'q' or ESC
// .INCLUDE vgui/vgui_quit_tableau.h
// .FILE vgui_quit_tableau.cxx

#include "vgui_quit_tableau_ref.h"
#include <vgui/vgui_tableau.h>

class vgui_quit_tableau : public vgui_tableau {
public:
  bool handle(vgui_event const &);
  vcl_string type_name() const;

protected:
  ~vgui_quit_tableau() { }
};

struct vgui_quit_tableau_new : public vgui_quit_tableau_ref {
  typedef vgui_quit_tableau_ref base;
};

#endif // vgui_quit_tableau_h_
