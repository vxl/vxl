// This is oxl/VPLAYER/vplayer_file.cxx
#include "vplayer_file.h"
//:
// \file

#include "vplayer.h"
#include "easy2D_sequencer.h"
#include "easy2D_loader.h"

#include <vul/vul_sprintf.h>

#include <vcl_iostream.h>

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_rubberband_tableau.h>

#include <vidl_vil1/vidl_vil1_io.h>
#include <vidl_vil1/vidl_vil1_movie.h>
#include <vidl_vil1/vidl_vil1_frame.h>

vidl_vil1_movie_sptr my_movie;
vidl_vil1_movie::frame_iterator pframe(my_movie);
easy2D_sequencer *seq;
vcl_vector<vgui_easy2D_tableau_sptr> tableaux_;

//-----------------------------------------------------------------------------
//: Loads a video sequence into memory
//-----------------------------------------------------------------------------
void vplayer_file::load_video_sequence()
{
  vgui_dialog load_image_dl("Load video sequence");
  static vcl_string image_filename = "bt.%03d.mit";
  load_image_dl.field("Filename:", image_filename);
  int n = 0;
  load_image_dl.field("Number of images: ",n);
  if (!load_image_dl.ask())
    return;
  vcl_vector <vcl_string> img_filenames;
  for (int i = 0;i<n;i++)
  {
    vul_sprintf name(image_filename.c_str(), i);
    img_filenames.push_back(name);
    vcl_cerr<<"Registering image: "<<name<<vcl_endl;
  }
  my_movie = vidl_vil1_io::load_movie(img_filenames);
  if (!my_movie) {
    vgui_error_dialog("Failed to load movie");
    return;
  }
  pframe = my_movie->first();
  unsigned col,row;
  get_current(&col,&row);
  tableaux_.clear();
  while (pframe!=my_movie->last())
  {
    tableaux_.push_back(vgui_easy2D_tableau_new(vgui_image_tableau_new(pframe->get_image())));
    ++pframe;
  }
  tableaux_.push_back(vgui_easy2D_tableau_new(vgui_image_tableau_new(pframe->get_image())));
  pframe = my_movie->first();
  vgui_rubberband_tableau_sptr r = get_rubberbander_at(col,row);
  r->init(new vgui_rubberband_easy2D_client(tableaux_.front()));
  tableaux_.front()->post_redraw();
}

//-----------------------------------------------------------------------------
//: Loads a video sequence into memory
//-----------------------------------------------------------------------------
void vplayer_file::load_video_file()
{
  vgui_dialog load_image_dl("Load video file");
  static vcl_string image_filename = "C:\\tmp\\shop1.avi";
  static vcl_string glob = "*.avi";
  load_image_dl.file("Filename:", glob, image_filename);
  if (!load_image_dl.ask())
    return;
  my_movie = vidl_vil1_io::load_movie(image_filename.c_str());
  if (!my_movie) {
    vgui_error_dialog("Failed to load movie file");
    return;
  }
  pframe = my_movie->first();
  unsigned col,row;
  get_current(&col,&row);
  tableaux_.clear();
  while (pframe!=my_movie->last())
  {
    tableaux_.push_back(vgui_easy2D_tableau_new(vgui_image_tableau_new(pframe->get_image())));
    ++pframe;
  }
  tableaux_.push_back(vgui_easy2D_tableau_new(vgui_image_tableau_new(pframe->get_image())));
  pframe = my_movie->first();
  vgui_rubberband_tableau_sptr r = get_rubberbander_at(col,row);
  r->init(new vgui_rubberband_easy2D_client(tableaux_.front()));
  tableaux_.front()->post_redraw();
}

//-----------------------------------------------------------------------------
//: Loads overlay geometry sequence into memory
//-----------------------------------------------------------------------------
void vplayer_file::load_geometry_sequence()
{
  vgui_dialog load_image_dl("Load geometry sequence");
  static vcl_string image_filename = "bt.%03d.gx";
  load_image_dl.field("Filename:", image_filename);
  int n = 0;
  load_image_dl.field("Number of geometry sequences: ",n);
  if (!load_image_dl.ask())
    return;
  vcl_vector <vcl_string> img_filenames;
  vcl_vector <vgui_easy2D_tableau_sptr>::iterator it = tableaux_.begin();
  for (int i = 0;i<n;i++)
  {
    vul_sprintf name(image_filename.c_str(), i);
    easy2D_loader loader(*it);
    loader.visit(name);
    it++;
  }
}

//-----------------------------------------------------------------------------
//: Save geometry sequence into memory
//-----------------------------------------------------------------------------
void vplayer_file::save_geometry_sequence()
{
}
//-----------------------------------------------------------------------------
//: Save geometry sequence into memory
//-----------------------------------------------------------------------------
void vplayer_file::save_video_sequence()
{
}
//-----------------------------------------------------------------------------
//: Quit the video player
//-----------------------------------------------------------------------------
void vplayer_file::exit_vplayer()
{
  vgui_dialog quit_dl("Quit");
  quit_dl.message("   Are you sure you want to quit the video player?   ");

  if (quit_dl.ask())
    vgui::quit();
}

//-----------------------------------------------------------------------------
//: Creates a menu containing all the functions in this file.
//-----------------------------------------------------------------------------
vgui_menu vplayer_file::create_file_menu()
{
  vgui_menu file_menu;
  file_menu.add("Load video sequence",load_video_sequence);
  file_menu.add("Load video file",load_video_file);
  file_menu.add("Load geometry sequence",load_geometry_sequence);
  file_menu.separator();
  //file_menu.add("Save video sequence",save_video_sequence);
  //file_menu.add("Save geometry sequence",save_geometry_sequence);
  //file_menu.separator();
  file_menu.add("Exit", exit_vplayer);
  return file_menu;
}
