// This is oxl/vgui/impl/Xm/vgui_Xm_dialog_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 25 Oct 99
//
//-----------------------------------------------------------------------------

#include "vgui_Xm_dialog_impl.h"

#include <vcl_vector.h>

#include <vgui/internals/vgui_dialog_impl.h>
#include <vgui/internals/vgui_dialog_field.h>
#include <vgui/internals/vgui_simple_field.h>
#include "vgui_Xm.h"

#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/BaseClassP.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/DesktopP.h>
#include <Xm/DisplayP.h>
#include <Xm/ScreenP.h>
#include <Xm/ShellEP.h>
#include <Xm/VendorSEP.h>
#include <Xm/DialogSP.h>
#include <Xm/DialogSEP.h>
#include <Xm/VendorSP.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Separator.h>
#include <Xm/MessageB.h>
#include <Xm/ToggleB.h>

//-----------------------------------------------------------------------------
//
// Default Constructor
//

vgui_Xm_dialog_impl::vgui_Xm_dialog_impl(const char* n)
  : vgui_dialog_impl(n)
{
}


vgui_Xm_dialog_impl::~vgui_Xm_dialog_impl()
{
}

struct vgui_Xm_dialog_choice
{
  vcl_vector<vcl_string> names;
  int index;
};

void* vgui_Xm_dialog_impl::choice_field_widget(const char* txt, const vcl_vector<vcl_string>& labels, int& v)
{
  vgui_Xm_dialog_choice *ch = new vgui_Xm_dialog_choice;
  ch->names = labels;
  ch->index = v;

  return (void*)ch;
}


static int loop_flag = 0;

void cancelCB(Widget w, XtPointer client_data, XtPointer call)
{
  //vcl_cerr << "cancel\n";
  loop_flag = 1;
}

void acceptCB(Widget w, XtPointer client_data, XtPointer call)
{
  //vcl_cerr << "accept\n";
  loop_flag = 2;
}

void gain_focusCB(Widget w, XtPointer client_data, XtPointer call)
{
  //vcl_cerr << "gain\n";
  XtVaSetValues(w, XmNcursorPositionVisible, True, NULL);
}

void lose_focusCB(Widget w, XtPointer client_data, XtPointer call)
{
  //vcl_cerr << "lose\n";
  XtVaSetValues(w, XmNcursorPositionVisible, False, NULL);
}

struct vgui_Xm_dialog_impl_int_pair
{
  int* val;
  int tmp;
};


void chooseCB(Widget w, XtPointer client_data, XtPointer call)
{
  vgui_Xm_dialog_impl_int_pair *ip = (vgui_Xm_dialog_impl_int_pair*) client_data;
  *(ip->val) = ip->tmp;
}

void simpleCB(Widget w, XtPointer client_data, XtPointer call)
{
  //vcl_cerr << "simpleCB " << (int) client_data << vcl_endl;
}


bool vgui_Xm_dialog_impl::ask()
{
  //vcl_cerr << "vgui_Xm_dialog_impl ask()\n";
  int width = 190;
  int height = (elements.size()+1)*35+10;
  //vcl_cerr << "elements size : " << elements.size() << vcl_endl;

  //dialog_shell = XmCreateFormDialog(vgui_Xm::instance()->app_widget, "dialog shell", NULL, 0);

  String t = (char*)(name.c_str());
  XmString dialog_title = XmStringCreateLocalized(t);

  dialog_shell = XmCreateMessageDialog(vgui_Xm::instance()->app_widget, "Box", NULL, 0);
  XtVaSetValues(dialog_shell,
                XmNdialogTitle, dialog_title,
                XmNdialogType, XmDIALOG_TEMPLATE,
                XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,
                NULL);

  XtAddCallback(dialog_shell, XmNcancelCallback, cancelCB, this);
  XtAddCallback(dialog_shell, XmNokCallback, acceptCB, this);


  XtUnmanageChild(XmMessageBoxGetChild(dialog_shell, XmDIALOG_HELP_BUTTON));

  Widget accept = XmMessageBoxGetChild(dialog_shell, XmDIALOG_OK_BUTTON);
  XmString label = XmStringCreateLocalized ("Accept");
  XtVaSetValues(accept,
                XmNlabelString, label,
                NULL);
  XmStringFree(label);


  //XtManageChild(accept);
  //XtManageChild(cancel);


  Widget form = XmCreateForm(dialog_shell, "topArea", NULL, 0);
  XtVaSetValues(form,
                XmNrightAttachment, XmATTACH_FORM,
                XmNtopAttachment, XmATTACH_FORM,
                XmNleftAttachment, XmATTACH_FORM,
                XmNbottomAttachment, XmATTACH_NONE,
                NULL);


  Widget inputs = XtVaCreateManagedWidget("inputs", xmRowColumnWidgetClass, form,
                                          //XtNtitle, name.c_str(),
                                          //XtNwidth, width,
                                          //XtNheight, height,
                                          //XmNpacking,XmPACK_COLUMN,
                                          XmNpacking,XmPACK_COLUMN,
                                          XmNnumColumns,elements.size(),
                                          XmNorientation,XmHORIZONTAL,
                                          XmNisAligned,True,
                                          XmNentryAlignment,XmALIGNMENT_END,
                                          XmNadjustLast, False,
                                          NULL);

  vcl_vector<Widget> wlist;

  for (vcl_vector<element>::iterator e_iter = elements.begin();
       e_iter != elements.end(); ++e_iter)
  {
    element l = *e_iter;
    vgui_dialog_field *field = l.field;
    // vcl_cerr << "making field " << field->label.c_str() << vcl_endl;
    Widget label = XtVaCreateManagedWidget(field->label.c_str(), xmLabelWidgetClass,inputs, NULL);


    if (l.type == int_elem ||
        l.type == long_elem ||
        l.type == double_elem ||
        l.type == float_elem ||
        l.type == string_elem ||
        l.type == file_bsr ||
        l.type == color_csr)
    {
      Widget widget = XtVaCreateManagedWidget("text", xmTextFieldWidgetClass, inputs,
                                              XmNcolumns, 8,
                                              //XmNvnl_resizeWidth, True,
                                              NULL);
      XtAddCallback(widget, XmNfocusCallback, gain_focusCB, this);
      XtAddCallback(widget, XmNlosingFocusCallback, lose_focusCB, this);


      XtVaSetValues(widget, XmNcursorPositionVisible, False, NULL);

      char* t = (char*)(l.field->current_value().c_str());
      XmTextFieldSetString(widget, t);

      wlist.push_back(widget);
    }
    else if (l.type == bool_elem)
    {
      Widget button= XtVaCreateManagedWidget(0, xmToggleButtonWidgetClass, inputs,
                                             NULL);
      vgui_bool_field *field = static_cast<vgui_bool_field*>(l.field);
      XmToggleButtonSetState(button, field->var, True);
      wlist.push_back(button);
    }
    else if (l.type == choice_elem)
    {
      vgui_Xm_dialog_choice *ch = (vgui_Xm_dialog_choice*)l.widget;
      vgui_int_field *field = static_cast<vgui_int_field*>(l.field);

      Widget pane = XmCreatePulldownMenu(inputs, "pulldown", NULL, 0);

      int count = 0;

      Widget active;
      for (vcl_vector<vcl_string>::iterator s_iter =  ch->names.begin();
           s_iter != ch->names.end(); ++s_iter, ++count)
      {
        Widget button = XtVaCreateManagedWidget(s_iter->c_str(),
                                                xmPushButtonWidgetClass,
                                                pane,
                                                NULL);

        vgui_Xm_dialog_impl_int_pair *ip = new vgui_Xm_dialog_impl_int_pair;
        ip->val = &(ch->index);
        ip->tmp = count;
        XtAddCallback(button,  XmNactivateCallback, chooseCB, ip);

        if (count == field->var)
        {
          active = button;
        }
      }

      Widget options = XmCreateOptionMenu(inputs, "option", NULL, 0);

      XtManageChild(pane);
      XtManageChild(options);

      XtVaSetValues(options, XmNsubMenuId, pane, NULL);
      XtVaSetValues(options, XmNmenuHistory, active, NULL);


      wlist.push_back(options);
    }
  }

  XtManageChild(form);
  XtManageChild(dialog_shell);

  loop_flag = 0;

  while (1)
  {
    XEvent xevent;
    XtAppNextEvent(vgui_Xm::instance()->app_context, &xevent);


    XtDispatchEvent(&xevent);

    if (loop_flag)
      break;
  }

  if (loop_flag == 2)
  {
    vcl_vector<Widget>::iterator w_iter = wlist.begin();
    for (vcl_vector<element>::iterator e_iter = elements.begin();
         e_iter != elements.end(); ++e_iter, ++w_iter)
    {
      element l = *e_iter;

      if (l.type == int_elem ||
          l.type == long_elem ||
          l.type == float_elem ||
          l.type == double_elem ||
          l.type == string_elem ||
          l.type == file_bsr ||
          l.type == color_csr)
      {
        Widget input = *w_iter;
        l.field->update_value(XmTextFieldGetString(input));
      }
      else if (l.type == bool_elem)
      {
        vgui_bool_field *field = static_cast<vgui_bool_field*>(l.field);
        field->var = (bool) XmToggleButtonGetState(*w_iter);
      }
      else if (l.type == choice_elem)
      {
        vgui_int_field *field = static_cast<vgui_int_field*>(l.field);
        vgui_Xm_dialog_choice *ch = static_cast<vgui_Xm_dialog_choice*>(l.widget);
        //vcl_cerr << "choice.index : " << ch->index << vcl_endl;
        field->var = ch->index;
      }
    }
    return true;
  }

  return false;
}
