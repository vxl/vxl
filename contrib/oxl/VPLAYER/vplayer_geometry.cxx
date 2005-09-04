// This is oxl/VPLAYER/vplayer_geometry.cxx
#include "vplayer_geometry.h"
//:
// \file

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_rubberband_tableau.h>
#include <vgui/vgui_color_text.h>
#include <vgui/vgui_soview.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_macro.h>

#include "vplayer.h"

//static bool debug = true;
extern void get_current(unsigned*, unsigned*);
extern vcl_vector<vgui_easy2D_tableau_sptr> get_easy2D_list();

//-----------------------------------------------------------------------------
//: Draw a point onto the currently selected tableau.
//-----------------------------------------------------------------------------
void vplayer_geometry::create_point()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_rubberband_tableau_sptr rubber = get_rubberbander_at(col, row);
  if (rubber)
    rubber->rubberband_point();
}

//-----------------------------------------------------------------------------
//: Rubberband a line on the currently selected tableau.
//-----------------------------------------------------------------------------
void vplayer_geometry::create_line()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_rubberband_tableau_sptr rubber = get_rubberbander_at(col, row);
  if (rubber)
    rubber->rubberband_line();
}

//-----------------------------------------------------------------------------
//: Rubberband a circle on the currently selected tableau.
//-----------------------------------------------------------------------------
void vplayer_geometry::create_circle()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_rubberband_tableau_sptr rubber = get_rubberbander_at(col, row);
  if (rubber)
    rubber->rubberband_circle();
}

//-----------------------------------------------------------------------------
//: Rubberband a polygon on the currently selected tableau.
//-----------------------------------------------------------------------------
void vplayer_geometry::create_polygon()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_rubberband_tableau_sptr rubber = get_rubberbander_at(col, row);
  if (rubber)
    rubber->rubberband_polygon();
}

//-----------------------------------------------------------------------------
//: Rubberband a linestrip on the currently selected tableau.
//-----------------------------------------------------------------------------
void vplayer_geometry::create_linestrip()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_rubberband_tableau_sptr rubber = get_rubberbander_at(col, row);
  if (rubber)
    rubber->rubberband_linestrip();
}

//-----------------------------------------------------------------------------
//: Rubberband infinite line on the currently selected tableau.
//-----------------------------------------------------------------------------
void vplayer_geometry::create_infinite_line()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_rubberband_tableau_sptr rubber = get_rubberbander_at(col, row);
  if (rubber)
    rubber->rubberband_infinite_line();
}

//-----------------------------------------------------------------------------
//: Change the color of all the selected geometric objects.
//-----------------------------------------------------------------------------
void vplayer_geometry::change_sel_color()
{
  static vcl_string  color_value = "yellow";
  vgui_dialog color_dl("Colour of selected objects");
  color_dl.inline_color("New colour for selected objects:", color_value);
  if (!color_dl.ask())
    return;

  vcl_vector<vgui_easy2D_tableau_sptr> easy_list = get_easy2D_list();
  for (unsigned i=0; i<easy_list.size(); i++)
  {
    vcl_vector<vgui_soview*> sel_objs = easy_list[i]->get_selected_soviews();
    for (vcl_vector<vgui_soview*>::iterator iter = sel_objs.begin();
         iter != sel_objs.end(); iter++)
    {
      vgui_soview* sv = (vgui_soview*)(*iter);
      sv->set_colour(red_value(color_value),
                     green_value(color_value),
                     blue_value(color_value));
    }
    easy_list[i]->deselect_all();
  }
}

//-----------------------------------------------------------------------------
//: Change the radius of all the selected points.
//-----------------------------------------------------------------------------
void vplayer_geometry::change_sel_radius()
{
  static float point_radius = 1;
  vgui_dialog radius_dl("Point radius");
  radius_dl.field("Radius for selected points:", point_radius);
  if (!radius_dl.ask())
    return;

  vcl_vector<vgui_easy2D_tableau_sptr> easy_list = get_easy2D_list();
  for (unsigned i=0; i<easy_list.size(); i++)
  {
    vcl_vector<vgui_soview*> sel_objs = easy_list[i]->get_selected_soviews();
    for (vcl_vector<vgui_soview*>::iterator it = sel_objs.begin(); it != sel_objs.end(); it++)
    {
      vgui_soview* sv = (vgui_soview*)(*it);
      if (sv->type_name() == "vgui_soview2D_point")
        sv->set_point_size(point_radius);
    }
    easy_list[i]->deselect_all();
  }
}

//-----------------------------------------------------------------------------
//: Change the line widths of all the selected geometric objects
// (that aren't points).
//-----------------------------------------------------------------------------
void vplayer_geometry::change_sel_width()
{
  static float line_width = 1.0;
  vgui_dialog width_dl("Line width");
  width_dl.field("Line width for selected objects:", line_width);
  if (!width_dl.ask())
    return;

  vcl_vector<vgui_easy2D_tableau_sptr> easy_list = get_easy2D_list();
  for (unsigned i=0; i<easy_list.size(); i++)
  {
    vcl_vector<vgui_soview*> sel_objs = easy_list[i]->get_selected_soviews();
    for (vcl_vector<vgui_soview*>::iterator it = sel_objs.begin(); it != sel_objs.end(); it++)
    {
      vgui_soview* sv = (vgui_soview*)(*it);
      if (sv->type_name() != "vgui_soview2D_point")
        sv->set_line_width(line_width);
    }
    easy_list[i]->deselect_all();
  }
}

//-----------------------------------------------------------------------------
//: Delete all the selected geometric objects.
//-----------------------------------------------------------------------------
void vplayer_geometry::delete_sel_objs()
{
  vgui_dialog del_dl("WARNING");
  del_dl.message("");
  del_dl.message("Are you sure you want to delete all the ");
  del_dl.message("selected geometric objects?");
  del_dl.message("");
  if (!del_dl.ask())
    return;

  vcl_vector<vgui_easy2D_tableau_sptr> easy_list = get_easy2D_list();
  for (unsigned i=0; i<easy_list.size(); i++)
  {
    vcl_vector<vgui_soview*> sel_objs = easy_list[i]->get_selected_soviews();
    for (vcl_vector<vgui_soview*>::iterator j = sel_objs.begin(); j != sel_objs.end(); j++)
      easy_list[i]->remove((vgui_soview*)(*j));
  }
}

//-----------------------------------------------------------------------------
//: Delete all geometric objects.
//-----------------------------------------------------------------------------
void vplayer_geometry::delete_all()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_dialog del_dl("WARNING");
  del_dl.message("");
  del_dl.message("Are you sure you want to delete all geometric objects ");
  del_dl.message("from the selected view?");
  del_dl.message("");
  if (!del_dl.ask())
    return;

  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col, row);
  if (!easy_tab)
  {
    vgui_macro_warning << "Unable to get current easy2D to delete te objects\n";
    return;
  }

  vcl_vector<vgui_soview*> sel_objs = easy_tab->get_all();
  for (vcl_vector<vgui_soview*>::iterator i = sel_objs.begin(); i != sel_objs.end(); i++)
  {
    easy_tab->remove((vgui_soview*)(*i));
  }
}

//-----------------------------------------------------------------------------
//: Delete all points
//-----------------------------------------------------------------------------
void vplayer_geometry::delete_points()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_dialog del_dl("WARNING");
  del_dl.message("");
  del_dl.message("Are you sure you want to delete all the points?");
  del_dl.message("");
  if (!del_dl.ask())
    return;

  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col,row);
  if (!easy_tab)
  {
    vgui_macro_warning << "Unable to get current easy2D to delete  points\n";
    return;
  }
  vcl_vector<vgui_soview*> all_objs = easy_tab->get_all();
  for (vcl_vector<vgui_soview*>::iterator i = all_objs.begin(); i != all_objs.end(); i++)
  {
    if ((*i)->type_name() == "vgui_soview2D_point")
        easy_tab->remove((vgui_soview*)(*i));
  }
}

//-----------------------------------------------------------------------------
//: Delete all lines
//-----------------------------------------------------------------------------
void vplayer_geometry::delete_lines()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_dialog del_dl("WARNING");
  del_dl.message("");
  del_dl.message("Are you sure you want to delete all the lines?");
  del_dl.message("");
  if (!del_dl.ask())
    return;

  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col,row);
  if (!easy_tab)
  {
    vgui_macro_warning << "Unable to get current easy2D to delete lines\n";
    return;
  }
  vcl_vector<vgui_soview*> all_objs = easy_tab->get_all();
  for (vcl_vector<vgui_soview*>::iterator i = all_objs.begin(); i != all_objs.end(); i++)
  {
    if ((*i)->type_name() == "vgui_soview2D_lineseg")
        easy_tab->remove((vgui_soview*)(*i));
  }
}

//-----------------------------------------------------------------------------
//: Delete all infinite lines
//-----------------------------------------------------------------------------
void vplayer_geometry::delete_inf_lines()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_dialog del_dl("WARNING");
  del_dl.message("");
  del_dl.message("Are you sure you want to delete all the infinite lines?");
  del_dl.message("");
  if (!del_dl.ask())
    return;

  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col,row);
  if (!easy_tab)
  {
    vgui_macro_warning << "Unable to get current easy2D to delete infinite lines?\n";
    return;
  }
  vcl_vector<vgui_soview*> all_objs = easy_tab->get_all();
  for (vcl_vector<vgui_soview*>::iterator i = all_objs.begin(); i != all_objs.end(); i++)
  {
    if ((*i)->type_name() == "vgui_soview2D_infinite_line")
        easy_tab->remove((vgui_soview*)(*i));
  }
}

//-----------------------------------------------------------------------------
//: Delete all circles
//-----------------------------------------------------------------------------
void vplayer_geometry::delete_circles()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_dialog del_dl("WARNING");
  del_dl.message("");
  del_dl.message("Are you sure you want to delete all the circles?");
  del_dl.message("");
  if (!del_dl.ask())
    return;

  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col,row);
  if (!easy_tab)
  {
    vgui_macro_warning << "Unable to get current easy2D to delete circles?\n";
    return;
  }
  vcl_vector<vgui_soview*> all_objs = easy_tab->get_all();
  for (vcl_vector<vgui_soview*>::iterator i = all_objs.begin(); i != all_objs.end(); i++)
  {
    if ((*i)->type_name() == "vgui_soview2D_circle")
        easy_tab->remove((vgui_soview*)(*i));
  }
}

//-----------------------------------------------------------------------------
//: Delete all linestrips
//-----------------------------------------------------------------------------
void vplayer_geometry::delete_linestrips()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_dialog del_dl("WARNING");
  del_dl.message("");
  del_dl.message("Are you sure you want to delete all the linestrips?");
  del_dl.message("");
  if (!del_dl.ask())
    return;

  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col,row);
  if (!easy_tab)
  {
    vgui_macro_warning << "Unable to get current easy2D to delete linestrips\n";
    return;
  }
  vcl_vector<vgui_soview*> all_objs = easy_tab->get_all();
  for (vcl_vector<vgui_soview*>::iterator i = all_objs.begin(); i != all_objs.end(); i++)
  {
    if ((*i)->type_name() == "vgui_soview2D_linestrips")
        easy_tab->remove((vgui_soview*)(*i));
  }
}

//-----------------------------------------------------------------------------
//: Change the default color for geometric objects.
//   Note, this will apply to geometric objects created in the future, not
//   to geometric objects already on the tableau.
//-----------------------------------------------------------------------------
void vplayer_geometry::change_default_color()
{
  static vcl_string  color_value = "yellow";
  vgui_dialog color_dl("Default colour");
  color_dl.inline_color("New default color:", color_value);
  if (!color_dl.ask())
    return;

  vcl_vector<vgui_easy2D_tableau_sptr> easy_list = get_easy2D_list();
  for (unsigned i=0; i<easy_list.size(); i++)
  {
    easy_list[i]->set_foreground(red_value(color_value),
                                 green_value(color_value),
                                 blue_value(color_value));
  }
}

//-----------------------------------------------------------------------------
//: Change the default radius for points.
//   Note, this will apply to points created in the future, not to points
//   already on the tableau.
//-----------------------------------------------------------------------------
void vplayer_geometry::change_default_radius()
{
  static int point_radius = 1;
  vgui_dialog radius_dl("Default point radius");
  radius_dl.field("Default point radius:", point_radius);
  if (!radius_dl.ask())
    return;

  vcl_vector<vgui_easy2D_tableau_sptr> easy_list = get_easy2D_list();
  for (unsigned i=0; i<easy_list.size(); i++)
  {
    easy_list[i]->set_point_radius(point_radius);
  }
}

//-----------------------------------------------------------------------------
//: Change the default line width for geometric objects.
//   Note, this will apply to geometric objects created in the future, not
//   to geometric objects already on the tableau.
//-----------------------------------------------------------------------------
void vplayer_geometry::change_default_width()
{
  static int line_width = 1;
  vgui_dialog width_dl("Default line width");
  width_dl.field("Default line width:", line_width);
  if (!width_dl.ask())
    return;

  vcl_vector<vgui_easy2D_tableau_sptr> easy_list = get_easy2D_list();
  for (unsigned i=0; i<easy_list.size(); i++)
    easy_list[i]->set_line_width(line_width);
}

//-----------------------------------------------------------------------------
//: Write coordinates into a file
//-----------------------------------------------------------------------------
void vplayer_geometry::save(const char *object_type,const char *dialog_name)
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col, row);
  vgui_dialog save_dl(dialog_name);
  static vcl_string filename = "temp.dat";
  static vcl_string regexp = "*.*";
  save_dl.inline_file("Filename: ", regexp, filename);
  vcl_ofstream fs;
  if (save_dl.ask())
  {
    fs.open(filename.c_str());
    vcl_vector<vgui_soview*> all_objs = easy_tab->get_all();
    for (vcl_vector<vgui_soview*>::iterator i = all_objs.begin(); i != all_objs.end(); i++)
    {
      vgui_soview* sv = (vgui_soview*)(*i);
      if (sv == NULL)
      {
         vgui_macro_warning << "Object in soview list is null\n";
         return;
      }
      if (sv->type_name() == "vgui_soview2D_point" && sv->type_name() == object_type)
      {
        vgui_soview2D_point* pt = (vgui_soview2D_point*)sv;
        fs<<pt->x<<' '<<pt->y<<vcl_endl;
      }
      else if (sv->type_name() == "vgui_soview2D_circle" && sv->type_name() == object_type)
      {
        vgui_soview2D_circle* circ = (vgui_soview2D_circle*)sv;
        fs<<circ->x<<' '<<circ->y<<' '<<circ->r<<vcl_endl;
      }
      else if (sv->type_name() == "vgui_soview2D_lineseg" && sv->type_name() == object_type)
      {
        vgui_soview2D_lineseg* line = (vgui_soview2D_lineseg*)sv;
        fs<<line->x0<<' '<<line->y0<<' '<<line->x1<<' '<<line->y1<<vcl_endl;
      }
      else if (sv->type_name() == "vgui_soview2D_linestrip" && sv->type_name() == object_type)
      {
        vgui_soview2D_linestrip *linestrip = (vgui_soview2D_linestrip *)sv;
        fs<<linestrip->n<<vcl_endl;
        for (unsigned int ii = 1; ii<linestrip->n; ++ii)
        {
          fs<<linestrip->x[ii-1]<<' '<<linestrip->y[ii-1]<<' '<<linestrip->x[ii]<<' '<<linestrip->y[ii]<<vcl_endl;
        }
      }
    }
  }
  fs.close();
}

void vplayer_geometry::save_circles()
{
  save("vgui_soview2D_circle","Save circles");
}

void vplayer_geometry::save_points()
{
  save("vgui_soview2D_point","Save points");
}

void vplayer_geometry::save_lines()
{
  save("vgui_soview2D_lineseg","Save lines");
}

void vplayer_geometry::save_linestrips()
{
  save("vgui_soview2D_linestrip","Save linestrip");
}

//-----------------------------------------------------------------------------
//: Load coordinates from a file
//-----------------------------------------------------------------------------
void vplayer_geometry::load(const char *object_type,const char *dialog_name)
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col, row);
  vgui_dialog load_dl(dialog_name);
  static vcl_string filename = "temp.dat";
  static vcl_string regexp = "*.*";
  load_dl.inline_file("Filename: ", regexp, filename);
  vcl_ifstream fs;
  if (load_dl.ask())
  {
    fs.open(filename.c_str());
    while (!fs.eof())
    {
      if (vcl_string(object_type) == "vgui_soview2D_point")
      {
        float x,y;
        fs>>x;
        fs>>y;
        easy_tab->add_point(x,y);
      }
      else if (vcl_string(object_type) == "vgui_soview2D_circle")
      {
        float x,y,r;
        fs>>x>>y>>r;
        easy_tab->add_circle(x,y,r);
      }
      else if (vcl_string(object_type) == "vgui_soview2D_lineseg")
      {
        float x0,y0,x1,y1;
        fs>>x0>>y0>>x1>>y1;
        easy_tab->add_line(x0,y0,x1,y1);
      }
      else if (vcl_string(object_type) == "vgui_soview2D_linestrip")
      {
        int n;
        fs>>n;
        for (int i = 1; i<n; i++)
        {
          float x0,y0,x1,y1;
          fs>>x0>>y0>>x1>>y1;
          easy_tab->add_line(x0,y0,x1,y1);
        }
      }
    }
  }
  fs.close();
}

void vplayer_geometry::load_circles()
{
  load("vgui_soview2D_circle","Load circles");
}

void vplayer_geometry::load_points()
{
  load("vgui_soview2D_point","Load points");
}

void vplayer_geometry::load_lines()
{
  load("vgui_soview2D_lineseg","Load lines");
}

void vplayer_geometry::load_linestrips()
{
  load("vgui_soview2D_linestrip","Load linestrip");
}

//-----------------------------------------------------------------------------
//: Create a menu containing all the functions in this file.
//-----------------------------------------------------------------------------
void vplayer_geometry::create_box()
{
unsigned col, row;
  get_current(&col, &row);
  vgui_rubberband_tableau_sptr rubber = get_rubberbander_at(col, row);
  if (rubber)
    rubber->rubberband_box();
}

vgui_menu vplayer_geometry::create_geometry_menu()
{
  vgui_menu obj_menu;
  obj_menu.add("Add point", create_point);
  obj_menu.add("Add line", create_line);
  obj_menu.add("Add infinite line", create_infinite_line);
  obj_menu.add("Add circle", create_circle);
  obj_menu.add("Add polygon", create_polygon);
  obj_menu.add("Add linestrip", create_linestrip);
  //obj_menu.add("Add box",create_box);
  vgui_menu sel_menu;
  sel_menu.add("Change colour", change_sel_color);
  sel_menu.add("Change point radius", change_sel_radius);
  sel_menu.add("Change line width", change_sel_width);

  vgui_menu del_menu;
  del_menu.add("Remove points",delete_points);
  del_menu.add("Remove lines",delete_lines);
  del_menu.add("Remove infinite lines",delete_inf_lines);
  del_menu.add("Remove circles",delete_circles);
  del_menu.add("Remove linestrips",delete_linestrips);
  del_menu.separator();
  del_menu.add("Remove all",delete_all);
  del_menu.add("Remove selected",delete_sel_objs);
  vgui_menu default_menu;

  default_menu.add("Change colour", change_default_color);
  default_menu.add("Change point radius", change_default_radius);
  default_menu.add("Change line width", change_default_width);

  vgui_menu geom_menu;
  geom_menu.add("Add geometric object", obj_menu);
  geom_menu.add("Change selected objects", sel_menu);
  geom_menu.add("Change default style", default_menu);
  geom_menu.add("Delete...",del_menu);
  geom_menu.separator();
  geom_menu.add("Load points",load_points);
  geom_menu.add("Load lines",load_lines);
  geom_menu.add("Load circles",load_circles);
  geom_menu.add("Load linestrips",load_linestrips);
  geom_menu.separator();
  geom_menu.add("Save points",save_points);
  geom_menu.add("Save lines",save_lines);
  geom_menu.add("Save circles",save_circles);
  geom_menu.add("Save linestrips",save_linestrips);
  return geom_menu;
}
