#ifndef rgrl_object_h_
#define rgrl_object_h_
//:
// \file
// \brief Base class for most rgrl classes
// \author Charlene Tsai
// \date April 2004

#include <vbl/vbl_ref_count.h>

#include <vcl_map.h>

#include "rgrl_command_sptr.h"
#include "rgrl_event_sptr.h"
#include "rgrl_macros.h"
#include "rgrl_command.h"
#include "rgrl_event.h"

//: Observer class to bind a command with an event
class rgrl_object_observer
{
 public:
  rgrl_object_observer() {}
  rgrl_object_observer(rgrl_command_sptr c,
                       rgrl_event_sptr event )
    :command_(c),
     event_(event)
  {}
  ~rgrl_object_observer(){}
  rgrl_command_sptr command_;
  rgrl_event_sptr event_;
};

//: rgrl_object implements callbacks (via object/observer), and debug flags.
//
//  Most rgrl classes should be a subclas of rgrl_object.
class rgrl_object
  : public vbl_ref_count
{
 public:
  //:
  rgrl_object();

  //: copy constructor
  rgrl_object( const rgrl_object& that )
    : vbl_ref_count(), debug_flag_(that.debug_flag_), warning_(that.warning_),
      observers_(that.observers_), observer_count_(that.observer_count_)
  {    }   //suppress copying of reference count between objects


  //: assignment operator
  const rgrl_object& operator=( const rgrl_object& rhs )
  {
    //suppress copying of reference count between objects
    debug_flag_     = rhs.debug_flag_;
    warning_        = rhs.warning_;
    observers_      = rhs.observers_;
    observer_count_ = rhs.observer_count_;
    return *this;
  }
  //:
  virtual ~rgrl_object();

  static const vcl_type_info& type_id()
  { return typeid(rgrl_object); }

  virtual bool is_type( const vcl_type_info& type ) const
  { return (typeid(rgrl_object) == type)!=0; }

  //: Set the value of the debug flag. A non-zero value turns debugging on.
  void set_debug_flag( unsigned int debugFlag ) const;

  //: Get the value of the debug flag.
  unsigned int debug_flag() const;

  //: Set the flag for warning messages
  void set_warning(bool) const;

  //: Get the warning flag
  bool warning() const;

  //: Allow people to add/remove/invoke observers (callbacks) to any rgrl object.
  //
  // This is an implementation of the subject/observer design
  // pattern. An observer is added by specifying an event to respond
  // to and an rgrl_ommand to execute. It returns an unsigned long tag
  // which can be used later to remove the event or retrieve the
  // command.
  unsigned int add_observer( rgrl_event_sptr event, rgrl_command_sptr );

  //: Get the command associated with the given tag.
  rgrl_command_sptr get_command(unsigned int tag);

  //: Call \a execute(.) on all the rgrl_commands observing this event id.
  void invoke_event( const rgrl_event & );

  //: Call \a execute(.) on all the rgrl_commands observing this event id.
  //
  //  The actions triggered by this call doesn't modify this object.
  void invoke_event( const rgrl_event & ) const;

  //: Remove the observer with this tag value.
  void remove_observer(unsigned int tag);

  //: Return true if an observer is registered for this event.
  bool has_observer( const rgrl_event & event ) const;

 private:
#if 0
  //: copy constructor and =operator are disabled on purpose
  rgrl_object( const rgrl_object& );
  void operator=( const rgrl_object& );
#endif

  // For debugging
  mutable unsigned int debug_flag_;
  mutable bool warning_;

  // For event handling
  typedef vcl_map< unsigned, rgrl_object_observer > observer_map;
  observer_map observers_;
  unsigned int observer_count_;
};

#endif
