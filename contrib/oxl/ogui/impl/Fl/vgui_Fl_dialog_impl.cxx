// This is oxl/ogui/impl/Fl/vgui_Fl_dialog_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   25 Oct 99
//-----------------------------------------------------------------------------

#include "vgui_Fl_dialog_impl.h"

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vul/vul_sprintf.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Menu_Item.H>

#include <vgui/vgui_macro.h>
#include <vgui/internals/vgui_dialog_impl.h>
#include <vgui/internals/vgui_simple_field.h>

//-----------------------------------------------------------------------------

vgui_Fl_dialog_impl::vgui_Fl_dialog_impl(const char* n)
  : vgui_dialog_impl(n)
{
}


vgui_Fl_dialog_impl::~vgui_Fl_dialog_impl()
{
}


void* vgui_Fl_dialog_impl::bool_field_widget(const char* txt, bool& v)
{
  Fl_Check_Button *widget = new Fl_Check_Button(0,0,10,10, txt);
  widget->down_box(FL_DOWN_BOX);
  widget->align(FL_ALIGN_LEFT);
  widget->value(v);

  return widget;
}


void* vgui_Fl_dialog_impl::int_field_widget(const char* txt, int& v)
{
  Fl_Int_Input *widget = new Fl_Int_Input(0,0,10,10, txt);
  widget->value(vul_sprintf("%d", v).c_str());

  return widget;
};


void* vgui_Fl_dialog_impl::long_field_widget(const char* txt, long& v)
{
  Fl_Int_Input *widget = new Fl_Int_Input(0,0,10,10, txt);
  widget->value(vul_sprintf("%d", v).c_str());

  return widget;
}

void* vgui_Fl_dialog_impl::float_field_widget(const char* txt, float& v)
{
  Fl_Float_Input *widget = new Fl_Float_Input(0,0,10,10, txt);
  widget->value(vul_sprintf("%g", v).c_str());

  return widget;
}

void* vgui_Fl_dialog_impl::double_field_widget(const char* txt, double& v)
{
  Fl_Float_Input *widget = new Fl_Float_Input(0,0,10,10, txt);
  widget->value(vul_sprintf("%g", v).c_str());

  return widget;
}

void* vgui_Fl_dialog_impl::string_field_widget(const char* txt, vcl_string& v)
{
  Fl_Input *widget = new Fl_Input(0,0,10,10, txt);
  widget->value(v.c_str());

  return widget;
}

void* vgui_Fl_dialog_impl::choice_field_widget(const char* txt, const vcl_vector<vcl_string>& labels, int& v)
{
  Fl_Choice *widget = new Fl_Choice(0,0,10,10, txt);
  Fl_Menu_Item *menu = new Fl_Menu_Item[labels.size()+1];

  // create a new block of memory for the subitems
  int i=0;
  for (vcl_vector<vcl_string>::const_iterator l_iter = labels.begin();
       l_iter != labels.end(); ++l_iter, ++i)
  {
    Fl_Menu_Item *item = menu+i;
    item->label((*l_iter).c_str());
    item->labeltype_ = item->labelfont_ = 0;
    item->labelsize_ = item->labelcolor_ = 0;
    item->flags = 0;
    item->shortcut_ = 0;
    item->callback_ = 0;
  }
  Fl_Menu_Item *item = menu+labels.size();
  item->text = 0;

  widget->menu(menu);
  widget->value(v);

  return widget;
}

bool vgui_Fl_dialog_impl::ask()
{
  vcl_cerr << "popping up dialog\n";

  int width = 190;
  int height = (elements.size()+1)*35+10;

  Fl_Window dialog(width, height, name.c_str());

  int yval = 10;

  for (vcl_vector<element>::iterator e_iter = elements.begin();
       e_iter != elements.end(); ++e_iter)
  {
    element l = *e_iter;
    Fl_Widget *widget = static_cast<Fl_Widget*>(l.widget);
    if (!widget)
    {
      vgui_macro_warning << "no widget defined for element type " <<  l.type << vcl_endl;
    }
    else
    {
      widget->resize(90,yval,80,25);
      dialog.add(widget);

      yval += 35;
    }
  }


  Fl_Button accept(10, yval, 80, 25, "accept");
  Fl_Button cancel(10+80+10, yval, 80, 25, "cancel");
  accept.color(FL_DARK3);
  accept.labelcolor(FL_LIGHT2);

  cancel.color(FL_DARK3);
  cancel.labelcolor(FL_LIGHT2);

  Fl_Box pad(10+80, 0, 10, 10);
  dialog.resizable(pad);
  dialog.end();
  dialog.set_modal();
  dialog.show();

  bool retv;
  for (;;)
  {
    Fl::wait();
    Fl_Widget *o;
    while ((o = Fl::readqueue()))
    {
      if (o == &accept)
      {
        retv = true;
        goto out;
      }
      else if (o == &cancel)
      {
        retv = false;
        goto out;
      }
    }
  }

 out:
  vcl_cerr << "pressed ";
  if (retv)
  {
    vcl_cerr << "ACCEPT\n";


    for (vcl_vector<element>::iterator e_iter = elements.begin();
         e_iter != elements.end(); ++e_iter)
    {
      element l = *e_iter;

      if (l.type == int_elem ||
          l.type == long_elem ||
          l.type == float_elem ||
          l.type == double_elem)
      {
        Fl_Input_ *input = static_cast<Fl_Input_*>(l.widget);
        l.field->update_value(input->value());
      }
      else if (l.type == string_elem ||
               l.type == file_bsr ||
               l.type == color_csr)
      {
        Fl_Input *input = static_cast<Fl_Input*>(l.widget);
        l.field->update_value(input->value());
      }
      else if (l.type == bool_elem)
      {
        Fl_Check_Button *button = static_cast<Fl_Check_Button*>(l.widget);
        vgui_bool_field *field = static_cast<vgui_bool_field*>(l.field);
        field->var = (bool) button->value();
      }
      else if (l.type == choice_elem)
      {
        Fl_Choice *button = static_cast<Fl_Choice*>(l.widget);
        vgui_int_field *field = static_cast<vgui_int_field*>(l.field);
        field->var = button->value();
      }
    }
  }
  else
    vcl_cerr << "CANCEL\n";

  return retv;
}
