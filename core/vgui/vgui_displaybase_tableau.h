// This is core/vgui/vgui_displaybase_tableau.h
#ifndef vgui_displaybase_tableau_h_
#define vgui_displaybase_tableau_h_
//:
// \file
// \brief  Tableau with display list functionality, can use any type of soview.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   14 Sep 1999
//
//  Contains classes  vgui_displaybase_tableau  vgui_displaybase_tableau_new
//                    vgui_displaybase_tableau_selection_callback
//
// \verbatim
//  Modifications
//   14-SEP-1999 P.Pritchett - Initial version.
//   01-OCT-2002 K.Y.McGaul - Moved displaybase to displaybase_tableau.
//   25-AUG-2003 M.Johnson - Altered to allow named groupings of soviews
//   06-OCT-2009 Ricardo Fabbri - add soview doesn't check for duplicates anymore
// \endverbatim

#include <vector>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgui/vgui_gl.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_style_sptr.h>

class vgui_soview;
class vgui_event;

//: Implement one of these to be told about picking etc.
struct vgui_displaybase_tableau_selection_callback
{
  virtual ~vgui_displaybase_tableau_selection_callback() {}
  virtual bool select(unsigned iden);
  virtual bool deselect(unsigned iden);
  virtual bool deselect_all();
};

//: Struct to maintain grouping information for soviews
struct vgui_displaybase_tableau_grouping
{
  // list of objects belonging to this group
  // duplicates entry in main objects list
  std::vector<vgui_soview*> objects;

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
  bool is_selected(unsigned iden);
  std::vector<unsigned> const & get_selected() const { return selections; }
  std::vector<vgui_soview*>     get_selected_soviews() const;
  bool select(unsigned iden);
  bool deselect(unsigned iden);
  bool deselect_all();

  // highlighting
  bool is_highlighted(unsigned iden) const { return iden == highlighted; }
  unsigned get_highlighted() const { return highlighted; }
  vgui_soview* get_highlighted_soview();
  bool highlight(unsigned iden) { highlighted = iden; return true; }

  // Add soview. The user is responsible for avoiding duplicates.
  // In a Debug mode build, a warning is issued when the soview has already been
  // added in a previous call.
  virtual void add(vgui_soview*);

  // remove soview (deletes the soview and sets the pointer to null)
  virtual void remove(vgui_soview*&);

  // removes all soviews from the display and deletes them
  virtual void clear();

  // grouping
  void set_current_grouping(std::string t_name) { current_grouping = t_name; }
  std::string get_current_grouping() const { return current_grouping; }
  vgui_displaybase_tableau_grouping* get_grouping_ptr( std::string t_name );
  std::vector< std::string > get_grouping_names();

  //: Attach your own selection callback.
  // You are in charge of deleting it later.
  void set_selection_callback(vgui_displaybase_tableau_selection_callback* cb);

  std::vector<vgui_soview*> const &get_all() const { return objects; }
  std::vector<unsigned>            get_all_ids() const;

  vgui_soview* contains_hit(std::vector<unsigned> hit);

  unsigned get_id() const { return id; }

 protected:
  std::vector<vgui_soview*> objects;

  std::map< std::string , vgui_displaybase_tableau_grouping > groupings;

  std::string current_grouping;

  std::vector<unsigned> selections;
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
  vgui_displaybase_tableau_new() : base(new vgui_displaybase_tableau()) {}
};

#endif // vgui_displaybase_tableau_h_
