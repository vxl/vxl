// This is core/vgui/vgui_grid_tableau.cxx
#include "vgui_grid_tableau.h"
//:
// \file
// \brief   See vgui_grid_tableau.h for a description of this file.
// \author  K.Y.McGaul
// \date    20-JAN-2000
//
// \verbatim
//  Modifications
//   20-JAN-2000 K.Y.McGaul - Initial version.
// \endverbatim

#include <vcl_iostream.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_clear_tableau.h>

bool operator==(vgui_grid_tableau_data const &a,
                vgui_grid_tableau_data const &b)
{
  return
    a.handle == b.handle &&
    a.tab_pos == b.tab_pos &&
    a.is_default == b.is_default &&
    a.time_selected == b.time_selected;
}

//------------------------------------------------------------------------------
// Initialisation, constructors and destructor.
//------------------------------------------------------------------------------
void vgui_grid_tableau::init(unsigned initial_cols, unsigned initial_rows)
{
  cond_row_add    = vgui_event_condition(vgui_key('='), vgui_CTRL);
  cond_row_remove = vgui_event_condition(vgui_key('-'), vgui_CTRL);
  cond_col_add    = vgui_event_condition(vgui_key('='));
  cond_col_remove = vgui_event_condition(vgui_key('-'));
  cond_flip_fwd   = vgui_event_condition(vgui_PAGE_DOWN);
  cond_flip_bwd   = vgui_event_condition(vgui_PAGE_UP);
  cond_select     = vgui_event_condition(vgui_LEFT);
  cond_deselect   = vgui_event_condition(vgui_MIDDLE);

  INCREMENT_COLS = 50;
  INCREMENT_ROWS = 50;

  grid_size_changeable = true;

#ifdef DEBUG
    vcl_cerr << "vgui_grid_tableau::init: initialising grid with "
             << initial_cols << " columns and " << initial_rows << " rows\n";
#endif
  nb_cols = initial_cols;
  nb_rows = initial_rows;
  max_cols = initial_cols + INCREMENT_COLS;
  max_rows = initial_rows + INCREMENT_ROWS;
  last_selected[0] = last_selected[1] = 0;
  default_tab = vgui_clear_tableau_new();

  grid_pos = vbl_array_2d<grid_data>(max_cols, max_rows);
  for (unsigned i = 0; i < nb_cols; i++)
  {
    for (unsigned j = 0; j < nb_rows; j++)
    {
      add_default(i, j);
    }
  }
}

//: Makes a bitab.
vgui_grid_tableau::vgui_grid_tableau(vgui_tableau_sptr const& l, vgui_tableau_sptr const& r)
{
  init(2, 1);
  add_next(l);
  add_next(r);
}

//: Makes a tritab.
vgui_grid_tableau::vgui_grid_tableau(vgui_tableau_sptr const& l, vgui_tableau_sptr const& m, vgui_tableau_sptr const& r)
{
  init(3, 1);
  add_next(l);
  add_next(m);
  add_next(r);
}

//------------------------------------------------------------------------------
//: Given the column number, returns the x coord for that column.
//   Note col_pos is numbered from 0.
//------------------------------------------------------------------------------
float vgui_grid_tableau::get_x(unsigned col_pos)
{
  if (col_pos < nb_cols)
    return col_pos * get_w();
  else
  {
    vgui_macro_warning << "Error in get_x: col_pos = " << col_pos << ", max = " << nb_cols << vcl_endl;
    return 0;
  }
}

//------------------------------------------------------------------------------
//: Given the row number, returns the y coord for that row.
//   Note row_pos is numbered from 0.
//   Note that poly_tableau coord system starts in the bottom left and we want
//   rows to be numbered from the top down.
//------------------------------------------------------------------------------
float vgui_grid_tableau::get_y(unsigned row_pos)
{
  if (row_pos < nb_rows)
    return (nb_rows - row_pos - 1) * get_h();
  else
  {
    vgui_macro_warning << "Error in get_y: row_pos = " << row_pos << ", max = " << nb_rows << vcl_endl;
    return 0;
  }
}

//------------------------------------------------------------------------------
//: Get the width of each column.
//------------------------------------------------------------------------------
float vgui_grid_tableau::get_w() { return 1.0/nb_cols; }

//------------------------------------------------------------------------------
//: Get the height of each row.
//------------------------------------------------------------------------------
float vgui_grid_tableau::get_h() { return 1.0/nb_rows; }

//------------------------------------------------------------------------------
//: Adds the default tableau to the given space in the grid (but not to the vcl_list of tableaux).
//   Note, it is assumed that the given grid position is empty or uninitialized so
//   nothing is removed from the grid position before the default is added.
//------------------------------------------------------------------------------
void vgui_grid_tableau::add_default(unsigned col_pos, unsigned row_pos)
{
  if (col_pos < nb_cols && row_pos < nb_rows)
  {
    grid_pos(col_pos, row_pos).handle
      = this->add(default_tab, get_x(col_pos), get_y(row_pos), get_w(), get_h());
    grid_pos(col_pos, row_pos).tab_pos = -1;
    grid_pos(col_pos, row_pos).is_default = true;
    grid_pos(col_pos, row_pos).time_selected = -1;
  }
}

//------------------------------------------------------------------------------
//: Adds a tableau to the next free space in the grid and to the end of the vcl_list of tableaux.
//   If there are no free spaces and the grid size is changeable then it adds a
//   new column to the RHS of the grid and adds the new tableau to the top of it.
//------------------------------------------------------------------------------
void vgui_grid_tableau::add_next(vgui_tableau_sptr const& tab)
{
  tabs.push_back(tab);

  for (unsigned j = 0; j < nb_rows; j++)
  {
    for (unsigned i = 0; i < nb_cols; i++)
    {
       // Find the next space in the grid without a tableau:
       if (grid_pos(i,j).is_default == true)
       {
         this->remove(grid_pos(i,j).handle);
         grid_pos(i,j).handle = this->add(tab, get_x(i), get_y(j), get_w(), get_h());
         grid_pos(i,j).tab_pos = tabs.size() - 1;
         grid_pos(i,j).is_default = false;
#ifdef DEBUG
         vcl_cerr << "vgui_grid_tableau::add_next: adding tableau to col = "<< i <<", row = "<< j << '\n';
#endif
         return;
       }
    }
  }
  // fsm: this flag should control how events are handled, not whether a client
  // can change the layout of the grid tableau.
  //if (uses_plus_minus_events == true)

  // kym: only because you gave it that funny name!!!  I'm putting this back in here
  // (with its original name) because I think it is needed. Events are handled separately
  if (grid_size_changeable == true)
  {
    // If we have got here then there are no free spaces in the grid.
#ifdef DEBUG
    vcl_cerr << "vgui_grid_tableau::add_next: current grid is full, adding another column\n";
#endif
    add_column();
    unsigned col_pos = nb_cols - 1;
    unsigned row_pos = 0;
    this->remove(grid_pos(col_pos, row_pos).handle);
    grid_pos(col_pos, row_pos).handle
      = this->add(tab, get_x(col_pos), get_y(row_pos), get_w(), get_h());
    grid_pos(col_pos, row_pos).tab_pos = tabs.size() - 1;
    grid_pos(col_pos, row_pos).is_default = false;
#ifdef DEBUG
    vcl_cerr << "vgui_grid_tableau::add_next: adding tableau to col = "<< col_pos <<", row = "<< row_pos << '\n';
#endif
  }
}

//------------------------------------------------------------------------------
//:  Add (or replace the tableau at the given position with) the given tableau.
//   Adds the given tableau to the end of the vcl_list of tableaux.
//------------------------------------------------------------------------------
void vgui_grid_tableau::add_at(vgui_tableau_sptr const& tab, unsigned col_pos, unsigned row_pos)
{
  // This function leaks core because a tableau which is replaced by another
  // is still referenced by the smart pointer in `tabs'. why do we need a
  // separate array of tableaux? can't we just put them in the grid_data
  // structure? -- fsm

  // kym - This isn't a core leak!! It is intentional that pointers to
  // tableaux are kept - this means that we can flip through the list of
  // tableaux using page up and down but not display all tableaux at the
  // same time (see xcv for an example of this).  Each grid position provides
  // a view of the deck of tableaux kept in 'tabs'.
  if (col_pos < nb_cols && row_pos < nb_rows)
  {
#ifdef DEBUG
    vcl_cerr << "vgui_grid_tableau::add_at: adding tableau at col = "<< col_pos <<", row = "<< row_pos << '\n';
#endif
    tabs.push_back(tab);
    this->remove(grid_pos(col_pos, row_pos).handle);
    grid_pos(col_pos, row_pos).handle = this->add(tab, get_x(col_pos), get_y(row_pos), get_w(), get_h());
    grid_pos(col_pos, row_pos).tab_pos = tabs.size()-1;
    grid_pos(col_pos, row_pos).is_default = false;
    layout_grid();
  }
  else {
    vcl_cerr << __FILE__ ": in add_at():\n"
             << __FILE__ ": nb_cols nb_rows = " << nb_cols << ' ' << nb_rows << vcl_endl
             << __FILE__ ": col_pos row_pos = " << col_pos << ' ' << row_pos << vcl_endl;
  }
}

//------------------------------------------------------------------------------
//:
//  Removes the tableau at the given grid coordinates from the display
//  and from the vcl_list of tableau.  It is replaced in the grid by the default tableau.
//------------------------------------------------------------------------------
void vgui_grid_tableau::remove_at(unsigned col_pos, unsigned row_pos)
{
#ifdef DEBUG
  vcl_cerr << "vgui_grid_tableau::remove_at: removing tableau at col = " << col_pos << ", row = " << row_pos << '\n';
#endif
  if (col_pos < nb_cols && row_pos < nb_rows)
  {
    if (grid_pos(col_pos, row_pos).is_default == false)
    {
      this->remove(grid_pos(col_pos, row_pos).handle);
      tabs.erase(tabs.begin() + grid_pos(col_pos, row_pos).tab_pos);
      grid_pos(col_pos, row_pos).handle
        = this->add(default_tab, get_x(col_pos), get_y(row_pos), get_w(), get_h());
      grid_pos(col_pos, row_pos).tab_pos = -1;
      grid_pos(col_pos, row_pos).is_default = true;
      layout_grid();
    }
  }
  else {
    vcl_cerr << __FILE__ ": in remove_at():\n"
             << __FILE__ ": nb_cols nb_rows = " << nb_cols << ' ' << nb_rows << vcl_endl
             << __FILE__ ": col_pos row_pos = " << col_pos << ' ' << row_pos << vcl_endl;
  }
}

//------------------------------------------------------------------------------
//: Returns a pointer to the tableau at the given position.
//------------------------------------------------------------------------------
vgui_tableau_sptr vgui_grid_tableau::get_tableau_at(unsigned col_pos, unsigned row_pos)
{
  if (col_pos < nb_cols && row_pos < nb_rows &&
      grid_pos(col_pos, row_pos).is_default == false)
    return this->get(grid_pos(col_pos, row_pos).handle);

  if (col_pos >= nb_cols)
    vgui_macro_warning << "Given column number " << col_pos
                       << " is out of range, max value = " << nb_cols-1 << vcl_endl;
  else if ( row_pos >= nb_rows)
    vgui_macro_warning << "Given row number " << row_pos
                       << " is out of range, max value = " << nb_rows-1 << vcl_endl;
#ifdef DEBUG
  else vgui_macro_warning << "Only default tableau at (" << col_pos << ", " << row_pos << ").\n";
#endif
  return vgui_tableau_sptr();
}

//------------------------------------------------------------------------------
//: Returns the active tableau, this is the tableau with the mouse in.
//   Note that there is only one active tableau, while there may be many
//   selected tableaux.
//------------------------------------------------------------------------------
void vgui_grid_tableau::get_active_position(unsigned* col_pos, unsigned* row_pos)
{
  *col_pos = nb_cols + 1;
  *row_pos = nb_rows + 1;
  int current_handle;
  current_handle = this->get_current_id();

  if (current_handle == -1)
  {
    return;
  }

  for (unsigned i = 0; i < nb_cols; i++)
  {
    for (unsigned j = 0; j < nb_rows; j++)
    {
      if (grid_pos(i,j).handle == current_handle)
      {
        *col_pos = i;
        *row_pos = j;
        return;
      }
    }
  }
}

//------------------------------------------------------------------------------
//: Returns the most recently selected column and row positions.
//------------------------------------------------------------------------------
void vgui_grid_tableau::get_last_selected_position(
  unsigned* col_pos, unsigned* row_pos)
{
  *col_pos = last_selected[0];
  *row_pos = last_selected[1];
}

//: Select a certain tableau
void vgui_grid_tableau::set_selected(int r, int c, bool onoff)
{
  int time = onoff ? 1000 : -1;

  if ((unsigned int)r < nb_rows && (unsigned int)c < nb_cols) {
    grid_pos(c,r).time_selected = time;
  } else {
    vcl_cerr << "vgui_grid_tableau::set_selected: (r,c) > (cols,rows)\n";
  }
}

//------------------------------------------------------------------------------
//: Gets the positions and times of selection of the selected tableaux.
//   The number of selected tableau is returned.  Their positions are returned
//   in the vcl_vectors passed in as parameters.
//   Note, a tableau is selected if it has been clicked on by the left mouse
//   button.  It can be deselected by clicking with the middle mouse button.
//------------------------------------------------------------------------------
int vgui_grid_tableau::get_selected_positions(vcl_vector<int>* col_pos,
vcl_vector<int>* row_pos, vcl_vector<int>* times)
{
  int nb_selected = 0;
  for (unsigned i = 0; i < nb_cols; i++)
  {
    for (unsigned j = 0; j < nb_rows; j++)
    {
      if (grid_pos(i,j).time_selected != -1)
      {
        col_pos->push_back(i);
        row_pos->push_back(j);
        times->push_back(grid_pos(i,j).time_selected);
        nb_selected++;
      }
    }
  }
  return nb_selected;
}

//------------------------------------------------------------------------------
//: Redraw the grid of tableaux keeping each tableau in its current row and column.
//   If for example a new column had been added, using this would redraw the
//   grid with that column empty.
//------------------------------------------------------------------------------
void vgui_grid_tableau::layout_grid()
{
#ifdef DEBUG
  vcl_cerr << "vgui_grid_tableau::layout_grid: redrawing grid keeping current row and column positions\n";
#endif
  for (unsigned i = 0; i < nb_cols; i++)
  {
    for (unsigned j = 0; j < nb_rows; j++)
    {
      this->move(grid_pos(i,j).handle, get_x(i), get_y(j), get_w(), get_h());
    }
  }
}

//------------------------------------------------------------------------------
//: Redraw the grid of tableaux packing them in without gaps, filling each row from top left downwards.
//------------------------------------------------------------------------------
void vgui_grid_tableau::layout_grid2()
{
#ifdef DEBUG
  vcl_cerr << "vgui_grid_tableau::layout_grid2: redrawing grid without gaps\n";
#endif
  unsigned grid_col = 0;
  unsigned grid_row = 0;
  for (unsigned j = 0; j < nb_rows; j++)
  {
    for (unsigned i = 0; i < nb_cols; i++)
    {
      this->move(grid_pos(i,j).handle, get_x(grid_col), get_y(grid_row), get_w(), get_h());
      grid_pos(grid_col, grid_row).handle = grid_pos(i,j).handle;
      grid_pos(grid_col, grid_row).tab_pos = grid_pos(i,j).tab_pos;
      if (i != grid_col || j != grid_row)
        grid_pos(i,j).handle = -1;
      if (grid_col < (nb_cols - 1))
        grid_col++;
      else
      {
        grid_row++;
        grid_col = 0;
      }
    }
  }
}

//------------------------------------------------------------------------------
//: Add an empty column to the RHS of the grid.
//------------------------------------------------------------------------------
void vgui_grid_tableau::add_column()
{
  nb_cols++;
#ifdef DEBUG
  vcl_cerr << "vgui_grid_tableau::add_column: number of columns is now " << nb_cols << '\n';
#endif

  if (nb_cols > max_cols)
  {
    // Increase size of max cols - FIXME
    vcl_cerr << "vgui_grid_tableau::add_column(): Warning: nb_cols > max_cols\n";
  }

  for (unsigned j = 0; j < nb_rows; j++)
    add_default(nb_cols-1, j);

  layout_grid();
}

//------------------------------------------------------------------------------
//: Remove last column on RHS of the grid.
//------------------------------------------------------------------------------
void vgui_grid_tableau::remove_column()
{
  if (nb_cols > 1)
  {
    for (unsigned j = 0; j < nb_rows; j++)
    {
      this->remove(grid_pos(nb_cols-1, j).handle);
    }
    nb_cols--;
#ifdef DEBUG
    vcl_cerr << "vgui_grid_tableau::remove_column: number of columns is now " << nb_cols << '\n';
#endif
    layout_grid();
  }
}

//------------------------------------------------------------------------------
//: Add an empty row to the bottom of the grid.
//------------------------------------------------------------------------------
void vgui_grid_tableau::add_row()
{
  nb_rows++;
#ifdef DEBUG
  vcl_cerr << "vgui_grid_tableau::add_row: number of rows is now " << nb_rows << '\n';
#endif
  if (nb_rows > max_rows)
  {
    // Increase size of max_rows - FIXME
    vcl_cerr << "vgui_grid_tableau::add_row(): Warning: nb_rows > max_rows\n";
  }
  for (unsigned i = 0; i < nb_cols; i++)
  {
    add_default(i, nb_rows-1);
  }
  layout_grid();
}

//------------------------------------------------------------------------------
//: Remove last row on the bottom of the grid
//------------------------------------------------------------------------------
void vgui_grid_tableau::remove_row()
{
  if (nb_rows > 1)
  {
    for (unsigned i = 0; i < nb_cols; i++)
    {
      this->remove(grid_pos(i, nb_rows-1).handle);
    }
    nb_rows--;
#ifdef DEBUG
    vcl_cerr << "vgui_grid_tableau::remove_row: number of rows is now " << nb_rows << '\n';
#endif
    layout_grid();
  }
}

//------------------------------------------------------------------------------
//: Flip forwards through the list of tableaux.
//------------------------------------------------------------------------------
void vgui_grid_tableau::page_up()
{
  unsigned row_pos, col_pos;
  get_active_position(&col_pos, &row_pos);
#ifdef DEBUG
  vcl_cerr << "vgui_grid_tableau::page_up called on col_pos = " <<  col_pos << ", row_pos = " << row_pos << '\n';
#endif

  if (col_pos < nb_cols && row_pos < nb_rows)
  {
    // Get the index of the tableau currently in that position:
    unsigned tab_index = grid_pos(col_pos, row_pos).tab_pos;

    if (tab_index < (tabs.size() - 1) && grid_pos(col_pos, row_pos).is_default == false)
    {
      this->replace(grid_pos(col_pos, row_pos).handle, tabs[tab_index + 1]);
      grid_pos(col_pos, row_pos).tab_pos++;
    }
    else
    {
      this->replace(grid_pos(col_pos, row_pos).handle, tabs[0]);
      grid_pos(col_pos, row_pos).tab_pos = 0;
      grid_pos(col_pos, row_pos).is_default = false;
    }
  }
}

//------------------------------------------------------------------------------
//: Flip backwards through the list of tableaux.
//------------------------------------------------------------------------------
void vgui_grid_tableau::page_down()
{
  unsigned row_pos, col_pos;
  get_active_position(&col_pos, &row_pos);
#ifdef DEBUG
  vcl_cerr << "vgui_grid_tableau::page_down called on col_pos = " <<  col_pos << ", row_pos = " << row_pos << '\n';
#endif

  if (col_pos < nb_cols && row_pos < nb_rows)
  {
    // Get the index of the tableau currently in that position:
    unsigned tab_index = grid_pos(col_pos, row_pos).tab_pos;

    if (tab_index > 0 && grid_pos(col_pos, row_pos).is_default == false)
    {
      this->replace(grid_pos(col_pos, row_pos).handle, tabs[tab_index - 1]);
      grid_pos(col_pos, row_pos).tab_pos--;
    }
    else
    {
      this->replace(grid_pos(col_pos, row_pos).handle, tabs[tabs.size()-1]);
      grid_pos(col_pos, row_pos).tab_pos = tabs.size()-1;
      grid_pos(col_pos, row_pos).is_default = false;
    }
  }
}

//------------------------------------------------------------------------------
//: Make the current tableau selected by saving the current time.
//   Set the outline color to red.
//------------------------------------------------------------------------------
void vgui_grid_tableau::select_current(int time)
{
  unsigned col, row;
  get_active_position(&col, &row);
  bool redraw_needed = false;
  if (grid_pos(col, row).time_selected == -1)
    redraw_needed = true;
  grid_pos(col, row).time_selected = time;
  last_selected[0] = col;
  last_selected[1] = row;
  if (redraw_needed)
  {
    set_outline_color(grid_pos(col,row).handle, 1,0,0);
    post_redraw();
  }
}

//------------------------------------------------------------------------------
//: Mark the current table as deselected by setting the time to -1.
//   Set the outline color to white.
//------------------------------------------------------------------------------
void vgui_grid_tableau::deselect_current()
{
  unsigned col, row;
  get_active_position(&col, &row);
  if (grid_pos(col,row).time_selected != -1)
  {
    grid_pos(col, row).time_selected = -1;
    set_outline_color(grid_pos(col,row).handle, 1,1,1);
    post_redraw();
  }
}

//------------------------------------------------------------------------------
//: Handle any events matching the {vgui_event_condition}s.
// All other events go to the base class.
//------------------------------------------------------------------------------
bool vgui_grid_tableau::handle(const vgui_event &e)
{
  if (cond_row_add(e))
  {
    add_row();
    return true;
  }
  else if (cond_row_remove(e))
  {
    remove_row();
    return true;
  }
  else if (cond_col_add(e))
  {
    add_column();
    return true;
  }
  else if (cond_col_remove(e))
  {
    remove_column();
    return true;
  }
  else if (cond_flip_fwd(e))
  {
    page_up();
    return true;
  }
  else if (cond_flip_bwd(e))
  {
    page_down();
    return true;
  }
  else if (cond_select(e))
  {
    select_current(e.timestamp);
    // let event go to base class too.
  }
  else if (cond_deselect(e))
  {
    deselect_current();
    // let event go to base class too.
  }

  // We are not interested in other events, so pass event to base class:
  return vgui_poly_tableau::handle(e);
}
