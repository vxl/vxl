// This is core/vgui/wx/wxVideoControl.h
#ifndef wxVideoControl_h_
#define wxVideoControl_h_
//:
// \file
// \brief  A control panel for frame-by-frame video navigation
// \author Matt Leotta
// \date   April 10, 2009
//

#include <vector>
#include <wx/panel.h>
#include <vgui/vgui_observable.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// forward declarations
class wxBitmapButton;
class wxSlider;
class wxTextCtrl;

//: A control panel for frame-by-frame video navigation
class wxVideoControl: public wxPanel, public vgui_observable
{
  DECLARE_DYNAMIC_CLASS( wxVideoControl )
  DECLARE_EVENT_TABLE()

 public:
  // Constructors
  wxVideoControl();
  wxVideoControl(wxWindow* parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxVSCROLL|wxSUNKEN_BORDER|wxTAB_TRAVERSAL,
                 const wxString& name = wxT("wxVideoControl"));

  //: Creation
  bool Create(wxWindow* parent,
              wxWindowID id = wxID_ANY,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxVSCROLL|wxSUNKEN_BORDER|wxTAB_TRAVERSAL,
              const wxString& name = wxT("wxVideoControl"));

  // Destructor
  ~wxVideoControl();

  //: Initialises member variables
  void Init();

  //: Creates the controls and sizers
  void CreateControls();


  //: Event handler
  void OnSliderTrack( wxScrollEvent& event );
  //: Event handler
  void OnSliderChange( wxScrollEvent& event );
  //: Event handler
  void OnEnterText( wxCommandEvent& event );
  //: Event handler
  void OnButton( wxCommandEvent& event );
  //: Event handler
  void OnKeyDown( wxKeyEvent& event );

  //: Handle Textbox loss of focus
  // \note the "this" argument is invalid because this must be called from the wxTextCtrl itself
  void OnKillTextFocus( wxCommandEvent& event );


  //: Return the number of frames
  unsigned int num_frames() const { return num_frames_; }

  //: Return the current frame
  unsigned int frame() const { return frame_; }

  //: Set the number of frames
  void set_num_frames(unsigned int num_frames);

  //: Set the current frame
  void set_frame(unsigned int frame, bool send_message = true);

  //: Advance to next frame
  void next();

  //: Step to previous frame
  void prev();

  //: Start the video playing
  void play();

  //: Pause the video
  void pause();

  //: Return true if in play mode
  bool is_playing() const { return is_playing_; }

  //: Should we show tooltips?
  static bool ShowToolTips();

  //: These static memory address are used to identify vgui_message types
  static const char m_preview[];
  static const char m_seek[];
  static const char m_next[];
  static const char m_prev[];
  static const char m_play[];
  static const char m_pause[];

 private:
  wxBitmapButton* prev_button_;
  wxBitmapButton* play_button_;
  wxBitmapButton* next_button_;
  wxSlider* slider_;
  wxTextCtrl* frame_text_;

  unsigned int frame_;
  unsigned int num_frames_;

  //: used to disable sending of message
  bool send_messages_;

  //: is the video playing
  bool is_playing_;
};


#endif // wxVideoControl_h_
