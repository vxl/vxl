#ifndef rgrl_event_h_
#define rgrl_event_h_
//:
// \file
// \brief   Base class for event
// \author  Charlene Tsai
// \date    April 2004

#include <vbl/vbl_ref_count.h>
#include <vcl_string.h>
#include "rgrl_event_sptr.h"

//: Event class provides a standard coding for sending and receiving messages.
//
// Common events are beginning of a process, end of a process, and end
// of iteration.
//
// rgrl_event, together with rgrl_command and rgrl_object, implement
// the Observer/Subject design patter. Observers (commands) register
// their interest in particular kinds of events produced by a specific
// rgrl_object.
//
class rgrl_event: public vbl_ref_count
{
 public:
  //:
  rgrl_event() {}

  // copy constructor - compiler-provided one sets ref_count to nonzero which is wrong -PVr
  rgrl_event(rgrl_event const&) : vbl_ref_count() {}

  //:
  virtual ~rgrl_event() {}

  //: Return the string name associated with the event
  virtual vcl_string name(void) const = 0;

  virtual bool is_same_type(rgrl_event const* e) const = 0;
};

// Macro for creating new Events (from ITK)
#define EventMacro( classname ) \
 class classname : public rgrl_event { \
  public: \
   classname() {} \
   classname(classname const& c) : rgrl_event(c) {} \
   virtual ~classname() {} \
   virtual vcl_string name() const { return #classname; } \
   virtual bool is_same_type(rgrl_event const* e) const \
     { return dynamic_cast<const classname*>(e) ? true : false; } \
 }

// Define some common events. To Add new type rgrl_event_X, simply add
// the line "EventMacro( rgrl_event_X, rgrl_event )"
//
EventMacro( rgrl_event_start );
EventMacro( rgrl_event_end );
EventMacro( rgrl_event_iteration );
EventMacro( rgrl_event_next_init );

#endif
