// This is core/vgui/bgui_displaybase_tableau.h
#ifndef bgui_displaybase_tableau_h_
#define bgui_displaybase_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Tableau with display list functionality, can use any type of soview.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   14 Sep 99
//
//  Contains classes  bgui_displaybase_tableau  bgui_displaybase_tableau_new
//                    bgui_displaybase_tableau_selection_callback
//
// \verbatim
//  Modifications
//   14-SEP-1999 P.Pritchett - Initial version.
//   01-OCT-2002 K.Y.McGaul - Moved displaybase to displaybase_tableau.
//   13-JUN-2003 M.R. Johnson - Made displaybase handle drawing highlights
// \endverbatim


#include <vcl_vector.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_tableau.h>

class vgui_soview;
class vgui_event;


//: Implement one of these to be told about picking etc.
struct bgui_displaybase_tableau_selection_callback
{
  virtual bool select(unsigned id);
  virtual bool deselect(unsigned id);
  virtual bool deselect_all();
};

#include "bgui_displaybase_tableau_sptr.h"

//: Tableau with display list functionality, can use any type of soview.
class bgui_displaybase_tableau : public vgui_tableau
{
 public:

  //: Constructor - don't use this, use bgui_displaybase_tableau_new.
  bgui_displaybase_tableau();
 ~bgui_displaybase_tableau();

  // vgui_tableau methods
  virtual bool handle(const vgui_event&);

  // bgui_displaybase_tableau methods/data
  GLenum gl_mode;

  void draw_soviews_render();
  void draw_soviews_select();

  // selections
  bool is_selected(unsigned id);
  vcl_vector<unsigned> const & get_selected() const;
  vcl_vector<vgui_soview*>     get_selected_soviews() const;
  bool select(unsigned id);
  bool deselect(unsigned id);
  bool deselect_all();

  // highlighting
  bool is_highlighted(unsigned id);
  unsigned get_highlighted();
  vgui_soview* get_highlighted_soview();
  bool highlight(unsigned id);

  // add soview
  void add(vgui_soview*);
  void remove(vgui_soview*);
  void clear();

  //: Attach your own selection callback.
  // You are in charge of deleting it later.
  void set_selection_callback(bgui_displaybase_tableau_selection_callback* cb);

  vcl_vector<vgui_soview*> const &get_all() const;
  vcl_vector<unsigned>     const  get_all_ids() const;

  //
  vgui_soview* contains_hit(vcl_vector<unsigned> hit);

  unsigned get_id() {return id;}

 protected:
  vcl_vector<vgui_soview*> objects;

  vcl_vector<unsigned> groups;
  vcl_vector<unsigned> selections;
  unsigned highlighted;

  int gl_display_list;

  bgui_displaybase_tableau_selection_callback* cb_;

 private:
  unsigned id;
};

//: Create a smart-pointer to a bgui_displaybase_tableau tableau.
struct bgui_displaybase_tableau_new : public bgui_displaybase_tableau_sptr
{
  typedef bgui_displaybase_tableau_sptr base;
  bgui_displaybase_tableau_new() : base(new bgui_displaybase_tableau()) { }
};

#endif // bgui_displaybase_tableau_h_
