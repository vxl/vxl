#ifndef vgui_displaybase_h_
#define vgui_displaybase_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_displaybase - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_displaybase.h
// .FILE vgui_displaybase.cxx
//
// .SECTION Description:
//
// vgui_displaybase is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author:
//              Philip C. Pritchett, 14 Sep 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_string.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_soview.h>

class vgui_event;


//: Implement one of these to be told about picking etc.
struct vgui_displaybase_selection_callback {
  virtual bool select(unsigned id);
  virtual bool deselect(unsigned id);
  virtual bool deselect_all();
};

//: General display list functionality, can use any type of soview.
// As long as it's black.
#include "vgui_displaybase_ref.h"

class vgui_displaybase : public vgui_tableau { 
public:

  vgui_displaybase();
 ~vgui_displaybase();

  // vgui_tableau methods
  virtual bool handle(const vgui_event&);

  // vgui_displaybase methods/data
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
  void set_selection_callback(vgui_displaybase_selection_callback* cb);
  
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

  vgui_displaybase_selection_callback* cb_;

private:
  unsigned id;
};

struct vgui_displaybase_new : public vgui_displaybase_ref {
  typedef vgui_displaybase_ref base;
  vgui_displaybase_new() : base(new vgui_displaybase()) { }
};

#endif // vgui_displaybase_h_
