#ifndef vgui_event_loop_h_
#define vgui_event_loop_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgui_event_loop
// .INCLUDE vgui/vgui_event_loop.h
// .FILE vgui_event_loop.cxx
// @author fsm@robots.ox.ac.uk

struct vgui_slab;
struct vgui_tableau_sptr;

void vgui_event_loop_attach(vgui_slab *s, vgui_tableau_sptr const& t);

void vgui_event_loop_detach(vgui_slab *s);

void vgui_event_loop_service();

int  vgui_event_loop();

void vgui_event_loop_finish();

#endif // vgui_event_loop_h_
