// This is oxl/ogui/impl/Xm/vgui_Xm_utils.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   02 Nov 99
//-----------------------------------------------------------------------------

#include "vgui_Xm_utils.h"

#include <vcl_iostream.h>
#include <X11/IntrinsicP.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>
#include <Xm/RowColumnP.h>
#include <Xm/Separator.h>

#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>

static bool debug = false;

static void execute_command(Widget w, XtPointer clientData, XtPointer callData)
{
  vgui_command *cmnd = static_cast<vgui_command*>(clientData);
  cmnd->execute();
}

void vgui_Xm_utils::add_submenu(Widget xpulldown, const vgui_menu& menu)
{
  for (unsigned i=0;i<menu.size();i++)
  {
    if (menu[i].is_separator())
    {
      if (debug) vcl_cerr << " <separator>\n";
      Widget xsep = XtVaCreateManagedWidget(menu[i].name.c_str(),
                                            xmSeparatorWidgetClass, xpulldown,
                                            NULL);
    }
    if (menu[i].is_command())
    {
      if (debug) vcl_cerr << " <command>\n";
      Widget xbutton = XtVaCreateManagedWidget(menu[i].name.c_str(),
                                               xmPushButtonWidgetClass, xpulldown,
                                               NULL);
      XtAddCallback(xbutton, XmNactivateCallback, execute_command, menu[i].cmnd.ptr());
    }
    else if (menu[i].is_submenu())
    {
      if (debug) vcl_cerr << " <submenu>\n";
      Widget xpulldown2 = XmCreatePulldownMenu(xpulldown,"menu",NULL, 0);
      Widget xcascade2 = XtVaCreateManagedWidget (menu[i].name.c_str(),
                                                  xmCascadeButtonWidgetClass, xpulldown,
                                                  XmNsubMenuId,   xpulldown2,
                                                  NULL);
      add_submenu(xpulldown2, *menu[i].menu);
    }
  }
}


void vgui_Xm_utils::set_menu(Widget widget, const vgui_menu& menu)
{
  for (unsigned i=0;i<menu.size();i++)
  {
    if (menu[i].is_separator())
    {
      if (debug) vcl_cerr << " <separator> " << menu[i].name << vcl_endl;
      Widget xsep = XtVaCreateManagedWidget(menu[i].name.c_str(),
                                            xmSeparatorWidgetClass, widget,
                                            NULL);
    }

    if (menu[i].is_command())
    {
      if (debug) vcl_cerr << " <command> " << menu[i].name << vcl_endl;
      Widget xbutton = XtVaCreateManagedWidget(menu[i].name.c_str(),
                                               xmPushButtonWidgetClass, widget,
                                               NULL);
#if 0 // commented out
      Widget xbutton = XtVaCreateManagedWidget(menu[i].name.c_str(),
                                               xmToggleButtonWidgetClass, widget,
                                               NULL);
#endif

      XtAddCallback(xbutton, XmNactivateCallback, execute_command, menu[i].cmnd.ptr());
    }
    else if (menu[i].is_submenu())
    {
      if (debug) vcl_cerr << " <submenu> " << menu[i].name << vcl_endl;
      Widget xpulldown = XmCreatePulldownMenu(widget,"menu",NULL, 0);
      Widget xcascade = XtVaCreateManagedWidget (menu[i].name.c_str(),
                                                 xmCascadeButtonWidgetClass, widget,
                                                 XmNsubMenuId,   xpulldown,
                                                 NULL);
      add_submenu(xpulldown, *menu[i].menu);
    }
  }
}
