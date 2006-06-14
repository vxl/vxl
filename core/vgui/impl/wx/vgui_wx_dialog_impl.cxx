// This is core/vgui/impl/wx/vgui_wx_dialog_impl.cxx
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of vgui_dialog_impl.
//
// See vgui_wx_dialog_impl.h for details.
//=========================================================================

#include "vgui_wx_dialog_impl.h"
#include "vgui_wx_adaptor.h"

#include <vgui/vgui_color_text.h>
#include <vgui/internals/vgui_dialog_field.h>
#include <vgui/internals/vgui_simple_field.h>

#include <wx/log.h>
#include <wx/filename.h>

#include <wx/dialog.h>
#include <wx/filedlg.h>
#include <wx/colordlg.h>

#include <wx/sizer.h>
#include <wx/valgen.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/arrstr.h>
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/button.h>

#ifndef wxCommandEventHandler        // wxWidgets-2.5.3 doesn't define this
#define wxCommandEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxCommandEventFunction, &func)
#endif

#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vcl_map.h>
#include <vcl_utility.h>

//-------------------------------------------------------------------------
// Private helpers - declarations.
//-------------------------------------------------------------------------
namespace
{
  class vgui_wx_event_handler : public wxEvtHandler
  {
  public:
    void insert_handle(int id, wxTextCtrl* control)
    {
      handles_.insert(vcl_pair<int,wxTextCtrl*>(id, control));
    }

    void file_browser(wxCommandEvent& e)
    {
      // ***** should add wildcard value,
      //       but need to pass it through from element->widget
      wxFileName filename(handles_[e.GetId()]->GetValue());
      wxFileDialog file_dialog(handles_[e.GetId()],
                               "Choose a file",
                               filename.GetPath(),
                               filename.GetFullName());

      if (file_dialog.ShowModal() == wxID_OK)
      {
        // update wxTextCtrl value
        handles_[e.GetId()]->SetValue(file_dialog.GetPath());
      }
    }

    void color_chooser(wxCommandEvent& e)
    {
      wxString text = handles_[e.GetId()]->GetValue();

      wxColour color;
      color.Set(red_value(text.c_str())   * 255.0,
                green_value(text.c_str()) * 255.0,
                blue_value(text.c_str())  * 255.0);
      wxColourData cdata;
      cdata.SetColour(color);

      wxColourDialog color_dialog(handles_[e.GetId()], &cdata);

      if (color_dialog.ShowModal() == wxID_OK)
      {
        color = color_dialog.GetColourData().GetColour();

        text.clear();
        text << color.Red()   / 255.0 << ' '
             << color.Green() / 255.0 << ' '
             << color.Blue()  / 255.0;

        // update wxTextCtrl value
        handles_[e.GetId()]->SetValue(text);
      }
    }

  private:
    vcl_map<int,wxTextCtrl*> handles_;
  };

  //: Used to transfer data between a vgui_dialog_field and wxTextControl.
  class vgui_wx_text_validator : public wxValidator
  {
  public:
    //: Constructor - default.
    vgui_wx_text_validator(vgui_dialog_field* field = 0)
      : field_(field)
    {
    }

    //: Constructor.
    vgui_wx_text_validator(const vgui_wx_text_validator& validator)
      : field_(validator.field_)
    {
      wxValidator::Copy(validator);
    }

    //: Destructor.
    virtual ~vgui_wx_text_validator() {}

    //: Make a clone of this validator.
    virtual wxObject* Clone() const
    {
      return new vgui_wx_text_validator(*this);
    }

    //: Called to transfer data to the window.
    virtual bool TransferToWindow()
    {
      if(!check_validator()) { return false; }
      if (field_)
      {
        dynamic_cast<wxTextCtrl*>(
          m_validatorWindow)->SetValue(field_->current_value().c_str());
      }
      return true;
    }

    //: Called to transfer data from the window.
    virtual bool TransferFromWindow()
    {
      if(!check_validator()) { return false; }
      if (field_)
      {
        field_->update_value(
          dynamic_cast<wxTextCtrl*>(m_validatorWindow)->GetValue().c_str());
      }
      return true;
    }

    //: Called when validation of the control data must be validated.
    virtual bool Validate(wxWindow* parent)
    {
      if(!check_validator()) { return false; }
      return true;
    }

  private:
    //: Check if validator parent window is valid.
    bool check_validator() const
    {
      wxCHECK_MSG(m_validatorWindow,
                  false,
                  wxT("No window associated with validator"));

      wxCHECK_MSG(m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl)),
                  false,
                  wxT("wxTextValidator is only for wxTextCtrl's") );

      return true;
    }

    //: vgui dialog field used to store the data.
    vgui_dialog_field* field_;
  };
} // unnamed namespace

//-------------------------------------------------------------------------
// vgui_wx_dialog_impl implementation - construction & destruction.
//-------------------------------------------------------------------------
//: Constructor - create an empty dialog with the given title.
vgui_wx_dialog_impl::vgui_wx_dialog_impl(const char* name)
  : vgui_dialog_impl(name)
  , title_(name)
  , dialog_(0)
  , last_element_count_(0)
  , max_label_width_(0)
  , is_modal_(true)
  , adaptor_(0)
{
  wxLogTrace(wxTRACE_RefCount, "vgui_wx_dialog_impl::vgui_wx_dialog_impl");
}

//: Destructor.
vgui_wx_dialog_impl::~vgui_wx_dialog_impl(void)
{
  wxLogTrace(wxTRACE_RefCount, "vgui_wx_dialog_impl::~vgui_wx_dialog_impl");
  destroy_wx_dialog();
}

//-------------------------------------------------------------------------
// vgui_wx_dialog_impl implementation.
//-------------------------------------------------------------------------
//: Display the dialog box form and collect data from the user.
//
// In modal mode, it returns true if ok was pressed and false otherwise.
// In modeless mode, it returns false if the dialog was already displayed
// and true if it wasn't.
//
// The vgui_dialog API doesn't really support modeless dialogs for data
// collection nor data monitoring. For example, for data monitoring we
// would need some sort of update() function to allow the user to
// periodically refresh the dialog values.
bool vgui_wx_dialog_impl::ask(void)
{
  // rebuild the dialog, if the elements have changed
  if (has_changed()) { build_wx_dialog(); }

  // show the dialog
  if (is_modal_)
  {
    return dialog_->ShowModal() == wxID_OK ? true : false;
  }
  else
  {
    return dialog_->Show();
  }
}

struct vgui_wx_dialog_choice
{
  vcl_vector<vcl_string> names;
  int index;
};

//: Create a choice widget.
void* vgui_wx_dialog_impl::
choice_field_widget(const char* WXUNUSED(txt),
                    const vcl_vector<vcl_string>& labels,
                    int& val)
{
  vgui_wx_dialog_choice* choice_data = new vgui_wx_dialog_choice;
  choice_data->names = labels;
  choice_data->index = val;
  return static_cast<void*>(choice_data);
}

struct vgui_wx_dialog_inline_tab
{
  vgui_tableau_sptr  tab;
  unsigned int       height;
  unsigned int       width;
};

//: Create the inline_tableau_widget (OpenGL area).
void*
vgui_wx_dialog_impl::inline_tableau_widget(const vgui_tableau_sptr tab,
                                           unsigned width,
                                           unsigned height)
{
  vgui_wx_dialog_inline_tab* tab_data = new vgui_wx_dialog_inline_tab;
  tab_data->tab    = tab;
  tab_data->height = height;
  tab_data->width  = width;
  return static_cast<void*>(tab_data);
}

//-------------------------------------------------------------------------
// vgui_wx_dialog_impl implementation - private helpers.
//-------------------------------------------------------------------------
void vgui_wx_dialog_impl::build_wx_dialog(void)
{
  // clean any previous construction
  destroy_wx_dialog();
  
  dialog_ = new wxDialog(0,
                         wxID_ANY,
                         wxString(title_.c_str()),
                         wxDefaultPosition,
                         wxDefaultSize,
                         wxDEFAULT_DIALOG_STYLE);

  // handler for dynamic connection
  vgui_wx_event_handler* handler = new vgui_wx_event_handler;
  int id = wxID_HIGHEST;

  // probe for column sizes
  max_label_width_ = probe_for_max_label_width();
  const int min_dialog_width = max_label_width_ + 200;

  // for outer separation border
  wxSizer* top_sizer = new wxBoxSizer(wxHORIZONTAL);

  // sizer that will hold all elements
  wxSizer* holder = new wxFlexGridSizer(1);
  holder->SetMinSize(max_label_width_+200, 0);
  top_sizer->Add(holder, 0, wxGROW | wxALL, 3);

  // flags to use on each element
  wxSizerFlags flags(wxSizerFlags(0).Expand().Border(wxALL, 2));

  // process each element
  for (vcl_vector<element>::const_iterator e = elements.begin();
       e != elements.end(); ++e)
  {
    switch (e->type)
    {
    case bool_elem:
      holder->Add(bool_element(e->field), flags);
      break;

    case int_elem   :
    case long_elem  :
    case float_elem :
    case double_elem:
    case string_elem:
      holder->Add(text_element(e->field), flags);
      break;

    case choice_elem:
      holder->Add(
        choice_element(e->field,
                       static_cast<vgui_wx_dialog_choice*>(e->widget)),
        flags);
      break;

    case text_msg:
      //holder->Add(separator_element(min_dialog_width), flags);
      holder->Add(new wxStaticText(dialog_,
                                   wxID_STATIC,
                                   e->field->label.c_str()),
                  flags);
      //holder->Add(separator_element(min_dialog_width), flags);
      break;

    case file_bsr:
    case inline_file_bsr: // ***** this should be different...
      { // create variable scope for txt_ctrl
      wxTextCtrl* txt_ctrl;
      holder->Add(
        text_with_button_element(e->field, txt_ctrl, "Browse...", ++id),
        flags);

      // dynamically connect a handler that launches a wxFileSelector
      handler->insert_handle(id, txt_ctrl);
      handler->Connect(id,
                       wxEVT_COMMAND_BUTTON_CLICKED,
                       wxCommandEventHandler(vgui_wx_event_handler::file_browser),
                       0,
                       handler);
      }
      break;

    case color_csr:
    case inline_color_csr: // ***** this should be different...
      { // create variable scope for txt_ctrl
      wxTextCtrl* txt_ctrl;
      holder->Add(
        text_with_button_element(e->field, txt_ctrl, "Color...", ++id),
        flags);

      // dynamically connect a handler that launches a wxFileSelector
      handler->insert_handle(id, txt_ctrl);
      handler->Connect(id,
                       wxEVT_COMMAND_BUTTON_CLICKED,
                       wxCommandEventHandler(vgui_wx_event_handler::color_chooser),
                       0,
                       handler);
      }
      break;

    case inline_tabl:
      { // create variable scope for adaptor
      // ***** error if more than one inline tableau in this dialog
      vgui_wx_adaptor* adaptor = new vgui_wx_adaptor(
        dialog_,
        wxID_ANY,
        wxDefaultPosition,
        wxSize(static_cast<vgui_wx_dialog_inline_tab*>(e->widget)->width,
               static_cast<vgui_wx_dialog_inline_tab*>(e->widget)->height),
        wxBORDER_SUNKEN);
      adaptor->set_tableau(static_cast<vgui_wx_dialog_inline_tab*>(e->widget)->tab);
      //adaptor->SetSize(static_cast<vgui_wx_dialog_inline_tab*>(e->widget)->width,
      //                 static_cast<vgui_wx_dialog_inline_tab*>(e->widget)->height);

      //wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
      //box_sizer->Add(adaptor);
      //holder->Add(box_sizer, flags);
      holder->Add(adaptor, flags);

      //adaptor->post_redraw();
      adaptor_ = adaptor;
      }
      break;

    default:
      vcl_cerr << "Unknown type = " << e->type << vcl_endl;
    }
  }

  // separator line and enforcer of the dialog's minimum width
  holder->Add(separator_element(min_dialog_width), flags);

  // add ok/cancel buttons
  holder->Add(exit_buttons_element(), 0, wxALIGN_RIGHT | wxALL, 2);

  // resize to contents
  // ***** fix for case when larger than screen, make scrollable maybe?
  dialog_->SetSize(dialog_->GetBestFittingSize());
  dialog_->PushEventHandler(handler);

  dialog_->SetSizer(top_sizer);
  top_sizer->SetSizeHints(dialog_);

  // save the element count to determine change
  last_element_count_ = elements.size();
}

void vgui_wx_dialog_impl::destroy_wx_dialog(void)
{
  if (dialog_)
  {
    dialog_->PopEventHandler(true);
    if (adaptor_)
    {
      adaptor_->post_destroy();
      //delete adaptor_;
      adaptor_ = 0;
    }
    dialog_->Destroy();
    dialog_ = 0;
  }
}

//: Determine if dialog has changed since last construction (i.e., ask()).
//
// Note that we assume that the only change that can occur is that more
// fields are added.
bool vgui_wx_dialog_impl::has_changed(void) const
{
  return last_element_count_ != elements.size();
}

int vgui_wx_dialog_impl::probe_for_max_label_width(void)
{
  wxStaticText temp(dialog_, wxID_ANY, wxString(""));
  int max_width = temp.GetSize().GetX();
  for (vcl_vector<element>::const_iterator e = elements.begin();
       e != elements.end(); ++e)
  {
    switch (e->type)
    {
    case int_elem:
    case long_elem:
    case float_elem:
    case double_elem:
    case string_elem:
    case choice_elem:
      {
        temp.SetLabel(e->field->label.c_str());
        max_width = vcl_max(max_width, temp.GetSize().GetX());
      }
      break;
    }
  }
  temp.Show(false);
  return max_width;
}

wxSizer* vgui_wx_dialog_impl::separator_element(int min_width)
{
  wxSizer* cell = new wxBoxSizer(wxHORIZONTAL);
  cell->Add(new wxStaticLine(dialog_,
                             wxID_ANY,
                             wxDefaultPosition,
                             wxSize(min_width, -1)), 1);
  return cell;
}

wxSizer* vgui_wx_dialog_impl::bool_element(vgui_dialog_field* field)
{
  assert(field);

  wxSizer* cell = new wxBoxSizer(wxHORIZONTAL);
  bool* var = &dynamic_cast<vgui_bool_field*>(field)->var;
  cell->Add(new wxCheckBox(dialog_,
                           wxID_ANY,
                           field->label.c_str(),
                           wxDefaultPosition,
                           wxDefaultSize,
                           wxCHK_2STATE,
                           wxGenericValidator(var)),
            0, wxALIGN_CENTER_VERTICAL);
  return cell;
}

wxSizer* vgui_wx_dialog_impl::choice_element(vgui_dialog_field* field,
                                             vgui_wx_dialog_choice* choices)
{
  assert(field);
  assert(choices);

  wxSizer* cell = new wxBoxSizer(wxHORIZONTAL);
  wxStaticText* st = new wxStaticText(dialog_,
                                      wxID_STATIC,
                                      field->label.c_str(),
                                      wxDefaultPosition,
                                      wxSize(max_label_width_, -1),
                                      wxALIGN_RIGHT);
  cell->Add(st, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);

  wxArrayString choice_labels;
  for (vcl_vector<vcl_string>::const_iterator label = choices->names.begin();
       label != choices->names.end(); ++label)
  {
    choice_labels.Add(label->c_str());
  }

  int* var = &dynamic_cast<vgui_int_field*>(field)->var;
  wxChoice* wx_choice = new wxChoice(dialog_,
                                     wxID_ANY,
                                     wxDefaultPosition,
                                     wxDefaultSize,
                                     choice_labels,
                                     0,
                                     wxGenericValidator(var));
  wx_choice->SetSelection(*var);
  cell->Add(wx_choice, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);

  return cell;
}

wxSizer* vgui_wx_dialog_impl::text_element(vgui_dialog_field* field)
{
  assert(field);

  wxSizer* cell = new wxBoxSizer(wxHORIZONTAL);

  wxStaticText* st = new wxStaticText(dialog_,
                                      wxID_STATIC,
                                      field->label.c_str(),
                                      wxDefaultPosition,
                                      wxSize(max_label_width_, -1),
                                      wxALIGN_RIGHT);
  cell->Add(st, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);

  wxTextCtrl* t_control;
  t_control = new wxTextCtrl(dialog_,
                             wxID_ANY,
                             field->current_value().c_str(),
                             wxDefaultPosition,
                             wxDefaultSize,
                             0,
                             vgui_wx_text_validator(field));
  cell->Add(t_control, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);

  return cell;
}

wxSizer*
vgui_wx_dialog_impl::text_with_button_element(vgui_dialog_field* field,
                                              wxTextCtrl*& text_control,
                                              const vcl_string& button,
                                              int event_id)
{
  assert(field);

  text_control = new wxTextCtrl(dialog_,
                                wxID_ANY,
                                field->current_value().c_str(),
                                wxDefaultPosition,
                                wxDefaultSize,
                                0,
                                vgui_wx_text_validator(field));

  // ***** this constructor not available in wxWidgets-2.5.3
  //wxSizer* box = new wxStaticBoxSizer(wxVERTICAL, dialog_, field->label.c_str());
  wxSizer* box = new wxStaticBoxSizer(
    new wxStaticBox(dialog_, wxID_ANY, field->label.c_str()), wxVERTICAL);
  box->Add(text_control, 0, wxGROW | wxALL, 2);
  box->Add(new wxButton(dialog_, event_id, button.c_str()),
           0, wxALIGN_RIGHT | wxALL, 2);

  return box;
}

wxSizer* vgui_wx_dialog_impl::exit_buttons_element(void)
{
  wxSizer* button_row = new wxBoxSizer(wxHORIZONTAL);
  if (ok_button_text_ != "")
  {
    button_row->Add(
      new wxButton(dialog_, wxID_OK, ok_button_text_.c_str()));
  }
  if (cancel_button_text_ != "")
  {
    button_row->Add(
      new wxButton(dialog_, wxID_CANCEL, cancel_button_text_.c_str()));
  }
  return button_row;
}

//-------------------------------------------------------------------------
// Private helpers - definitions.
// - Left definition above. Getting error in VS8. (miguelfv)
//-------------------------------------------------------------------------
