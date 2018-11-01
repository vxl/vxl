// This is core/vgui/vgui_text_tableau.h
#ifndef vgui_text_tableau_h_
#define vgui_text_tableau_h_
//:
// \file
// \brief  Tableau for rendering text.
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   19 Oct 1999
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


#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  std::string type_name() const;

  //: Add the given text to the display at the given x,y position.
  int add(float x, float y, char const *text);

  //: Add the given std::string to the display at the given x,y position.
  int add(float x, float y, std::string const &text) { return add(x,y,text.c_str()); }

  //: Set the colour of the text
  void set_colour(float r, float g, float b);

  //: Set the size of the text.
  //
  // This is one of the sizes supported by vgui_text_put.
  void set_size( unsigned sz );

  //: Return the x-coordinate of the text associated to given handle.
  float get_posx(int hndl) const;

  //: Return the y-coordinate of the text associated to given handle.
  float get_posy(int hndl) const;

  //: Return the text associated to the given handle.
  std::string const &get_text(int hndl) const;

  //: Move text associated to given handle to the given x,y position.
  void move(int hndl, float nx, float ny);

  //: Change the text associated to given handle to the given new text.
  void change(int hndl, char const *ntext);

  //: Change the text associated to given handle to the given new std::string.
  void change(int hndl, std::string const &ntext) { change(hndl, ntext.c_str()); }

  //: Delete text associated to given handle from the display.
  void remove(int hndl);

  //: Handles all events sent to this tableau.
  //  In particular, uses draw events to render the text.
  bool handle(vgui_event const &);

 protected:
  //: Destructor - called by vgui_text_tableau_sptr.
  ~vgui_text_tableau() { }

 private:
  std::vector<float> xs;
  std::vector<float> ys;
  std::vector<float> r_, g_, b_;
  std::vector<std::string> ts;
  std::vector<unsigned> sz_;

  float cur_r_, cur_g_, cur_b_;
  unsigned cur_sz_;

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
