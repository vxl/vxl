// This is brl/bbas/bmsh3d/vis/bmsh3d_vis_face.h
#ifndef bmsh3d_vis_face_h_
#define bmsh3d_vis_face_h_
//:
// \file
// \author Ming-Ching Chang
// \date May 03, 2005.

#include <vgl/vgl_point_3d.h>
#include <bmsh3d/bmsh3d_face.h>
#include <Inventor/SbColor.h>

class bmsh3d_mesh;
class SoGroup;
class SoSeparator;
class SoFaceSet;
class SbColor;
class SoBaseColor;
class SoMaterial;
class SoCoordinate3;

//: return the SoCoordinate3 to enable online alternation.
SoCoordinate3* _draw_F_ifs_geom (SoGroup* group, const bmsh3d_face* F);
SoCoordinate3* _draw_F_mhe_geom (SoGroup* group, const bmsh3d_face* F);

//: Check if the vertex is too close to any previous vertex.
//  return true if no duplicate.
bool _check_duplicate (SbVec3f* verts, unsigned int count, const vgl_point_3d<double>& pt);

void draw_F_geom (SoGroup* group, const bmsh3d_face* F);

void draw_F_geom_vispt (SoGroup* group, const bmsh3d_face* F);

SoSeparator* draw_F (const bmsh3d_face* F, const SbColor& color,
                     const float transp = 0.0f,
                     const bool user_defined_class = true);

SoSeparator* draw_F (const bmsh3d_face* F,
                     const SoMaterial* material,
                     const bool user_defined_class = true);

SoSeparator* draw_F_with_id (const bmsh3d_face* F,
                             const SbColor& color, const SoBaseColor* idbasecolor,
                             const float transp = 0.0f,
                             const bool user_defined_class = true);

SoSeparator* draw_F_with_id (const bmsh3d_face* F,
                             const SoMaterial* material,
                             const SoBaseColor* idbasecolor,
                             const bool user_defined_class = true);
#endif


