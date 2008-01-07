// This is brl/bbas/bmsh3d/vis/bmsh3d_vis_vertex.cxx
#include "bmsh3d_vis_vertex.h"
//:
// \file
// \author MingChing Chang
// \date   May 03, 2005.

#include <bmsh3d/bmsh3d_mesh.h>

#include <bmsh3d/vis/bmsh3d_vis_backpt.h>
#include <bmsh3d/vis/bmsh3d_vis_utils.h>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoDrawStyle.h>

#include <vcl_cstdio.h> // std::sprintf()

SoSeparator* draw_vertex_geom_SoPoint (const bmsh3d_vertex* V)
{
  SoSeparator* root = new SoSeparator;

  SoCoordinate3 * coord3 = new SoCoordinate3;
  coord3->point.set1Value (0, V->pt().x(), V->pt().y(), V->pt().z());

  SoPointSet* pointSet = new SoPointSet;
  root->addChild (coord3);
  root->addChild (pointSet);

  return root;
}

SoSeparator* draw_vertex_geom_vispt_SoPoint (const bmsh3d_vertex* V)
{
  SoSeparator* root = new SoSeparator;
  SoCoordinate3 * coord3 = new SoCoordinate3;
  coord3->point.set1Value (0, V->pt().x(), V->pt().y(), V->pt().z());

  bmsh3dvis_vertex_SoPointSet* pointSet = new bmsh3dvis_vertex_SoPointSet (V);
  root->addChild (coord3);
  root->addChild (pointSet);
  return root;
}

SoSeparator* draw_vertex_vispt_SoPoint (const bmsh3d_vertex* V,
                                        const SbColor& color, const float size)
{
  SoSeparator* root = new SoSeparator;

  //assign color for each
  SoBaseColor* basecol = new SoBaseColor;
  basecol->rgb = color;
  root->addChild (basecol);

  //assign size
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = size;
  root->addChild(drawstyle);

  SoTranslation* trans = new SoTranslation;
  trans->translation.setValue (V->pt().x(), V->pt().y(), V->pt().z());
  root->addChild (trans);

  bmsh3dvis_vertex_SoPointSet* pt = new bmsh3dvis_vertex_SoPointSet (V);
  root->addChild (pt);
  return root;
}

SoSeparator* draw_vertex_geom_SoCube (const bmsh3d_vertex* V,
                                      const float size)
{
  SoSeparator* root = new SoSeparator;
  SoTranslation* trans = new SoTranslation;
  trans->translation.setValue (V->pt().x(), V->pt().y(), V->pt().z());
  SoCube* cube = new SoCube;
  cube->height = size; //1.0f;
  cube->width = size;
  cube->depth = size;
  root->addChild (trans);
  root->addChild (cube);
  return root;
}

SoSeparator* draw_vertex_geom_vispt_SoCube (const bmsh3d_vertex* V,
                                            const float size)
{
  SoSeparator* root = new SoSeparator;
  SoTranslation* trans = new SoTranslation;
  trans->translation.setValue (V->pt().x(), V->pt().y(), V->pt().z());
  bmsh3dvis_vertex_SoCube* cube = new bmsh3dvis_vertex_SoCube (V);
  cube->height = size; //1.0f;
  cube->width = size;
  cube->depth = size;
  root->addChild (trans);
  root->addChild (cube);
  return root;
}

SoSeparator* draw_vertex_SoCube (const bmsh3d_vertex* V,
                                 const SbColor& color, const float size)
{
  SoSeparator* root = new SoSeparator;
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);

  SoTranslation* trans = new SoTranslation;
  trans->translation.setValue (V->pt().x(),
                               V->pt().y(),
                               V->pt().z());

  SoCube* cube = new SoCube;
  cube->height = size; //1.0f;
  cube->width = size;
  cube->depth = size;
  root->addChild (trans);
  root->addChild (cube);
  return root;
}

SoSeparator* draw_vertex_vispt_SoCube (const bmsh3d_vertex* V,
                                       const SbColor& color, const float size,
                                       const bool show_id)
{
  SoSeparator* root = new SoSeparator;
  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);

  SoTranslation* trans = new SoTranslation;
  trans->translation.setValue (V->pt().x(), V->pt().y(), V->pt().z());

  bmsh3dvis_vertex_SoCube* cube = new bmsh3dvis_vertex_SoCube (V);
  cube->height = size; //1.0f;
  cube->width = size;
  cube->depth = size;
  root->addChild (trans);
  root->addChild (cube);

  if (show_id) {
    char buf[64];
    vcl_sprintf (buf, "%d", V->id());
    draw_text2d_geom (root, buf);
  }

  return root;
}
