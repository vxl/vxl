// This is oxl/xcv/xcv.cxx

//:
// \file
// \author  K.Y.McGaul
// \brief Main function for xcv.
//
//  This contains the main function for xcv, a program designed to demonstrate
//  the functionality of vxl.
//
// \verbatim
//  Modifications:
//   K.Y.McGaul     27-JAN-2000    Initial version.
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>

#include <vul/vul_arg.h>

#include <vil/vil_image.h>
#include <vil/vil_load.h>

#include <vgui/vgui.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_easy2D.h>
#include <vgui/vgui_rubberbander.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>

#include "xcv_file.h"
#include "xcv_display.h"
#include "xcv_geometry.h"
#include "xcv_processing.h"
#include "xcv_segmentation.h"
#include "xcv_multiview.h"
#include "xcv_image_tableau.h"
#include "xcv_picker_tableau.h"

#define MENUBAR_HEIGHT  70

//-----------------------------------------------------------------------------
class xcv_tableau : public vgui_grid_tableau
{
public:

  //xcv_tableau(int nb_images) : vgui_grid_tableau(nb_images, 1) { }
  xcv_tableau(int rows, int cols) : vgui_grid_tableau(cols, rows) { }

  ~xcv_tableau() {}

  bool handle(const vgui_event& e)
  {
    return vgui_grid_tableau::handle(e);
  }
};

xcv_tableau* xcv_tab;

//-----------------------------------------------------------------------------
//: Get file last used for saving data.
//-----------------------------------------------------------------------------
vcl_string* get_savefile()
{
  static vcl_string savefile = "";
  return &savefile;
}

//-----------------------------------------------------------------------------
//: Get file last used for loading data.
//-----------------------------------------------------------------------------
vcl_string* get_loadfile()
{
  static vcl_string loadfile = "";
  return &loadfile;
}

//-----------------------------------------------------------------------------
//: Displays given message on the status bar.
//-----------------------------------------------------------------------------
void post_to_status_bar(const char* msg)
{
  vgui::out << msg << vcl_endl;
}

vgui_tableau_sptr get_top(unsigned col,unsigned row)
{
  return xcv_tab->get_tableau_at(col,row);
}

//-----------------------------------------------------------------------------
//: Gets the list of all image tableaux in xcv.
//-----------------------------------------------------------------------------
vcl_vector<xcv_image_tableau_sptr> get_image_list()
{
  vcl_vector<xcv_image_tableau_sptr> img_tabs;
  vcl_vector<vgui_tableau_sptr> all_tabs = xcv_tab->get_tableau_list();
  for (unsigned i=0; i<all_tabs.size(); i++)
  {
    xcv_image_tableau_sptr img 
      = (xcv_image_tableau*)vgui_find_below_by_type_name(all_tabs[i], 
      vcl_string("xcv_image_tableau")).operator->();
    img_tabs.push_back(img);
  }
  return img_tabs;
}

//-----------------------------------------------------------------------------
//: Gets the list of all easy2D tableaux in xcv.
//-----------------------------------------------------------------------------
vcl_vector<vgui_easy2D_sptr> get_easy2D_list()
{
  vcl_vector<vgui_easy2D_sptr> easy_tabs;
  vcl_vector<vgui_tableau_sptr> all_tabs = xcv_tab->get_tableau_list();
  for (unsigned i=0; i<all_tabs.size(); i++)
  {
    vgui_easy2D_sptr easy = (vgui_easy2D*)vgui_find_below_by_type_name(
      all_tabs[i], vcl_string("vgui_easy2D")).operator->();
    easy_tabs.push_back(easy);
  }
  return easy_tabs;
}

//-----------------------------------------------------------------------------
//: Gets the last selected row and column position.
//-----------------------------------------------------------------------------
void get_current(unsigned* col, unsigned* row)
{
  xcv_tab->get_last_selected_position(col, row);
}

//-----------------------------------------------------------------------------
//: Returns true if there are exactly two selected views.
//-----------------------------------------------------------------------------
bool get_twoviews(vcl_vector<int>* col_pos, vcl_vector<int>* row_pos)
{
  vcl_vector<int> cols, rows, times;
  int nb_views = xcv_tab->get_selected_positions(&cols, &rows, &times);
  // if not selected,  pick top left pair.
  if (nb_views != 2) {
    for (unsigned int i = 0; i < xcv_tab->rows(); ++i)
      for (unsigned int j = 0; j < xcv_tab->cols(); ++j)
        xcv_tab->set_selected(i,j, false);
    cols.clear();
    rows.clear();
    times.clear();
    xcv_tab->set_selected(0,0, true);
    xcv_tab->set_selected(0,1, true);
    nb_views = xcv_tab->get_selected_positions(&cols, &rows, &times);
  }

  // If still not selected, may be a funny layout or summat.
  if (nb_views != 2)
  {
    vgui_dialog two_dl("Error");
    two_dl.message("");
    two_dl.message("You must select exactly two views.");
    two_dl.message("");
    two_dl.ask();
    vgui_macro_warning 
      << "You must select exactly two views, not the current "
      << nb_views << vcl_endl;
    return false;
  }

  // Sort the views into time order:
  if (times[0] < times[1])
  {
    col_pos->push_back(cols[0]); col_pos->push_back(cols[1]);
    row_pos->push_back(rows[0]); row_pos->push_back(rows[1]);
  }
  else
  {
    col_pos->push_back(cols[1]); col_pos->push_back(cols[0]);
    row_pos->push_back(rows[1]); row_pos->push_back(rows[0]);
  }
  return true;
}

//-----------------------------------------------------------------------------
//: Returns true if there are exactly three selected views.
//-----------------------------------------------------------------------------
bool get_threeviews(vcl_vector<int>* col_pos, vcl_vector<int>* row_pos)
{
  vcl_vector<int> cols, rows, times;
  int nb_views = xcv_tab->get_selected_positions(&cols, &rows, &times);
  if (nb_views != 3)
  {
    vgui_dialog dl("Error");
    dl.message("");
    dl.message("You need to select exactly three views.");
    dl.message("");
    dl.ask();
    vgui_macro_warning 
      << "You need to selected exactly three views. Number selected = "
      << nb_views << vcl_endl;
    return false;
  }
  // Sort the view into time order:
  int first, second, third;
  if (times[0] <= times[1] && times[1] <= times[2])
  { first = 0; second = 1; third = 2; }
  else if (times[0] <= times[2] && times[2] <= times[1])
  { first = 0; second = 2; third = 1; }
  else if (times[1] <= times[0] && times[0] <= times[2])
  { first = 1; second = 0; third = 2; }
  else if (times[1] <= times[2] && times[2] <= times[0])
  { first = 1; second = 2; third = 0; }
  else if (times[2] <= times[0] && times[0] <= times[1])
  { first = 2; second = 0; third = 1; }
  else
  { first = 2; second = 1; third = 0; }

  col_pos->push_back(cols[first]); row_pos->push_back(rows[first]);
  col_pos->push_back(cols[second]); row_pos->push_back(rows[second]);
  col_pos->push_back(cols[third]); row_pos->push_back(rows[third]);
  return true;
}

//-----------------------------------------------------------------------------
//: Return the underlying rubberbander from the tableau at the given position.
//  This function returns NULL if it fails.
//-----------------------------------------------------------------------------
vgui_rubberbander_sptr get_rubberbander_at(unsigned col, unsigned row)
{
  vgui_tableau_sptr top_tab = xcv_tab->get_tableau_at(col, row);
  if (top_tab)
  {
    vcl_string type_name("vgui_rubberbander");
    vgui_rubberbander_sptr tab;
    tab.vertical_cast(vgui_find_below_by_type_name(top_tab, type_name));
    if (tab)
      return tab;
  }
  vgui_macro_warning << "Unable to get rubberbander tableau at (" << col 
    <<", "<<row<<")"<<vcl_endl;
  return vgui_rubberbander_sptr();
}

//-----------------------------------------------------------------------------
//: Return the underlying easy2D from the tableau at the given position.
//  This function returns NULL if it fails.
//-----------------------------------------------------------------------------
vgui_easy2D_sptr get_easy2D_at(unsigned col, unsigned row)
{
  vgui_tableau_sptr top_tab = xcv_tab->get_tableau_at(col, row);
  if (top_tab)
  {
    vcl_string type_name("vgui_easy2D");
    vgui_easy2D_sptr tab;
    tab.vertical_cast(vgui_find_below_by_type_name(top_tab, type_name));
    if (tab)
      return tab;
  }
  vgui_macro_warning << "Unable to get easy2D at (" << col << ", " << row 
    << ")" << vcl_endl;
  return vgui_easy2D_sptr();
}

//-----------------------------------------------------------------------------
//: Return the underlying easy2D from the tableau at the given position.
//  This function returns NULL if it fails.
//-----------------------------------------------------------------------------
vgui_composite_sptr get_composite_at(unsigned col, unsigned row)
{
  vgui_tableau_sptr top_tab = xcv_tab->get_tableau_at(col, row);
  if (top_tab)
  {
    vcl_string type_name("vgui_composite");
    vgui_composite_sptr tab;
    tab.vertical_cast(vgui_find_below_by_type_name(top_tab, type_name));
    if (tab)
      return tab;
  }
  vgui_macro_warning << "Unable to get composite at (" << col << ", " 
    << row << ")" << vcl_endl;
  return vgui_composite_sptr();
}

//-----------------------------------------------------------------------------
//: Return the viewer2D at the given position.
//  This function returns NULL if it fails.
//-----------------------------------------------------------------------------
vgui_viewer2D_sptr get_viewer2D_at(unsigned col, unsigned row)
{
  vgui_tableau_sptr top_tab = xcv_tab->get_tableau_at(col, row);
  if (top_tab)
  {
    vgui_viewer2D_sptr view;
    view.vertical_cast(vgui_find_below_by_type_name(top_tab, 
      vcl_string("vgui_viewer2D")));
    if (view)
      return view;
  }
  vgui_macro_warning << "Unable to get viewer2D tableau at (" << col 
    << ", " << row << ")" << vcl_endl;
  return vgui_viewer2D_sptr();
}

//-----------------------------------------------------------------------------
//: Return currently active easy2d
//-----------------------------------------------------------------------------
vgui_easy2D_sptr get_current_easy2D()
{
  unsigned i,j;
  get_current(&i,&j);
  return get_easy2D_at(i,j);
}

//-----------------------------------------------------------------------------
//:  Gets the image tableau at the given position.
//   This function returns a new image tableau if it fails.
//-----------------------------------------------------------------------------
xcv_image_tableau_sptr get_image_tableau_at(unsigned col, unsigned row)
{
  vgui_easy2D_sptr tab = get_easy2D_at(col, row);
  if (tab)
  {
    xcv_image_tableau_sptr tt;
    tt.vertical_cast(vgui_find_below_by_type_name(tab, 
      vcl_string("xcv_image_tableau")));
    if (tt)
      return tt;
  }
  vgui_macro_warning << "Unable to get xcv_image_tableau at (" << col << ", "
    << row << ")" << vcl_endl;
  return xcv_image_tableau_sptr();
}

//-----------------------------------------------------------------------------
//: Gets the picker tableau at the given position.
//  Returns a new picker_tableau if it fails.
//-----------------------------------------------------------------------------
xcv_picker_tableau_sptr get_picker_tableau_at(unsigned col, unsigned row)
{
  vgui_tableau_sptr top_tab = xcv_tab->get_tableau_at(col, row);
  if (top_tab)
  {
    xcv_picker_tableau_sptr tt;
    tt.vertical_cast(vgui_find_below_by_type_name(top_tab, 
      vcl_string("xcv_picker_tableau")));
    if (tt)
      return tt;
  }
  vgui_macro_warning << "Unable to get xcv_picker_tableau at (" << col << ", "
    << row << ")" << vcl_endl;
  return xcv_picker_tableau_sptr();
}

//-----------------------------------------------------------------------------
//: Gets the underlying image from the tableau at the given position and 
//  returns it in the given image pointer.  Returns true if the image is OK, 
//  otherwise returns false.
//-----------------------------------------------------------------------------
bool get_image_at(vil_image* img, unsigned col, unsigned row)
{
  xcv_image_tableau_sptr img_tab = get_image_tableau_at(col, row);
  if (img_tab)
  {
    *img = img_tab->get_image();
    return true;
  }
  vgui_macro_warning << "Unable to get image at (" << col << ", " << row
    << ")" << vcl_endl;
  return false;
}

//-----------------------------------------------------------------------------
//: Given an image, returns a tableau suitable to display in xcv.
//-----------------------------------------------------------------------------
vgui_tableau_sptr create_tableau(vil_image img)
{
  xcv_image_tableau_new   image (img);
  vgui_easy2D_new         easy  (image);
  vgui_rubberbander_new   rubber(new vgui_rubberbander_easy2D_client(easy));
  vgui_composite_new      comp(easy,rubber);
  xcv_picker_tableau_new  picker(comp);
  vgui_viewer2D_new       view  (picker);
  return view;
}

//-----------------------------------------------------------------------------
//: Displays the given image on XCV at the given position.
//-----------------------------------------------------------------------------
void add_image_at(vcl_string image_filename, unsigned col, unsigned row)
{
  vil_image img = vil_load(image_filename.c_str());
  vgui_tableau_sptr tab = create_tableau(img);
  xcv_tab->add_at(tab, col, row);
}


//-----------------------------------------------------------------------------
//: Add image to next available slot
//-----------------------------------------------------------------------------
void add_image(vil_image& img)
{
  vgui_tableau_sptr tab = create_tableau(img);
  xcv_tab->add_next(tab);
}

//-----------------------------------------------------------------------------
//: Removes the image at the given posistion from the display.
//-----------------------------------------------------------------------------
void remove_image_at(unsigned col, unsigned row)
{
  xcv_tab->remove_at(col, row);
}

//-----------------------------------------------------------------------------
//: Create the menubar.
//-----------------------------------------------------------------------------
vgui_menu xcv_menubar;

vgui_menu create_menubar()
{
  xcv_menubar.add("File", xcv_file::create_file_menu());
  xcv_menubar.add("Display", xcv_display::create_display_menu());
  xcv_menubar.add("Image-processing", xcv_processing::create_processing_menu());
  xcv_menubar.add("Geometry", xcv_geometry::create_geometry_menu());
  xcv_menubar.add("Segmentation", xcv_segmentation::create_segmentation_menu());
  xcv_menubar.add("Multiview", xcv_multiview::create_multiview_menu());

  return xcv_menubar;
}

//-----------------------------------------------------------------------------
//: Choosing a window size
// This function chooses a window size by adding up the total image size
// along each row (column) and choosing the row (column) sum which is
// greatest as the window width (height).
//-----------------------------------------------------------------------------
void xcv_window_size_traditional(int rows, int cols,
                                 vcl_vector<vil_image> const &images,
                                 unsigned *window_w, unsigned *window_h,
                                 double *viewer_scale)
{
  assert(rows > 0 && cols > 0);

  assert(window_w && window_h && viewer_scale);
  *window_w = 0;
  *window_h = 0;
  *viewer_scale = 1;

  // set width of window
  for (int i=0; i<rows; ++i) 
  {
    unsigned int winnie = 0;
    for (int j=0; j<cols; ++j) 
    {
      unsigned int d = i*cols + j;
      if (d < images.size())
        winnie += images[d].width();
    }
    if (winnie > *window_w)
      *window_w = winnie;
  }

  // set height of window
  for (int j=0; j<cols; ++j) 
  {
    unsigned int winnie = 0;
    for (int i=0; i<rows; ++i) 
    {
      unsigned int d = i*cols + j;
      if (d < images.size())
        winnie += images[d].height();
    }
    if (winnie > *window_h)
      *window_h = winnie;
  }
  // Add a bit to the height for the menu and status bars:
  *window_h += MENUBAR_HEIGHT;
}

//-----------------------------------------------------------------------------
//: This tries to resize the window to fill some proportion of the screen.
//  Useful for very small or very large images.
//-----------------------------------------------------------------------------
void xcv_window_size_adaptive(int rows, int cols,
                              vcl_vector<vil_image> const &images,
                              unsigned *window_w, unsigned *window_h,
                              double *viewer_scale)
{
  xcv_window_size_traditional(rows, cols, images, window_w, window_h, 
    viewer_scale);

  // resize the window to occupy roughly 64% of a 1024x1280 display by area.
  double mw = 0.80 * 1280;
  double mh = 0.80 * 1024;

  double dw = *window_w;
  double dh = *window_h;
  double factor = 1.1;
  // first make it nice and big
  while (factor*dw < mw && factor*dh < mh) {
    dw *= factor;
    dh *= factor;
    *viewer_scale *= factor;
  }
  // then shrink it to fit onto the screen
  while (factor*dw > mw || factor*dh > mh) {
    dw /= factor;
    dh /= factor;
    *viewer_scale /= factor;
  }
  *window_w = unsigned(dw);
  *window_h = unsigned(dh);
}

//-----------------------------------------------------------------------------
// main.
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Select the toolkit: command line or environment variable
  // can override, but the default is 'gtk' or 'mfc'.
  if (! vgui::select(argc, argv)) 
  {
    if      (vgui::exists("gtk"))
      vgui::select("gtk");
    else if (vgui::exists("mfc"))
      vgui::select("mfc");
    else if (vgui::exists("qt"))
      vgui::select("qt");
    else 
    {
      // ??
    }
  }
  // Initialize the chosen toolkit.
  vgui::init(argc, argv);

  // Let the vbl arg parser loose on those command line
  // arguments not used by vgui::init();
  vul_arg<bool> a_adaptive("-adaptive", "resize window adaptively");
  vul_arg<int>  a_rows("-rows", "desired number of rows in array of images", 0);
  vul_arg<int>  a_cols("-cols", 
    "desired number of col(umn)s in array of images", 0);
  vul_arg_parse(argc, argv);

  int rows, cols;
  if (a_rows() && a_cols()) 
  {
    rows = a_rows();
    cols = a_cols();
  }
  else 
  {
    rows = 1;
    if (argc-1 > 0)
      cols = argc-1;
    else
      cols = 1;
  }
  xcv_tab = new xcv_tableau(rows, cols);

  unsigned window_width  = 0;
  unsigned window_height = 0;

  {
    vcl_vector<vgui_tableau_sptr> viewers;
    vcl_vector<vil_image> images;

    xcv_tab->set_grid_size_changeable(false);
    for (int argcount=1; argcount<argc && vcl_strcmp(argv[argcount], "-d"); 
      ++argcount)
    {
      vil_image img = vil_load(argv[argcount]);
      vgui_tableau_sptr tab = create_tableau(img);
      xcv_tab->add_next(tab);

      images.push_back(img);
      viewers.push_back(tab);
    }
    xcv_tab->set_grid_size_changeable(true);

    double viewer_scale;
    if (a_adaptive())
      xcv_window_size_adaptive(rows, cols, images, &window_width, 
      &window_height, &viewer_scale);
    else
      xcv_window_size_traditional(rows, cols, images, &window_width, 
      &window_height, &viewer_scale);

    for (unsigned int i=0; i<viewers.size(); ++i) 
    {
      vgui_viewer2D_sptr v; v.vertical_cast(viewers[i]);
      v->token.scaleX *= viewer_scale;
      v->token.scaleY *= viewer_scale;
    }
  }


  // safety net, in case something silly happened.
  if (window_width > 1024)
    window_width = 1024;
  if (window_height > 1024)
    window_height = 1024;

  if (window_width <= 0)
    window_width = 512;
  if (window_height <= MENUBAR_HEIGHT)
    window_height = 512;
    
  vcl_cerr << "window_width  = " << window_width << vcl_endl;
  vcl_cerr << "window_height = " << window_height << vcl_endl;

  // Create a window, add the tableau and show it on screen:
  vgui_window *win = vgui::produce_window(window_width, window_height,
                                          create_menubar(),
                                          "xcv"); // title
  win->get_adaptor()->set_tableau(vgui_shell_tableau_new(xcv_tab));
  win->set_statusbar(true);
  win->enable_vscrollbar(false);
  win->enable_hscrollbar(false);
  win->show();
  return vgui::run();
}
