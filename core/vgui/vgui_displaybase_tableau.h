// This is core/vgui/vgui_displaybase_tableau.h
#ifndef vgui_displaybase_tableau_h_
#define vgui_displaybase_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Tableau with display list functionality, can use any type of soview.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   14 Sep 99
//
//  Contains classes  vgui_displaybase_tableau  vgui_displaybase_tableau_new
//                    vgui_displaybase_tableau_selection_callback
//
// \verbatim
//  Modifications
//   14-SEP-1999 P.Pritchett - Initial version.
//   01-OCT-2002 K.Y.McGaul - Moved displaybase to displaybase_tableau.
//   25-AUG-2003 M.Johnson - Altered to allow named groupings of soviews
// \endverbatim

#include <vcl_vector.h>
#include <vcl_map.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_style_sptr.h>

class vgui_soview;
class vgui_event;

//: Implement one of these to be told about picking etc.
struct vgui_displaybase_tableau_selection_callback
{
  virtual bool select(unsigned id);
  virtual bool deselect(unsigned id);
  virtual bool deselect_all();
};

//: Struct to maintain grouping information for soviews
struct vgui_displaybase_tableau_grouping {

  // list of objects belonging to this group
  // duplicates entry in main objects list
  vcl_vector<vgui_soview*> objects;

  // style that will be used for override features
  vgui_style_sptr style;

  // used to hide or show this group of soviews
  bool hide;

  // applies a new temporary color to all soviews in this grouping
  bool color_override;

  // applies a new temporary point size to all soviews in this grouping
  bool point_size_override;

  // applies a new temporary line width to all soviews in this grouping
  bool line_width_override;
};

#include "vgui_displaybase_tableau_sptr.h"

//: Tableau with display list functionality, can use any type of soview.
class vgui_displaybase_tableau : public vgui_tableau
{
 public:

  //: Constructor - don't use this, use vgui_displaybase_tableau_new.
  vgui_displaybase_tableau();
 ~vgui_displaybase_tableau();

  // vgui_tableau methods
  virtual bool handle(const vgui_event&);

  // vgui_displaybase_tableau methods/data
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

  // grouping
  void set_current_grouping( vcl_string name );
  vcl_string get_current_grouping();
  vgui_displaybase_tableau_grouping* get_grouping_ptr( vcl_string name );
  vcl_vector< vcl_string > get_grouping_names();

  //: Attach your own selection callback.
  // You are in charge of deleting it later.
  void set_selection_callback(vgui_displaybase_tableau_selection_callback* cb);

  vcl_vector<vgui_soview*> const &get_all() const;
  vcl_vector<unsigned>     const  get_all_ids() const;

  vgui_soview* contains_hit(vcl_vector<unsigned> hit);

  unsigned get_id() {return id;}

 protected:
  vcl_vector<vgui_soview*> objects;

  vcl_map< vcl_string , vgui_displaybase_tableau_grouping > groupings;

  vcl_string current_grouping;

  // This vector appears to be unused by other classes in VXL.
  // Hopefully whatever functionality it was initially intended
  // to provide will instead be satisfied by the new grouping system.
  //
  //vcl_vector<unsigned> groups;

  vcl_vector<unsigned> selections;
  unsigned highlighted;

  int gl_display_list;

  vgui_displaybase_tableau_selection_callback* cb_;

 private:
  unsigned id;
};

//: Create a smart-pointer to a vgui_displaybase_tableau tableau.
struct vgui_displaybase_tableau_new : public vgui_displaybase_tableau_sptr
{
  typedef vgui_displaybase_tableau_sptr base;
  vgui_displaybase_tableau_new() : base(new vgui_displaybase_tableau()) { }
};

#endif // vgui_displaybase_tableau_h_
