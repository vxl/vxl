// This is core/vgui/vgui_grid_tableau.h
#ifndef vgui_grid_tableau_h_
#define vgui_grid_tableau_h_
//:
// \file
// \author K.Y.McGaul
// \brief  A tableau which renders its child tableaux as a rectangular grid.
//
//  Contains classes:  vgui_grid_tableau   vgui_grid_tableau_data
//
// \verbatim
//  Modifications
//   K.Y.McGaul 20-JAN-2000 Initial version.
//   K.Y.McGaul 11-FEB-2000 Added a clear_tableau to the empty grid positions.
//   K.Y.McGaul 21-FEB-2002 Added comments and documentation.
// \endverbatim

#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>

#include "vgui_grid_tableau_sptr.h"
#include <vgui/vgui_poly_tableau.h>
#include <vgui/vgui_event_condition.h>

//: Data required by each rectangle in the grid of vgui_grid_tableau.
struct vgui_grid_tableau_data
{
  //: Handle returned by poly_tableau.
  int handle;
  //: Position of our tableau in the array of tableaux (`tabs')
  int tab_pos;
  //: True if this is a default tableau (not set by the user)
  bool is_default;
  //: Time last left clicked on (-1 if not selected)
  int time_selected;
};

bool operator==(vgui_grid_tableau_data const &a,
                vgui_grid_tableau_data const &b);

//: A tableau which renders its child tableaux as a rectangular grid.
//
//  It is derived from vgui_poly_tableau but unlike vgui_poly_tablea, the
//  child tableaux of vgui_grid_tableau can only be laid out in an m by n
//  rectangular grid.  Rows and columns are numbered starting from 0 with
//  (0,0) in the top left and (m-1,n-1) in the bottom right.
//
//  The default behaviour of vgui_grid is to keep a list of every tableau
//  added to the grid (using add_next to add to the next free position
//  or add_at to add at a specific grid position).  Users can flip through
//  this list of tableaux, 'tabs', by clicking in the grid position they
//  wish to change, and using PageUp and PageDown to go through the list.
//  When a new tableau is added using add_at the old tableau at that position
//  is kept in the 'tabs' list and can still be viewed by using PageUp and
//  PageDown.  Each grid position provides a view of the deck of tableaux kept
//  in 'tabs'.  A tableau will only be removed from 'tabs' by using remove_at.
//
//  By default, the user can make the grid larger or smaller by using
//  (CTRL) + and - to add and remove (rows and) columns.
//
//  These default behaviours can be changed using set_uses_paging_events and
//  set_grid_size_changeable.  This stops vgui_grid using the events, but
//  still passes them down to the child tableaux.  By disabling PageUp and
//  PageDown you can prevent users from changing the displayed tableau - this
//  could also be useful if you want to show two vgui_deck tableau and so want
//  the PageUp and PageDown events to pass through vgui_grid and be used by the
//  child decks. Disabling plus and minus events gives a grid tableau of fixed
//  size.
//
//  This tableau was originally written for xcv, so look at this application
//  to get a better idea what it does.
class vgui_grid_tableau : public vgui_poly_tableau
{
 public:
  typedef vgui_grid_tableau_data grid_data;

  //: Returns the type name of the tableau ("vgui_grid_tableau" in this case).
  vcl_string type_name() const { return "vgui_grid_tableau"; }

  //: Constructor - don't use this, use vgui_grid_tableau_new.
  //  Takes the initial number of columns and rows.
  vgui_grid_tableau(unsigned initial_columns = 1, unsigned initial_rows = 1)
  { init(initial_columns, initial_rows); }

  //: Constructor - don't use this, use vgui_grid_tableau_new.
  //  This creates a bi-tab, taking the two tableaux as parameters.
  vgui_grid_tableau(vgui_tableau_sptr const& l, vgui_tableau_sptr const& r);

  //: Constructor - don't use this, use vgui_grid_tableau_new.
  //  This creates a tri-tab, taking the three tableau as parameters.
  vgui_grid_tableau(vgui_tableau_sptr const& l, vgui_tableau_sptr const& m,
                    vgui_tableau_sptr const& r);

  //: Given the column number, returns the x coord for that column.
  float get_x(unsigned index);

  //: Given the row number, returns the y coord for that row.
  float get_y(unsigned index);

  //: Get the width of each column.
  float get_w();

  //: Get the height of each row.
  float get_h();

  //: Adds a tableau to the next free space in the grid and the list of tableaux
  void add_next(vgui_tableau_sptr const& tab);

  //: Add (or replace the tableau at the given position with) the given tableau.
  void add_at(vgui_tableau_sptr const& tab, unsigned col_pos, unsigned row_pos);

  //: Removes the tableau at the given grid coordinates from the display
  void remove_at(unsigned col_pos, unsigned row_pos);

  //: Returns the number of rows in the grid.
  unsigned rows() const { return nb_rows; }

  //: Returns the number of columns in the grid.
  unsigned cols() const { return nb_cols; }

  //: Returns a pointer to the tableau at the given position.
  vgui_tableau_sptr get_tableau_at(unsigned col_pos, unsigned row_pos);

  //: Returns the list of tableaux.
  vcl_vector<vgui_tableau_sptr> get_tableau_list() { return tabs; }

  //: Returns the active tableau, this is the tableau with the mouse in.
  void get_active_position(unsigned* col_pos, unsigned* row_pos);

  //: Returns the most recently selected column and row positions.
  void get_last_selected_position(unsigned* col_pos, unsigned* row_pos);

  //: Gets the positions and times of selection of the selected tableaux.
  int get_selected_positions(vcl_vector<int>* col_pos, vcl_vector<int>* row_pos,
                             vcl_vector<int>* times);

  //: Select a certain tableau
  void set_selected(int r, int c, bool onoff = true);

  //: True to allow the grid size to change, false to have fixed size.
  void set_grid_size_changeable(bool v) {
    cond_row_add   .enable(v);
    cond_row_remove.enable(v);
    cond_col_add   .enable(v);
    cond_col_remove.enable(v);
    grid_size_changeable = v;
  }

  //: True to use paging events, false to ignore them.
  void set_uses_paging_events(bool v) {
    cond_flip_fwd.enable(v);
    cond_flip_bwd.enable(v);
  }

  //: True to use mouse down events, false to ignore them.
  void set_frames_selectable(bool v) {
    cond_select  .enable(v);
    cond_deselect.enable(v);
  }

  //: Use this to emulate the deprecated bitab and tritab:
  void emulate_ntab() {
    set_grid_size_changeable(false);
    set_uses_paging_events(false);
    set_frames_selectable(false);
  }

  //: Redraw the grid keeping each tableau in its current row and column.
  void layout_grid();

  //: Redraw the grid of tableaux packing them in without gaps.
  //  Fill each row from top left downwards.
  void layout_grid2();

  //: Add an empty column to the RHS of the grid.
  void add_column();

  //: Remove last column on RHS of the grid.
  void remove_column();

  //: Add an empty row to the bottom of the grid.
  void add_row();

  //: Remove last row on the bottom of the grid
  void remove_row();

  //: Flip forwards through the list of tableaux.
  void page_up();

  //: Flip backwards through the list of tableaux.
  void page_down();

  //: Handle any events matching the {vgui_event_condition}s.
  bool handle(const vgui_event&);

 protected:
  //: Destructor - called by vgui_grid_tableau_sptr.
  ~vgui_grid_tableau() {}

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

  int INCREMENT_COLS;        // Amount to increase the number of columns
  int INCREMENT_ROWS;        // Amount to increase the number of rows.

  bool grid_size_changeable; // Whether the grid size is allowed to change.

  unsigned nb_cols;
  unsigned max_cols;
  unsigned nb_rows;
  unsigned max_rows;
  unsigned last_selected[2]; // stores col_pos, row_pos of last selected tableau

  vgui_tableau_sptr default_tab;

  vcl_vector<vgui_tableau_sptr> tabs;
  vbl_array_2d<grid_data> grid_pos;

  //: Initialisation called by all constructors.
  void init(unsigned initial_cols, unsigned initial_rows);

  //: Adds the default tableau to the given space in the grid.
  //  (but not to the vcl_list of tableaux).
  void add_default(unsigned col_pos, unsigned row_pos);

  //: Make the current tableau selected by saving the current time.
  void select_current(int time);

  //: Mark the current table as deselected by setting the time to -1.
  void deselect_current();
};

//: Create a smart pointer to a vgui_grid_tableau.
struct vgui_grid_tableau_new : public vgui_grid_tableau_sptr {
  typedef vgui_grid_tableau_sptr base;
  vgui_grid_tableau_new(unsigned initial_columns = 1, unsigned initial_rows = 1)
    : base(new vgui_grid_tableau(initial_columns,initial_rows)) {}
  vgui_grid_tableau_new(vgui_tableau_sptr const& l, vgui_tableau_sptr const& r)
    : base(new vgui_grid_tableau(l, r)) {}
  vgui_grid_tableau_new(vgui_tableau_sptr const& l, vgui_tableau_sptr const& m,
                        vgui_tableau_sptr const& r)
    : base(new vgui_grid_tableau(l, m, r)) {}
};

#endif // vgui_grid_tableau_h_
