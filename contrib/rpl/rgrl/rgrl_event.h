#ifndef rgrl_event_h_
#define rgrl_event_h_
//:
// \file
// \brief   Base class for event
// \author  Charlene Tsai
// \date    April 2004

#include <iostream>
#include <string>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  rgrl_event() = default;

  // copy constructor - compiler-provided one sets ref_count to nonzero which is wrong -PVr
  rgrl_event(rgrl_event const&) : vbl_ref_count() {}

  //:
  ~rgrl_event() override = default;

  //: Return the string name associated with the event
  virtual std::string name() const = 0;

  virtual bool is_same_type(rgrl_event const& e) const = 0;
};

// Macro for creating new Events (from ITK)
#define EventMacro( classname ) \
 class classname : public rgrl_event { \
  public: \
   classname() {} \
   classname(classname const& c) : rgrl_event(c) {} \
   virtual ~classname() {} \
   virtual std::string name() const { return #classname; } \
   virtual bool is_same_type(rgrl_event const& e) const \
     { rgrl_event const* p=&e; return dynamic_cast<classname const*>(p) ? true : false; } \
 }

// Define some common events. To Add new type rgrl_event_X, simply add
// the line "EventMacro( rgrl_event_X, rgrl_event )"
//
EventMacro( rgrl_event_start );
EventMacro( rgrl_event_end );
EventMacro( rgrl_event_iteration );
EventMacro( rgrl_event_verification );
EventMacro( rgrl_event_next_init );
EventMacro( rgrl_matches_formed );

#endif
