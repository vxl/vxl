#ifndef rgrl_command_h_
#define rgrl_command_h_

//:
// \file
// \brief Base class for callback/observer method
// \author Charlene Tsai
// \date April 2004

#include <vbl/vbl_ref_count.h>
//#include "rgrl_object.h"
//#include "rgrl_event.h"

class rgrl_object;
class rgrl_event;

//: rgrl_command is an implementation of the command design pattern that is usedin callbacks.
//
//  rgrl_object implements the subject. When a subject needs to notify
//  an observer, it does so using a rgrl_command. The \a execute(.)
//  method is called to run the command.
class rgrl_command: public vbl_ref_count
{
public:
  //: 
  rgrl_command() {}

  //: 
  virtual ~rgrl_command() {}

  //: Abstract method that defines the action to be taken by the command
  virtual 
  void execute(rgrl_object* caller, const rgrl_event & event ) = 0;

  //: Abstract method that defines the action to be taken by the command.
  //
  //  This variant is expected to be used when requests comes from a
  //  const rgrl_object
  virtual 
  void execute(const rgrl_object* caller, const rgrl_event & event ) = 0;
};


#endif
