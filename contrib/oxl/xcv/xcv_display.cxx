//-----------------------------------------------------------------------------
// .DESCRIPTION:
//   See xcv_display.h.
//-----------------------------------------------------------------------------

#include <vcl_cstdio.h> // sprintf()
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_enhance.h>
#include <vgui/vgui_easy2D.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_composite.h>
#include <vgui/vgui_rubberbander.h>
#include <vgui/vgui_roi_tableau.h>
#include <vgui/vgui_event_server.h>
#include <xcv/xcv_image_tableau.h>
#include "xcv_display.h"

//vgui_roi_tableau_make_roi *xcv_display::roi_tableau_client_ = 0;
extern void post_to_status_bar(const char*);
extern void get_current(unsigned*, unsigned*);
extern vcl_vector<xcv_image_tableau_ref> get_image_list();
extern vgui_easy2D_ref get_easy2D_at(unsigned, unsigned);
extern vgui_composite_ref get_composite_at(unsigned, unsigned);
extern vgui_viewer2D_ref get_viewer2D_at(unsigned, unsigned);
extern xcv_image_tableau_ref get_image_tableau_at(unsigned, unsigned);
extern vgui_rubberbander_ref get_rubberbander_at(unsigned, unsigned);
extern vgui_tableau_ref get_top(unsigned,unsigned);
extern bool get_image_at(vil_image*, unsigned, unsigned);


static bool debug = true;
static bool is_magnifying = false;
static bool is_enhancing = false;
static vgui_composite_ref comp;
static vgui_enhance_ref enhance;
static xcv_image_tableau_ref img;
static vgui_easy2D_ref easy;

//-----------------------------------------------------------------------------
//-- Centre the current image.
//-----------------------------------------------------------------------------
void xcv_display::centre_image()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_viewer2D_ref view = get_viewer2D_at(col, row);
 
  vil_image image;
  if (get_image_at(&image, col, row))
    //view->center_image(image.width(), image.height());
    view->center_event();
}

//-----------------------------------------------------------------------------
//-- Toggle display of the enhance lens.
//   This is done by slotting in an enhance tableau above the image tableau.
//-----------------------------------------------------------------------------
void xcv_display::toggle_enhance()
{
  if (is_magnifying)
    toggle_magnify();

  if (is_enhancing == false)
  {
    unsigned col, row;
    get_current(&col, &row);
    img = get_image_tableau_at(col, row);
    if (!img) return;
    easy = get_easy2D_at(col, row);
    if (!easy) return;
   
    // Get the full list of images and get the user to select one:
    vcl_vector<xcv_image_tableau_ref> img_tabs = get_image_list();
    static int selected_image = 1;
    vcl_vector<vcl_string> labels;
    for (unsigned int i=0; i<img_tabs.size(); i++)
      labels.push_back(img_tabs[i]->file_name());     
    vgui_dialog dl("Images");
    dl.choice("Enhance image:", labels, selected_image);
    if (!dl.ask())
      return;
 
    // Replace the image with an enhance tableau containing the same image:
    enhance = vgui_enhance_new(img, img_tabs[selected_image]);
    easy->set_child(enhance);
    
    is_enhancing = true;
   
    char msg[100];
    sprintf(msg, "Enhance lens added to position (%d, %d).", col, row); 
    post_to_status_bar(msg);
  }
  else
  {
    if(debug) vcl_cout << "removing enhance lens" << vcl_endl;

    vgui_slot::replace_child_everywhere(enhance, img);
    is_enhancing = false;
  }
}

//-----------------------------------------------------------------------------
//-- Toggle display of the magnify glass.
//   This is done by slotting in an enhance tableau above the image tableau.
//-----------------------------------------------------------------------------
void xcv_display::toggle_magnify()
{ 
  if (is_enhancing)
    toggle_enhance();

  if (is_magnifying == false)
  {
    if (debug) vcl_cerr << "adding magnify lens" << vcl_endl;

    unsigned col, row;
    get_current(&col, &row);
    comp = get_composite_at(col, row);
    if (!comp) return;

    enhance = vgui_enhance_new();
    vgui_slot::replace_child_everywhere(comp, enhance);
    enhance->set_child(comp);

    is_magnifying = true;
  }
  else
  {
    if(debug) vcl_cout << "removing magnify lens" << vcl_endl;

    enhance->set_child(0);
    vgui_slot::replace_child_everywhere(enhance, comp);
    enhance = vgui_enhance_ref(); //0;
    is_magnifying = false;
  } 
}
//-----------------------------------------------------------------------------
//-- Define region of interest
//-----------------------------------------------------------------------------
void xcv_display::make_roi()
{
  unsigned col, row;

  get_current(&col, &row);
  xcv_image_tableau_ref imt = get_image_tableau_at(col,row);
  vgui_rubberbander_ref rubber = get_rubberbander_at(col, row);
  if(!rubber)
    return;

  //roi_tableau_client_ = new vgui_roi_tableau_make_roi(imt);
  vgui_roi_tableau_make_roi roi_tableau_client_(imt);
  
  vgui_event_server *es = new vgui_event_server(imt);
  vgui_rubberbander_client* old_client = rubber->get_client();  // save to put back in later
  rubber->set_client(&roi_tableau_client_);
  rubber->rubberband_box();
  while(!roi_tableau_client_.is_done())
    es->next();
  es = 0;
  rubber->set_client(old_client);
  //roi_tableau_client_ = 0;
}
//-----------------------------------------------------------------------------
//-- Remove region of interest
//-----------------------------------------------------------------------------
void xcv_display::remove_roi()
{
  unsigned col, row;
  get_current(&col,&row);
  vgui_rubberbander_ref rubber = get_rubberbander_at(col, row);
  
  //if(rubber)
  //{  
    //rubber->set_client(0);
    //delete roi_tableau_client_;
    //roi_tableau_client_ = 0;
  //}
  xcv_image_tableau_ref imt = get_image_tableau_at(col,row);
  imt->unset_roi();
}

//-----------------------------------------------------------------------------
//-- Creates a menu containing all the functions in this file.
//-----------------------------------------------------------------------------
vgui_menu xcv_display::create_display_menu()
{
  vgui_menu display_menu;
  //display_menu.add("Centre image", centre_image);
  //display_menu.separator();
  display_menu.add("Toggle magnify lens", toggle_magnify);
  display_menu.add("Toggle enhance lens", toggle_enhance);
  display_menu.separator();
  display_menu.add("Make ROI",make_roi);
  display_menu.add("Remove ROI",remove_roi);
  return display_menu;
}

