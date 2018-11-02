// This is brl/bbas/bmsh3d/vis/bmsh3d_vis_utils.cxx
#include <iostream>
#include <vector>
#include "bmsh3d_vis_utils.h"
//:
// \file
// \author MingChing Chang
// \date May 03, 2005.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_vector_3d.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/actions/SoSearchAction.h>

SbColor color_from_code (const int colorcode)
{
  switch (colorcode)
  {
   case COLOR_NOT_SPECIFIED:return SbColor(0.5f, 0.5f, 0.5f);
   case COLOR_BLACK:     return SbColor(0.0f, 0.0f, 0.0f);
   case COLOR_RED:       return SbColor(1.0f, 0.0f, 0.0f);
   case COLOR_GREEN:     return SbColor(0.0f, 1.0f, 0.0f);
   case COLOR_BLUE:      return SbColor(0.0f, 0.0f, 1.0f);
   case COLOR_YELLOW:    return SbColor(1.0f, 1.0f, 0.0f);
   case COLOR_CYAN:      return SbColor(0.0f, 1.0f, 1.0f);
   case COLOR_PINK:      return SbColor(1.0f, 0.0f, 1.0f);
   case COLOR_GRAY:      return SbColor(0.6f, 0.6f, 0.6f);
   case COLOR_DARKGRAY:  return SbColor(0.4f, 0.4f, 0.4f);
   case COLOR_DARKRED:   return SbColor(0.5f, 0.0f, 0.0f);
   case COLOR_DARKGREEN: return SbColor(0.0f, 0.5f, 0.0f);
   case COLOR_DARKBLUE:  return SbColor(0.0f, 0.0f, 0.5f);
   case COLOR_DARKYELLOW:return SbColor(0.5f, 0.5f, 0.0f);
   case COLOR_DARKCYAN:  return SbColor(0.0f, 0.5f, 0.5f);
   case COLOR_PURPLE:    return SbColor(0.5f, 0.0f, 0.5f);
   case COLOR_WHITE:     return SbColor(1.0f, 1.0f, 1.0f);
   case COLOR_LIGHTGRAY: return SbColor(0.6f, 0.6f, 0.6f);
   case COLOR_GRAYBLUE:  return SbColor(0.5f, 0.5f, 1.0f);
   case COLOR_LIGHTGREEN:return SbColor(0.5f, 1.0f, 0.5f);
   case COLOR_ORANGE:    return SbColor(1.0f, 0.7f, 0.0f);
   case COLOR_GOLD:      return SbColor(0.8f, 0.7f, 0.2f);
   case COLOR_SILVER:    return SbColor(0.8f, 0.8f, 0.8f);
   case COLOR_RANDOM:    return get_next_rand_color();
   default: assert(0);   return SbColor(0.0f, 0.0f, 0.0f);
  }

  return SbColor (0.0f, 0.0f, 0.0f);
}

//: The scenegraph getParent function.
//  See http://doc.coin3d.org/Coin/classSoGroup.html
SoGroup* getParent (SoNode* node, SoNode* root)
{
  SoSearchAction sa;
  sa.setNode(node);
  sa.setInterest(SoSearchAction::FIRST);
  sa.apply(root);
  SoPath * p = sa.getPath();
  assert(p && "getPath() not found");
  if (p->getLength() < 2) {
    return NULL;
  } // no parent
  return (SoGroup *) p->getNodeFromTail(1);
}

void assign_rgb_light (SoSeparator* root)
{
  SoDirectionalLight* light1 = new SoDirectionalLight;
  light1->direction = SbVec3f(1,0,0);
  light1->color = SbColor(0.5,0,0);
  root->addChild (light1);

  SoDirectionalLight* light2 = new SoDirectionalLight;
  light2->direction = SbVec3f(0,1,0);
  light2->color = SbColor(0,0.5,0);
  root->addChild (light2);

  SoDirectionalLight* light3 = new SoDirectionalLight;
  light3->direction = SbVec3f(0,0,1);
  light3->color = SbColor(0,0,0.5);
  root->addChild (light3);

  SoDirectionalLight* light4 = new SoDirectionalLight;
  light4->direction = SbVec3f(-1,0,0);
  light4->color = SbColor(0.5,0,0);
  root->addChild (light4);

  SoDirectionalLight* light5 = new SoDirectionalLight;
  light5->direction = SbVec3f(0,-1,0);
  light5->color = SbColor(0,0.5,0);
  root->addChild (light5);

  SoDirectionalLight* light6 = new SoDirectionalLight;
  light6->direction = SbVec3f(0,0,-1);
  light6->color = SbColor(0,0,0.5);
  root->addChild (light6);
}

// cvalue 0: blue, cvalue 1: red
SbColor get_color_tone (float cvalue)
{
  SbColor color;

  if (cvalue < 0.25f)      // From Blue to Aqua
    color = SbColor (0.0f, cvalue*4, 1.0f);
  else if (cvalue < 0.5f)  // From Aqua to Green
    color = SbColor (0.0f, 1.0f, (0.5f-cvalue)*4);
  else if (cvalue < 0.75f) // From Green to Yellow
    color = SbColor ((cvalue-0.5f)*4, 1.0f, 0.0f);
  else                     // From Yellow to Red
    color = SbColor (1.0f, (1.0f-cvalue)*4, 0.0f);

  return color;
}

void generate_color_table (unsigned long seed, unsigned int size,
                           std::vector<SbColor>& color_table)
{
  color_table.clear();

  //: initialize the random color by given seed
  init_rand_color (seed);

  for (unsigned int i=0; i<size; i++) {
    //: generate a random color and store it
    SbColor color = get_next_rand_color ();
    color_table.push_back (color);
  }
}

// #######################################################

SoSeparator* draw_cube_geom (const float x, const float y, const float z,
                             const float size)
{
  SoSeparator* root = new SoSeparator;

  SoTranslation* trans = new SoTranslation;
  trans->translation.setValue (SbVec3f(x, y, z));
  root->addChild (trans);

  SoCube* cube = new SoCube ();
  cube->width = size;
  cube->height = size;
  cube->depth = size;
  root->addChild (cube);

  return root;
}

SoSeparator* draw_cube_geom (const vgl_point_3d<double> pt, const float size)
{
  return draw_cube_geom ((float)pt.x(), (float)pt.y(), (float)pt.z(), size);
}

SoSeparator* draw_cube (const float x, const float y, const float z,
                        const float size, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  // color
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);

  SoTranslation* trans = new SoTranslation;
  trans->translation.setValue (SbVec3f(x, y, z));
  root->addChild (trans);

  SoCube* cube = new SoCube ();
  cube->width = size;
  cube->height = size;
  cube->depth = size;
  root->addChild (cube);

  return root;
}

SoSeparator* draw_cube (const vgl_point_3d<double> pt,
                        const float size, const SbColor& color)
{
  return draw_cube ((float)pt.x(), (float)pt.y(), (float)pt.z(), size, color);
}

SoSeparator* draw_sphere_geom (const float x, const float y, const float z,
                               const float radius)
{
  SoSeparator* root = new SoSeparator;

  SoTranslation* trans = new SoTranslation;
  trans->translation.setValue (SbVec3f(x, y, z));
  root->addChild (trans);

  SoSphere* sphere = new SoSphere ();
  sphere->radius = radius;
  root->addChild (sphere);

  return root;
}

SoSeparator* draw_sphere_geom (const vgl_point_3d<double> pt, const float radius)
{
  return draw_sphere_geom ((float)pt.x(), (float)pt.y(), (float)pt.z(), radius);
}

SoSeparator* draw_sphere (const float x, const float y, const float z,
                          const float radius, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);

  SoTranslation* trans = new SoTranslation;
  trans->translation.setValue (SbVec3f(x, y, z));
  root->addChild (trans);

  SoSphere* sphere = new SoSphere ();
  sphere->radius = radius;
  root->addChild (sphere);

  return root;
}

SoSeparator* draw_sphere (const vgl_point_3d<double> pt,
                          const float radius, const SbColor& color)
{
  return draw_sphere ((float)pt.x(), (float)pt.y(), (float)pt.z(), radius, color);
}


//: draw a cylinder, given the radius, the coordinates of the start and end points of the centerline
SoSeparator* draw_cylinder(float x1, float y1, float z1,
                           float x2, float y2, float z2,
                           float radius,
                           const SbColor& color,
                           float transparency)
{
  SoSeparator* root = new SoSeparator;
#if 0
  // color
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);
#endif // 0
  // Create a SoMaterial to draw color in detail.
  SoMaterial *material = new SoMaterial;
  material->diffuseColor.setValue (color);
  material->emissiveColor.setValue(color/2);
  material->transparency = transparency;
  root->addChild (material);

  // translation
  SoTranslation* trans = new SoTranslation;
  trans->translation.setValue (SbVec3f(x1, y1, z1));
  root->addChild (trans);

  // rotaiton
  SoRotation* rot = new SoRotation;

  vgl_vector_3d<float > y_axis(0, 1, 0);
  vgl_vector_3d<float > cylinder_orient(x2-x1, y2-y1, z2-z1);
  vgl_vector_3d<float > rot_axis = cross_product(y_axis, cylinder_orient);
  float rot_angle = (float)angle(y_axis, cylinder_orient);

  rot->rotation.setValue(SbVec3f(rot_axis.x(), rot_axis.y(), rot_axis.z()), rot_angle);
  root->addChild (rot);

  SoCylinder* cylinder = new SoCylinder();
  cylinder->radius = radius;
  cylinder->height = std::sqrt(vnl_math::sqr(x2-x1) +
                              vnl_math::sqr(y2-y1) +
                              vnl_math::sqr(z2-z1));

  root->addChild(cylinder);
  return root;
}

SoSeparator* draw_cylinder(const vgl_point_3d<double>& centerline_start,
                           const vgl_point_3d<double>& centerline_end,
                           float radius,
                           const SbColor& color,
                           float transparency)
{
  return draw_cylinder((float)centerline_start.x(), (float)centerline_start.y(),
                       (float)centerline_start.z(), (float)centerline_end.x(),
                       (float)centerline_end.y(), (float)centerline_end.z(),
                       radius,
                       color,
                       transparency);
}


//:
// Important:
//  Should use the draw_geometry to draw group of things for
//  better performance.

SoSeparator* draw_polyline (std::vector<vgl_point_3d<double> >& vertices,
                            const float& width, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);

  // Line width
  SoDrawStyle*  drawStyle = new SoDrawStyle;
  drawStyle->lineWidth.setValue (width);
  root->addChild (drawStyle);

  draw_polyline_geom (root, vertices);

  return root;
}

void draw_polyline_geom (SoGroup *root, std::vector<vgl_point_3d<double> >& vertices)
{
  unsigned int nVertices = vertices.size();
  SbVec3f* verts = new SbVec3f[nVertices];

  for (unsigned int i=0; i<nVertices; i++) {
    vgl_point_3d<double> p = vertices[i];
    verts[i] = SbVec3f( (float)p.x(), (float)p.y(), (float)p.z() );
  }

  SoCoordinate3* coords = new SoCoordinate3;
  coords->point.setValues (0, nVertices, verts);
  root->addChild (coords);

  SoLineSet* lineSet = new SoLineSet;
  lineSet->numVertices.set1Value (0, nVertices);
  root->addChild (lineSet);

  delete[] verts;
}

// draws a polyline with the specified color and drawstyle
SoSeparator* draw_polyline (const float** vertices, const unsigned int nVertices,
                            const SbColor& color, const SoDrawStyle* drawStyle)
{
  SoSeparator* root = new SoSeparator;

  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);

  // Line width
  root->addChild ((SoDrawStyle*) drawStyle); // casting away const !!!

  SbVec3f* verts = new SbVec3f[nVertices];
  for (unsigned int i=0; i<nVertices; i++)
    verts[i] = SbVec3f( vertices[i][0], vertices[i][1], vertices[i][2] );

  draw_polyline_geom (root, verts, nVertices);

  delete[] verts;

  return root;
}

// draws a single polyline with no special properties
void draw_polyline_geom (SoGroup* root, SbVec3f* vertices, const unsigned int nVertices)
{
  SoCoordinate3* coords = new SoCoordinate3;
  coords->point.setValues (0, nVertices, vertices);
  root->addChild (coords);

  SoLineSet* lineSet = new SoLineSet;
  lineSet->numVertices.set1Value (0, nVertices);
  root->addChild (lineSet);
}


void draw_filled_polygon_geom_ (SoGroup* root, const std::vector<bmsh3d_vertex*>& vertices)
{
  SbVec3f* verts = new SbVec3f[vertices.size()];
  for (unsigned int i=0; i<vertices.size(); i++)
    verts[i] = SbVec3f(float(vertices[i]->pt().x()), float(vertices[i]->pt().y()), float(vertices[i]->pt().z()));

  draw_filled_polygon_geom_ (root, verts, vertices.size());

  delete[] verts;
}

//:
//  Ming: We have two options to draw the IndexedFaceSet.
//  Option 1: make the emissiveColor the same as diffuseColor
//            The sheet will look continuously.
//  Option 2: make the emissiveColor half value of diffuseColor (color/2)
//            to show individual sheet elements.
// draws a polygon with the specified color, drawstyle and transparency

SoSeparator* draw_filled_polygon (SbVec3f* vertices, const unsigned int nVertices,
                                  const SbColor& color, const float fTransparency)
{
  SoSeparator* root = new SoSeparator;

  // Create a SoMaterial to draw color in detail.
  SoMaterial *material = new SoMaterial;
  material->diffuseColor.setValue (color);
  material->emissiveColor.setValue(color/2);
  material->transparency = fTransparency;
  root->addChild (material);

  draw_filled_polygon_geom_ (root, vertices, nVertices);

  return root;
}

SoSeparator* draw_filled_polygon (float** vertices, const unsigned int nVertices,
                                  const SbColor& color, const float fTransparency)
{
  SoSeparator* root = new SoSeparator;

  // Create a SoMaterial to draw color in detail.
  SoMaterial *material = new SoMaterial;
  material->diffuseColor.setValue (color);
  material->emissiveColor.setValue(color/2);
  material->transparency = fTransparency;
  root->addChild (material);

  SbVec3f* verts = new SbVec3f[nVertices];
  for (unsigned int i=0; i<nVertices; i++)
    verts[i] = SbVec3f (vertices[i][0], vertices[i][1], vertices[i][2]);

  draw_filled_polygon_geom_ (root, verts, nVertices);

  delete[] verts;
  return root;
}

// draw a simple filled polygon with no special properties
void draw_filled_polygon_geom_ (SoGroup* root, SbVec3f* vertices, const unsigned int nVertices)
{
  SoCoordinate3* coords = new SoCoordinate3;
  coords->point.setValues (0, nVertices, vertices);
  root->addChild (coords);

  SoFaceSet* faceSet = new SoFaceSet;
  faceSet->numVertices.set1Value (0, nVertices);
  root->addChild (faceSet);
}

SoSeparator* draw_line_set (const std::vector<std::pair<vgl_point_3d<double>, vgl_point_3d<double> > >& lines,
                            const SbColor& color)
{
  SoSeparator* root = new SoSeparator;
  // color
  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);

  draw_line_set_geom (root, lines);

  return root;
}

//: draw the whole set of lines in a single SoIndexedLineSet.
void draw_line_set_geom(SoSeparator* root,
                        const std::vector<std::pair<vgl_point_3d<double>, vgl_point_3d<double> > >& lines)
{
  unsigned int nVertices = lines.size() * 2;
  float (*xyz)[3] = new float[nVertices][3];
  unsigned int nLinesIndices = lines.size() * 3; // sid : eid : -1
  int* ind = new int [nLinesIndices];

  // Assign vertices and lines
  for (unsigned int i=0; i < lines.size(); i++) {
    vgl_point_3d<double> Ps = lines[i].first;
    vgl_point_3d<double> Pe = lines[i].second;
    xyz[i*2][0] = (float) Ps.x();
    xyz[i*2][1] = (float) Ps.y();
    xyz[i*2][2] = (float) Ps.z();
    xyz[i*2+1][0] = (float) Pe.x();
    xyz[i*2+1][1] = (float) Pe.y();
    xyz[i*2+1][2] = (float) Pe.z();
    ind[i*3] = i*2;
    ind[i*3+1] = i*2+1;
    ind[i*3+2] = -1;
  }

  SoVertexProperty* vp = new SoVertexProperty;
  vp->vertex.setValues (0, nVertices, xyz);

  SoIndexedLineSet* indexedLineSet = new SoIndexedLineSet ();
  indexedLineSet->vertexProperty = vp;
  indexedLineSet->coordIndex.setValues (0, nLinesIndices, ind);

  delete []ind;
  delete []xyz;

  root->addChild (indexedLineSet);
}

void draw_triangle_geom (SoGroup* root,
                         const vgl_point_3d<double>& Pt1,
                         const vgl_point_3d<double>& Pt2,
                         const vgl_point_3d<double>& Pt3)
{
  SoCoordinate3* coords = new SoCoordinate3;
  coords->point.set1Value( 0, SbVec3f( (float)Pt1.x(), (float)Pt1.y(), (float)Pt1.z()) );
  coords->point.set1Value( 1, SbVec3f( (float)Pt2.x(), (float)Pt2.y(), (float)Pt2.z()) );
  coords->point.set1Value( 2, SbVec3f( (float)Pt3.x(), (float)Pt3.y(), (float)Pt3.z()) );
  root->addChild(coords);

  SoFaceSet* faceSet = new SoFaceSet;
  faceSet->numVertices.set1Value(0,3);
  root->addChild(faceSet);
}

// draws a single triangle with no special properties
void draw_triangle_geom (SoGroup *root,
                         const float x1, const float y1, const float z1,
                         const float x2, const float y2, const float z2,
                         const float x3, const float y3, const float z3)
{
  SoCoordinate3* coords = new SoCoordinate3;
  coords->point.set1Value(0,SbVec3f(x1,y1,z1));
  coords->point.set1Value(1,SbVec3f(x2,y2,z2));
  coords->point.set1Value(2,SbVec3f(x3,y3,z3));
  root->addChild(coords);

  SoFaceSet* faceSet = new SoFaceSet;
  faceSet->numVertices.set1Value(0,3);
  root->addChild(faceSet);
}

// draws a line with the specified color and drawstyle
SoSeparator* draw_line (const float x1, const float y1, const float z1,
                        const float x2, const float y2, const float z2,
                        const SbColor& color, const SoDrawStyle* drawStyle)
{
  SoSeparator* root = new SoSeparator;

  // color
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);

  if (drawStyle)
    root->addChild ((SoDrawStyle*) drawStyle); // casting away const !!!

  draw_line_geom (root, x1, y1, z1, x2, y2, z2);
  return root;
}

// draws a line with the specified color and drawstyle
SoSeparator* draw_line (const vgl_point_3d<double>& pt1, const vgl_point_3d<double>& pt2,
                        const SbColor& color, const SoDrawStyle* drawStyle)
{
  return draw_line ((float)pt1.x(), (float)pt1.y(), (float)pt1.z(),
                    (float)pt2.x(), (float)pt2.y(), (float)pt2.z(), color, drawStyle);
}

// draws a single line with no special properties
void draw_line_geom (SoGroup* root,
                     const float x1, const float y1, const float z1,
                     const float x2, const float y2, const float z2)
{
  SoCoordinate3* coord = new SoCoordinate3;
  coord->point.set1Value(0, SbVec3f( x1,y1,z1 ) );
  coord->point.set1Value(1, SbVec3f( x2,y2,z2 ) );
  root->addChild( coord );

  SoLineSet* lineSet = new SoLineSet;
  lineSet->numVertices.set1Value( 0, 2 );
  root->addChild( lineSet );
}

void draw_line_geom (SoGroup* root, const vgl_point_3d<double>& pt1, const vgl_point_3d<double>& pt2)
{
  draw_line_geom (root,
                  (float)pt1.x(), (float)pt1.y(), (float)pt1.z(),
                  (float)pt2.x(), (float)pt2.y(), (float)pt2.z());
}

SoSeparator* draw_text2d (const char* text,
                          const float x, const float y, const float z)
{
  SoSeparator* root = new SoSeparator;

  SoTransform *instrTransf = new SoTransform;
  instrTransf->translation.setValue (x, y, z);
  root->addChild(instrTransf);

  draw_text2d_geom (root, text);

  return root;
}

SoSeparator* draw_box (const double& min_x, const double& min_y, const double& min_z,
                       const double& max_x, const double& max_y, const double& max_z,
                       const float& width, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  // color
  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);

  SoDrawStyle* drawStyle = new SoDrawStyle;
  drawStyle->lineWidth.setValue (width);
  root->addChild (drawStyle);

  // side parallel to X
  draw_line_geom (root, (float)min_x, (float)min_y, (float)min_z, (float)max_x, (float)min_y, (float)min_z);
  draw_line_geom (root, (float)min_x, (float)min_y, (float)max_z, (float)max_x, (float)min_y, (float)max_z);
  draw_line_geom (root, (float)min_x, (float)max_y, (float)min_z, (float)max_x, (float)max_y, (float)min_z);
  draw_line_geom (root, (float)min_x, (float)max_y, (float)max_z, (float)max_x, (float)max_y, (float)max_z);

  // side parallel to Y
  draw_line_geom (root, (float)min_x, (float)min_y, (float)min_z, (float)min_x, (float)max_y, (float)min_z);
  draw_line_geom (root, (float)min_x, (float)min_y, (float)max_z, (float)min_x, (float)max_y, (float)max_z);
  draw_line_geom (root, (float)max_x, (float)min_y, (float)min_z, (float)max_x, (float)max_y, (float)min_z);
  draw_line_geom (root, (float)max_x, (float)min_y, (float)max_z, (float)max_x, (float)max_y, (float)max_z);

  // side parallel to Z
  draw_line_geom (root, (float)min_x, (float)min_y, (float)min_z, (float)min_x, (float)min_y, (float)max_z);
  draw_line_geom (root, (float)min_x, (float)max_y, (float)min_z, (float)min_x, (float)max_y, (float)max_z);
  draw_line_geom (root, (float)max_x, (float)min_y, (float)min_z, (float)max_x, (float)min_y, (float)max_z);
  draw_line_geom (root, (float)max_x, (float)max_y, (float)min_z, (float)max_x, (float)max_y, (float)max_z);

  return root;
}

SoSeparator* draw_box (const vgl_box_3d<double>& box,
                       const float& width, const SbColor& color)
{
  return draw_box (box.min_x(), box.min_y(), box.min_z(),
                   box.max_x(), box.max_y(), box.max_z(),
                   width, color);
}

SoSeparator* draw_text2d (const char* text,
                          const float x, const float y, const float z,
                          const SoBaseColor* basecolor)
{
  SoSeparator* root = new SoSeparator;

  SoTransform *instrTransf = new SoTransform;
  instrTransf->translation.setValue (x, y, z);
  root->addChild(instrTransf);

  // basecolor
  root->addChild ((SoBaseColor*) basecolor); // casting away const !!!

  draw_text2d_geom (root, text);

  return root;
}

void draw_text2d_geom (SoSeparator* root, const char* text)
{
  SoText2 * text2 = new SoText2;
  SbString txt = SbString (text);
  text2->string.setValues (0, sizeof(text) / sizeof(char *), &txt);
  text2->justification = SoText2::LEFT;
  root->addChild(text2);
}

// ############################################################

vnl_random colrand;

void init_rand_color (unsigned long seed)
{
  colrand.reseed (seed);
}

VIS_COLOR_CODE get_next_rand_ccode ()
{
  double c = colrand.drand32 (1, COLOR_ORANGE);
  return (VIS_COLOR_CODE) int (c);
}

SbColor get_next_rand_color ()
{
  float r = (float) colrand.drand32 (0, 1);
  float g = (float) colrand.drand32 (0, 1);
  float b = (float) colrand.drand32 (0, 1);

  SbColor color = SbColor (r, g, b);

  return color;
}

SoSeparator* shift_output (SoSeparator* vis, float shift_x, float shift_y, float shift_z)
{
  SoSeparator* trans_group = new SoSeparator;
  SoTranslation *trans = new SoTranslation;
  trans->translation.setValue (SbVec3f (shift_x, shift_y, shift_z));
  trans_group->addChild (trans);
  trans_group->addChild (vis);
  return trans_group;
}

// ##################################################################

#if 0

void draw_point_test (SoSeparator* root)
{
  constexpr int NUMPOINTS = 5000;
  float xyz[NUMPOINTS][3];

  for (int i=0; i < NUMPOINTS; i++) {
    xyz[i][0] = (float)rand() / (float)RAND_MAX * 10;
    xyz[i][1] = (float)rand() / (float)RAND_MAX * 10;
    xyz[i][2] = (float)rand() / (float)RAND_MAX * 10;
  }

  SoCoordinate3 * coord3 = new SoCoordinate3;
  coord3->point.setValues(0, NUMPOINTS, xyz);
  root->addChild(coord3);

  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = 1;
  root->addChild(drawstyle);

  SoPointSet * pointset = new SoPointSet;
  root->addChild(pointset);
}

void draw_text_test (SoSeparator* root)
{
  SoText2 * text = new SoText2;

  const char * str[] = {
    "Yo Yo Yo. 2D Text is working!",
    "LEMS Brown University",
    "1 2 3 4 5 6 7 8 9 10.",
  };

  text->string.setValues(0, sizeof(str) / sizeof(char *), str);
  text->justification = SoText2::LEFT;
  SoTransform *instrTransf = new SoTransform;
  instrTransf->translation.setValue(-1.0f,5.0f,0.0f);
  root->addChild(instrTransf);
  root->addChild(text);
}

void draw_polyline_test (SoSeparator* root)
{
  // Points in N*3 float array.
  SoVertexProperty *vertexList = new SoVertexProperty();
  root->addChild(vertexList);

  int N = 3;
#if 0
  for (int i=0; i<N; i++)
    vertexList->vertex.set1Value (i, 1.2*i, 1.3*i*i, 1.4*i);
#endif // 0
  vertexList->vertex.set1Value (0, 0, 0, 0);
  vertexList->vertex.set1Value (1, 1, 1, 1);
  vertexList->vertex.set1Value (2, 5, 1, 1);

  // Line
  int N_LINE = N-1;
  int *lineindex = new int[N_LINE*3];
  for (int i=0; i<N_LINE; i++) {
    lineindex[i*3] = i;
    lineindex[i*3+1] = i+1;
    lineindex[i*3+2] = -1;
  }

  SoIndexedLineSet *vertIndex = new SoIndexedLineSet();
  vertIndex->coordIndex.setValues (0, N_LINE*3, lineindex);
  root->addChild(vertIndex);

  delete[] lineindex;
}

void draw_polygon_test (SoSeparator* root)
{
  // Points in N*3 float array.
  SoVertexProperty *vertexList = new SoVertexProperty();
  root->addChild(vertexList);

  int N = 3;
#if 0
  for (int i=0; i<N; i++)
    vertexList->vertex.set1Value (i, 1.2*i, 1.3*i*i, 1.4*i);
#endif // 0
  vertexList->vertex.set1Value (0, 0, 0, 0);
  vertexList->vertex.set1Value (1, 1, 1, 1);
  vertexList->vertex.set1Value (2, 5, 1, 1);

  // Line
  int N_LINE = N;
  int *lineindex = new int[N_LINE*3];
  for (int i=0; i<N_LINE-1; i++) {
    lineindex[i*3] = i;
    lineindex[i*3+1] = i+1;
    lineindex[i*3+2] = -1;
  }
  int i = N_LINE-1;
  lineindex[i*3] = i;
  lineindex[i*3+1] = 0;
  lineindex[i*3+2] = -1;

  SoIndexedLineSet *vertIndex = new SoIndexedLineSet();
  vertIndex->coordIndex.setValues (0, N_LINE*3, lineindex);
  root->addChild(vertIndex);

  delete []lineindex;
}


void draw_filled_polygon_test (SoSeparator* root)
{
  // Points in N*3 float array.
  SoVertexProperty *vertexList = new SoVertexProperty();
  root->addChild(vertexList);

  int N = 3;
#if 0
  for (int i=0; i<N; i++)
    vertexList->vertex.set1Value (i, 1.2*i, 1.3*i*i, 1.4*i);
#endif // 0
  vertexList->vertex.set1Value (0, 0, 0, 0);
  vertexList->vertex.set1Value (1, 1, 1, 1);
  vertexList->vertex.set1Value (2, 5, 1, 1);

  // Face 1
  int N_VERTICES = (N+1)*2;
  int *faceindex = new int[N_VERTICES];
  for (int i=0; i<N; i++)
    faceindex[i] = i;
  faceindex[N] = -1;

  // Face 2
  for (int i=0; i<N; i++)
    faceindex[N+1+i] = N-1-i;
  faceindex[N*2+1] = -1;

  SoIndexedFaceSet *vertIndex = new SoIndexedFaceSet();
  vertIndex->coordIndex.setValues (0, N_VERTICES, faceindex);
  root->addChild(vertIndex);

  SoShapeHints * hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  hints->shapeType = SoShapeHints::SOLID;
  root->addChild(hints);

  delete[] faceindex;
}

void draw_indexed_line_test (SoSeparator* root)
{
  // Put a wireframe cube in the scene.
  SoSeparator *cubeSep = new SoSeparator();

  const SbVec3f CubeMin   = SbVec3f(-1.0, -1.0, -1.0);
  const SbVec3f CubeMax   = SbVec3f( 1.0,  1.0,  1.0);

  ////////////////////////////////////////////////////////////

  //:
  // \verbatim
  //  Create an array of points at the vertices of the cube.
  //
  //                             y
  //                             ^
  //                  2          |                 3
  //                 .-----------|----------------.
  //                /            |               /|
  //               / |           |              / |
  //              /                            /  |
  //           7 /   |                      6 /   |
  //            .----------------------------.    |
  //            |    |                       |    |
  //            |                            |    |
  //            |    |                       |  ------> x
  //            |                            |    |
  //            |    |                       |    |
  //            |                            |    |
  //            |    |                       |    |
  //            |    .- - - - - - - - - - - -|- - .
  //            |     0                      |   /  1
  //            |  /                         |  /
  //            |                            | /
  //            |/                           |/
  //            .----------------------------.
  //          4                                5
  // \endverbatim

  constexpr int numCubeVerts = 8;
  static float cubeVerts[numCubeVerts][3] =
  {
    { CubeMin[0],  CubeMin[1],  CubeMax[2] },  // vert  0
    { CubeMax[0],  CubeMin[1],  CubeMax[2] },  // vert  1
    { CubeMin[0],  CubeMax[1],  CubeMax[2] },  // vert  2
    { CubeMax[0],  CubeMax[1],  CubeMax[2] },  // vert  3

    { CubeMin[0],  CubeMin[1],  CubeMin[2] },  // vert  4
    { CubeMax[0],  CubeMin[1],  CubeMin[2] },  // vert  5
    { CubeMin[0],  CubeMax[1],  CubeMin[2] },  // vert  6
    { CubeMax[0],  CubeMax[1],  CubeMin[2] }   // vert  7
  };

  ////////////////////////////////////////////////////////////
  //
  //  Index into the array of vertices to create segments
  //  defining the edges of the cube.
  //
  //  For example,  0, 1, -1  will designate a segment from
  //  cubeVerts[0] to cubeVerts[1].

  SoVertexProperty *vertexList = new SoVertexProperty();
  vertexList->vertex.setValues( 0,  numCubeVerts,  cubeVerts);
  root->addChild(vertexList);

  constexpr int numEdgeIndexes = 36;
  static int32_t cubeIndex[numEdgeIndexes] =
  {
    0, 1, -1,              // -1 terminates the edge.
    2, 3, -1,
    0, 2, -1,
    1, 3, -1,

    4, 5, -1,
    6, 7, -1,
    4, 6, -1,
    5, 7, -1,

    0, 4, -1,
    1, 5, -1,
    2, 6, -1,
    3, 7, -1,
  };

  SoIndexedLineSet *vertIndex = new SoIndexedLineSet();
  vertIndex->coordIndex.setValues(0, numEdgeIndexes, cubeIndex);
  root->addChild(vertIndex);
  root->addChild(cubeSep);
}

void draw_primitives_test (SoSeparator* root)
{
  root->addChild (new SoCone);
  root->addChild (new SoCylinder);
  root->addChild (new SoSphere);
}

#endif
