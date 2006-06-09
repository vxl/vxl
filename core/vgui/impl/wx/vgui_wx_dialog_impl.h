// This is core/vgui/impl/wx/vgui_wx_dialog_impl.h
#ifndef vgui_wx_dialog_impl_h_
#define vgui_wx_dialog_impl_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of vgui_dialog_impl.
// \author Miguel A. Figueroa-Villanueva (miguelfv)
//
// \verbatim
//  Modifications
//   04/05/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <vgui/internals/vgui_dialog_impl.h>
class  vgui_wx_adaptor;
class  vgui_dialog_field;
struct vgui_wx_dialog_choice;

#include <vcl_string.h>
#include <vcl_cstddef.h>

class wxDialog;
class wxTextCtrl;
class wxSizer;

//-------------------------------------------------------------------------
//: wxWidgets implementation of vgui_dialog_impl.
//
// Creates a dialog box in wxWidgets and provides methods for it's
// customization.
//-------------------------------------------------------------------------
class vgui_wx_dialog_impl : public vgui_dialog_impl
{
public:
  //: Constructor - create an empty dialog with the given title.
  vgui_wx_dialog_impl(const char* name);

protected:
  //: Destructor.
  virtual ~vgui_wx_dialog_impl();

  //: Display the dialog box form and collect data from the user.
  virtual bool ask();

  //: Create a choice widget.
  virtual void* choice_field_widget(const char* txt,
                                    const vcl_vector<vcl_string>& labels,
                                    int& val);

  //: Create the inline_tableau_widget (OpenGL area).
  virtual void* inline_tableau_widget(const vgui_tableau_sptr tab,
                                      unsigned int width,
                                      unsigned int height);

  //: Set the modality of the dialog box.
  virtual void modal(bool m) { is_modal_ = m; }

private:
  // private helpers
  void build_wx_dialog();
  void destroy_wx_dialog();
  bool has_changed() const;
  int probe_for_max_label_width();

  // set of helpers to setup the different widgets in the dialog
  wxSizer* separator_element(int min_width);
  wxSizer* bool_element     (vgui_dialog_field* field);
  wxSizer* choice_element   (vgui_dialog_field* field,
                             vgui_wx_dialog_choice* choices);
  wxSizer* text_element     (vgui_dialog_field* field);
  wxSizer* text_with_button_element(vgui_dialog_field* field,
                                    wxTextCtrl*& text_control,
                                    const vcl_string& button,
                                    int event_id);
  wxSizer* exit_buttons_element();

  //: Title of the dialog widget.
  vcl_string title_;

  //: Pointer to wxWidgets dialog widget.
  wxDialog* dialog_;

  //: Element count at the last dialog construction (i.e., ask()).
  vcl_size_t last_element_count_;

  //: The width of the largest label; used for graphical alignment.
  int max_label_width_;

  //: True if the dialog box is modal (true by default).
  bool is_modal_;

  vgui_wx_adaptor* adaptor_;
};

#endif // vgui_wx_dialog_impl_h_
