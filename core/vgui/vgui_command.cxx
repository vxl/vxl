// This is oxl/vgui/vgui_command.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm
// \brief  See vgui_command.h for a description of this file.

#include "vgui_command.h"
#include <vcl_iostream.h>

//-----------------------------------------------------------------------------
vgui_command::vgui_command()
{
  //vcl_cerr << "command ctor : " << (void*)this << vcl_endl;
}

vgui_command::~vgui_command()
{
  //vcl_cerr << "command dtor : " << (void*)this << vcl_endl;
}

//-----------------------------------------------------------------------------
vgui_command_cfunc::vgui_command_cfunc(function_pv f, void const *d)
  : fn_pv(f)
  , fn(0)
  , data(d)
{
}

//-----------------------------------------------------------------------------
vgui_command_cfunc::vgui_command_cfunc(function f)
  : fn_pv(0)
  , fn(f)
  , data(0)
{
}

//-----------------------------------------------------------------------------
vgui_command_cfunc::~vgui_command_cfunc()
{
}

//-----------------------------------------------------------------------------
void vgui_command_cfunc::execute()
{ 
  if (fn_pv)
    (*fn_pv)(data);
  else if (fn)
    (*fn)();
  else
    vcl_cerr << "vgui_command_cfunc : fn is null\n";
}

//-----------------------------------------------------------------------------
vgui_command_toggle::~vgui_command_toggle()
{
}

//-----------------------------------------------------------------------------
void vgui_command_toggle::execute()
{
  //vcl_cerr << vcl_endl;
  //vcl_cerr << "toggle. old state : " << state << vcl_endl;
  state = !state;
  //vcl_cerr << "toggle. new state : " << state << vcl_endl;
  //vcl_cerr << vcl_endl;
}

