// This is core/vgui/impl/gtk/vgui_gtk_dialog_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   28 Dec 99
// \brief  See vgui_gtk_dialog_impl.h for a description of this file.

#include "vgui_gtk_dialog_impl.h"

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vul/vul_sprintf.h>

#include <vgui/vgui_gl.h>
#include <vgui/impl/gtk/vgui_gtk_adaptor.h>
#include <vgui/internals/vgui_dialog_field.h>
#include <vgui/internals/vgui_simple_field.h>
#include <gtk/gtkfilesel.h>

static bool debug = false;
static bool is_modal = true;


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
vgui_gtk_dialog_impl::~vgui_gtk_dialog_impl()
{
}


struct vgui_gtk_dialog_impl_choice
{
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
//
// This returns a vgui_gtk_adaptor, not a GtkWidget!
void* vgui_gtk_dialog_impl::inline_tableau_widget(const vgui_tableau_sptr tab,
                                                  unsigned width, unsigned height)
{
  vgui_gtk_adaptor *ct = new vgui_gtk_adaptor();
  ct->set_tableau(tab);
  GtkWidget *glarea= (( vgui_gtk_adaptor *)ct)->get_glarea_widget();
  gtk_widget_set_usize(glarea, width, height);
  gtk_widget_show(glarea);

  return (void*)ct;
}

// GTK callbacks should have C linkage
extern "C" {

static
void accept_cb(GtkWidget* /*widget*/,
               gpointer   data)
{
  if (debug) vcl_cerr << "accept\n";
  vgui_gtk_dialog_impl::status_type* d = static_cast<vgui_gtk_dialog_impl::status_type*>(data);
  *d = vgui_gtk_dialog_impl::OK;
}

static
void cancel_cb(GtkWidget* /*widget*/,
               gpointer data)
{
  if (debug) vcl_cerr << "cancel\n";
  vgui_gtk_dialog_impl::status_type* d = static_cast<vgui_gtk_dialog_impl::status_type*>(data);
  *d = vgui_gtk_dialog_impl::CANCEL;
}

static
gint close_window_cb(GtkWidget* /*widget*/,
                     GdkEvent* /*event*/,
                     gpointer data)
{
  if (debug) vcl_cerr << "close window\n";
  vgui_gtk_dialog_impl::status_type* d = static_cast<vgui_gtk_dialog_impl::status_type*>(data);
  *d = vgui_gtk_dialog_impl::CLOSE;
  return FALSE; // propagate as necessary
}

struct vgui_gtk_dialog_impl_int_pair
{
  int* val;
  int tmp;
};


void choose_cb(GtkWidget* /*widget*/,
               gpointer data)
{
  vgui_gtk_dialog_impl_int_pair *ip = (vgui_gtk_dialog_impl_int_pair*) data;
  *(ip->val) = ip->tmp;
  if (debug) vcl_cerr << "choose " << (ip->tmp) << vcl_endl;
}

} // extern "C"

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

extern "C" {

struct file_ok_data
{
  GtkFileSelection* filew;
  GtkEntry* file_entry;
};

//--------------------------------------------------------------------------------
//: Gets the file selection for the file browser.
void ok_file_browse(file_ok_data* data)
{
  gtk_entry_set_text( data->file_entry,
                      gtk_file_selection_get_filename(data->filew) );
  gtk_widget_destroy( GTK_WIDGET(data->filew) );
  delete data;
}

//--------------------------------------------------------------------------------
//: Cancels the file selection
void cancel_file_browse(file_ok_data* data)
{
  gtk_widget_destroy( GTK_WIDGET(data->filew) );
  delete data;
}

//--------------------------------------------------------------------------------

void browse_files(GtkWidget* /*w*/, GtkEntry* file_entry)
{
  GtkFileSelection* filew = GTK_FILE_SELECTION( gtk_file_selection_new ("File selection") );

  file_ok_data* data = new file_ok_data;
  data->filew = filew;
  data->file_entry = file_entry;

  gtk_signal_connect_object (GTK_OBJECT(filew->cancel_button), "clicked",
                             (GtkSignalFunc) cancel_file_browse, (GtkObject*)(data));
  gtk_signal_connect_object (GTK_OBJECT(filew->ok_button), "clicked",
                             (GtkSignalFunc) ok_file_browse, (GtkObject*)(data));

  gtk_file_selection_set_filename (filew, gtk_entry_get_text(file_entry));

  // Hide buttons for create directory, delete and rename file:
  gtk_file_selection_hide_fileop_buttons(filew);

  gtk_window_set_modal(GTK_WINDOW(filew), is_modal);
  gtk_widget_show( GTK_WIDGET(filew) );
}

//-------------------------------------------------------------------------------
//: Handles a change of color.
// Done by passing the value back to the (hidden) text entry in the dialog box.
void color_changed_cb(GtkColorSelection *colorsel, GtkEntry* color_entry)
{
  // Get the color from the color chooser (r,g,b,a):
  gdouble color[4];
  gtk_color_selection_get_color(colorsel, color);

  vul_sprintf color_str("%.3f %.3f %.3f", color[0], color[1], color[2]);

  // Set the text entry back in the dialog to the color selected in the color chooser:
  gtk_entry_set_text(GTK_ENTRY(color_entry), color_str.c_str());
}


struct cancel_color_data
{
  vcl_string* orig_color;
  GtkEntry* color_entry;
  GtkColorSelectionDialog* colord;
};

//-------------------------------------------------------------------------------
//: Handles OK button on color chooser.
// by closing the dialog window and releasing memory.
void ok_color_chooser(cancel_color_data* data)
{
  gtk_widget_destroy(GTK_WIDGET(data->colord));
  delete data->orig_color;
  delete data;
}

//-------------------------------------------------------------------------------
//: Handles cancel button on color chooser.
// by resetting the color to its original value, shutting down the color
// chooser and passing control back to the dialog.
void cancel_color_chooser(cancel_color_data* data)
{
  gtk_entry_set_text(GTK_ENTRY(data->color_entry), data->orig_color->c_str());

  gtk_widget_destroy(GTK_WIDGET(data->colord));
  delete data->orig_color;
  delete data;
}

//-------------------------------------------------------------------------------
//: Display a colour chooser.
void choose_color(GtkWidget* /*w*/, GtkEntry* color_entry)
{
  GtkColorSelectionDialog* colord = GTK_COLOR_SELECTION_DIALOG( gtk_color_selection_dialog_new("Select color") );
  gtk_widget_hide( GTK_WIDGET(colord->help_button) );

  cancel_color_data* data = new cancel_color_data;
  data->orig_color = new vcl_string(gtk_entry_get_text(GTK_ENTRY(color_entry)));
  data->color_entry = color_entry;
  data->colord = colord;

  gtk_signal_connect(GTK_OBJECT(colord->colorsel), "color_changed",
                     (GtkSignalFunc)color_changed_cb, color_entry);
  gtk_signal_connect_object (GTK_OBJECT(colord->cancel_button), "clicked",
                             (GtkSignalFunc)cancel_color_chooser, (GtkObject*)data);
  gtk_signal_connect_object (GTK_OBJECT(colord->ok_button), "clicked",
                             (GtkSignalFunc)ok_color_chooser, (GtkObject*)data);

  gtk_window_set_modal(GTK_WINDOW(colord), is_modal);
  gtk_widget_show(GTK_WIDGET(colord));
}

} // extern "C"

//-------------------------------------------------------------------------------
//: Display the dialog box.
bool vgui_gtk_dialog_impl::ask()
{
  GtkWidget* dialog = gtk_dialog_new();

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
                       &dialog_status_);
    gtk_widget_show(accept);
  }
  if (cancel_text.compare("REMOVEBUTTON"))
  {
    GtkWidget *cancel = gtk_button_new_with_label (cancel_text.c_str());
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
                     cancel, TRUE, TRUE, 0);
    gtk_signal_connect(GTK_OBJECT(cancel), "clicked",
                       GTK_SIGNAL_FUNC(cancel_cb),
                       &dialog_status_);
    gtk_widget_show(cancel);
  }

  // closing the dialog box is equivalent to canceling.
  //
  gtk_signal_connect(GTK_OBJECT(dialog), "delete_event",
                     GTK_SIGNAL_FUNC(close_window_cb),
                     &dialog_status_);

  // list of widgets used to extract values
  vcl_vector<GtkWidget*> wlist;

  // to delete the adaptors associated with inline tableaux
  vcl_vector<vgui_gtk_adaptor*> adaptor_list;

  // to delete the file selection dialog for the inline selection
  vcl_vector<GtkWidget*> delete_wlist;

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
      GtkWidget* label = gtk_label_new(field->label.c_str());
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), label, TRUE, TRUE, 0);
      gtk_widget_show(label);
      wlist.push_back(entry);
    }
    else if (l.type == file_bsr){
      GtkWidget* hbox = gtk_hbox_new(FALSE, 10);
      GtkWidget* label = gtk_label_new(field->label.c_str());
      gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
      GtkWidget* button = gtk_button_new_with_label("Choose file...");
      GtkWidget* entry = gtk_entry_new_with_max_length(150);
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
      // This is a complete hack. To get an "inline" browser, we
      // generate a file browser dialog, but "steal" the main box to
      // display within the current dialog. We cannot embed the file
      // browser in this dialog because the latter is a top level
      // window. However, we must make sure we delete this file
      // browser widget when we close this dialog, so we have to keep
      // an extra list of "auxiliary widgets to delete".
      //
      // This is ugly. A better solution would be most welcome.

      GtkWidget* filew = gtk_file_selection_new ("File selection");
      gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew),
                                       l.field->current_value().c_str());

      // Hide buttons for create directory, delete and rename file:
      gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(filew));
      // Hide Ok and Cancel buttons
      gtk_widget_hide(GTK_FILE_SELECTION(filew)->ok_button);
      gtk_widget_hide(GTK_FILE_SELECTION(filew)->cancel_button);

      GtkWidget* file_main_vbox = GTK_FILE_SELECTION(filew)->main_vbox;

      gtk_widget_ref( file_main_vbox );
      gtk_container_remove( GTK_CONTAINER(filew), file_main_vbox);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), file_main_vbox, TRUE, TRUE, 0);
      gtk_widget_unref( file_main_vbox );

      gtk_widget_show( file_main_vbox );
      wlist.push_back(filew);
      delete_wlist.push_back(filew);
    }
    else if (l.type == color_csr){
      GtkWidget* hbox = gtk_hbox_new(FALSE, 10);
      GtkWidget* label = gtk_label_new(field->label.c_str());
      gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
      GtkWidget* button = gtk_button_new_with_label("Choose color...");
      GtkWidget* entry = gtk_entry_new_with_max_length(50);
      gtk_entry_set_text(GTK_ENTRY(entry), l.field->current_value().c_str());

      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (choose_color), entry);
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
      GtkWidget* colorw = gtk_color_selection_new();
      GtkWidget* color_entry = gtk_entry_new_with_max_length(50);

      gtk_entry_set_text(GTK_ENTRY(color_entry),
                         l.field->current_value().c_str());
      gtk_signal_connect(GTK_OBJECT(colorw), "color_changed",
                         (GtkSignalFunc)color_changed_cb, color_entry);

      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), colorw, TRUE,
                         TRUE, 0);
      gtk_widget_show(colorw);

      // Connect the color entry text field to the dialog so that the
      // widget will be automatically destroyed.
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), color_entry, TRUE,
                         TRUE, 0);
      gtk_widget_hide(color_entry);
      wlist.push_back(color_entry);
    }
    else if (l.type == inline_tabl) {
      vgui_gtk_adaptor* adapt = static_cast<vgui_gtk_adaptor*>(l.widget);
      GtkWidget* widg = adapt->get_glarea_widget();
      GtkWidget* hbox = gtk_hbox_new(FALSE, 10);

      gtk_box_pack_start(GTK_BOX(hbox), widg, TRUE, TRUE, 0);
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, TRUE, 0);
      gtk_widget_show(hbox);
      wlist.push_back(widg);
      adaptor_list.push_back( adapt );
    }
    else
      vcl_cerr << "Unknown type = " << int(l.type) << vcl_endl;
  }

  gtk_widget_show(dialog);

  dialog_status_ = BUSY;

  while ( dialog_status_ == BUSY ) {
    gtk_main_iteration();
  }

  // At this point, the adaptors of the inline tableaux have performed
  // their duties. No further signals should go to this adaptor since
  // the associated glarea should be destroyed by the time this
  // function call ends. That is, by the time further iterations of
  // the gtk main loop occur.
  for ( vcl_vector<vgui_gtk_adaptor*>::iterator iter = adaptor_list.begin();
        iter != adaptor_list.end(); ++iter ) {
    delete *iter;
  }

  //gtk_signal_disconnect(GTK_OBJECT(dialog), destroy_handler_id);

  bool ret_value = false;
  if (dialog_status_ == CLOSE) {
    // the window was closed without pressing any buttons. The windows will be destroyed, etc, so
    // we don't need to worry about anything.
  }
  else if (dialog_status_ == OK)  // OK button has been pressed
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
        l.field->update_value(gtk_file_selection_get_filename(GTK_FILE_SELECTION(*w_iter)));

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

    ret_value = true;
  }
  else // Cancel button has been pressed
  {
    gtk_widget_destroy(dialog);
  }

  // Destroy widgets that weren't inserted into this dialog
  for ( vcl_vector<GtkWidget*>::iterator iter = delete_wlist.begin();
       iter != delete_wlist.end(); ++iter ) {
    gtk_widget_destroy( *iter );
  }

  return ret_value;
}
