// This is brl/bbas/bmsh3d/vis/bmsh3d_vis_utils.h
#ifndef bmsh3d_vis_utils_h_
#define bmsh3d_vis_utils_h_
//:
// \file
// \author MingChing Chang
// \date May 03, 2005.

#include <vcl_cmath.h>
#include <vcl_utility.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <bmsh3d/bmsh3d_utils.h>
#include <bmsh3d/bmsh3d_vertex.h>

class SoSeparator;
class SoBaseColor;
class SoNode;
class SoGroup;
class SoDrawStyle;
class SbColor;
class SbVec3f;

///extern SbColor VIS_COLOR;
SbColor color_from_code (const int colorcode);

//: Some pre-defined color seed.
#define BMSH3D_SHEET_COLOR_SEED      622
#define BMSH3D_MESH_COLOR_SEED       4321

//: produce random colors
void init_rand_color (unsigned long seed);
VIS_COLOR_CODE get_next_rand_ccode ();
SbColor get_next_rand_color ();

// cvalue 0: blue, cvalue 1: red
SbColor get_color_tone (float cvalue);

void generate_color_table (unsigned long seed, unsigned int size,
                           vcl_vector<SbColor>& color_table);

#define VIS_CLOSE_TH  1E-6 //1E-6

inline bool vis_too_close_ (const float a, const float b)
{
  //If relative error < threshold, too close.
  if (vcl_fabs((a-b)/a) < VIS_CLOSE_TH)
    return true;
  else
    return false;
}

//: check two points are too close (in float prevision) to visualize or not.
inline bool bmsh3d_vis_too_close (const float x1, const float y1, const float z1,
                                   const float x2, const float y2, const float z2)
{
  //if (x1==x2 && y1==y2 && z1==z2)
  if (vis_too_close_(x1, x2) && vis_too_close_(y1, y2) && vis_too_close_(z1, z2))
    return true;
  else
    return false;
}

void assign_rgb_light (SoSeparator* root);

SoGroup* getParent (SoNode* node, SoNode* root);

//##################################################################

//:
//  Ming Note: the draw_geometry functions just draw the object in the simplest way
//  without additional scenegraph group.

SoSeparator* draw_cube_geom (const float x, const float y, const float z,
                             const float size);
SoSeparator* draw_cube_geom (const vgl_point_3d<double> pt, const float size);

SoSeparator* draw_cube (const float x, const float y, const float z,
                        const float size, const SbColor& color);
SoSeparator* draw_cube (const vgl_point_3d<double> pt,
                        const float size, const SbColor& color);

SoSeparator* draw_sphere_geom (const float x, const float y, const float z,
                               const float radius);
SoSeparator* draw_sphere_geom (const vgl_point_3d<double> pt, const float radius);

SoSeparator* draw_sphere (const float x, const float y, const float z,
                          const float radius, const SbColor& color);
SoSeparator* draw_sphere (const vgl_point_3d<double> pt,
                          const float radius, const SbColor& color);


//: draw a cylinder, given the radius, the coordinates of the start and end points of the centerline
SoSeparator* draw_cylinder(float x1, float y1, float z1,
                           float x2, float y2, float z2,
                           float radius,
                           const SbColor& color,
                           float transparency);

SoSeparator* draw_cylinder(const vgl_point_3d<double>& centerline_start,
                           const vgl_point_3d<double>& centerline_end,
                           float radius,
                           const SbColor& color,
                           float transparency);


SoSeparator* draw_polyline (vcl_vector<vgl_point_3d<double> >& polyline_vertices,
                            const float& width, const SbColor& color);
void draw_polyline_geom (SoGroup *root, vcl_vector<vgl_point_3d<double> >& vertices);

SoSeparator* draw_polyline (const float** polylineVertices, const unsigned int nVertices,
                            const SbColor& color, const SoDrawStyle* drawStyle);
  void draw_polyline_geom (SoGroup *root, SbVec3f* vertices, const unsigned int nVertices);


void draw_filled_polygon_geom (SoGroup *root, const vcl_vector<bmsh3d_vertex*>& vertices);

SoSeparator* draw_filled_polygon (SbVec3f* vertices, const int unsigned nVertices,
                                  const SbColor& color, const float fTransparency);
SoSeparator* draw_filled_polygon (float** vertices, const int unsigned nVertices,
                                  const SbColor& color, const float fTransparency);
void draw_filled_polygon_geom_ (SoGroup* root, SbVec3f* vertices, const int unsigned nVertices);

SoSeparator* draw_line_set (const vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > >& lines,
                            const SbColor& color);

//: draw the whole set of lines in a single SoIndexedLineSet.
void draw_line_set_geom (SoSeparator* root,
        const vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > >& lines);

void draw_triangle_geom (SoGroup* root,
                         const vgl_point_3d<double>& Pt1,
                         const vgl_point_3d<double>& Pt2,
                         const vgl_point_3d<double>& Pt3);
void draw_triangle_geom (SoGroup *root,
                         const float x1, const float y1, const float z1,
                         const float x2, const float y2, const float z2,
                         const float x3, const float y3, const float z3);

SoSeparator* draw_line (const float x1, const float y1, const float z1,
                        const float x2, const float y2, const float z2,
                        const SbColor& color, const SoDrawStyle* drawStyle=NULL);
SoSeparator* draw_line (const vgl_point_3d<double>& pt1, const vgl_point_3d<double>& pt2,
                        const SbColor& color, const SoDrawStyle* drawStyle=NULL);
  void draw_line_geom (SoGroup* root,
                       const float x1, const float y1, const float z1,
                       const float x2, const float y2, const float z2);
  void draw_line_geom (SoGroup* root, const vgl_point_3d<double>& pt1, const vgl_point_3d<double>& pt2);

SoSeparator* draw_box (const double& min_x, const double& min_y, const double& min_z,
                       const double& max_x, const double& max_y, const double& max_z,
                       const float& width, const SbColor& color);

SoSeparator* draw_box (const vgl_box_3d<double>& box,
                       const float& width, const SbColor& color);

SoSeparator* draw_text2d (const char* text, const float x, const float y, const float z);
SoSeparator* draw_text2d (const char* text,
                          const float x, const float y, const float z,
                          const SoBaseColor* basecolor);
  void draw_text2d_geom (SoSeparator* root, const char* text);

SoSeparator* shift_output (SoSeparator* vis, float shift_x, float shift_y, float shift_z);

#endif // bmsh3d_vis_utils_h_
