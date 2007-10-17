// This is basic/bgui3d/bgui3d_translate_event.cxx
//:
// \file

#include "bgui3d_translate_event.h"
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoLocation2Event.h>

#include <vcl_iostream.h>
#include <vul/vul_get_timestamp.h>

//: Translate a \p vgui_event to an \p SoEvent*
SoEvent*
bgui3d_translate_event(const vgui_event& v_event)
{
  SoEvent* i_event = NULL;
  switch(v_event.type){
  case vgui_BUTTON_DOWN:
    {
      SoMouseButtonEvent * temp = new SoMouseButtonEvent;
      temp->setButton(SoMouseButtonEvent::Button(v_event.button));
      temp->setState(SoButtonEvent::DOWN);
      i_event = temp;
      break;
    }
  case vgui_BUTTON_UP:
    {
      SoMouseButtonEvent * temp = new SoMouseButtonEvent;
      temp->setButton(SoMouseButtonEvent::Button(v_event.button));
      temp->setState(SoButtonEvent::UP);
      i_event = temp;
      break;
    }
  case vgui_MOTION:
    {
      i_event = new SoLocation2Event;
      break;
    }
  case vgui_KEY_PRESS:
    {
      SoKeyboardEvent * temp = new SoKeyboardEvent;
      temp->setKey(SoKeyboardEvent::Key(v_event.key));
      temp->setState(SoButtonEvent::DOWN);
      i_event = temp;
      break;
    }
  case vgui_KEY_RELEASE:
    {
      SoKeyboardEvent * temp = new SoKeyboardEvent;
      temp->setKey(SoKeyboardEvent::Key(v_event.key));
      temp->setState(SoButtonEvent::UP);
      i_event = temp;
      break;
    }
  default:
    {
      i_event = new SoEvent;
      break;
    }
  }
  
  // Compute the absolute timestamp since vgui_events only store
  // time relative to the start of the application
  int secs_now, msecs_now;
  vul_get_timestamp(secs_now, msecs_now);
  // adjust for time since this event was constructucted (usually very small)
  long usecs_now = long(msecs_now)*1000 + v_event.usecs_since(vgui_event());
  i_event->setTime(SbTime(secs_now, usecs_now));
  
  i_event->setPosition(SbVec2s(v_event.wx,v_event.wy));
  
  switch(v_event.modifier){
  case vgui_CTRL:
    i_event->setCtrlDown(true);
    break;
  case vgui_SHIFT:
    i_event->setShiftDown(true);
    break;
  case vgui_ALT:
    i_event->setAltDown(true);
    break;
  default:
    break;
  }
  
  return i_event;
}

