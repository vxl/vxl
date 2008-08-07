// This is vgui/wx/wxSliderPanel.cxx

#include "wxSliderPanel.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif



#include <vcl_iostream.h>
#include <vgui/vgui_message.h>


IMPLEMENT_DYNAMIC_CLASS( wxSliderPanel, wxPanel )


//: Event Table 
BEGIN_EVENT_TABLE( wxSliderPanel, wxPanel )
  EVT_SCROLL_THUMBTRACK( wxSliderPanel::OnSliderTrack )
  EVT_TEXT( wxID_ANY, wxSliderPanel::OnChangeText )
END_EVENT_TABLE()


//: Constructor - Default
wxSliderPanel::wxSliderPanel()
: base_id_(10100)
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
  wxPanel::Create(parent, id, pos, size, style, name);
  CreateControls();
  if (GetSizer())
      GetSizer()->Fit(this);
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
  
  vcl_cout << "num sliders = "<<vals_.size()<<vcl_endl;

  for(unsigned int i=0; i<vals_.size(); ++i)
  {
    wxStaticText* itemLabel = new wxStaticText( this, wxID_STATIC,
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
                                              wxDefaultPosition, wxSize(70, -1), 0 );
    itemFlexGridSizer->Add(itemTextCtrl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 3);
    // Numeric text validator
    itemTextCtrl->SetValidator( wxTextValidator(wxFILTER_NUMERIC) );
  }

}

void wxSliderPanel::CreateSliders(const vcl_vector<double>& init_vals,
                                  const vcl_vector<double>& min_vals,
                                  const vcl_vector<double>& max_vals)
{
  assert(init_vals.size() == min_vals.size());
  assert(init_vals.size() == max_vals.size());
  vals_ = init_vals;
  min_vals_ = min_vals;
  max_vals_ = max_vals;
  
  DestroyChildren();
  CreateControls();
  if (GetSizer())
    GetSizer()->Fit(this);
}


bool wxSliderPanel::ShowToolTips()
{
    return true;
}




//: Handle Slider Tracking (dragging)
void wxSliderPanel::OnSliderTrack( wxScrollEvent& event )
{
  int idx = (event.GetId()-base_id_)/2;
  if(idx < 0 || idx >= static_cast<int>(vals_.size()))
    return;
  
  int id = event.GetId() + 1; // id of text box
  wxTextCtrl* text = dynamic_cast<wxTextCtrl*>(FindWindowById(id));
  int spos = event.GetInt();
  double val = sp_to_val(idx,spos);
  text->SetValue(wxString::Format(wxT("%g"),val));
  event.Skip();
}


//: Handle Textbox value change (typing)
void wxSliderPanel::OnChangeText( wxCommandEvent& event )
{
  int idx = (event.GetId()-base_id_)/2;
  if(idx < 0 || idx >= static_cast<int>(vals_.size()))
    return;
  
  int id = event.GetId() - 1; // id of slider
  wxSlider* slider = dynamic_cast<wxSlider*>(FindWindowById(id));
  event.GetString().ToDouble(&vals_[idx]);
  int spos = val_to_sp(idx,vals_[idx]);
  slider->SetValue(spos);
  event.Skip();
  
  vgui_message m;
  m.from = this;
  m.data = &idx;
  notify(m);
}


//: Update the data
void wxSliderPanel::update_data(vcl_vector<double>& data)
{
  for(unsigned int i=0; i<data.size() && i<vals_.size(); ++i)
  {
    update_data(i,data[i]);
  }
}

//: Update a single value
void wxSliderPanel::update_data(unsigned int i, double val)
{
  vals_[i] = val;
  int id = base_id_+2*i+1; // id of text control
  wxTextCtrl* text = dynamic_cast<wxTextCtrl*>(FindWindowById(id));
  text->SetValue(wxString::Format(wxT("%g"),val));
}
