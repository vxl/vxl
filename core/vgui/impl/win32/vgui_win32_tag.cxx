// This is core/vgui/impl/win32/vgui_win32_tag.cxx

// author: Lianqing Yu

#include "vgui_win32.h"
#include <vgui/vgui_tag.h>
#include <vul/vul_trace.h>

// According to vgui_tag.h, the macro vgui_tag(win32) is expanded as:
// int vgui_win32_tag_function();
// int vgui_win32_tag = vgui_tag_add(vgui_win32_tag_function, "win32");
// static int vgui_win32_tag_function_();
// int vgui_win32_tag_function()
// {
//   static bool once = false;
//   static int  value = 0;
//   if (!once)
//     value = vgui_win32_tag_function_();
//   once = true;
//   return value;
// }
//
// int vgui_win32_tag_function_()

vgui_tag(win32)
{
  vul_trace;
  vgui_win32::instance();
  vul_trace;
  return 0;
}
