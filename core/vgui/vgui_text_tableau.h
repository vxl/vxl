// This is core/vgui/vgui_text_tableau.h
#ifndef vgui_text_tableau_h_
#define vgui_text_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Tableau for rendering text.
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   19 Oct 99
//
//  Contains classes  vgui_text_tableau  vgui_text_tableau_new
//
// \verbatim
//  Modifications
//   19-OCT-1999 P.Pritchett - Initial version.
//   30 Dec 1999 fsm. modified to render arbitrary text.
//   23-FEB-2000 K.Y.McGaul - Handle now remains constant when text is removed,
//                            this means the vector will never get smaller.
//   06-AUG-2002 K.Y.McGaul - Changed to and added Doxygen comments.
// \endverbatim


#include <vcl_string.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_text_tableau_sptr.h>

//: Tableau for rendering text.
//
//  Each piece of text is associated with an integer handle through
//  which it can be retrieved, moved about, changed or removed.
//
//  This tableau will not display any text unless you have
//  compiled with GLUT.
class vgui_text_tableau : public vgui_tableau
{
 public:
  //: Constructor - don't use this, use vgui_text_tableau_new.
  //  Creates empty text tableau.
  vgui_text_tableau();

  //: Remove all text from the display.
  void clear();

  //: Returns the number of pieces of text displayed in this tableau.
  unsigned size() const;

  //: Returns the tableau name ("vgui_text_tableau").
  vcl_string type_name() const;

  //: Add the given text to the display at the given x,y position.
  int add(float x, float y, char const *text);

  //: Add the given vcl_string to the display at the given x,y position.
  int add(float x, float y, vcl_string const &text) { return add(x,y,text.c_str()); }

  //: Set the colour of the text
  void set_colour(float r, float g, float b);

  //: Return the x-coordinate of the text associated to given handle.
  float get_posx(int handle) const;

  //: Return the y-coordinate of the text associated to given handle.
  float get_posy(int handle) const;

  //: Return the text associated to the given handle.
  vcl_string const &get_text(int handle) const;

  //: Move text associated to given handle to the given x,y position.
  void move(int handle, float nx, float ny);

  //: Change the text associated to given handle to the given new text.
  void change(int handle, char const *ntext);

  //: Change the text associated to given handle to the given new vcl_string.
  void change(int handle, vcl_string const &ntext) { change(handle, ntext.c_str()); }

  //: Delete text associated to given handle from the display.
  void remove(int handle);

 protected:
  //: Destructor - called by vgui_text_tableau_sptr.
  ~vgui_text_tableau() { }

  //: Handles all events sent to this tableau.
  //  In particular, uses draw events to render the text.
  bool handle(vgui_event const &);

 private:
  vcl_vector<float> xs;
  vcl_vector<float> ys;
  vcl_vector<float> r_, g_, b_;
  vcl_vector<vcl_string> ts;

  float cur_r_, cur_g_, cur_b_;

  //: Position of the first empty space in the vectors
  unsigned first_empty;
};

//: Create a smart-pointer to a vgui_text_tableau.
struct vgui_text_tableau_new : public vgui_text_tableau_sptr
{
  typedef vgui_text_tableau_sptr base;

  //: Constructor - creates a default vgui_text_tableau.
  vgui_text_tableau_new() : base(new vgui_text_tableau()) { }
};

#endif // vgui_text_tableau_h_
