#ifndef vgui_command_h_
#define vgui_command_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgui_command
// .INCLUDE vgui/vgui_command.h
// .FILE vgui_command.cxx
/*
  fsm@robots.ox.ac.uk \and pcp@robots.ox.ac.uk
  awf renamed derived classes to be consistent with 
  header-file naming convention.
  fsm fixed everything afterwards....
*/
#include <vbl/vbl_ref_count.h>

// this defines the abstract interface to commands.
class vgui_command : public vbl_ref_count {
public:
  vgui_command();
  virtual ~vgui_command();
  virtual void execute() =0;
};

// an implementation using a C callback function.
class vgui_command_cfunc : public vgui_command {
public:
  typedef void (*function_pv)(const void*);
  typedef void (*function)();
  function_pv fn_pv;
  function fn;
  const void *data;

  vgui_command_cfunc(function_pv, const void *);
  vgui_command_cfunc(function);
 ~vgui_command_cfunc();
  void execute();
};

// pcp's templated bound member functions. All methods are inline, so 
// we don't need a separate .cxx file.
//
// vgui_command_simple is a convenient way to build vgui_commands from 
// object/method pairs where the method is of the form 
// void receiver::method(void);
template <class receiver>
class vgui_command_simple : public vgui_command {
public:
  typedef void (receiver::* action)();

  // 2.7 barfs unless this is inline.
  vgui_command_simple(receiver* o, action m) : vgui_command(), obj(o), mem(m) { } 
  ~vgui_command_simple() { }
  void execute() { (obj->*mem)(); }
  
  receiver* obj;
  action mem;
};
// instantiation macro :
#define INSTANTIATE_VGUI_simple_command(receiver) \
template class vgui_command_simple<receiver >

// command for toggle buttons
class vgui_command_toggle : public vgui_command {
public:
  bool state;
  vgui_command_toggle(bool v) : state(v) { }
  ~vgui_command_toggle();
  void execute();
};

#endif // vgui_command_h_
