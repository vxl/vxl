// This is core/vgui/wx/wxSliderPanel.h
#ifndef wxSliderPanel_h_
#define wxSliderPanel_h_
//:
// \file
// \brief  A wxPanel with a bank of sliders and text fields.
// \author Matt Leotta
// \date   August 6, 2008
//

#include <wx/scrolwin.h>
#include <vgui/vgui_observable.h>
#include <vcl_vector.h>

// forward declarations
class wxSlider;
class wxTextCtrl;

//: A wxPanel with a bank of sliders and text fields
// This is useful for adjusting a point in N-D parameter space
// Each dimension has a min and max (for the slider)
class wxSliderPanel: public wxScrolledWindow, public vgui_observable
{
  DECLARE_DYNAMIC_CLASS( wxSliderPanel )
  DECLARE_EVENT_TABLE()

 public:
  // Constructors
  wxSliderPanel();
  wxSliderPanel(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                wxWindowID base_id = 10100,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxVSCROLL|wxSUNKEN_BORDER|wxTAB_TRAVERSAL,
                const wxString& name = wxT("wxSliderPanel"));

  //: Creation
  bool Create(wxWindow* parent,
              wxWindowID id = wxID_ANY,
              wxWindowID base_id = 10100,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxVSCROLL|wxSUNKEN_BORDER|wxTAB_TRAVERSAL,
              const wxString& name = wxT("wxSliderPanel"));

  // Destructor
  ~wxSliderPanel();

  //: Initialises member variables
  void Init();

  //: Creates the controls and sizers
  void CreateControls();

  //: Create new sliders with these bounds and initial values
  void CreateSliders(const vcl_vector<double>& init_vals,
                     const vcl_vector<double>& min_vals,
                     const vcl_vector<double>& max_vals);

  //: Event handler
  void OnSliderTrack( wxScrollEvent& event );
  //: Event handler
  void OnSliderChange( wxScrollEvent& event );
  //: Event handler
  void OnChangeText( wxCommandEvent& event );
  //: Event handler
  void OnEnterText( wxCommandEvent& event );

  //: Used by event handles to validate and lookup widgets
  int GetWidgets(const wxEvent& event, wxSlider*& slider, wxTextCtrl*& text);

  //: Return the vector of data
  const vcl_vector<double>& data() const { return vals_; }

  //: Update the data
  void update_data(vcl_vector<double>& data,
                   bool send_messages = true);
  //: Update the data
  void update_data(unsigned int i, double val,
                   bool send_messages = true);

  //: Return the number of sliders
  unsigned int size() const { return vals_.size(); }

  //: Convert a slider position to a double value
  double sp_to_val(unsigned int idx, int sp) const;
  //: Convert a double value to a slider position
  int val_to_sp(unsigned int idx, double val) const;

  //: Should we show tooltips?
  static bool ShowToolTips();

  //: These static memory address are used to identify vgui_message types
  static const char update[];
  static const char enter[];

 private:
  wxWindowID base_id_;
  vcl_vector<double> vals_;
  vcl_vector<double> min_vals_;
  vcl_vector<double> max_vals_;

  //: used to disable sending of message
  bool send_messages_;
};


#endif // wxSliderPanel_h_
