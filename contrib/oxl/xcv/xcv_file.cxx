// This is oxl/xcv/xcv_file.cxx
#include <iostream>
#include "xcv_file.h"
//:
//  \file
// \author  K.Y.McGaul
//
// See xcv_file.h for a description of this file.
//
// \verbatim
//  Modifications
//    K.Y.McGaul  27-JAN-2000   Initial version.
// \endverbatim
//

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil1/vil1_image.h>
#include <vil1/vil1_save.h>

#include <vgui/vgui.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_rubberband_tableau.h>

#include <xcv/xcv_image_tableau.h>

extern std::string* get_loadfile();
extern std::string* get_savefile();
extern void get_current(unsigned*, unsigned*);
extern void add_image_at(std::string, unsigned, unsigned);
extern bool get_image_at(vil1_image*, unsigned, unsigned);
extern vgui_rubberband_tableau_sptr get_rubberbander_at(unsigned, unsigned);
extern vgui_tableau_sptr get_top(unsigned,unsigned);
extern void remove_image_at(unsigned, unsigned);
extern vgui_easy2D_tableau_sptr get_easy2D_at(unsigned, unsigned);
extern xcv_image_tableau_sptr get_image_tableau_at(unsigned col, unsigned row);

//-----------------------------------------------------------------------------
//: Load the selected image file into the current grid position.
//-----------------------------------------------------------------------------
void xcv_file::load_image()
{
  unsigned col, row;
  get_current(&col, &row);

  vgui_dialog load_image_dl("Load image");
  std::string* image_filename = get_loadfile();
  static std::string regexp = "*.*";
  load_image_dl.inline_file("Filename of image:", regexp, *image_filename);

  if (!load_image_dl.ask())
    return;

#ifdef DEBUG
  std::cerr << "Loading image file: " << *image_filename << std::endl;
#endif
  add_image_at(*image_filename, col, row);
}

//-----------------------------------------------------------------------------
//: Save image to specified file.
//-----------------------------------------------------------------------------
void xcv_file::save_image()
{
  vgui_dialog load_image_dl(" Save image");
  unsigned col,row;
  get_current(&col,&row);
  xcv_image_tableau_sptr imt = get_image_tableau_at(col,row);
  static std::string* image_filename = get_savefile();
  static std::string regexp = "*.*";
  load_image_dl.inline_file("Filename to save image:", regexp, *image_filename);
  std::vector<std::string> labels;
  labels.push_back(std::string("JPEG"));
  labels.push_back(std::string("TIFF"));
  labels.push_back(std::string("PNM"));
  labels.push_back(std::string("PNG"));
  labels.push_back(std::string("IRIS"));
  labels.push_back(std::string("MIT"));
  labels.push_back(std::string("VIFF"));
  static int choice_value = 0;
  load_image_dl.choice("File format ",labels,choice_value);
  if (load_image_dl.ask())
  {
#ifdef DEBUG
    std::cerr << "Saving image to file: " << image_filename->c_str() << std::endl;
#endif
    static const char *format[] = {
      "jpg","tiff","pnm","png","iris","mit","viff"};
    vil1_save(imt->get_image(),image_filename->c_str(),format[choice_value]);
  }
}

//-----------------------------------------------------------------------------
//: Dump OpenGL buffer to specified file.
//-----------------------------------------------------------------------------
void xcv_file::dump_image()
{
  vgui_dialog dump_image_dl(" Dump window to image file");
  static std::string* image_filename = get_savefile();
  static std::string regexp = "*.*";
  dump_image_dl.inline_file("Filename to save image:", regexp, *image_filename);
  if (dump_image_dl.ask())
  {
#ifdef DEBUG
    std::cerr << "Saving image to file: " << image_filename->c_str() << std::endl;
#endif
    vgui_utils::dump_colour_buffer( image_filename->c_str() );
  }
}

//-----------------------------------------------------------------------------
//: Save image and spatial objects to specified file.
//-----------------------------------------------------------------------------
void xcv_file::save_as_ps()
{
  unsigned col, row;
  get_current(&col, &row);

  vgui_dialog ps_dl("Save postscript ");
  std::string* image_filename = get_savefile();
  static std::string regexp = "*.ps";
  static bool save_objs = true;
  static int reduction_factor = 1;
  ps_dl.inline_file("Enter file name to save to:", regexp, *image_filename);
  ps_dl.field("Reduction factor:", reduction_factor);
  ps_dl.checkbox("Save spatial objects?", save_objs);
  if (ps_dl.ask())
  {
#ifdef DEBUG
    std::cerr << "Saving data to postscript file: "<< image_filename << std::endl;
#endif
    vgui_easy2D_tableau_sptr easy = get_easy2D_at(col, row);
    if (easy)
      easy->print_psfile(*image_filename, reduction_factor, save_objs);
  }
}

//-----------------------------------------------------------------------------
//: Remove currently selected image.
//-----------------------------------------------------------------------------
void xcv_file::remove_image()
{
  unsigned col, row;
  get_current(&col, &row);

  vgui_dialog dl("Warning");
  dl.message(" ");
  dl.message("   Are you sure you want to delete the current image?");
  dl.message(" ");

  if (dl.ask())
    remove_image_at(col, row);
}


//-----------------------------------------------------------------------------
//: Exit XCV.
//-----------------------------------------------------------------------------
void xcv_file::exit_xcv()
{
  vgui_dialog quit_dl("Quit");
  quit_dl.message(" ");
  quit_dl.message("   Are you sure you want to quit XCV?   ");
  quit_dl.message(" ");

  if (quit_dl.ask())
    vgui::quit();
}

//-----------------------------------------------------------------------------
//: Creates a menu containing all the functions in this file.
//-----------------------------------------------------------------------------
vgui_menu xcv_file::create_file_menu()
{
  vgui_menu file_menu;
  file_menu.add("Load image", load_image);
  file_menu.add("Save image", save_image);
  file_menu.add("Dump window to image file", dump_image);
  file_menu.add("Save as Postscript", save_as_ps);
  file_menu.add("Remove image", remove_image);
  file_menu.separator();
  file_menu.add("Exit", exit_xcv);
  return file_menu;
}
