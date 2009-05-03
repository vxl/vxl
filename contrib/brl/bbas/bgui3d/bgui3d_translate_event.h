// This is brl/bbas/bgui3d/bgui3d_translate_event.h
#ifndef bgui3d_translate_event_h_
#define bgui3d_translate_event_h_
//:
// \file
// \brief translates vgui events into Coin3d events
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date May 24, 2004
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vgui/vgui_event.h>
#include <Inventor/events/SoEvent.h>

//: Translate a \p vgui_event to an \p SoEvent*
// The user is responsible for deleting the SoEvent
SoEvent* bgui3d_translate_event(const vgui_event& v_event);


#endif // bgui3d_translate_event_h_
