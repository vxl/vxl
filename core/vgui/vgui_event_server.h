#ifndef vgui_event_server_h_
#define vgui_event_server_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_event_server - Procedural event grabbing from a tableau
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_event_server.h
// .FILE vgui_event_server.cxx
//
// .SECTION Description:
//
// vgui_event_server allows you to grab events intended for a tableau in a
// procedural way, (i.e. in a "for" loop), in order to temporarily take control
// of a tableau's event stream.  This is useful for modal operations such as
// picking.
//
// .SECTION Author:
//              Philip C. Pritchett, 11 Sep 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vgui/vgui_event.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_tableau.h>
class vgui_event_server_interpose_tableau;

class vgui_event_server {
public:
  vgui_event_server(vgui_tableau_sptr const&);

  void reset();
  bool next();

  void set_popup(const vgui_menu&);

  vgui_event last_event();

protected:
 ~vgui_event_server();

private:

  friend class vgui_event_server_interpose_tableau;
  vgui_event_server_interpose_tableau* grabber_;
  vgui_tableau_sptr grabber_reference_;
  vgui_event last_event_;
  bool use_event_;
  vgui_menu menu;
};

#endif // vgui_event_server_h_
