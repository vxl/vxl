//-*- c++ -*-------------------------------------------------------------------
#ifndef vgui_text_tableau_h_
#define vgui_text_tableau_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// This is vgui/vgui_text_tableau.h
//
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   19 Oct 99
// \brief  Tableau for rendering text.
// Each piece of text is associated with an integer handle through
// which it can be retrieved, moved about, changed or removed.
// \verbatim
//  Modifications:
//   30 Dec 1999 fsm@robots. modified to render arbitrary text.
//   23-FEB-2000 K.Y.McGaul - Handle now remains constant when text is removed.
//                            Note that this means that the vector will never get smaller.
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_text_tableau_sptr.h>

class vgui_text_tableau : public vgui_tableau {
public:
  vgui_text_tableau();

  void clear();
  unsigned size() const;

  vcl_string type_name() const;

  int add(float x, float y, char const *text);
  int add(float x, float y, vcl_string const &text) { return add(x,y,text.c_str()); }

  //: Set the colour of the text
  void set_colour(float r, float g, float b);
  float get_posx(int handle) const;
  float get_posy(int handle) const;
  vcl_string const &get_text(int handle) const;

  void move(int handle, float nx, float ny);

  void change(int handle, char const *ntext);
  void change(int handle, vcl_string const &ntext) { change(handle, ntext.c_str()); }

  void remove(int handle);

protected:
  ~vgui_text_tableau() { }
  bool handle(vgui_event const &);

private:
  vcl_vector<float> xs;
  vcl_vector<float> ys;
  vcl_vector<vcl_string> ts;

  float r_, g_, b_;

  unsigned first_empty;    // Position of the first empty space in the vectors
};

struct vgui_text_tableau_new : public vgui_text_tableau_sptr {
  typedef vgui_text_tableau_sptr base;
  vgui_text_tableau_new() : base(new vgui_text_tableau()) { }
};

#endif // vgui_text_tableau_h_
