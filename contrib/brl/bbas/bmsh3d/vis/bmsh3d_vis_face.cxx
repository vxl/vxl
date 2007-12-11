// This is brl/bbas/bmsh3d/vis/bmsh3d_vis_face.cxx
#include "bmsh3d_mesh.h"
//:
// \file
// \author Ming-Ching Chang
// \date May 03, 2005.

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <bmsh3d/vis/bmsh3d_vis_backpt.h>
#include <bmsh3d/vis/bmsh3d_vis_face.h>
#include <bmsh3d/vis/bmsh3d_vis_utils.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoShapeHints.h>

//: Visualize the geometry of a polygonal face.
//
SoCoordinate3* _draw_F_ifs_geom (SoGroup* group, const bmsh3d_face* F)
{
  const unsigned int nVertices = F->vertices().size();
  if (nVertices<3)
    return NULL;

  SoCoordinate3* coords = new SoCoordinate3;
  SbVec3f* verts = new SbVec3f[nVertices];
  // Put each vertex position into a coordinate array.
  // Skip the vertex if it is too close to any other vertex.
  unsigned int count = 0;
  for (unsigned int j=0; j<nVertices; j++) {
    const bmsh3d_vertex* V = (const bmsh3d_vertex*) F->vertices(j);
    //Check if the vertex is too close to any previous vertex.
    if (_check_duplicate (verts, count, V->pt())) {
      verts[count] = SbVec3f (V->pt().x(), V->pt().y(), V->pt().z());
      count++;
    }
  }

  // Ignore the polygon with fewer than 3 sides.
  if (count > 2) {
    coords->point.setValues (0, count, verts); //nVertices
    group->addChild (coords);
  }
  else
    vul_printf (vcl_cerr, "  -- Skip drawing face %d: only %d salient edges.\n", F->id(), count);
  delete[] verts;
  return coords;
}

//: Visualize the geometry of a polygonal face.
//
SoCoordinate3* _draw_F_mhe_geom (SoGroup* group, const bmsh3d_face* F)
{
  vcl_vector<bmsh3d_vertex*> vertices;
  F->get_ordered_Vs (vertices);
  const unsigned int nVertices = vertices.size();
  if (nVertices<3)
    return NULL;

  SoCoordinate3* coords = new SoCoordinate3;
  SbVec3f* verts = new SbVec3f[nVertices];
  // Put each vertex position into a coordinate array.
  // Skip the vertex if it is too close to any other vertex.
  unsigned int count = 0;
  for (unsigned int j=0; j<nVertices; j++) {
    const bmsh3d_vertex* V = (const bmsh3d_vertex*) vertices[j];
    //Check if the vertex is too close to any previous vertex.
    if (_check_duplicate (verts, count, V->pt())) {
      verts[count] = SbVec3f (V->pt().x(), V->pt().y(), V->pt().z());
      count++;
    }
  }

  // Ignore the polygon with fewer than 3 sides.
  if (count > 2) {
    coords->point.setValues (0, count, verts); //nVertices
    group->addChild (coords);
  }
  else
    vul_printf (vcl_cerr, "  -- Skip drawing face %d: only %d salient edges.\n", F->id(), count);
  delete[] verts;
  return coords;
}

//: Check if the vertex is too close to any previous vertex.
//  return true if no duplicate.
bool _check_duplicate (SbVec3f* verts, unsigned int count, const vgl_point_3d<double>& pt)
{
  for (unsigned int k=0; k<count; k++) {
    if (bmsh3d_vis_too_close (verts[k][0], verts[k][1], verts[k][2],
                               (float) pt.x(), (float) pt.y(), (float) pt.z()))
      return false;
  }
  return true;
}

void draw_F_geom (SoGroup* group, const bmsh3d_face* F)
{
  if (F->vertices().size() != 0)
    _draw_F_ifs_geom (group, F);
  else
    _draw_F_mhe_geom (group, F);

  SoFaceSet* faceSet = new SoFaceSet ();
  group->addChild (faceSet);
}

void draw_F_geom_vispt (SoGroup* group, const bmsh3d_face* F)
{
  if (F->vertices().size() != 0)
    _draw_F_ifs_geom (group, F);
  else
    _draw_F_mhe_geom (group, F);

  bmsh3dvis_face_SoFaceSet* faceSet = new bmsh3dvis_face_SoFaceSet (F);
  group->addChild (faceSet);
}

SoSeparator* draw_F (const bmsh3d_face* F,
                     const SbColor& color, const float transp,
                     const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  SoMaterial *material = new SoMaterial;
  material->diffuseColor.setValue (color);
  material->emissiveColor.setValue (color/2);
  material->transparency = transp;
  root->addChild (material);

  if (user_defined_class)
    draw_F_geom_vispt (root, F);
  else
    draw_F_geom (root, F);

  return root;
}

SoSeparator* draw_F (const bmsh3d_face* F,
                     const SoMaterial* material,
                     const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  root->addChild ((SoMaterial*) material);

  if (user_defined_class)
    draw_F_geom_vispt (root, F);
  else
    draw_F_geom (root, F);

  return root;
}

SoSeparator* draw_F_with_id (const bmsh3d_face* F, const SbColor& color,
                             const SoBaseColor* idbasecolor,
                             const float transp,
                             const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  SoMaterial *material = new SoMaterial;
  material->diffuseColor.setValue (color);
  material->emissiveColor.setValue (color/2);
  material->transparency = transp;
  root->addChild (material);

  if (user_defined_class)
    draw_F_geom_vispt (root, F);
  else
    draw_F_geom (root, F);

  //show id
  char buf[64];
  vcl_sprintf (buf, "%d", F->id());
  vgl_point_3d<double> cen = F->compute_center_pt ();
  root->addChild (draw_text2d (buf, cen.x(), cen.y(), cen.z(), idbasecolor));

  return root;
}

SoSeparator* draw_F_with_id (const bmsh3d_face* F,
                                const SoMaterial* material,
                                const SoBaseColor* idbasecolor,
                                const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  root->addChild ((SoMaterial*) material);

  if (user_defined_class)
    draw_F_geom_vispt (root, F);
  else
    draw_F_geom (root, F);

  //show id
  char buf[64];
  vcl_sprintf (buf, "%d", F->id());
  vgl_point_3d<double> cen = F->compute_center_pt ();
  root->addChild (draw_text2d (buf, cen.x(), cen.y(), cen.z(), idbasecolor));

  return root;
}
