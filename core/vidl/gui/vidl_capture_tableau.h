// This is core/vidl/gui/vidl_capture_tableau.h
#ifndef vidl_capture_tableau_h_
#define vidl_capture_tableau_h_
//:
// \file
// \brief  Tableau for capturing OpenGL rendering to a video
// \author Matt Leotta
// \date   21 Nov 08
//
//  Contains classes  vidl_capture_tableau   vidl_capture_tableau_new


#include <vidl/gui/vidl_capture_tableau_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vidl/vidl_ostream_sptr.h>

//: Tableau for capturing OpenGL rendering to a video
//
//  After each draw events this tableau captures the current
//  OpenGL buffer and writes it as a frame of video to a vidl_ostream
class vidl_capture_tableau : public vgui_wrapper_tableau
{
 public:

  //: Constructor - don't use this, use vidl_capture_tableau_new.
  //  Takes the single child tableau as a parameter.
  vidl_capture_tableau(vgui_tableau_sptr const& child);

  //: Returns the type of this tableau ('vidl_capture_tableau').
  std::string type_name() const;

  //: Handle all events sent to this tableau
  bool handle( vgui_event const &e);

  //: Set the output video stream
  void set_ostream(const vidl_ostream_sptr& os) { ostream_ = os; }

  //: Prompt the user to set an ostream with a gui dialog
  void prompt_for_ostream();

  //: Stop the video capture and close the ostream
  void close_ostream();

 protected:
  //: Destructor - called by vidl_capture_tableau_sptr.
  ~vidl_capture_tableau() { }

  vidl_ostream_sptr ostream_;

};

//: Create a smart-pointer to a vidl_capture_tableau tableau.
struct vidl_capture_tableau_new : public vidl_capture_tableau_sptr
{
  typedef vidl_capture_tableau_sptr base;

  //: Constructor - takes the single child tableau as a parameter.
  vidl_capture_tableau_new(vgui_tableau_sptr const& child)
  : base(new vidl_capture_tableau(child)) { }
};

#endif // vidl_capture_tableau_h_
