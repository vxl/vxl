// This is core/vgui/wx/wxSliderPanel.cxx
#include "wxSliderPanel.h"
//:
// \file

//: For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_message.h>


IMPLEMENT_DYNAMIC_CLASS( wxSliderPanel, wxScrolledWindow )


//: Event Table
BEGIN_EVENT_TABLE( wxSliderPanel, wxPanel )
  EVT_SCROLL_THUMBTRACK( wxSliderPanel::OnSliderTrack )
  EVT_SCROLL_PAGEDOWN( wxSliderPanel::OnSliderTrack )
  EVT_SCROLL_PAGEUP( wxSliderPanel::OnSliderTrack )
  EVT_SCROLL_LINEDOWN( wxSliderPanel::OnSliderTrack )
  EVT_SCROLL_LINEUP( wxSliderPanel::OnSliderTrack )
  EVT_SCROLL_CHANGED( wxSliderPanel::OnSliderChange )
  EVT_TEXT( wxID_ANY, wxSliderPanel::OnChangeText )
  EVT_TEXT_ENTER( wxID_ANY, wxSliderPanel::OnEnterText )
END_EVENT_TABLE()


const char wxSliderPanel::update[] = "";
const char wxSliderPanel::enter[] = "";


//: Constructor - Default
wxSliderPanel::wxSliderPanel()
: base_id_(10100),
  send_messages_(true)
{
  Init();
}

//: Constructor
wxSliderPanel::wxSliderPanel(wxWindow* parent,
                             wxWindowID id,
                             wxWindowID base_id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
  : send_messages_(true)
{
  Init();
  Create(parent, id, base_id, pos, size, style, name);
}


//: Creator
bool wxSliderPanel::Create(wxWindow* parent,
                           wxWindowID id,
                           wxWindowID base_id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name)
{
  base_id_ = base_id;
  wxScrolledWindow::Create(parent, id, pos, size, style, name);
  return true;
}


//: Destructor
wxSliderPanel::~wxSliderPanel()
{
}


//: Initialization
void wxSliderPanel::Init()
{
}

//: Convert slider position to a double value
double wxSliderPanel::sp_to_val(unsigned int idx, int sp) const
{
  return (sp/1000.0) * (max_vals_[idx] - min_vals_[idx]) + min_vals_[idx];
}

//: Convert a double value to a slider position
int wxSliderPanel::val_to_sp(unsigned int idx, double val) const
{
  return static_cast<int>((1000.0*(val -min_vals_[idx])
                                 /(max_vals_[idx]-min_vals_[idx])));
}


//: Create the controls
void wxSliderPanel::CreateControls()
{
  wxFlexGridSizer* itemFlexGridSizer = new wxFlexGridSizer(vals_.size(), 3, 0, 0);
  itemFlexGridSizer->AddGrowableCol(1);
  this->SetSizer(itemFlexGridSizer);

  itemFlexGridSizer->Add(0,3,0,wxGROW,0);
  itemFlexGridSizer->Add(0,3,0,wxGROW,0);
  itemFlexGridSizer->Add(0,3,0,wxGROW,0);
  for (unsigned int i=0; i<vals_.size(); ++i)
  {
    wxStaticText* itemLabel = new wxStaticText(this, wxID_STATIC,
                                               wxString::Format(wxT("%u"),i+1),
                                               wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer->Add(itemLabel, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxLEFT, 3);

    int pos = val_to_sp(i,vals_[i]);
    wxSlider* itemSlider = new wxSlider(this, base_id_+2*i, pos, 0, 1000,
                                        wxDefaultPosition, wxSize(100, -1),
                                        wxSL_HORIZONTAL );
    itemFlexGridSizer->Add(itemSlider, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT, 3);

    wxTextCtrl* itemTextCtrl = new wxTextCtrl(this, base_id_+2*i+1,
                                              wxString::Format(wxT("%g"),vals_[i]),
                                              wxDefaultPosition, wxSize(70, -1), wxTE_PROCESS_ENTER );
    itemFlexGridSizer->Add(itemTextCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 3);
    // Numeric text validator
    itemTextCtrl->SetValidator( wxTextValidator(wxFILTER_NUMERIC) );
  }
  itemFlexGridSizer->Add(0,3,0,wxGROW,0);
  itemFlexGridSizer->Add(0,3,0,wxGROW,0);
  itemFlexGridSizer->Add(0,3,0,wxGROW,0);
}

void wxSliderPanel::CreateSliders(const std::vector<double>& init_vals,
                                  const std::vector<double>& min_vals,
                                  const std::vector<double>& max_vals)
{
  assert(init_vals.size() == min_vals.size());
  assert(init_vals.size() == max_vals.size());
  vals_ = init_vals;
  min_vals_ = min_vals;
  max_vals_ = max_vals;


  Freeze();
  DestroyChildren();
  wxScrolledWindow::Create(GetParent(), GetId(), GetPosition(), GetSize(), GetWindowStyleFlag(), GetName());
  CreateControls();
  FitInside();
  SetScrollRate(0,20);
  Thaw();
  Refresh();
}


bool wxSliderPanel::ShowToolTips()
{
  return true;
}


//: Used by event handles to validate and lookup widgets
int wxSliderPanel::GetWidgets(const wxEvent& event,
                              wxSlider*& slider,
                              wxTextCtrl*& text)
{
  slider = NULL;
  text = NULL;
  int idx = (event.GetId()-base_id_)/2;
  if (idx < 0 || idx >= static_cast<int>(vals_.size()))
    return -1;

  text = dynamic_cast<wxTextCtrl*>(FindWindowById(base_id_+2*idx+1));
  slider = dynamic_cast<wxSlider*>(FindWindowById(base_id_+2*idx));
  return idx;
}


//: Handle Slider Tracking (dragging)
void wxSliderPanel::OnSliderTrack( wxScrollEvent& event )
{
  wxSlider* slider;
  wxTextCtrl* text;
  int idx = GetWidgets(event, slider, text);
  if (!slider || !text){
    event.Skip();
    return;
  }

  int spos = event.GetInt();
  double val = sp_to_val(idx,spos);
  text->SetValue(wxString::Format(wxT("%g"),val));
}

//: Handle Slider Release (stop dragging)
void wxSliderPanel::OnSliderChange( wxScrollEvent& event )
{
  wxSlider* slider;
  wxTextCtrl* text;
  int idx = GetWidgets(event, slider, text);
  if (!slider || !text){
    event.Skip();
    return;
  }

  if (send_messages_)
  {
    vgui_message m;
    m.from = this;
    m.user = wxSliderPanel::enter;
    m.data = &idx;
    notify(m);
  }
}


//: Handle Textbox value change (typing)
void wxSliderPanel::OnChangeText( wxCommandEvent& event )
{
  wxSlider* slider;
  wxTextCtrl* text;
  int idx = GetWidgets(event, slider, text);
  if (!slider || !text){
    event.Skip();
    return;
  }

  event.GetString().ToDouble(&vals_[idx]);
  int spos = val_to_sp(idx,vals_[idx]);
  slider->SetValue(spos);
  event.Skip();

  if (send_messages_)
  {
    vgui_message m;
    m.from = this;
    m.user = wxSliderPanel::update;
    m.data = &idx;
    notify(m);
  }
}


//: Handle Textbox press of Enter key
void wxSliderPanel::OnEnterText( wxCommandEvent& event )
{
  wxSlider* slider;
  wxTextCtrl* text;
  int idx = GetWidgets(event, slider, text);
  if (!slider || !text){
    event.Skip();
    return;
  }

  if (send_messages_)
  {
    vgui_message m;
    m.from = this;
    m.user = wxSliderPanel::enter;
    m.data = &idx;
    notify(m);
  }
}


//: Update the data
void wxSliderPanel::update_data(std::vector<double>& data,
                                bool send_messages)
{
  for (unsigned int i=0; i<data.size() && i<vals_.size(); ++i)
  {
    update_data(i,data[i],send_messages);
  }
}

//: Update a single value
void wxSliderPanel::update_data(unsigned int i, double val,
                                bool send_messages)
{
  vals_[i] = val;
  int id = base_id_+2*i+1; // id of text control
  wxTextCtrl* text = dynamic_cast<wxTextCtrl*>(FindWindowById(id));

  // optionally disable sending messages about this update
  send_messages_ = send_messages;
  text->SetValue(wxString::Format(wxT("%g"),val));
  send_messages_ = true;
}
