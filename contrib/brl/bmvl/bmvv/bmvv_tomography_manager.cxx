// This is brl/bmvl/bmvv/bmvv_tomography_manager.cxx
#include "bmvv_tomography_manager.h"
//:
// \file
// \author J.L. Mundy

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_iostream.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <bxml/bxml_vtol_io.h>
#include <vgui/vgui.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_soview.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_picker_tableau.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_modifier.h>
#include <sdet/sdet_region_proc_params.h>
#include <sdet/sdet_region_proc.h>
#include <brip/brip_vil1_float_ops.h>
#include <btom/btom_slice_simulator.h>

//static live_video_manager instance
bmvv_tomography_manager *bmvv_tomography_manager::instance_ = 0;
//===============================================================
//: The singleton pattern - only one instance of the manager can occur
//==============================================================
bmvv_tomography_manager *bmvv_tomography_manager::instance()
{
  if (!instance_)
    {
      instance_ = new bmvv_tomography_manager();
      instance_->init();
    }
  return bmvv_tomography_manager::instance_;
}

//==================================================================
//: constructors/destructor
//==================================================================
bmvv_tomography_manager::
bmvv_tomography_manager() : vgui_wrapper_tableau()
{
}

bmvv_tomography_manager::~bmvv_tomography_manager()
{
}

//======================================================================
//: set up the tableaux at each grid cell
//  the vtol2D_tableaux have been initialized in the constructor
//======================================================================
void bmvv_tomography_manager::init()
{
  grid_ = vgui_grid_tableau_new(2,1);
  grid_->set_grid_size_changeable(true);
  unsigned int col=0, row = 0;
  for (; col<2; col++)
    {
      vgui_image_tableau_sptr itab = bgui_image_tableau_new();
      bgui_vtol2D_tableau_sptr btab = bgui_vtol2D_tableau_new(itab);
      vtol_tabs_.push_back(btab);
      bgui_picker_tableau_new pcktab(btab);
      vgui_viewer2D_tableau_sptr v2d = vgui_viewer2D_tableau_new(pcktab);
      grid_->add_at(v2d, col, row);
    }
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid_);
  this->add_child(shell);
}

//=========================================================================
//: make an event handler
// note that we have to get an adaptor and set the tableau to receive events
// this handler does nothing but is a place holder for future event processing
// For now, just pass the events down to the child tableaux
//=========================================================================
bool bmvv_tomography_manager::handle(const vgui_event &e)
{
  return this->child.handle(e);
}

//====================================================================
//: Gets the picker tableau at the given position.
//  Returns a null tableau if it fails
//====================================================================
bgui_picker_tableau_sptr
bmvv_tomography_manager::get_picker_tableau_at(unsigned col, unsigned row)
{
  vgui_tableau_sptr top_tab = grid_->get_tableau_at(col, row);
  if (top_tab)
    {
      bgui_picker_tableau_sptr tt;
      tt.vertical_cast(vgui_find_below_by_type_name(top_tab,
                                                    vcl_string("bgui_picker_tableau")));
      if (tt)
        return tt;
    }
  vgui_macro_warning << "Unable to get bgui_picker_tableau at (" << col << ", "
                     << row << ")\n";
  return 0;
}

//====================================================================
//: Gets the picker tableau at the currently selected grid position
//  Returns a null tableau if it fails
//====================================================================
bgui_picker_tableau_sptr bmvv_tomography_manager::get_selected_picker_tableau()
{
  unsigned int row =0, col=0;
  grid_->get_last_selected_position(&col, &row);
  return this->get_picker_tableau_at(col, row);
}

//====================================================================
//: Gets the picker tableau at the given position.
//  if col, row are out of bounds then null is returned
//  row is currently not used but may be when we have more than 2 cameras
//====================================================================
bgui_vtol2D_tableau_sptr bmvv_tomography_manager::get_vtol2D_tableau_at(unsigned col, unsigned row)
{
  if (row!=0)
    return 0;
  bgui_vtol2D_tableau_sptr btab = 0;
  if (col==0||col==1)
    btab = vtol_tabs_[col];
  return btab;
}

//=================================================================
//: Get the vtol_2D_tableau at the currently selected grid cell.
//=================================================================
bgui_vtol2D_tableau_sptr bmvv_tomography_manager::get_selected_vtol2D_tableau()
{
  unsigned int row =0, col=0;
  grid_->get_last_selected_position(&col, &row);
  return this->get_vtol2D_tableau_at(col, row);
}

void bmvv_tomography_manager::quit()
{
  vcl_exit(1);
}

//=========================================================================
//: load an image an put it in the currently selected grid cell
//=========================================================================
void bmvv_tomography_manager::load_image()
{
  bool greyscale = false;
  vgui_dialog load_image_dlg("Load Image");
  static vcl_string image_filename = "";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  load_image_dlg.checkbox("greyscale ", greyscale);
  if (!load_image_dlg.ask())
    return;
  img_ = vil1_load(image_filename.c_str());
  bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
  if (btab)
    {
      vgui_image_tableau_sptr itab = btab->get_image_tableau();
      itab->set_image(img_);
      return;
    }
  vcl_cout << "In bmvv_tomography_manager::load_image() - null tableau\n";
}

//=========================================================================
//: Save the sinogram and its corresponding image
//=========================================================================
void bmvv_tomography_manager::save_sinogram()
{
  if (!sino_||!img_)
    {
      vcl_cout << "In bmvv_tomography_manager::save_sinogram() - no images\n";
      return;
    }
  vgui_dialog load_image_dlg("Save Sinogram");
  static vcl_string image_filename = "", sino_filename="";
  static vcl_string ext = "*.*";
  load_image_dlg.file("Image Filename:", ext, image_filename);
  load_image_dlg.file("Sinogram Filename:", ext, sino_filename);
  if (!load_image_dlg.ask())
    return;
  vil1_save(img_, image_filename.c_str(), "tiff");
  vil1_save(sino_, sino_filename.c_str(), "tiff");
}

//===================================================================
//: Clear the display
//===================================================================
void bmvv_tomography_manager::clear_display()
{
  bgui_vtol2D_tableau_sptr btab = this->get_selected_vtol2D_tableau();
  if (btab)
    btab->clear();
  else
    vcl_cout << "In bmvv_tomography_manager::clear_display() - null tableau\n";
}

//===================================================================
//: clear all selections in both panes
//===================================================================
void bmvv_tomography_manager::clear_selected()
{
  for (vcl_vector<bgui_vtol2D_tableau_sptr>::iterator bit = vtol_tabs_.begin();
       bit != vtol_tabs_.end(); bit++)
    if (*bit)
      (*bit)->deselect_all();
}


//===================================================================
//: compute and display a gaussian sinogram
//===================================================================
void bmvv_tomography_manager::sinogram()
{
  static float scale;
  static btom_slice_simulator_params ssp;
  vgui_dialog gauss_dialog("Gauss Sinogram");
  gauss_dialog.field("N Cylinders ", ssp.ncyl_);
  gauss_dialog.field("min radius", ssp.min_xy_sigma_);
  gauss_dialog.field("max radius", ssp.max_xy_sigma_);
  gauss_dialog.field("display scale", scale);
  if (!gauss_dialog.ask())
    return;
  btom_slice_simulator ss(ssp);
  vil1_memory_image_of<float> sinogram;
  vil1_memory_image_of<float> reconst;
  ss.gaussian_sinogram(sinogram, reconst);
  //vil1_memory_image_of<unsigned char> char_sinogram =
  sino_= brip_vil1_float_ops::convert_to_byte(sinogram, 0.0, scale);
  //  vil1_memory_image_of<unsigned char> char_reconst =
  img_ =  brip_vil1_float_ops::convert_to_byte(reconst);
  bgui_vtol2D_tableau_sptr btab0 = vtol_tabs_[0];
  bgui_vtol2D_tableau_sptr btab1 = vtol_tabs_[1];
  if (btab0)
    {
      vgui_image_tableau_sptr itab = btab0->get_image_tableau();
      itab->set_image(img_);
    }
  if (btab1)
    {
      vgui_image_tableau_sptr itab = btab1->get_image_tableau();
      itab->set_image(sino_);
    }
}
