// This is core/vgui/wx/wxVideoControl.cxx
#include <iostream>
#include "wxVideoControl.h"
//:
// \file

//: For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#include <wx/utils.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <cassert>
#include <vgui/vgui_message.h>
#include "bitmaps/prev.xpm"
#include "bitmaps/play.xpm"
#include "bitmaps/pause.xpm"
#include "bitmaps/next.xpm"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


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
  EVT_BUTTON( wxID_ANY, wxVideoControl::OnButton )
END_EVENT_TABLE()


const char wxVideoControl::m_preview[] = "";
const char wxVideoControl::m_seek[] = "";
const char wxVideoControl::m_next[] = "";
const char wxVideoControl::m_prev[] = "";
const char wxVideoControl::m_play[] = "";
const char wxVideoControl::m_pause[] = "";


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

  prev_button_ = new wxBitmapButton(this, wxNewId(), wxBitmap(prev_xpm, wxBITMAP_TYPE_XPM));
  itemBoxSizer->Add(prev_button_, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 3);

  play_button_ = new wxBitmapButton(this, wxNewId(), wxBitmap(play_xpm, wxBITMAP_TYPE_XPM));
  itemBoxSizer->Add(play_button_, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 3);

  next_button_ = new wxBitmapButton(this, wxNewId(), wxBitmap(next_xpm, wxBITMAP_TYPE_XPM));
  itemBoxSizer->Add(next_button_, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 3);

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
  while (num_frames/=10)
    ++digits;
  frame_text_->SetMaxLength(digits);

  itemBoxSizer->Add(frame_text_, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 3);
  // Numeric text validator
  frame_text_->SetValidator( wxTextValidator(wxFILTER_NUMERIC) );

  frame_text_->Connect(-1, wxEVT_KILL_FOCUS,
                       (wxObjectEventFunction)&wxVideoControl::OnKillTextFocus);


  slider_->Connect(-1, wxEVT_KEY_DOWN,
                   (wxObjectEventFunction)&wxVideoControl::OnKeyDown);
}


//: Set the number of frames
void wxVideoControl::set_num_frames(unsigned int num_frames)
{
  num_frames_ = num_frames;
  // count the number of digits in the maximum frame number
  int digits = 1;
  --num_frames;
  while (num_frames/=10)
    ++digits;
  if (slider_)
    slider_->SetRange(0,num_frames_-1);
  if (frame_text_)
    frame_text_->SetMaxLength(digits);
}


//: Set the current frame
void wxVideoControl::set_frame(unsigned int frame, bool send_message)
{
  frame_ = frame;
  if (slider_ && frame_text_) {
    send_messages_ = send_message;
    slider_->SetValue(frame_);
    frame_text_->SetValue(wxString::Format(wxT("%d"),frame_));
    send_messages_ = true;
  }
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
    m.user = wxVideoControl::m_preview;
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
    m.user = wxVideoControl::m_seek;
    m.data = &frame_;
    notify(m);
  }
}


//: Handle Textbox press of Enter key
void wxVideoControl::OnEnterText( wxCommandEvent& event )
{
  long fnum;
  event.GetString().ToLong(&fnum);
  if (fnum < 0)
    fnum = 0;
  if (fnum >= (long)num_frames_)
    fnum = num_frames_-1L;
  frame_text_->SetValue(wxString::Format(wxT("%d"),fnum));
  frame_ = fnum;
  slider_->SetValue(frame_);
  if (send_messages_)
  {
    vgui_message m;
    m.from = this;
    m.user = wxVideoControl::m_seek;
    m.data = &frame_;
    notify(m);
  }
}


//: Event handler
void wxVideoControl::OnButton( wxCommandEvent& event )
{
  int id = event.GetId();
  if (id == next_button_->GetId())
    next();
  else if (id == play_button_->GetId()) {
    if (is_playing_)
      pause();
    else
      play();
  }
  else if (id == prev_button_->GetId())
    prev();
}


//: Event handler
void wxVideoControl::OnKeyDown( wxKeyEvent& event )
{
  std::cout << "key press!"<<std::endl;
}

//: Advance to next frame
void wxVideoControl::next()
{
  if (frame_ >= num_frames_)
    return;
  ++frame_;
  frame_text_->SetValue(wxString::Format(wxT("%d"),frame_));
  slider_->SetValue(frame_);
  if (send_messages_)
  {
    vgui_message m;
    m.from = this;
    m.user = wxVideoControl::m_next;
    m.data = &frame_;
    notify(m);
  }
}

//: Step to previous frame
void wxVideoControl::prev()
{
  if (frame_ <= 0)
    return;
  --frame_;
  frame_text_->SetValue(wxString::Format(wxT("%d"),frame_));
  slider_->SetValue(frame_);
  if (send_messages_)
  {
    vgui_message m;
    m.from = this;
    m.user = wxVideoControl::m_prev;
    m.data = &frame_;
    notify(m);
  }
}


//: Start the video playing
void wxVideoControl::play()
{
  play_button_->SetBitmapLabel(wxBitmap(pause_xpm, wxBITMAP_TYPE_XPM));
  is_playing_ = true;
  prev_button_->Disable();
  next_button_->Disable();
  frame_text_->Disable();
  slider_->Disable();
  if (send_messages_)
  {
    vgui_message m;
    m.from = this;
    m.user = wxVideoControl::m_play;
    m.data = &frame_;
    notify(m);
  }
}

//: Pause the video
void wxVideoControl::pause()
{
  play_button_->SetBitmapLabel(wxBitmap(play_xpm, wxBITMAP_TYPE_XPM));
  is_playing_ = false;
  prev_button_->Enable();
  next_button_->Enable();
  frame_text_->Enable();
  slider_->Enable();
  if (send_messages_)
  {
    vgui_message m;
    m.from = this;
    m.user = wxVideoControl::m_pause;
    m.data = &frame_;
    notify(m);
  }
}


//: Handle Textbox loss of focus
// \note the "this" argument is invalid because this must be called from the wxTextCtrl itself
void wxVideoControl::OnKillTextFocus( wxCommandEvent& event )
{
  wxTextCtrl* text = dynamic_cast<wxTextCtrl*>(FindWindowById(event.GetId()));
  wxVideoControl* vc = dynamic_cast<wxVideoControl*>(text->GetParent());
  text->SetValue(wxString::Format(wxT("%d"),vc->frame_));
}
