// This is brl/bbas/bmsh3d/vis/bmsh3d_vis_mesh.h
#ifndef bmsh3d_vis_mesh_h_
#define bmsh3d_vis_mesh_h_
//:
// \file
// \author Ming-Ching Chang
// \date May 03, 2005.

#include <iostream>
#include <set>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <bmsh3d/bmsh3d_mesh.h>

#include <bmsh3d/vis/bmsh3d_vis_utils.h>
#include <bmsh3d/vis/bmsh3d_vis_face.h>

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/engines/SoTimeCounter.h>

class SoGroup;
class SoSeparator;
class SoVertexProperty;

//###############################################################
int _count_faces_indices (const std::vector<std::vector<int> >& faces);

void draw_ifs_geom (SoGroup* root,
                    const std::vector<vgl_point_3d<double> >& pts,
                    const std::vector<std::vector<int> >& faces);

void draw_ifs_geom (SoGroup* root, std::set<bmsh3d_vertex*>& pts,
                    std::set<bmsh3d_face*>& faces);

SoSeparator* draw_ifs (const std::vector<vgl_point_3d<double> >& pts,
                       const std::vector<std::vector<int> >& faces,
                       const int colorcode = COLOR_SILVER,
                       const bool b_shape_hints = false,
                       const float transp = 0.0f);

SoSeparator* draw_ifs (const std::vector<vgl_point_3d<double> >& pts,
                       const std::vector<std::vector<int> >& faces,
                       const SbColor& color,
                       const bool b_shape_hints = false,
                       const float transp = 0.0f);

SoSeparator* draw_ifs (std::set<bmsh3d_vertex*>& pts,
                       std::set<bmsh3d_face*>& faces,
                       const int colorcode = COLOR_SILVER,
                       const bool b_shape_hints = false,
                       const float transp = 0.0f);

SoSeparator* draw_ifs (std::set<bmsh3d_vertex*>& pts,
                       std::set<bmsh3d_face*>& faces,
                       const SbColor& color,
                       const bool b_shape_hints = false,
                       const float transp = 0.0f);

//###############################################################
// input: bmsh3d_mesh* M
//  return:
//     SoVertexProperty* vp,
//     int *ind
void _draw_M_ifs_geom (bmsh3d_mesh* M, SoVertexProperty* vp, int n_ind, int* ind);
void _draw_M_mhe_geom (bmsh3d_mesh* M, SoVertexProperty* vp, int n_ind, int* ind);

void draw_M_ifs_geom (SoGroup* root, bmsh3d_mesh* M);
void draw_M_mhe_geom (SoGroup* root, bmsh3d_mesh* M);

//: default color code is COLOR_SILVER.
//  return the SoSeparator created for this mesh.
SoSeparator* draw_M (bmsh3d_mesh* M, const bool b_shape_hints = false,
                     const float transp = 0.0f, const int colorcode = COLOR_SILVER);

//: Similar to the above but only draw the marked face.
void _draw_M_visited_ifs_geom (bmsh3d_mesh* M, SoVertexProperty* vp, int* ind);
void draw_M_visited_ifs_geom (SoGroup* root, bmsh3d_mesh* M);

SoSeparator* draw_M_ifs_visited(bmsh3d_mesh* M, const int colorcode = 0,
                                const bool b_shape_hints = false,
                                const float transp = 0.0f);

//: option 1: draw non-manifold-1-ring ones in RED.
//  option 2: draw non-1-ring vertices in BLUE.
//  option 3: draw both.
SoSeparator* draw_M_topo_vertices (bmsh3d_mesh* M, const int option,
                                   const float size, const bool user_defined_class = true);

SoSeparator* draw_M_edges_idv (bmsh3d_mesh* M, const SbColor& color,
                               const float width, const bool user_defined_class = true);

SoSeparator* draw_M_edges (bmsh3d_mesh* M, const SbColor& color, const float width);

void draw_M_mhe_edges_geom (SoSeparator* root, bmsh3d_mesh* M);
void draw_M_ifs_edges_geom (SoSeparator* root, bmsh3d_mesh* M);

SoSeparator* draw_M_bndcurve (bmsh3d_mesh* M, const int colorcode, const float width);

SoSeparator* draw_M_faces_idv (bmsh3d_mesh* M, const bool b_shape_hints = false,
                               const float transp = 0.0f, const int colorcode = 0,
                               const bool user_defined_class = true);

SoSeparator* draw_M_color (bmsh3d_mesh* M,
                           const bool b_shape_hints, const float trans,
                           const std::vector<SbColor>& color_set,
                           const bool user_defined_class = true);

//############ Visualize Mesh Geometry & Topology Info ############

SoSeparator* draw_M_bnd_faces_cost_col (bmsh3d_mesh* M, const bool draw_idv,
                                        const bool showid = false,
                                        const float transp = 0.0f);

SoSeparator* draw_M_bnd_faces_topo_col (bmsh3d_mesh* M, const bool draw_idv,
                                        const bool showid = false,
                                        const float transp = 0.0f,
                                        const bool user_defined_class = true);
VIS_COLOR_CODE get_M_face_topo_color (const TRIFACE_TYPE type);

SoSeparator* draw_M_bnd_faces_anim (bmsh3d_mesh* M, const int nF_batch = 30);

void draw_faces_in_switch (SoSeparator* root,
                           const std::vector<bmsh3d_face*>& Faces);


#endif
