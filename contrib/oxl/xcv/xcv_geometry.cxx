// This is oxl/xcv/xcv_geometry.cxx
#include "xcv_geometry.h"
//:
// \file
// See xcv_geometry.h for a description of this file.
// \author K.Y.McGaul

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_clip.h>

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_rubberband_tableau.h>
#include <vgui/vgui_color_text.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_error_dialog.h>

//static bool debug = true;
extern void get_current(unsigned*, unsigned*);
extern vcl_vector<vgui_easy2D_tableau_sptr> get_easy2D_list();
extern vgui_rubberband_tableau_sptr get_rubberbander_at(unsigned, unsigned);
extern vgui_easy2D_tableau_sptr get_easy2D_at(unsigned, unsigned);
extern vgui_easy2D_tableau_sptr get_current_easy2D();

// Filename for save and load
static vcl_string filename = "/tmp/temp.gx";
static vcl_string regexp = "*.*";

//-----------------------------------------------------------------------------
//: Draw a point onto the currently selected tableau.
//-----------------------------------------------------------------------------
void xcv_geometry::create_point()
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
void xcv_geometry::create_line()
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
void xcv_geometry::create_circle()
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
void xcv_geometry::create_polygon()
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
void xcv_geometry::create_linestrip()
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
void xcv_geometry::create_infinite_line()
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
void xcv_geometry::change_sel_color()
{
  static vcl_string color_value = "yellow";
  vgui_dialog color_dl("Colour of selected objects");
  color_dl.inline_color("New colour for selected objects:", color_value);
  if (!color_dl.ask())
    return;

  vcl_vector<vgui_easy2D_tableau_sptr> easy_list = get_easy2D_list();
  for (unsigned i=0; i<easy_list.size(); i++)
  {
    vcl_vector<vgui_soview*> sel_objs = easy_list[i]->get_selected_soviews();
    for (vcl_vector<vgui_soview*>::iterator iter = sel_objs.begin(); iter !=
      sel_objs.end(); iter++)
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
void xcv_geometry::change_sel_radius()
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
void xcv_geometry::change_sel_width()
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
void xcv_geometry::delete_sel_objs()
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
void xcv_geometry::delete_all()
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
//: Delete all points.
//-----------------------------------------------------------------------------
void xcv_geometry::delete_points()
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
    vgui_macro_warning << "Unable to get current easy2D to delete points\n";
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
//: Delete all lines.
//-----------------------------------------------------------------------------
void xcv_geometry::delete_lines()
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
//: Delete all infinite lines.
//-----------------------------------------------------------------------------
void xcv_geometry::delete_inf_lines()
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
//: Delete all circles.
//-----------------------------------------------------------------------------
void xcv_geometry::delete_circles()
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
//: Delete all linestrips.
//-----------------------------------------------------------------------------
void xcv_geometry::delete_linestrips()
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
    if ((*i)->type_name() == "vgui_soview2D_linestrip")
        easy_tab->remove((vgui_soview*)(*i));
  }
}

//-----------------------------------------------------------------------------
//: Change the default color for geometric objects.
//   Note, this will apply to geometric objects created in the future, not
//   to geometric objects already on the tableau.
//-----------------------------------------------------------------------------
void xcv_geometry::change_default_color()
{
  static vcl_string color_value = "yellow";
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
void xcv_geometry::change_default_radius()
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
void xcv_geometry::change_default_width()
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
void xcv_geometry::save(const char *object_type,const char *dialog_name)
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col, row);
  vgui_dialog save_dl(dialog_name);
  save_dl.inline_file("Filename: ", regexp, filename);
  vcl_ofstream fs;
  if (save_dl.ask())
  {
    fs.open(filename.c_str());
    vcl_vector<vgui_soview*> all_objs = easy_tab->get_all();
    for (vcl_vector<vgui_soview*>::iterator i = all_objs.begin(); i != all_objs.end(); i++)
    {
      vgui_soview* sv = (vgui_soview*)(*i);
      if (!sv)
      {
        vgui_macro_warning << "Object in soview list is null\n";
        return;
      }
      bool matched = (object_type == 0) || (sv->type_name() == object_type);
      vcl_string svtype = sv->type_name();
      vgui_style* style = sv->get_style();
      if (style)
        fs << "c " << style->rgba[0] << " " << style->rgba[1] <<  " " << style->rgba[2] << vcl_endl
           << "r " << style->point_size << vcl_endl
           << "w " << style->line_width << vcl_endl;

      if (svtype == "vgui_soview2D_point" && matched)
      {
        vgui_soview2D_point* pt = (vgui_soview2D_point*)sv;
        fs<<"p "<<pt->x<<" "<<pt->y<< vcl_endl;
      }
      else if (svtype == "vgui_soview2D_circle" && matched)
      {
        vgui_soview2D_circle* circ = (vgui_soview2D_circle*)sv;
        fs<<"circle "<<circ->x<<" "<<circ->y<<" "<<circ->r<< vcl_endl;
      }
      else if (svtype == "vgui_soview2D_lineseg" && matched)
      {
        vgui_soview2D_lineseg* line = (vgui_soview2D_lineseg*)sv;
        fs<<"l "<<line->x0<<" "<<line->y0<<" "<<line->x1<<" "<<line->y1<< vcl_endl;
      }
      else if (svtype == "vgui_soview2D_infinite_line" && matched)
      {
        vgui_soview2D_infinite_line* line = (vgui_soview2D_infinite_line*)sv;
        fs<<"il "<<line->a<<" "<<line->b<<" "<<line->c<< vcl_endl;
      }
      else if (svtype == "vgui_soview2D_linestrip" && matched)
      {
        vgui_soview2D_linestrip *linestrip = (vgui_soview2D_linestrip *)sv;
        fs<<"L "<<linestrip->n;
        for (unsigned int ii = 0; ii<linestrip->n; ++ii)
          fs<<" "<<linestrip->x[ii]<<" "<<linestrip->y[ii];
        fs << vcl_endl;
      }
      else if (svtype == "vgui_soview2D_polygon" && matched)
      {
        vgui_soview2D_polygon *polygon = (vgui_soview2D_polygon *)sv;
        fs<<"y "<<polygon->n;
        for (unsigned int ii = 0; ii<polygon->n; ++ii)
          fs<<" "<<polygon->x[ii]<<" "<<polygon->y[ii];
        fs << vcl_endl;
      }
    }
  }
  fs.close();
}

//: Save all circles into a file.
void xcv_geometry::save_circles()
{
  save("vgui_soview2D_circle","Save circles");
}

//: Save all points into a file.
void xcv_geometry::save_points()
{
  save("vgui_soview2D_point","Save points");
}

//: Save all lines into a file.
void xcv_geometry::save_lines()
{
  save("vgui_soview2D_lineseg","Save lines");
}

//: Save all linestrips into a file.
void xcv_geometry::save_linestrips()
{
  save("vgui_soview2D_linestrip","Save linestrip");
}

//: Save all geometric objects into a file.
void xcv_geometry::save_geometry()
{
  save(0,"Save geometry");
}

//-----------------------------------------------------------------------------
//: Load coordinates from a file
//-----------------------------------------------------------------------------
void xcv_geometry::load(const char *object_type,const char *dialog_name)
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col, row);
  if (!easy_tab) return;
  vgui_dialog load_dl(dialog_name);
  load_dl.inline_file("Filename: ", regexp, filename);
  vcl_ifstream fs;
  if (load_dl.ask())
  {
    fs.open(filename.c_str());
    if (!fs.good()) {
      vgui_error_dialog("Failed to open file");
      return;
    }

    while (!fs.eof())
    {
      vcl_string tag;
      fs >> tag >> vcl_ws;
      if (tag == "c" && (object_type == 0 || tag==vcl_string(object_type)))
      {
        // colour
        float r,g,b;
        fs>>r>>g>>b;
        easy_tab->set_foreground(r,g,b);
      }
      else if (tag == "w" && (object_type == 0 || tag==vcl_string(object_type)))
      {
        // line width
        float w;
        fs>>w;
        easy_tab->set_line_width(w);
      }
      else if (tag == "r" && (object_type == 0 || tag==vcl_string(object_type)))
      {
        // point radius
        float w;
        fs>>w;
        easy_tab->set_point_radius(w);
      }
      else if (tag == "p" && (object_type == 0 || tag==vcl_string(object_type)))
      {
        float x,y;
        fs>>x;
        fs>>y;
        easy_tab->add_point(x,y);
      }
      else if (tag == "circle" && (object_type == 0 || tag==vcl_string(object_type)))
      {
        float x,y,r;
        fs>>x>>y>>r;
        easy_tab->add_circle(x,y,r);
      }
      else if (tag == "l" && (object_type == 0 || tag==vcl_string(object_type)))
      {
        float x0,y0,x1,y1;
        fs>>x0>>y0>>x1>>y1;
        easy_tab->add_line(x0,y0,x1,y1);
      }
      else if (tag == "il" && (object_type == 0 || tag==vcl_string(object_type)))
      {
        float a,b,c;
        fs>>a>>b>>c;
        easy_tab->add_infinite_line(a,b,c);
      }
      else if (tag == "L" && (object_type == 0 || tag==vcl_string(object_type)))
      {
        int n;
        fs>>n;
        float x0,y0;
        fs>>x0>>y0;
        for (int i = 1; i<n; i++)
        {
          float x1,y1;
          fs>>x1>>y1;
          easy_tab->add_line(x0,y0,x1,y1);
          x0 = x1;
          y0 = y1;
        }
      }
      else if (tag == "y" && (object_type == 0 || tag==vcl_string(object_type)))
      {
        int n;
        fs>>n;
        vcl_vector<float> x(n);
        vcl_vector<float> y(n);
        for (int i = 0; i<n; i++)
          fs>>x[i]>>y[i];
        easy_tab->add_polygon(n, &x[0], &y[0]);
      }
      else vcl_cerr << "Unrecognised tag " << tag << " in file " << filename
                    << "\nencountered in xcv_geometry::load(" << object_type << ")\n";
    }
  }
  fs.close();
}

//: Load all geometric objects from a file.
void xcv_geometry::load_geometry()
{
  load(0,"Load geometry");
}

//: Add sheets to gui
static void add(vgl_polygon<float> const& p)
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_easy2D_tableau_sptr easy_tab = get_easy2D_at(col, row);

  for (int i = 0; i < p.num_sheets(); ++i) {
    vgl_polygon_sheet_as_array<float> sp(p[i]);
    easy_tab->add_polygon(sp.n, sp.x, sp.y);
  }
}

//========================================================================
//:  Intersect two polygons to get a set of non-intersecting polygons.
void xcv_geometry::polygon_intersect()
{
  vcl_vector<vgl_polygon<float> > all_polys;

  vcl_vector<vgui_easy2D_tableau_sptr> easy_list = get_easy2D_list();
  vcl_vector<vgui_soview*> all_soviews;
  for (unsigned i=0; i<easy_list.size(); i++)
  {
    vcl_vector<vgui_soview*> sel_objs = easy_list[i]->get_selected_soviews();
    for (vcl_vector<vgui_soview*>::iterator it = sel_objs.begin(); it != sel_objs.end(); it++)
    {
      vgui_soview* sv = (vgui_soview*)(*it);
      if (sv->type_name() == "vgui_soview2D_polygon") {
        vgui_soview2D_polygon* pv = (vgui_soview2D_polygon*)sv;
        all_polys.push_back(vgl_polygon<float>(pv->x, pv->y, pv->n));
        all_soviews.push_back(sv);
      }
    }
  }

  if (all_polys.size() != 2) {
    vgui_error_dialog("Must select exactly two polygons");
    return;
  }

  // Delete old polys
  for (unsigned i=0; i<easy_list.size(); i++)
    for (unsigned j =0; j < all_soviews.size(); ++j)
      easy_list[i]->remove(all_soviews[j]);

  // Intersect :  a and b
  add(vgl_clip(all_polys[0], all_polys[1], vgl_clip_type_intersect));

  // a - b
  add(vgl_clip(all_polys[0], all_polys[1], vgl_clip_type_difference));
  add(vgl_clip(all_polys[1], all_polys[0], vgl_clip_type_difference));
}

//========================================================================
//: Move geometric objects apart.
static void xcv_geometry_explode_geometry()
{
  double d = 20;
  vgui_dialog del_dl("Explode Geometry");
  del_dl.field("Explode distance", d);
  if (!del_dl.ask())
    return;

  vgui_easy2D_tableau_sptr easy_tab = get_current_easy2D();
  if (!easy_tab)
  {
    vgui_macro_warning << "Unable to get current easy2D to delete te objects\n";
    return;
  }

  // Accumulate centroids
  double cx = 0;
  double cy = 0;
  double ca = 0;
  vcl_vector<vgui_soview*> sel_objs = easy_tab->get_all();
  for (vcl_vector<vgui_soview*>::iterator i = sel_objs.begin(); i != sel_objs.end(); i++)
  {
    vgui_soview2D* sv = (vgui_soview2D*)*i;
    float x,y;
    sv->get_centroid(&x, &y);
    cx += x;
    cy += y;
    ca += 1;
  }
  cx /= ca;
  cy /= ca;

  // Translate
  for (vcl_vector<vgui_soview*>::iterator i = sel_objs.begin(); i != sel_objs.end(); i++)
  {
    vgui_soview2D* sv = (vgui_soview2D*)*i;
    float x,y;
    sv->get_centroid(&x, &y);
    double dx = x - cx;
    double dy = y - cy;
    double r = vcl_sqrt(dx*dx+dy*dy);
    double s = d / r;
    dx *= s;
    dy *= s;
    sv->translate(dx,dy);
  }

  // Tell easy alll has changed
  easy_tab->post_redraw();
}

//-----------------------------------------------------------------------------
//: Create a menu containing all the functions in this file.
//-----------------------------------------------------------------------------
void xcv_geometry::create_box()
{
  unsigned col, row;
  get_current(&col, &row);
  vgui_rubberband_tableau_sptr rubber = get_rubberbander_at(col, row);
  if (rubber)
    rubber->rubberband_box();
}

vgui_menu xcv_geometry::create_geometry_menu()
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

  vgui_menu default_menu;
  default_menu.add("Change colour", change_default_color);
  default_menu.add("Change point radius", change_default_radius);
  default_menu.add("Change line width", change_default_width);

  vgui_menu del_menu;
  del_menu.add("Remove points",delete_points);
  del_menu.add("Remove lines",delete_lines);
  del_menu.add("Remove infinite lines",delete_inf_lines);
  del_menu.add("Remove circles",delete_circles);
  del_menu.add("Remove linestrips",delete_linestrips);
  del_menu.separator();
  del_menu.add("Remove all",delete_all);
  del_menu.add("Remove selected",delete_sel_objs);

  vgui_menu save_menu;
  save_menu.add("Save geometry",save_geometry);
  save_menu.add("Save points",save_points);
  save_menu.add("Save lines",save_lines);
  save_menu.add("Save circles",save_circles);
  save_menu.add("Save linestrips",save_linestrips);

  vgui_menu compgeom_menu;
  compgeom_menu.add("Intersect polygons", polygon_intersect);
  compgeom_menu.add("Explode spatial objects", xcv_geometry_explode_geometry);

  vgui_menu geom_menu;
  geom_menu.add("Add geometric object", obj_menu);
  geom_menu.add("Change selected objects", sel_menu);
  geom_menu.add("Change default style", default_menu);
  geom_menu.add("Save...", save_menu);
  geom_menu.add("Load geometry",load_geometry);
  geom_menu.add("Delete...",del_menu);
  geom_menu.add("Computational geometry", compgeom_menu);

  return geom_menu;
}
