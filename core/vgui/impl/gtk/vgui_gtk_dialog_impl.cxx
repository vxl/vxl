// This is core/vgui/impl/gtk/vgui_gtk_dialog_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   28 Dec 99
// \brief  See vgui_gtk_dialog_impl.h for a description of this file.

#define private public // hack ...
#include "vgui_gtk_dialog_impl.h"
#undef private

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

#include <vgui/vgui_gl.h>
#include <vgui/impl/gtk/vgui_gtk_adaptor.h>
#include <vgui/internals/vgui_dialog_field.h>
#include <vgui/internals/vgui_simple_field.h>
#include <vgui/internals/vgui_string_field.h>
#include <gtk/gtkfilesel.h>

static bool debug = false;
static bool is_modal = true;
vcl_string ok_text;
vcl_string cancel_text;
vcl_string title;

// Stuff for file browser and color chooser:
GtkWidget *dialog;     // Dialog widget (global so we can pass modality to filew or colorw)
GtkWidget *filew;      // File browser widget
GtkWidget *colorw;     // Color chooser widget
GtkWidget *file_entry; // This points to the text entry box for a file when a file
                       // file browser is started, so the value can be passed back
                       // from the file browser to the text entry.
GtkWidget *color_entry;// Same for colors.
vcl_string orig_color; // For when color chooser is cancelled.

//--------------------------------------------------------------------------------
//: Constructor
vgui_gtk_dialog_impl::vgui_gtk_dialog_impl(const char* name)
  : vgui_dialog_impl(name)
{
  title = name;
  ok_text = "OK";
  cancel_text = "Cancel";
}


//--------------------------------------------------------------------------------
//: Destructor
vgui_gtk_dialog_impl::~vgui_gtk_dialog_impl() {
}


struct vgui_gtk_dialog_impl_choice {
  vcl_vector<vcl_string> names;
  int index;
};


//--------------------------------------------------------------------------------
//: Make a choice widget
void* vgui_gtk_dialog_impl::choice_field_widget(const char* /*txt*/,
                                                const vcl_vector<vcl_string>& labels,
                                                int& val)
{
  vgui_gtk_dialog_impl_choice *ch = new vgui_gtk_dialog_impl_choice;
  ch->names = labels;
  ch->index = val;

  return (void*)ch;
}

//--------------------------------------------------------------------------------
//: Make a tableau widget.
void* vgui_gtk_dialog_impl::inline_tableau_widget(const vgui_tableau_sptr tab,
  unsigned width, unsigned height)
{
  vgui_gtk_adaptor *ct = new vgui_gtk_adaptor();
  ct->set_tableau(tab);
  GtkWidget *glarea= (( vgui_gtk_adaptor *)ct)->get_glarea_widget();
  gtk_widget_set_usize(glarea, width, height);
  gtk_widget_show(glarea);

  return (void*)glarea;
}

static int loop_flag = 0;

static
void accept_cb(GtkWidget* /*widget*/,
               gpointer /*data*/) {
  if (debug) vcl_cerr << "accept\n";
  loop_flag = 2;
}

static
void cancel_cb(GtkWidget* /*widget*/,
               gpointer /*data*/) {
  if (debug) vcl_cerr << "cancel\n";
  loop_flag = 1;
}

struct vgui_gtk_dialog_impl_int_pair {
  int* val;
  int tmp;
};


void choose_cb(GtkWidget* /*widget*/,
               gpointer data) {

  vgui_gtk_dialog_impl_int_pair *ip = (vgui_gtk_dialog_impl_int_pair*) data;
  *(ip->val) = ip->tmp;
  if (debug) vcl_cerr << "choose " << (ip->tmp) << vcl_endl;
}


//--------------------------------------------------------------------------------
//: Changes the modality of the dialog.
//  True makes the dialog modal (i.e. the dialog 'grabs' all events), this is the default.
//  False makes the dialog non-modal.
void vgui_gtk_dialog_impl::modal(const bool m)
{
  is_modal = m;
}

void vgui_gtk_dialog_impl::set_ok_button(const char* txt)
{
  if (txt)
    ok_text = vcl_string(txt);
  else
    ok_text = vcl_string("REMOVEBUTTON");
}

void vgui_gtk_dialog_impl::set_cancel_button(const char* txt)
{
  if (txt)
    cancel_text = vcl_string(txt);
  else
    cancel_text = vcl_string("REMOVEBUTTON");
}

//--------------------------------------------------------------------------------
//: Gets the file selection for the file browser.
void file_ok_sel(GtkWidget* /*w*/,
                 GtkFileSelection* fs) {

  // Set the text entry back in the dialog to the filename selected in the file browser:
  gtk_entry_set_text(GTK_ENTRY(file_entry),
                     gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs)));
}

//--------------------------------------------------------------------------------

#if 0 // This static function is not used
//: Display a file browser
static void filebrowser_selection_class_init(GtkFileSelectionClass *klass)
{
  GtkObjectClass *object_class = (GtkObjectClass*) klass;

  //parent_class = gtk_type_class (GTK_TYPE_WIDGET);

  object_class->destroy = NULL;
}
#endif

extern void gtk_file_selection_init(GtkFileSelectionClass *klass);

void browse_files(GtkWidget* /*w*/, gpointer data)
{
  file_entry = (GtkWidget*)data;
  //vgui_dialog_field* field = (vgui_dialog_field*)data;

  filew = gtk_file_selection_new ("File selection");

  gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
    "clicked", (GtkSignalFunc) &file_ok_sel, filew );
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION
    (filew)->cancel_button), "clicked", (GtkSignalFunc) gtk_widget_destroy, GTK_OBJECT (filew));
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION
    (filew)->ok_button), "clicked", (GtkSignalFunc) gtk_widget_destroy, GTK_OBJECT (filew));

  gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew),
                                   //field->current_value().c_str());
                                   gtk_entry_get_text(GTK_ENTRY(file_entry)));

  // Hide buttons for create directory, delete and rename file:
  gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(filew));

  // Display only the files matching the given pattern:
  //gtk_file_selection_complete(GTK_FILE_SELECTION(filew),
  //                            gtk_entry_get_text(GTK_ENTRY(??))

  gtk_window_set_modal(GTK_WINDOW(filew), is_modal);
  gtk_widget_show(filew);
}

//-------------------------------------------------------------------------------
//: Handles a change of color.
// Done by passing the value back to the text entry in the dialog box.
void color_changed_cb(GtkWidget* /*w*/, GtkColorSelection *colorsel) {
  // Get the color from the color chooser:
  gdouble color[3];
  gtk_color_selection_get_color(colorsel, color);

  vul_sprintf color_str("%.3f %.3f %.3f", color[0], color[1], color[2]);

  // Set the text entry back in the dialog to the color selected in the color chooser:
  gtk_entry_set_text(GTK_ENTRY(color_entry), color_str.c_str());
}

//-------------------------------------------------------------------------------
//: Handles OK button on color chooser.
// by shutting down the color chooser and passing control back to the dialog.
void ok_color_chooser(GtkWidget* /*w*/, gpointer /*p*/) {
  gtk_widget_destroy(colorw);
}

//-------------------------------------------------------------------------------
//: Handles cancel button on color chooser.
// by resetting the color to its original value, shutting down the color
// chooser and passing control back to the dialog.
void cancel_color_chooser(GtkWidget* /*w*/, gpointer /*p*/) {
  gtk_entry_set_text(GTK_ENTRY(color_entry), orig_color.c_str());

  gtk_widget_destroy(colorw);
}

//-------------------------------------------------------------------------------
//: Display a colour chooser.
void choose_color(GtkWidget* /*w*/, gpointer data) {

  GtkWidget *colorsel;
  color_entry = (GtkWidget*)data;
  orig_color = vcl_string(gtk_entry_get_text(GTK_ENTRY(color_entry)));
  colorw = gtk_color_selection_dialog_new("Select color");

  colorsel = GTK_COLOR_SELECTION_DIALOG(colorw)->colorsel;

  //gtk_signal_connect (GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(colorw)->ok_button), "clicked",
  //  (GtkSignalFunc) color_changed_cb, colorw );
  gtk_signal_connect(GTK_OBJECT(colorsel), "color_changed",
    (GtkSignalFunc)color_changed_cb, (gpointer)colorsel);
  gtk_signal_connect_object (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG
    (colorw)->cancel_button), "clicked", (GtkSignalFunc)cancel_color_chooser, GTK_OBJECT(colorw));
  gtk_signal_connect_object (GTK_OBJECT (GTK_COLOR_SELECTION_DIALOG
    (colorw)->ok_button), "clicked", (GtkSignalFunc)ok_color_chooser, GTK_OBJECT(colorw));

  gtk_window_set_modal(GTK_WINDOW(colorw), is_modal);
  gtk_widget_show(colorw);
}

//-------------------------------------------------------------------------------
//: Display the dialog box.
bool vgui_gtk_dialog_impl::ask() {
  dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
  gtk_window_set_modal(GTK_WINDOW(dialog), is_modal);

  if (ok_text.compare("REMOVEBUTTON"))
  {
    GtkWidget *accept = gtk_button_new_with_label (ok_text.c_str());
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
                     accept, TRUE, TRUE, 0);
    gtk_signal_connect(GTK_OBJECT(accept), "clicked",
                     GTK_SIGNAL_FUNC(accept_cb),
                     dialog);
    gtk_widget_show(accept);
  }
  if (cancel_text.compare("REMOVEBUTTON"))
  {
    GtkWidget *cancel = gtk_button_new_with_label (cancel_text.c_str());
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
                     cancel, TRUE, TRUE, 0);
    gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
                     GTK_SIGNAL_FUNC(cancel_cb),
                     dialog);
    gtk_widget_show(cancel);
  }

  vcl_vector<GtkWidget*> wlist;

  for (vcl_vector<element>::iterator e_iter = elements.begin();
       e_iter != elements.end(); ++e_iter) {

    element l = *e_iter;
    vgui_dialog_field *field = l.field;

    GtkWidget* entry;

    if (l.type == int_elem ||
        l.type == long_elem ||
        l.type == float_elem ||
        l.type == double_elem ||
        l.type == string_elem) {

      GtkWidget* hbox = gtk_hbox_new(FALSE, 10);
      GtkWidget* label = gtk_label_new(field->label.c_str());
      gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);

      entry = gtk_entry_new_with_max_length(50);
      gtk_entry_set_text(GTK_ENTRY(entry), l.field->current_value().c_str());

      gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
      gtk_box_pack_end(GTK_BOX(hbox), entry, FALSE, FALSE, 5);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);

      gtk_widget_show(label);
      gtk_widget_show(entry);
      gtk_widget_show(hbox);
      wlist.push_back(entry);
    }
    else if (l.type == bool_elem) {
      vgui_bool_field *field = static_cast<vgui_bool_field*>(l.field);
      entry = gtk_check_button_new_with_label(field->label.c_str());
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(entry), field->var);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), entry, TRUE, TRUE, 0);
      gtk_widget_show(entry);
      wlist.push_back(entry);
    }
    else if (l.type == choice_elem) {
      vgui_int_field *field = static_cast<vgui_int_field*>(l.field);

      GtkWidget* hbox = gtk_hbox_new(FALSE, 10);
      GtkWidget* label = gtk_label_new(field->label.c_str());
      gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);

      entry = gtk_option_menu_new();
      GtkWidget* menu = gtk_menu_new();


      vgui_gtk_dialog_impl_choice *ch = (vgui_gtk_dialog_impl_choice*)l.widget;

      int count = 0;
      for (vcl_vector<vcl_string>::iterator s_iter =  ch->names.begin();
           s_iter != ch->names.end(); ++s_iter, ++count) {

        GtkWidget* item = gtk_menu_item_new_with_label(s_iter->c_str());
        gtk_widget_show(item);
        gtk_menu_append(GTK_MENU(menu), item);

        vgui_gtk_dialog_impl_int_pair *ip = new vgui_gtk_dialog_impl_int_pair;
        ip->val = &(ch->index);
        ip->tmp = count;

        gtk_signal_connect(GTK_OBJECT(item), "activate",
                           GTK_SIGNAL_FUNC(choose_cb), ip);
      }

      gtk_option_menu_set_menu(GTK_OPTION_MENU(entry), menu);
      gtk_option_menu_set_history(GTK_OPTION_MENU(entry), field->var);

      gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
      gtk_box_pack_start(GTK_BOX(hbox), entry, FALSE, FALSE, 5);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);

      gtk_widget_show(label);
      gtk_widget_show(entry);
      gtk_widget_show(hbox);
      wlist.push_back(entry);
    }
    else if (l.type == text_msg) {
      //GtkWidget* hbox = gtk_hbox_new(FALSE, 10);
      GtkWidget* label = gtk_label_new(field->label.c_str());
      //gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);

      //gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
      //gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
      gtk_widget_show(label);
      //gtk_widget_show(hbox);
      wlist.push_back(entry);
    }
    else if (l.type == file_bsr){
      GtkWidget* hbox = gtk_hbox_new(FALSE, 10);
      GtkWidget* label = gtk_label_new(field->label.c_str());
      gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
      GtkWidget* button = gtk_button_new_with_label("Choose file...");
      entry = gtk_entry_new_with_max_length(150);
      gtk_entry_set_text(GTK_ENTRY(entry), l.field->current_value().c_str());

      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (browse_files), (gpointer)entry);
      gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), button,TRUE,TRUE, 0);

      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);
      gtk_widget_show(label);
      gtk_widget_show(entry);
      gtk_widget_show(button);
      gtk_widget_show(hbox);
      wlist.push_back(entry);
    }
    else if (l.type == inline_file_bsr) {
      GtkWidget* hbox = gtk_hbox_new(FALSE, 10);

      filew = gtk_file_selection_new ("File selection");
      gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew),
                                       l.field->current_value().c_str());
      // Hide buttons for create directory, delete and rename file:
      gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(filew));
      // Destroy Ok,Cancel and Help buttons
      gtk_widget_hide(GTK_FILE_SELECTION(filew)->ok_button);
      gtk_widget_hide(GTK_FILE_SELECTION(filew)->cancel_button);

      gtk_widget_reparent(GTK_FILE_SELECTION(filew)->main_vbox,hbox);
      // capes@robots - this doesn't appear to be needed...
      //      gtk_box_pack_start(GTK_BOX(hbox),GTK_FILE_SELECTION(filew)->main_vbox, TRUE,TRUE,0);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);

      gtk_widget_show(GTK_FILE_SELECTION(filew)->main_vbox);
      gtk_widget_show(hbox);
      wlist.push_back(GTK_FILE_SELECTION(filew)->selection_entry);
    }
    else if (l.type == color_csr){
      GtkWidget* hbox = gtk_hbox_new(FALSE, 10);
      GtkWidget* label = gtk_label_new(field->label.c_str());
      gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
      GtkWidget* button = gtk_button_new_with_label("Choose color...");
      entry = gtk_entry_new_with_max_length(50);
      gtk_entry_set_text(GTK_ENTRY(entry), l.field->current_value().c_str());

      gtk_signal_connect (GTK_OBJECT (button), "clicked",
        GTK_SIGNAL_FUNC (choose_color), (gpointer)entry);
      gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0);
      gtk_box_pack_start(GTK_BOX(hbox), button, TRUE, TRUE, 0);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);

      gtk_widget_show(label);
      gtk_widget_show(entry);
      gtk_widget_show(button);
      gtk_widget_show(hbox);
      wlist.push_back(entry);
    }
    else if (l.type == inline_color_csr) {
      GtkWidget* hbox = gtk_hbox_new(FALSE, 10);
      colorw = gtk_color_selection_dialog_new("dialog");
      color_entry = gtk_entry_new_with_max_length(50);

      gtk_entry_set_text(GTK_ENTRY(color_entry),
                         l.field->current_value().c_str());
      gtk_signal_connect(GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(colorw)->
                                    colorsel),"color_changed",
                         (GtkSignalFunc)color_changed_cb,
                         (gpointer)GTK_COLOR_SELECTION_DIALOG(colorw)->
                                    colorsel);
      gtk_widget_reparent(GTK_COLOR_SELECTION_DIALOG(colorw)->main_vbox,hbox);
      gtk_widget_hide(GTK_COLOR_SELECTION_DIALOG(colorw)->ok_button);
      gtk_widget_hide(GTK_COLOR_SELECTION_DIALOG(colorw)->cancel_button);
      gtk_widget_hide(GTK_COLOR_SELECTION_DIALOG(colorw)->help_button);

      gtk_box_pack_start(GTK_BOX(hbox),
                         GTK_COLOR_SELECTION_DIALOG(colorw)->main_vbox,
                         TRUE, TRUE, 0);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE,
                         TRUE, 0);
      gtk_widget_show(GTK_COLOR_SELECTION_DIALOG(colorw)->main_vbox);
      gtk_widget_show(hbox);
      wlist.push_back(color_entry);
    }
    else if (l.type == inline_tabl) {
      GtkWidget* widg = (GtkWidget*)l.widget;
      GtkWidget* hbox = gtk_hbox_new(FALSE, 10);
      gtk_widget_reparent(widg,hbox);

      gtk_box_pack_start(GTK_BOX(hbox), widg, TRUE, TRUE, 0);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);
      gtk_widget_show(hbox);
      wlist.push_back(widg);
    }
    else
      vcl_cerr << "Unknown type = " << int(l.type) << vcl_endl;
  }

  gtk_widget_show(dialog);

  loop_flag = 0;

  while (1) {
    gtk_main_iteration();
    if (loop_flag)
      break;
  }

  if (loop_flag == 2)  // OK button has been pressed
  {
    vcl_vector<GtkWidget*>::iterator w_iter = wlist.begin();
    for (vcl_vector<element>::iterator e_iter = elements.begin();
         e_iter != elements.end(); ++e_iter, ++w_iter) {
      element l = *e_iter;
      if (l.type == int_elem ||
          l.type == long_elem ||
          l.type == float_elem ||
          l.type == double_elem ||
          l.type == string_elem ||
          l.type == file_bsr ||
          l.type == color_csr ||
          l.type == inline_color_csr) {
        GtkWidget *input = *w_iter;
        l.field->update_value(gtk_entry_get_text(GTK_ENTRY(input)));
      }
      if (l.type == inline_file_bsr)
        l.field->update_value(gtk_file_selection_get_filename(GTK_FILE_SELECTION(filew)));

      if (l.type == bool_elem) {
        vgui_bool_field *field = static_cast<vgui_bool_field*>(l.field);
        GtkWidget *input = *w_iter;
        field->var = (bool) gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(input));
      }
      if (l.type == choice_elem) {
        vgui_int_field *field = static_cast<vgui_int_field*>(l.field);
        vgui_gtk_dialog_impl_choice *ch = static_cast<vgui_gtk_dialog_impl_choice*>(l.widget);
        field->var = ch->index;
      }
    }
    // Need to flush GL events if we have a tableau field, otherwise it crashes:
    glFlush();
    gtk_widget_destroy(dialog);

    return true;
  }
  gtk_widget_destroy(dialog);

  return false;
}
