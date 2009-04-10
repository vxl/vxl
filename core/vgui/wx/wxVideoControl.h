// This is core/vgui/wx/wxVideoControl.h
#ifndef wxVideoControl_h_
#define wxVideoControl_h_
//:
// \file
// \brief  A control panel for frame-by-frame video navigation
// \author Matt Leotta
// \date   April 10, 2009
//

#include <wx/panel.h>
#include <vgui/vgui_observable.h>
#include <vcl_vector.h>

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
  void set_frame(unsigned int frame);

  //: Should we show tooltips?
  static bool ShowToolTips();

  //: These static memory address are used to identify vgui_message types
  static const char preview[];
  static const char seek[];

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
};


#endif // wxVideoControl_h_
