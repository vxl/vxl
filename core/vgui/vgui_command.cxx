/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vgui_command.h"
#include <vcl_iostream.h>

//--------------------------------------------------------------------------------

vgui_command::vgui_command()
{
  //cerr << "command ctor : " << (void*)this << endl;
}

vgui_command::~vgui_command()
{
  //cerr << "command dtor : " << (void*)this << endl;
}

//--------------------------------------------------------------------------------

vgui_command_cfunc::vgui_command_cfunc(function_pv f, void const *d)
  : fn_pv(f)
  , fn(0)
  , data(d)
{
}

vgui_command_cfunc::vgui_command_cfunc(function f)
  : fn_pv(0)
  , fn(f)
  , data(0)
{
}

vgui_command_cfunc::~vgui_command_cfunc()
{
}

void vgui_command_cfunc::execute()
{ 
  if (fn_pv)
    (*fn_pv)(data);
  else if (fn)
    (*fn)();
  else
    vcl_cerr << "vgui_command_cfunc : fn is null" << vcl_endl;
}

//--------------------------------------------------------------------------------

vgui_command_toggle::~vgui_command_toggle()
{
}

void vgui_command_toggle::execute()
{
  //cerr << endl;
  //cerr << "toggle. old state : " << state << endl;
  state = !state;
  //cerr << "toggle. new state : " << state << endl;
  //cerr << endl;
}

//--------------------------------------------------------------------------------
