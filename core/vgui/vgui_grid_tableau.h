#ifndef vgui_grid_tableau_h_
#define vgui_grid_tableau_h_

//--------------------------------------------------------------------------------
// .NAME	vgui_grid_tableau
// .INCLUDE	vgui/vgui_grid_tableau.h
// .FILE	vgui_grid_tableau.cxx
// .SECTION Description
//   Derived class from vgui_polytab. Tableaux are laid out in a rectagular grid.
//   Can use (CTRL) + and - to add and remove (rows and) columns.  Can use
//   PageUp and PageDown to flip through the loaded tableaux.
// .SECTION Author
//   K.Y.McGaul
// .SECTION Modifications
//   K.Y.McGaul     20-JAN-2000    Initial version.
//   K.Y.McGaul     11-FEB-2000    Added a clear_tableau to the empty grid positions.
//--------------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>

#include "vgui_grid_tableau_ref.h"
#include <vgui/vgui_polytab.h>
#include <vgui/vgui_event_condition.h>

class vgui_grid_tableau : public vgui_polytab
{
public:

  struct grid_data
  {
    int handle;        // Handle returned by polytab
    int tab_pos;       // Position in tableaux vcl_list (tabs)
    bool is_default;   // True if this is a default tableau (not set by the user)
    int time_selected; // Time last left clicked on (-1 if not selected)
  };

  vcl_string type_name() const;

  vgui_grid_tableau(unsigned initial_columns = 1, unsigned initial_rows = 1);
  vgui_grid_tableau(vgui_tableau_ref const& l, vgui_tableau_ref const& r);
  vgui_grid_tableau(vgui_tableau_ref const& l, vgui_tableau_ref const& m, vgui_tableau_ref const& r);

  float get_x(unsigned index);
  float get_y(unsigned index);
  float get_w();
  float get_h();

  void add_next(vgui_tableau_ref const& tab);
  void add_at(vgui_tableau_ref const& tab, unsigned col_pos, unsigned row_pos);
  void remove_at(unsigned col_pos, unsigned row_pos);

  unsigned rows() const { return nb_rows; }
  unsigned cols() const { return nb_cols; }
  vgui_tableau_ref get_tableau_at(unsigned col_pos, unsigned row_pos);

  vcl_vector<vgui_tableau_ref> get_tableau_list();
  void get_active_position(unsigned* col_pos, unsigned* row_pos);
  void get_last_selected_position(unsigned* col_pos, unsigned* row_pos);
  int get_selected_positions(vcl_vector<int>* col_pos, vcl_vector<int>* row_pos,
    vcl_vector<int>* times);
  void set_selected(int r, int c, bool onoff = true);
  
  void set_grid_size_changeable(bool v) {
    cond_row_add   .enable(v);
    cond_row_remove.enable(v);
    cond_col_add   .enable(v);
    cond_col_remove.enable(v);
  }
  void set_uses_paging_events(bool v) {
    cond_flip_fwd.enable(v);
    cond_flip_bwd.enable(v);
  }
  void set_frames_selectable(bool v) {
    cond_select  .enable(v);
    cond_deselect.enable(v);
  }
  
  // use this to emulate the deprecated bitab and tritab:
  void emulate_ntab() {
    set_grid_size_changeable(false);
    set_uses_paging_events(false);
    set_frames_selectable(false);
  }

  void layout_grid();
  void layout_grid2();
  void add_column();
  void remove_column();
  void add_row();
  void remove_row();
  void page_up();
  void page_down();

  bool handle(const vgui_event&);
  
protected:
  ~vgui_grid_tableau();
  
private:
  // The number of rows and columns can be changed.
  vgui_event_condition cond_row_add;     // CTRL =
  vgui_event_condition cond_row_remove;  // CTRL -
  vgui_event_condition cond_col_add;     // =
  vgui_event_condition cond_col_remove;  // -

  // One can flip through the list of tableaux.
  vgui_event_condition cond_flip_fwd;    // PGUP
  vgui_event_condition cond_flip_bwd;    // PGDN
  
  // Frames can be selected and deselected.
  vgui_event_condition cond_select;      // left mouse button
  vgui_event_condition cond_deselect;    // middle mouse button
  
  int INCREMENT_COLS;          // Amount to increase the number of columns
  int INCREMENT_ROWS;          // Amount to increase the number of rows.

  unsigned nb_cols;
  unsigned max_cols;
  unsigned nb_rows;
  unsigned max_rows;
  unsigned last_selected[2];  // stores col_pos, row_pos of last selected tableau

  vgui_tableau_ref default_tab;

  vcl_vector<vgui_tableau_ref> tabs;
  vbl_array_2d<grid_data> grid_pos;

  void init(unsigned initial_cols, unsigned initial_rows);
  void add_default(unsigned col_pos, unsigned row_pos);

  void select_current(int time);
  void deselect_current();
};

struct vgui_grid_tableau_new : public vgui_grid_tableau_ref {
  typedef vgui_grid_tableau_ref base;
  vgui_grid_tableau_new(unsigned initial_columns = 1, unsigned initial_rows = 1) : base(new vgui_grid_tableau(1, 1)) { }
  vgui_grid_tableau_new(vgui_tableau_ref const& l, vgui_tableau_ref const& r) : base(new vgui_grid_tableau(l, r)) { }
  vgui_grid_tableau_new(vgui_tableau_ref const& l, vgui_tableau_ref const& m, vgui_tableau_ref const& r) : base(new vgui_grid_tableau(l, m, r)) { }
};

#endif // vgui_grid_tableau_h_
