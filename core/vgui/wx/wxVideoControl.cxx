// This is core/vgui/wx/wxVideoControl.cxx
#include "wxVideoControl.h"
//:
// \file

//: For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#include <wx/utils.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vcl_cassert.h>
#include <vgui/vgui_message.h>
#include "bitmaps/play.xpm"

#include <vcl_iostream.h>


IMPLEMENT_DYNAMIC_CLASS( wxVideoControl, wxPanel )


//: Event Table
BEGIN_EVENT_TABLE( wxVideoControl, wxPanel )
  EVT_SCROLL_THUMBTRACK( wxVideoControl::OnSliderTrack )
  EVT_SCROLL_PAGEDOWN( wxVideoControl::OnSliderTrack )
  EVT_SCROLL_PAGEUP( wxVideoControl::OnSliderTrack )
  EVT_SCROLL_LINEDOWN( wxVideoControl::OnSliderTrack )
  EVT_SCROLL_LINEUP( wxVideoControl::OnSliderTrack )
  EVT_SCROLL_THUMBRELEASE( wxVideoControl::OnSliderChange )
  EVT_SCROLL_CHANGED( wxVideoControl::OnSliderChange )
  EVT_TEXT_ENTER( wxID_ANY, wxVideoControl::OnEnterText )
END_EVENT_TABLE()


const char wxVideoControl::preview[] = "";
const char wxVideoControl::seek[] = "";


//: Constructor - Default
wxVideoControl::wxVideoControl()
: send_messages_(true)
{
  Init();
}

//: Constructor
wxVideoControl::wxVideoControl(wxWindow* parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxString& name)
  : send_messages_(true)
{
  Init();
  Create(parent, id, pos, size, style, name);
}


//: Creator
bool wxVideoControl::Create(wxWindow* parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name)
{
  wxPanel::Create(parent, id, pos, size, style, name);
  CreateControls();
  return true;
}


//: Destructor
wxVideoControl::~wxVideoControl()
{
}


//: Initialization
void wxVideoControl::Init()
{
}



//: Create the controls
void wxVideoControl::CreateControls()
{
  wxBoxSizer* itemBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  this->SetSizer(itemBoxSizer);
  
  prev_button_ = new wxBitmapButton(this, wxNewId(), wxBitmap(play_xpm, wxBITMAP_TYPE_XPM));
  itemBoxSizer->Add(prev_button_, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 3);
  
  play_button_ = new wxBitmapButton(this, wxNewId(), wxBitmap(play_xpm, wxBITMAP_TYPE_XPM));
  itemBoxSizer->Add(play_button_, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 3);
  
  next_button_ = new wxBitmapButton(this, wxNewId(), wxBitmap(play_xpm, wxBITMAP_TYPE_XPM));
  itemBoxSizer->Add(next_button_, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 3);
  
  slider_ = new wxSlider(this, wxNewId(), 0, 0, num_frames_, 
                         wxDefaultPosition, wxSize(100, -1),
                         wxSL_HORIZONTAL );
  itemBoxSizer->Add(slider_, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 3);
  
  frame_text_ = new wxTextCtrl(this, wxNewId(),
                               wxString::Format(wxT("%d"),frame_),
                               wxDefaultPosition, wxSize(50, -1), wxTE_PROCESS_ENTER );
  // count the number of digits in the maximum frame number
  int digits = 1;
  unsigned int num_frames = num_frames_-1;
  while(num_frames/=10) 
    ++digits;
  frame_text_->SetMaxLength(digits);
  
  itemBoxSizer->Add(frame_text_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 3);
  // Numeric text validator
  frame_text_->SetValidator( wxTextValidator(wxFILTER_NUMERIC) );
  
  frame_text_->Connect(-1, wxEVT_KILL_FOCUS, 
                       (wxObjectEventFunction)&wxVideoControl::OnKillTextFocus);
  
  
  slider_->Connect(-1, wxEVT_KEY_DOWN, 
                       (wxObjectEventFunction)&wxVideoControl::OnKeyDown);
  
  //EVT_KEY_DOWN(wxVideoControl::OnKeyDown)

}


//: Set the number of frames
void wxVideoControl::set_num_frames(unsigned int num_frames)
{
  num_frames_ = num_frames;
  // count the number of digits in the maximum frame number
  int digits = 1;
  --num_frames;
  while(num_frames/=10) 
    ++digits;
  if(slider_)
    slider_->SetRange(0,num_frames_-1);
  if(frame_text_)
    frame_text_->SetMaxLength(digits);
}


//: Set the current frame
void wxVideoControl::set_frame(unsigned int frame)
{
  frame_ = frame;
  if(slider_)
    slider_->SetValue(frame_);
}



bool wxVideoControl::ShowToolTips()
{
  return true;
}



//: Handle Slider Tracking (dragging)
void wxVideoControl::OnSliderTrack( wxScrollEvent& event )
{
  int spos = event.GetInt();
  frame_text_->SetValue(wxString::Format(wxT("%d"),spos));
  
  if (send_messages_)
  {
    vgui_message m;
    m.from = this;
    m.user = wxVideoControl::preview;
    m.data = &spos;
    notify(m);
  }
}

//: Handle Slider Release (stop dragging)
void wxVideoControl::OnSliderChange( wxScrollEvent& event )
{
  frame_ = slider_->GetValue();
  if (send_messages_)
  {
    frame_ = event.GetInt();
    vgui_message m;
    m.from = this;
    m.user = wxVideoControl::seek;
    m.data = &frame_;
    notify(m);
  }
}


//: Handle Textbox press of Enter key
void wxVideoControl::OnEnterText( wxCommandEvent& event )
{
  long fnum;
  event.GetString().ToLong(&fnum);
  if(fnum < 0)
    fnum = 0;
  if(fnum >= num_frames_)
    fnum = num_frames_-1;
  frame_text_->SetValue(wxString::Format(wxT("%d"),fnum));
  frame_ = fnum;
  slider_->SetValue(frame_);
  if (send_messages_)
  {
    vgui_message m;
    m.from = this;
    m.user = wxVideoControl::seek;
    m.data = &frame_;
    notify(m);
  }
}


//: Event handler
void wxVideoControl::OnKeyDown( wxKeyEvent& event )
{
  vcl_cout << "key press!"<<vcl_endl;
}


//: Handle Textbox loss of focus
// \note the "this" argument is invalid because this must be called from the wxTextCtrl itself
void wxVideoControl::OnKillTextFocus( wxCommandEvent& event )
{
  wxTextCtrl* text = dynamic_cast<wxTextCtrl*>(FindWindowById(event.GetId()));
  wxVideoControl* vc = dynamic_cast<wxVideoControl*>(text->GetParent());
  text->SetValue(wxString::Format(wxT("%d"),vc->frame_));
}




