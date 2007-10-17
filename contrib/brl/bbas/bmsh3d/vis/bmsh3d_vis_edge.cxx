//: This is lemsvxlsrc/brcv/shp/bmsh3d/vis/bmsh3d_vis_mesh.cxx
//  MingChing Chang
//  May 03, 2005.

#include <vcl_cstdio.h>

#include <bmsh3d/bmsh3d_vertex.h>

#include <bmsh3d/vis/bmsh3d_vis_face.h>
#include <bmsh3d/vis/bmsh3d_vis_mesh.h>
#include <bmsh3d/vis/bmsh3d_vis_backpt.h>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoCoordinate3.h>

// ###################################################################
  
void draw_edge_geom (SoSeparator* root, const bmsh3d_edge* E,
                     const bool user_defined_class)
{
  const bmsh3d_vertex* sV = E->sV();
  const bmsh3d_vertex* eV = E->eV();

  SoCoordinate3* coords = new SoCoordinate3;
  coords->point.set1Value (0, sV->pt().x(), sV->pt().y(), sV->pt().z());
  coords->point.set1Value (1, eV->pt().x(), eV->pt().y(), eV->pt().z());
  root->addChild (coords);

  if (user_defined_class) {
    bmsh3dvis_edge_SoLineSet* lineSet = new bmsh3dvis_edge_SoLineSet (E);
    lineSet->setCoordinate3 (coords);
    lineSet->numVertices.set1Value (0, 2);
    root->addChild (lineSet);
  }
  else {
    SoLineSet* lineSet = new SoLineSet;
    lineSet->numVertices.set1Value (0, 2);
    root->addChild (lineSet);
  }
}

SoSeparator* draw_edge (bmsh3d_edge* E, const SbColor& color, const float width,
                        const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  //Color
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);
  
  //Line width
  SoDrawStyle* drawStyle = new SoDrawStyle;
  drawStyle->lineWidth.setValue (width);
  root->addChild (drawStyle);

  draw_edge_geom (root, E, user_defined_class);
  return root;
}









