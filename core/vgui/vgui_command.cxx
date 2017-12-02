// This is core/vgui/vgui_command.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm
// \brief  See vgui_command.h for a description of this file.

#include <iostream>
#include "vgui_command.h"
#include <vcl_compiler.h>

//-----------------------------------------------------------------------------
vgui_command::vgui_command()
{
#ifdef DEBUG
  std::cerr << "vgui_command ctor : " << (void*)this << '\n';
#endif
}

vgui_command::~vgui_command()
{
#ifdef DEBUG
  std::cerr << "vgui_command dtor : " << (void*)this << '\n';
#endif
}

//-----------------------------------------------------------------------------
vgui_command_cfunc::vgui_command_cfunc(function_pv f, void const *d)
  : fn_pv(f)
  , fn(VXL_NULLPTR)
  , data(d)
{
}

//-----------------------------------------------------------------------------
vgui_command_cfunc::vgui_command_cfunc(function f)
  : fn_pv(VXL_NULLPTR)
  , fn(f)
  , data(VXL_NULLPTR)
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
    std::cerr << "vgui_command_cfunc : fn is null\n";
}

//-----------------------------------------------------------------------------
vgui_command_toggle::~vgui_command_toggle()
{
}

//-----------------------------------------------------------------------------
void vgui_command_toggle::execute()
{
#ifdef DEBUG
  std::cerr << "\nvgui_command_toggle. old state : " << state;
#endif
  state = !state;
#ifdef DEBUG
  std::cerr << "; new state : " << state << ".\n\n";
#endif
}

