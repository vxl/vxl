// This is core/vgui/impl/gtk2/vgui_gtk2_utils.cxx
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   19 Dec 99
// \brief  See vgui_gtk2_utils.h for a description of this file.

#include <iostream>
#include <cstdlib>
#include "vgui_gtk2_utils.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgui/vgui_gl.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>

static bool debug = false;
GtkAccelGroup *vgui_gtk2_utils::accel_group = NULL;

vgui_button vgui_gtk2_utils::translate_button(int button)
{
  if (button == 1)
    return vgui_LEFT;
  else if (button == 2)
    return vgui_MIDDLE;
  else if (button == 3)
    return vgui_RIGHT;
#if 1
  // spinning the wheel generates button events no 4 and 5 -- fsm
  else if (button == 4)
    return vgui_MIDDLE;
  else if (button == 5)
    return vgui_MIDDLE;
#endif
  else
    std::abort();
  return vgui_BUTTON_NULL;
}

vgui_key vgui_gtk2_utils::translate_key(GdkEventKey const *gev)
{
  if (gev->length == 1)
    return vgui_key( *(gev->string) );

  switch (gev->keyval)
  {
   case GDK_Page_Up:
    return vgui_PAGE_UP;
   case GDK_Page_Down:
    return vgui_PAGE_DOWN;
   case GDK_Home:
    return vgui_HOME;
   case GDK_End:
    return vgui_END;
   case GDK_Delete:
    return vgui_DELETE;
   case GDK_Insert:
    return vgui_INSERT;
   case GDK_Up:
    return vgui_CURSOR_UP;
   case GDK_Down:
    return vgui_CURSOR_DOWN;
   case GDK_Left:
    return vgui_CURSOR_LEFT;
   case GDK_Right:
    return vgui_CURSOR_RIGHT;
   default:
    return vgui_KEY_NULL;
  }
}

guint vgui_gtk2_utils::translate_key_reverse(vgui_key key)
{
  guint gdk_key;

  if ( key >= 'A' && key <= 'Z' )
    return char(key);

  if ( key >= 'a' && key <= 'z' )
    return char(key+'A'-'a');

  switch ( key )
  {
    // Function keys
    case vgui_F1:
    case vgui_F2:
    case vgui_F3:
    case vgui_F4:
    case vgui_F5:
    case vgui_F6:
    case vgui_F7:
    case vgui_F8:
    case vgui_F9:
    case vgui_F10:
    case vgui_F11:
    case vgui_F12:
      gdk_key = GDK_F1+key-vgui_F1;
      break;
    case vgui_CURSOR_LEFT:
    case vgui_CURSOR_UP:
    case vgui_CURSOR_RIGHT:
    case vgui_CURSOR_DOWN:
    case vgui_PAGE_UP:
    case vgui_PAGE_DOWN:
      gdk_key = GDK_Left+key-vgui_CURSOR_LEFT;
      break;
    case vgui_HOME:
      gdk_key = GDK_Home;
      break;
    case vgui_END:
      gdk_key = GDK_End;
      break;
    case vgui_DELETE:
      gdk_key = GDK_Delete;
      break;
    case vgui_INSERT:
      gdk_key = GDK_Insert;
      break;
    default: // undefined
      gdk_key = GDK_VoidSymbol;
      break;
  }

  return gdk_key;
}

void vgui_gtk2_utils::set_coordinates(vgui_event &e, const gdouble x, const gdouble y)
{
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  // FIXME : the size of the current glViewport settings is not
  // really what we want -- we want the size of the _window_.
  e.wx = (int)x;
  e.wy = vp[3]-1-(int)y;
}

void vgui_gtk2_utils::set_modifiers(vgui_event &e, const guint state)
{
  e.modifier = vgui_modifier( ((state & 4)?vgui_CTRL:0) |
                              ((state & 8)?vgui_ALT:0) |
                              ((state & 1)?vgui_SHIFT:0) );
}


bool vgui_gtk2_utils::is_modifier(GdkEvent const *gev)
{
  if (gev->type != GDK_KEY_PRESS &&
      gev->type != GDK_KEY_RELEASE)
    return false;

  GdkEventKey *e = (GdkEventKey*)(const_cast<GdkEvent*>(gev));
  // cannot use static_cast<> here since GdkEventKey and GdkEvent are unrelated

#if 0
  // This code would only return 'true' if any of the modifier keys is solely
  // pressed. However we want to return 'true' so long as any of the modifier
  // keys is pressed simultaneously with some other key. This is a must
  // if we want to allow menu accelerator keys. - u97mb

  return e->keyval & GDK_Shift_L ||
         e->keyval & GDK_Shift_R ||
         e->keyval & GDK_Control_L ||
         e->keyval & GDK_Control_R ||
         e->keyval & GDK_Meta_L ||
         e->keyval & GDK_Meta_R ||
         e->keyval & GDK_Alt_L ||
         e->keyval & GDK_Alt_R;
#endif // 0
  // - u97mb
  // GDK_MOD1_MASK corresponds to META key(at least on Sun Solaris)
  return e->state & GDK_CONTROL_MASK ||
         e->state & GDK_SHIFT_MASK ||
         e->state & GDK_MOD1_MASK;
}


static void execute_command(GtkWidget*, gpointer c)
{
  vgui_command *cmnd = static_cast<vgui_command*>(c);
  cmnd->execute();
}


void vgui_gtk2_utils::add_submenu(GtkWidget *widget, const vgui_menu& menu)
{
  for (unsigned i=0;i<menu.size();i++)
  {
    if (menu[i].is_separator())
    {
      if (debug) std::cerr << " <separator>\n";
      GtkWidget* item = gtk_menu_item_new();
      gtk_menu_append(GTK_MENU(widget), item);
      gtk_widget_show(item);
    }

    else if (menu[i].is_command())
    {
      if (debug) std::cerr << " <command>\n";
      GtkWidget* item = gtk_menu_item_new_with_label(menu[i].name.c_str());
      gtk_signal_connect(GTK_OBJECT(item), "activate",
                         GTK_SIGNAL_FUNC(execute_command),
                         (void*)menu[i].cmnd.as_pointer());

      gtk_menu_append(GTK_MENU(widget), item);

      gtk_widget_show(item);
      if (menu[i].short_cut.mod!=vgui_MODIFIER_NULL ||
          menu[i].short_cut.key!=vgui_KEY_NULL)
      {
        GdkModifierType mask = GdkModifierType(0);
        // Health warning - It seems that GDK_MOD1_MASK corresponds
        // to META on Solaris and ALT has no correspondance
        if (menu[i].short_cut.mod & vgui_CTRL)
          mask = GdkModifierType(mask | GDK_CONTROL_MASK);
        if (menu[i].short_cut.mod & vgui_META)
          mask = GdkModifierType(mask | GDK_MOD1_MASK);
        if (menu[i].short_cut.mod & vgui_SHIFT)
          mask = GdkModifierType(mask | GDK_SHIFT_MASK);
        if (menu[i].short_cut.mod & vgui_ALT)
          mask = GdkModifierType(mask | GDK_MOD1_MASK);
        gtk_widget_add_accelerator (item,
                                    "activate",
                                    accel_group,
                                    translate_key_reverse(menu[i].short_cut.key),
                                    mask,
                                    GtkAccelFlags(GTK_ACCEL_VISIBLE|GTK_ACCEL_LOCKED));
      }
    }
    else if (menu[i].is_submenu())
    {
      if (debug) std::cerr << " <submenu>\n";
      GtkWidget* item = gtk_menu_item_new_with_label(menu[i].name.c_str());
      GtkWidget* submenu = gtk_menu_new();

      gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu);
      gtk_menu_append(GTK_MENU(widget), item);
      gtk_widget_show(item);

      add_submenu(submenu, *menu[i].menu);

      GtkWidget *tearoffitem= gtk_tearoff_menu_item_new();
      gtk_menu_prepend(GTK_MENU(submenu), tearoffitem);
      gtk_widget_show(tearoffitem);
    }
  }
}

void vgui_gtk2_utils::set_menu(GtkWidget *widget, const vgui_menu& menu, bool is_menubar)
{
  for (unsigned i=0;i<menu.size();i++)
  {
    if (menu[i].is_separator())
    {
      if (debug) std::cerr << " <separator>\n";
      GtkWidget* item = gtk_menu_item_new();
      if (!is_menubar)
        gtk_menu_append(GTK_MENU(widget), item);
      gtk_widget_show(item);
    }
    else if (menu[i].is_command())
    {
      if (debug) std::cerr << " <command> " << menu[i].name << std::endl;
      GtkWidget* item = gtk_menu_item_new_with_label(menu[i].name.c_str());

      if (is_menubar)
        gtk_menu_bar_append(GTK_MENU_BAR(widget), item);
      else
      {
        if (menu[i].short_cut.mod!=vgui_MODIFIER_NULL ||
            menu[i].short_cut.key!=vgui_KEY_NULL)
        {
          GdkModifierType mask = GdkModifierType(0);
          // Health warning - It seems that GDK_MOD1_MASK corresponds
          // to META on Solaris and ALT has no correspondance
          if (menu[i].short_cut.mod & vgui_CTRL)
            mask = GdkModifierType(mask | GDK_CONTROL_MASK);
          if (menu[i].short_cut.mod & vgui_META)
            mask = GdkModifierType(mask | GDK_MOD1_MASK);
          if (menu[i].short_cut.mod & vgui_SHIFT)
            mask = GdkModifierType(mask | GDK_SHIFT_MASK);
          if (menu[i].short_cut.mod & vgui_ALT)
            mask = GdkModifierType(mask | GDK_MOD1_MASK);
          gtk_widget_add_accelerator (item,
                                      "activate",
                                      accel_group,
                                      translate_key_reverse(menu[i].short_cut.key),
                                      mask,
                                      GtkAccelFlags(GTK_ACCEL_VISIBLE|GTK_ACCEL_LOCKED));
        }
        gtk_menu_append(GTK_MENU(widget), item);
      }

      gtk_signal_connect(GTK_OBJECT(item), "activate",
                         GTK_SIGNAL_FUNC(execute_command),
                         (void*)menu[i].cmnd.as_pointer());

      gtk_widget_show(item);
    }
    else if (menu[i].is_submenu())
    {
      if (debug) std::cerr << " <submenu> " << menu[i].name << std::endl;

      GtkWidget* item = gtk_menu_item_new_with_label(menu[i].name.c_str());
      GtkWidget* submenu = gtk_menu_new();

      gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu);

      if (is_menubar)
        gtk_menu_bar_append(GTK_MENU_BAR(widget), item);
      else
        gtk_menu_append(GTK_MENU(widget), item);

      gtk_widget_show(item);

      add_submenu(submenu, *menu[i].menu);

      GtkWidget *tearoffitem= gtk_tearoff_menu_item_new();
      gtk_menu_prepend(GTK_MENU(submenu), tearoffitem);
      gtk_widget_show(tearoffitem);
    }


  }

}
