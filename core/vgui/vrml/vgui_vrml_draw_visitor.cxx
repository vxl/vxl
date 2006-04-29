// This is core/vgui/vrml/vgui_vrml_draw_visitor.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   09 Jan 99
//-----------------------------------------------------------------------------

#include "vgui_vrml_draw_visitor.h"

#include <vcl_functional.h>
#include <vcl_string.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_map.h>

#include <vnl/vnl_float_3.h>
#include <vnl/vnl_cross.h>
#include <vnl/vnl_math.h>

#include <Qv/QvString.h>
#include <Qv/QvInput.h>
#include <Qv/QvState.h>
#include <Qv/QvNode.h>

#include <Qv/QvGroup.h>
#include <Qv/QvCoordinate3.h>
#include <Qv/QvIndexedLineSet.h>
#include <Qv/QvIndexedFaceSet.h>
#include <Qv/QvPointSet.h>
#include <Qv/QvSphere.h>
#include <Qv/QvCone.h>
#include <Qv/QvCylinder.h>
#include <Qv/QvCube.h>

#include <Qv/QvTransform.h>
#include <Qv/QvTranslation.h>
#include <Qv/QvRotation.h>
#include <Qv/QvScale.h>

#include <Qv/QvMaterial.h>
#include <Qv/QvTexture2.h>

#include "vgui_vrml_texture_map.h"


static inline void GlVertex(const point3D& p)
{
  glVertex3f(p.x, p.y, p.z);
}


// FIXME : see Templates/map+string.vgui_vrml_texture_map~-.C for explanation.
#if 0
typedef vcl_map<vcl_string, vgui_vrml_texture_map*, vcl_less<vcl_string> > vgui_vrml_texture_mapMap;
static vgui_vrml_texture_map* current_texmap = 0;
static vgui_vrml_texture_mapMap texturemaps; // Images

static vgui_vrml_texture_map* gettexture(char const* filename)
{
  vgui_vrml_texture_mapMap::iterator p = texturemaps.find(filename);
  if (p != texturemaps.end())
    return (*p).second;

  vgui_vrml_texture_map* newmap = vgui_vrml_texture_map::create(filename);

  return texturemaps[filename] = newmap;
}
#else // 0

static vgui_vrml_texture_map *current_texmap = 0;
static vgui_vrml_texture_map* gettexture(char const* filename)
{
  static vcl_vector<vcl_string> names;
  static vcl_vector<vgui_vrml_texture_map*> tmaps;

  unsigned N=names.size();
  for (unsigned i=0; i<N; ++i)
    if (names[i] == vcl_string(filename))
      return tmaps[i];

  vgui_vrml_texture_map *newmap = vgui_vrml_texture_map::create(filename);
  names.push_back(filename);
  tmaps.push_back(newmap);

  return newmap;
}
#endif // 0


vgui_vrml_draw_visitor::vgui_vrml_draw_visitor()
{
  gl_mode = textured;
  quadric = gluNewQuadric();
  twosided = true;
  remake_texture = true;
}

vgui_vrml_draw_visitor::~vgui_vrml_draw_visitor()
{
  gluDeleteQuadric(quadric);
}

// GROUPS

bool vgui_vrml_draw_visitor::Visit(QvSeparator* node)
{
  // Mask Bit                 Attribute Group
  // GL_ACCUM_BUFFER_BIT      accum-buffer
  // GL_ALL_ATTRIB_BITS       ---
  // GL_COLOR_BUFFER_BIT      color-buffer
  // GL_CURRENT_BIT           current
  // GL_DEPTH_BUFFER_BIT      depth-buffer
  // GL_ENABLE_BIT            enable
  // GL_EVAL_BIT              eval
  // GL_FOG_BIT               fog
  // GL_HINT_BIT              hint
  // GL_LIGHTING_BIT          lighting
  // GL_LINE_BIT              line
  // GL_LIST_BIT              list
  // GL_PIXEL_MODE_BIT        pixel
  // GL_POINT_BIT             point
  // GL_POLYGON_BIT           polygon
  // GL_POLYGON_STIPPLE_BIT   polygon-stipple
  // GL_SCISSOR_BIT           scissor
  // GL_STENCIL_BUFFER_BIT    stencil-buffer
  // GL_TEXTURE_BIT           texture
  // GL_TRANSFORM_BIT         transform
  // GL_VIEWPORT_BIT          viewport
  glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT);
  glPushMatrix();
  QvVisitor::Visit(node);
  glPopMatrix();
  glPopAttrib();
  return true;
}

bool vgui_vrml_draw_visitor::Visit(QvTransformSeparator* node)
{
  glPushMatrix();
  QvVisitor::Visit(node);
  glPopMatrix();
  return true;
}

#define QUADRIC_COMPLEXITY 24

bool vgui_vrml_draw_visitor::Visit(QvShapeHints* /*node*/)
{
  if (gl_mode == wireframe)
    return false;
  return true;
}


// GEOMETRY
bool vgui_vrml_draw_visitor::Visit(QvCube* node)
{
  // cube has width height depth

  static GLfloat normals[6][3] = {
    {-1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {1.0, 0.0, 0.0},
    {0.0, -1.0, 0.0},
    {0.0, 0.0, 1.0},
    {0.0, 0.0, -1.0}
  };
  static GLint faces[6][4] = {
    {0, 1, 2, 3},
    {3, 2, 6, 7},
    {7, 6, 5, 4},
    {4, 5, 1, 0},
    {5, 6, 2, 1},
    {7, 4, 0, 3}
  };

  GLfloat vertices[8][3];
  GLint i;
  float width = node->width.value;
  float height = node->height.value;
  float depth = node->depth.value;

  vertices[0][0] = vertices[1][0] = vertices[2][0] = vertices[3][0] = -width/2;
  vertices[4][0] = vertices[5][0] = vertices[6][0] = vertices[7][0] = width/2;
  vertices[0][1] = vertices[1][1] = vertices[4][1] = vertices[5][1] = -height/2;
  vertices[2][1] = vertices[3][1] = vertices[6][1] = vertices[7][1] = height/2;
  vertices[0][2] = vertices[3][2] = vertices[4][2] = vertices[7][2] = -depth/2;
  vertices[1][2] = vertices[2][2] = vertices[5][2] = vertices[6][2] = depth/2;

  for (i = 5; i >= 0; i--) {
    if (gl_mode != wireframe)
      glBegin(GL_QUADS);
    else
      glBegin(GL_LINE_LOOP);
    glNormal3fv(&normals[i][0]);
    glVertex3fv(&vertices[faces[i][0]][0]);
    glVertex3fv(&vertices[faces[i][1]][0]);
    glVertex3fv(&vertices[faces[i][2]][0]);
    glVertex3fv(&vertices[faces[i][3]][0]);
    glEnd();
  }
  return true;
}


bool vgui_vrml_draw_visitor::Visit(QvSphere* node)
{
  float r = node->radius.value;

  // gluQuadricDrawStyle GLU_LINE GLU_FILL GLU_POINT GLU_SILHOUETTE
  if (gl_mode == wireframe)
    gluQuadricDrawStyle(quadric, GLenum(GLU_LINE));
  else
    gluQuadricDrawStyle(quadric, GLenum(GLU_FILL));

  gluSphere(quadric, r, QUADRIC_COMPLEXITY, QUADRIC_COMPLEXITY);
  return true;
}

bool vgui_vrml_draw_visitor::Visit(QvCylinder* node)
{
  float r = node->radius.value;
  float h = node->height.value;
  int parts = (int)node->parts.value;

  glRotated(-90, 1, 0, 0);

  glTranslatef(0,0,-h/2);

  if (gl_mode == wireframe)
    gluQuadricDrawStyle(quadric, GLenum(GLU_LINE));
  else
    gluQuadricDrawStyle(quadric, GLenum(GLU_FILL));


  // fsm : GLU_OUTSIDE/GLU_INSIDE are not GLenums. cast avoids warning (egcs).
  gluQuadricOrientation(quadric, GLenum(GLU_OUTSIDE) );
  if (parts && (QvCylinder::SIDES | QvCylinder::ALL))
    gluCylinder(quadric, r,r, h, QUADRIC_COMPLEXITY, 2);

  if (parts && (QvCylinder::BOTTOM | QvCylinder::ALL)) {
    gluQuadricOrientation(quadric, GLenum(GLU_INSIDE) );
    gluDisk(quadric, 0, r, QUADRIC_COMPLEXITY, 3);
    gluQuadricOrientation(quadric, GLenum(GLU_OUTSIDE) );
  }

  glTranslatef(0,0,h);

  if (parts && (QvCylinder::TOP | QvCylinder::ALL))
    gluDisk(quadric, 0, r, QUADRIC_COMPLEXITY, 3); // z = h

  glTranslatef(0,0,-h/2);

  glRotated(90, 1, 0, 0);
  return true;
}

bool vgui_vrml_draw_visitor::Visit(QvCone* node)
{
  float r = node->bottomRadius.value;
  float h = node->height.value;
  int parts = (int)node->parts.value;
  float tz = h/2;

  glRotated(90, 1, 0, 0);
  glTranslatef(0,0, -tz);

  if (gl_mode == wireframe)
    gluQuadricDrawStyle(quadric, GLenum(GLU_LINE));
  else
    gluQuadricDrawStyle(quadric, GLenum(GLU_FILL));

  if (parts && (QvCone::SIDES | QvCone::ALL))
    gluCylinder(quadric, 0, r, h, QUADRIC_COMPLEXITY, 2);

  glTranslatef(0,0, 2 * tz);

  if (parts && (QvCone::BOTTOM | QvCone::ALL))
    gluDisk(quadric, 0, r, QUADRIC_COMPLEXITY, 3);

  glTranslatef(0,0, -tz);
  glRotated(-90, 1, 0, 0);
  return true;
}

bool vgui_vrml_draw_visitor::Visit(QvPointSet* ps)
{
  glPushAttrib(GL_CURRENT_BIT);
  glDisable(GL_TEXTURE_2D);
  glBegin(GL_POINTS);

  int n = (ps->numPoints.value == -1) ? ps->num_ : ps->numPoints.value;
  n += ps->startIndex.value;
  for (int i = ps->startIndex.value; i < n; ++i)
    GlVertex(ps->points_[i]);
  glEnd();
  glPopAttrib();

  return true;
}

bool vgui_vrml_draw_visitor::Visit(QvIndexedLineSet* node)
{
  glPushAttrib(GL_CURRENT_BIT);
  glDisable(GL_TEXTURE_2D);

  const point3D* vertexlist = node->vertexlist_;   // vertex data
  int numvertinds = node->numvertinds_;            // no. of vertex indices
  const int* vertindices = node->vertindices_;     // vertex index list

  glBegin(GL_LINES);
  for (int j = 0; j < numvertinds-1; ++j) {
    int i1 = vertindices[j];
    int i2 = vertindices[j+1];

    if (i1 != -1 && i2 != -1) {
      GlVertex(vertexlist[i1]);
      GlVertex(vertexlist[i2]);
    }
  }
  glEnd();
  glPopAttrib();
  return true;
}

bool vgui_vrml_draw_visitor::Visit(QvIndexedFaceSet* node)
{
  // members of QvIndexedFaceSet
  //const point3D* vertexlist_;         // vertex data
  //int numvertinds_;                   // no. of vertex indices
  //const int* vertindices_;            // vertex index list
  // Fields:
  //     QvMFLong               coordIndex;             // Coordinate indices
  //     QvMFLong               materialIndex;          // Material indices
  //     QvMFLong               normalIndex;            // Surface normal indices
  //     QvMFLong               textureCoordIndex;      // Texture Coordinate indices

  const point3D* vertexlist = node->vertexlist_;   // vertex data
  int numvertinds = node->numvertinds_;            // no. of vertex indices
  const int* vertindices = node->vertindices_;     // vertex index list

  const point2D* texvertlist = node->texvertlist_;        // texture vertices
  int numtextureinds = node->numtextureinds_;                // no. of texture indices
  const int* textureindices = node->textureindices_;         // texture index list

  vcl_vector<point3D const*> polyverts;
  vcl_vector<point2D const*> polytexcoords;
  //int i = 0;
  for (int i = 0; i < numvertinds; ++i)
  {
    // Collect verts
    polyverts.clear();
    polytexcoords.clear();
    while (i < numvertinds && vertindices[i] != -1) {
      polyverts.push_back(&vertexlist[vertindices[i]]);
      if (i < numtextureinds && gl_mode == textured)
        polytexcoords.push_back(&texvertlist[textureindices[i]]);
      ++i;
    }
    unsigned n = polyverts.size();
    if (n < 3) {
      vcl_cerr << "Bad poly, n = " << n << '\n';
      continue;
    }

    // Begin poly or wireframe
    vnl_float_3 normal;
    if (gl_mode == wireframe) {
      glBegin(GL_LINE_LOOP);
    }
    else {
      // Compute normal if shaded
      vnl_float_3 a(polyverts[0]->x, polyverts[0]->y, polyverts[0]->z);
      vnl_float_3 b(polyverts[1]->x, polyverts[1]->y, polyverts[1]->z);
      vnl_float_3 c(polyverts[2]->x, polyverts[2]->y, polyverts[2]->z);
      normal = vnl_cross_3d(b - a, c - b);
      normal *= -1.0 / normal.magnitude();

      glBegin(GL_POLYGON);

      glNormal3f(normal[0], normal[1], normal[2]);
    }

    for (unsigned v = 0; v < n; ++v) {
      if (gl_mode == textured && v < polytexcoords.size())
        glTexCoord2f(polytexcoords[v]->x, polytexcoords[v]->y);

      GlVertex(*polyverts[v]);
    }
    glEnd();

    // If twosided, do reverse also
    if (twosided && gl_mode != wireframe) {
      glBegin(GL_POLYGON);
      glNormal3f(-normal[0], -normal[1], -normal[2]);
      for (unsigned v = n; v > 0;) {
        --v;
        if (v < polytexcoords.size())
          glTexCoord2f(polytexcoords[v]->x, polytexcoords[v]->y);
        GlVertex(*polyverts[v]);
      }
      glEnd();
    }
  }
  return true;
}

bool vgui_vrml_draw_visitor::Visit(QvAsciiText* /*node*/)
{
  return true;
}

static void GlRotate(double angle_rad, const float* axis)
{
  static const double RAD2DEG = 180.0 / vnl_math::pi;
  glRotatef(angle_rad * RAD2DEG, axis[0], axis[1], axis[2]);
}

static void GlRotate(const QvSFRotation& r)
{
  GlRotate(r.angle, r.axis);
}

// TRANSFORMS
bool vgui_vrml_draw_visitor::Visit(QvTransform* node)
{
  const vector3D* tran1 = (const vector3D*) node->translation.value;
  const vector3D* tran2 = (const vector3D*) node->center.value;
  const float* scale = node->scaleFactor.value;

  glTranslatef (tran1->x + tran2->x, tran1->y + tran2->y, tran1->z + tran2->z);
  GlRotate(node->rotation);

  // just rotates the scaling vector
  GlRotate(node->scaleOrientation.angle, node->scaleOrientation.axis);
  glScalef (scale[0], scale[1], scale[2]);

  // just rotates the scaling vector
  GlRotate(-node->scaleOrientation.angle, node->scaleOrientation.axis);
  glTranslatef (-tran2->x, -tran2->y, -tran2->z);  // - tran2
  return true;
}

bool vgui_vrml_draw_visitor::Visit(QvScale* node)
{
  const float* scale = node->scaleFactor.value;
  if (!scale[0] || !scale[1] || !scale[2])
    vcl_cerr << "Scale: scaling factors must all be non-zero\n";
  glScalef (scale[0], scale[1], scale[2]);
  return true;
}

bool vgui_vrml_draw_visitor::Visit(QvRotation* node)
{
  GlRotate(node->rotation);
  return true;
}

bool vgui_vrml_draw_visitor::Visit(QvTranslation* node)
{
  float* t = node->translation.value;
  glTranslatef(t[0], t[1], t[2]);
  return true;
}

// APPEARANCE

bool vgui_vrml_draw_visitor::Visit(QvMaterial* node)
{
  //// Fields
  //QvMFColor           ambientColor;   // Ambient color
  //QvMFColor           diffuseColor;   // Diffuse color
  //QvMFColor           specularColor;  // Specular color
  //QvMFColor           emissiveColor;  // Emissive color
  //QvMFFloat           shininess;      // Shininess
  //QvMFFloat           transparency;   // Transparency

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, node->ambientColor.values);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, node->diffuseColor.values);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, node->specularColor.values);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, node->emissiveColor.values);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, node->shininess.values[0]);
  float* c = node->emissiveColor.values;
#ifdef DEBUG
  vcl_cerr << "rgba " << c[0] <<' '<< c[1] <<' '<< c[2] <<' '<< node->transparency.values[0] << '\n';
#endif
  glColor4f(c[0], c[1], c[2], node->transparency.values[0]);
  return true;
}

bool vgui_vrml_draw_visitor::Visit(QvTexture2* node)
{
  //    enum Wrap {                     // Texture wrap type
  //      REPEAT,                       // Repeats texture outside 0-1
  //                                    //  texture coordinate range
  //      CLAMP                         // Clamps texture coordinates
  //                                    //  to lie within 0-1 range
  //    };
  //
  //    // Fields.
  //    QvSFString              filename;       // file to read texture from
  //    QvSFImage               image;          // The texture
  //    QvSFEnum                wrapS;
  //    QvSFEnum                wrapT;
  //
  //    virtual QvBool  readInstance(QvInput *in);
  //    QvBool          readImage();
  //
  //    void setHandle (int handle, int alpha);  // mpichler, 19960506
  //
  if (gl_mode == textured) {
    char const* filename = node->filename.value.getString();

    vgui_vrml_texture_map* tm = gettexture(filename);
    if (tm == current_texmap && !remake_texture) {
      return true;
    }
    current_texmap = tm;
    if (tm) {
      glEnable(GL_TEXTURE_2D);
      glTexImage2D(GL_TEXTURE_2D, 0, 3,
                   tm->rgb.width(), tm->rgb.height(),
                   0,
                   GL_RGB, GL_UNSIGNED_BYTE, tm->rgb.get_buffer());
      remake_texture = false;
    } else {
      glDisable(GL_TEXTURE_2D);
    }
  }
  return true;
}
