// This is core/vgui/vgui_command.h
#ifndef vgui_command_h_
#define vgui_command_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm and pcp@robots.ox.ac.uk
// \brief  Defines the abstract interface to commands.
//
// \verbatim
//  Modifications
//   awf - Renamed derived classes to be consistent with header-file naming convention.
//   fsm - Fixed everything afterwards....
//   2004/09/10 Peter Vanroose - Added explicit copy constructor (ref_count !)
// \endverbatim

#include <vbl/vbl_ref_count.h>

#include "vgui_command_sptr.h"

//: Defines the abstract interface to commands
struct vgui_command : public vbl_ref_count
{
  vgui_command();
  vgui_command(vgui_command const&) : vbl_ref_count() {}
  virtual ~vgui_command();
  virtual void execute() =0;
};

//: An implementation using a C callback function
struct vgui_command_cfunc : public vgui_command
{
  typedef void (*function_pv)(void const*);
  typedef void (*function)();
  function_pv fn_pv;
  function fn;
  void const *data;

  vgui_command_cfunc(function_pv, void const *);
  vgui_command_cfunc(function);
 ~vgui_command_cfunc();
  void execute();
};

//: Command for toggle buttons
struct vgui_command_toggle : public vgui_command
{
  bool state;
  vgui_command_toggle(bool v) : state(v) { }
  ~vgui_command_toggle();
  void execute();
};

template <class receiver>
//: pcp's templated bound member functions.
// All methods are inline, so we don't need a separate .cxx file.
//
// vgui_command_simple is a convenient way to build vgui_commands from
// object/method pairs where the method is of the form
// void receiver::method();
// So, if you have
// \code
//    class myclass {
//      void do_thing();
//    };
//    myclass* my_app;
// \endcode
// You can make a command such as
//    vgui_command_simple<myclass>(my_app, myclass::do_thing);
//  and pass it to a menu.
struct vgui_command_simple : public vgui_command
{
  typedef void (receiver::* action)();

  vgui_command_simple(receiver* o, action m) : obj(o), mem(m) { }
  void execute() { (obj->*mem)(); }

  receiver* obj;
  action mem;
};

#define VGUI_COMMAND_SIMPLE_INSTANTIATE(receiver) \
template struct vgui_command_simple<receiver >

template <class object_t, class data_t>

//: For methods that take a single argument (fsm).
struct vgui_command_bound_method : public vgui_command
{
  typedef void (object_t::*action_t)(data_t);

  vgui_command_bound_method(object_t *o, action_t m, data_t d) : obj(o), mem(m), dat(d) { }
  void execute() { (obj->*mem)(dat); }

  object_t *obj;
  action_t mem;
  data_t dat;
};

#define VGUI_COMMAND_BOUND_METHOD_INSTANTIATE(O, D) \
template struct vgui_command_bound_method<O, D >

//----------------------------------------------------------------------

#define INSTANTIATE_VGUI_simple_command(receiver) \
VGUI_COMMAND_SIMPLE_INSTANTIATE(receiver) /* backwards compat */

#endif // vgui_command_h_
