//<copyright>
//
// Copyright (c) 1995,96
// Institute for Information Processing and Computer Supported New Media (IICM),
// Graz University of Technology, Austria.
//
// This file is part of VRweb.
//
// VRweb is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// VRweb is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VRweb; see the file LICENCE. If not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//
//</copyright>

//<file>
//
// Name:        wrlbuild.cxx
//
// Purpose:     preprocessing of VRML nodes
//
// Created:      2 May 95   Michael Pichler
//
// Changed:      9 Sep 96   Michael Pichler
//
//
//</file>


#if defined(PMAX) || defined (HPUX9)
enum Part { goofyPart };        // cfront confused about QvCone::Part and QvCylinder::Part
enum Binding { goofyBinding };  // cfront confused about QvMaterialBinding/QvNormalBinding::Binding
#endif

#include <vcl_cstdio.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>

#include "vecutil.h"

#include "QvElement.h"
#include "QvNodes.h"
#include "QvExtensions.h"
#include "QvUnknownNode.h"
#include "QvDebugError.h"
#include "QvState.h"
#include "Qv_pi.h" // for QV_ONE_OVER_PI

void ge3d_pop_matrix () {}
void ge3dPushIdentity() {}

static QvSwitch* camswitch;
static int camswindex;

#define DEBUGNL(x) (vcl_cerr << x << vcl_endl)


// convert radians to degrees
#define DEGREES(R) ( (R) * 180 * (float)QV_ONE_OVER_PI )

// special names (info nodes)
#define BACKGROUND_INFO "BackgroundColor"
#define VIEWER_INFO "Viewer"

// predefined viewpoints
#define CAMERAS_SWITCH "Cameras"

// important: to set up correct bounding boxes, all transfomation
// stuff must be done in build too (check for ge3d functions that do
// matrix transformations)

// a word about multiple instancing: the computed world bounding box
// will be overwritten by subsequent instances and is therefore *not*
// valid in draw; generally: all information stored in build stays
// only valid if it is local to the node and when it does not depend
// from the traversal state

// TODO: check for multiple instances if state dependend variables
// need to be stored - strictly taken this already happens with
// IndexedFaceSet that has to use the current point array etc.

// world bounding wmin_/wmax_ are only valid during build
// (may be overwritten on multiple instances);
// due to this fact world bounding boxes have been changed
// to be local to separators (mpichler, 19951006);
// object bounding box omin_/omax_ remains valid throughout (if
// defined at all)

void rootbuild (QvNode* root)
{
  // no bounding box changes etc. will occur here
  QvState state;
  ge3dPushIdentity ();  // top node need not be a separator
  root->build (&state);  // node->build () not sufficient (global dependencies possible)
  ge3d_pop_matrix ();
}

/***** groups *****/


void QvGroup::build (QvState* state)
{
  int n = getNumChildren ();
  QvNode* child;
  point3D& wmin = wmin_;
  point3D& wmax = wmax_;

  emptyBoundingbox (wmin, wmax);

  // if any child has an extent, then a group node has the extent of
  // the unions of all bounding boxes (all in world coordinates)

  for (int i = 0; i < n; i++)
  {
    child = getChild (i);
    child->build (state);
    if (child->hasextent_)
    {
      hasextent_ = 1;
      extendBoundingbox (child->wmin_, child->wmax_, wmin, wmax);
    }
  } // for all children

  if (hasextent_)   // (for Separator)
  { omin_ = wmin_;  // no object bounding box defined for Groups
    omax_ = wmax_;  // (dependent on current transformation)
  }
} // QvGroup


void QvLOD::build (QvState* state)
{
  center_ = (const point3D*) &center.value;
  QvGroup::build (state);  // preprocessing of all children
}


void QvSeparator::build (QvState* state)
{
  state->push ();  // push attribute stacks
  ge3dPushIdentity ();

  // set object boundig to union of children's world boundings
  QvGroup::build (state);

  //vcl_cerr << "QvSeparator::build: pop ()\n";
  ge3d_pop_matrix ();
  state->pop ();  // pop stacks (cleans up)

  if (hasextent_)
    computeBoundingbox (omin_, omax_, wmin_, wmax_);  // applies current transformation
}


void QvSwitch::build (QvState* state)
{
  const char* name = objName->getString ();  // objName non nil

  if (!name || vcl_strcmp (name, CAMERAS_SWITCH))  // ordinary Switch
  {
    int which = whichChild.value;

    // preprocessing all children all times would lead to wrong bounding boxes
    // changing the active child must therefore be followed by a rebuild
    if (which == QV_SWITCH_NONE)
      return;
    if (which == QV_SWITCH_ALL)
      QvGroup::build (state);
    else if (which < getNumChildren ())
    {
      QvNode* child = getChild (which);
      child->build (state);  // build the active child

      if (child->hasextent_)
      {
        hasextent_ = 1;
        wmin_ = omin_ = child->wmin_;
        wmax_ = omax_ = child->wmax_;
      }
    }
    return;
  }

  camswitch = this;  // camera switch
  // like QvGroup::build, but cameras need to know their index within this switch

  int n = getNumChildren ();
  QvNode* child;
  point3D& wmin = wmin_;
  point3D& wmax = wmax_;

  emptyBoundingbox (wmin, wmax);
  // only active camera is allowed to put its matrix onto the stack
  // there should be no other children than cameras to work properly

  for (int i = 0; i < n; i++)
  {
    camswindex = i;
    child = getChild (i);
    child->build (state);
    if (child->hasextent_)
    {
      hasextent_ = 1;
      extendBoundingbox (child->wmin_, child->wmax_, wmin, wmax);
    }
  } // for all children

  if (hasextent_)
  { omin_ = wmin_;
    omax_ = wmax_;
  }

  camswitch = 0;
  camswindex = 0;

} // QvSwitch


void QvTransformSeparator::build (QvState* state)
{
  // like Separator, except that state is not pushed
  // might compute cumulative transformation of each Separator

  ge3dPushIdentity ();

  QvGroup::build (state);

  ge3d_pop_matrix ();

  if (hasextent_)
    computeBoundingbox (omin_, omax_, wmin_, wmax_);  // applies current transformation
}


/***** coordinates *****/


void QvCoordinate3::build (QvState* state)
{
  QvElement* el = new QvElement;  // stack elements are deleted in stack pop of Separator
  el->data = this;
  state->addElement (QvState::Coordinate3Index, el);

  // calculate min/max values only once
  if (minmaxbuilt_)
    return;
  minmaxbuilt_ = 1;

  int n = point.num;
  if (!n)  // pathologic: empty coordinate list
  { init3D (omin_, 0, 0, 0);
    omax_ = omin_;
    return;
  }

  point3D& min = omin_;
  point3D& max = omax_;
  emptyBoundingbox (min, max);

  const point3D* vert = (const point3D*) point.values;
  while (n--)
  {
    // vcl_cerr << "point " << *vert << vcl_endl;
    extendBoundingbox (*vert, *vert, min, max);
    vert++;
  }

  // vcl_cerr << "coordinate3 boundings: " << omin_ << ", " << omax_ << vcl_endl;

  // a Coordinate3 node has *no* extent itself, only shapes that use
  // it (which must transform the boundings into world coordinates -
  // transformations may be applied after specifying the coordinates)
} // QvCoordinate3


void QvNormal::build (QvState* state)
{
  QvElement* el = new QvElement;
  el->data = this;
  state->addElement (QvState::NormalIndex, el);
}


void QvTextureCoordinate2::build (QvState* state)
{
  QvElement* el = new QvElement;
  el->data = this;
  state->addElement (QvState::TextureCoordinate2Index, el);
}


/***** properties *****/


#define BUILD(className)  \
void className::build (QvState*)  { }


// mapping of QvMaterialBinding to ge3d_matbinding_t
// be sure to match order of QvMaterialBinding::Binding

void QvMaterial::build (QvState* state)
{
  static colorRGB zeroambient = { 0.0, 0.0, 0.0 };

  // material definition arrays
  materialsGE3D& mats = *materials_;  // hilit_none

  mats.num_ambient = ambientColor.num;
  mats.num_diffuse = diffuseColor.num;
  mats.num_specular = specularColor.num;
  mats.num_emissive = emissiveColor.num;
  mats.num_shininess = shininess.num;
  mats.num_transparency = transparency.num;

  if (mats.num_ambient  > 1 || mats.num_specular  > 1 ||
      mats.num_emissive > 1 || mats.num_shininess > 1)
  {
    // multiple diffuse values can be handled efficiently in OpenGL and
    // are the only ones allowed in VRML 2.0
    // ambient would be possible too if identical to diffuse (not checked)
    // transparency not yet implemented (TODO)
    mats.multival = 1;
    vcl_cerr << ("multivalued material (other than diffuse) will hit performance");
  }

  mats.rgb_ambient = (colorRGB*) ambientColor.values;
  mats.rgb_diffuse = (colorRGB*) diffuseColor.values;
  mats.rgb_specular = (colorRGB*) specularColor.values;
  mats.rgb_emissive = (colorRGB*) emissiveColor.values;
  mats.val_shininess = shininess.values;
  mats.val_transparency = transparency.values;

  // first ("base") material definitions (used for overall binding)
  // might leave out default settings (but then had to care for push/pop)

  // when Alpha is handled via stippling, ge3dMaterial may take the
  // same array as ge3d_apply_material

//float alpha = 1.0;
//if (transparency.num)
//  alpha -= *transparency.values;
//vcl_cerr << "material transparency: " << 1.0 - alpha << " (alpha: " << alpha << ")\n";

  // compute modified materials (for highlighting)
  // copy and modify diffuse and emissive color (complete arrays)

  materialsGE3D* bright = materials_ + hilit_bright;
  //materialsGE3D* dark = materials_ + hilit_dark;
  //materialsGE3D* colshade = materials_ + hilit_colshade;
  //materialsGE3D* greyshade = materials_ + hilit_greyshade;


  // use all other components than diffuse and emissive unchanged in highlighting
  // these are: ambient, specular, shininess, transparency
  // (be sure to be consistent with ~QvMaterial)

  int i;
  materialsGE3D* mat;
  for (i = 1, mat = materials_ + 1;  i < hilit_num;  i++, mat++)
  {
    mat->num_ambient = mats.num_ambient;
    mat->num_specular = mats.num_specular;
    mat->num_shininess = mats.num_shininess;
    mat->num_transparency = mats.num_transparency;
    mat->rgb_ambient = mats.rgb_ambient;
    mat->rgb_specular = mats.rgb_specular;
    mat->val_shininess = mats.val_shininess;
    mat->val_transparency = mats.val_transparency;
  }

  bright->num_ambient = 1;  // otherwise too bright
  bright->rgb_ambient = &zeroambient;

  // "base" material: emissive if nonblack, diffuse otherwise (also on highlighting)

  const float* rgbval = 0;
  if (emissiveColor.num)
    rgbval = emissiveColor.values;
  if (emissiveColor.num && (rgbval [0] || rgbval [1] || rgbval [2]))
  {
    for (i = 0, mat = materials_;  i < hilit_num;  i++, mat++)
    {
      mat->num_base = mat->num_emissive;
      mat->rgb_base = mat->rgb_emissive;
    }
  }
  else
  {
    for (i = 0, mat = materials_;  i < hilit_num;  i++, mat++)
    {
      mat->num_base = mat->num_diffuse;
      mat->rgb_base = mat->rgb_diffuse;
    }
  }

  // put current material on stack
  QvElement* el = new QvElement;
  el->data = this;
  state->addElement (QvState::MaterialIndex, el);
} // QvMaterial


void QvFontStyle::build (QvState* state)
{
  QvElement* el = new QvElement;
  el->data = this;
  state->addElement (QvState::FontStyleIndex, el);
}


void QvMaterialBinding::build (QvState* state)
{
  QvElement* el = new QvElement;
  el->data = this;
  state->addElement (QvState::MaterialBindingIndex, el);
}


void QvNormalBinding::build (QvState* state)
{
  QvElement* el = new QvElement;
  el->data = this;
  state->addElement (QvState::NormalBindingIndex, el);
}


void QvShapeHints::build (QvState* state)
{
  backfaceculling_ = (vertexOrdering.value == COUNTERCLOCKWISE)
                  && (shapeType.value == SOLID);
//vcl_cerr << "backfaceculling: " << backfaceculling_ << vcl_endl;
  // TODO: must be undone at end of separator (pushed on stack ShapeHintsIndex)

  QvElement* el = new QvElement;
  el->data = this;
  state->addElement (QvState::ShapeHintsIndex, el);
}


void QvTexture2::build (QvState*)
{}
 // QvTexture2


void QvTexture2Transform::build (QvState*)
{
  // does not transform geometry, therefore may be considered a "property"
#if 0 // commented out
  const point2D* trans = (const point2D*) translation.value;
   conceptually trans is a vector2D, but ge3d only defines point2D
  vector3D scale;
  init3D (scale, scaleFactor.value[0], scaleFactor.value[1], 1.f);
  const point2D* centr = (const point2D*) center.value;

   read transformations bottom to top (see also QvTransform::build)
  ge3dPushIdentity ();
  ge3d_translate (trans->x + centr->x, trans->y + centr->y, 0.0);
  ge3d_rotate_axis ('z', DEGREES (rotation.value));
  ge3dScale ((const float*) &scale);
  ge3d_translate (- centr->x, - centr->y, 0.0);
  ge3d_get_and_pop_matrix (mat_);
   no need to set up transformation here or to calculate its inverse
#endif
} // QvTexture2Transform


/***** lights *****/


void QvDirectionalLight::build (QvState*)
{
  float intens = intensity.value;
  float* col = color.value;
  float* dir = direction.value;

  initRGB (color_, intens * col [0], intens * col [1], intens * col [2]);
  // VRML: vector parallel to light direction
  // GL/ge3d: vector towards light source
  init3D (direction_, - dir [0], - dir [1], - dir [2]);
}


void QvPointLight::build (QvState*)
{
  float intens = intensity.value;
  float* col = color.value;

  initRGB (color_, intens * col [0], intens * col [1], intens * col [2]);

  position_ = (const point3D*) location.value;
}


void QvSpotLight::build (QvState*)
{
  float intens = intensity.value;
  float* col = color.value;

  initRGB (color_, intens * col [0], intens * col [1], intens * col [2]);

  position_ = (const point3D*) location.value;
  direction_ = (const vector3D*) direction.value;  // along light emission
  cutangle_ = DEGREES (cutOffAngle.value);  // measured from center to edge
  // dropoffrate: light rays diverging from main emission line have lower intensity;
  // 0.0: constant intensity; 1.0: sharp dropoff
}


/***** cameras *****/


void QvOrthographicCamera::build (QvState*)
{
  // give warning on invalid fields; reset to sentinel
  if (nearDistance.value >= 0 || farDistance.value >= 0)
  {
    QvDebugError::post ("OrthographicCamera", "illegal fields nearDistance/farDistance");
    nearDistance.value = -1;
    farDistance.value = -1;
  }

  pos_ = (const point3D*) position.value;
  rotangle_ = orientation.angle;  // radians
  rotaxis_ = (const vector3D*) orientation.axis;
  height_ = height.value;
// vcl_cerr << "orthographic camera: position " << *pos_ << ", rotangle " << rotangle_
//          << ", rotaxis " << *rotaxis_ << ", height " << height_ << vcl_endl;

  camswitch_ = camswitch;
  camswindex_ = camswindex;
#if 0 // commented out
  if (!registered_)
    {
      //vrmlscene_->hasCamera (this, objName->getString ());  // objName non nil
      registered_ = 1;
    }

  ge3dPushIdentity ();
  ge3dRotate (rotaxis_, - rotangle_);
  ge3d_translate (-pos_->x, -pos_->y, -pos_->z);
  ge3d_get_and_pop_matrix (mat_);

  if (vrmlscene_->activeOCam () == this)
    ge3dMultMatrix ((const float (*)[4]) mat_);  // set up transformation (as in draw)

  copymatrix (mat_, invmat_);
  invertmatrix (invmat_);
#endif
} // QvOrthographicCamera


void QvPerspectiveCamera::build (QvState*)
{
  // give warning on invalid fields; reset to sentinel
  if (nearDistance.value >= 0 || farDistance.value >= 0)
  {
    QvDebugError::post ("PerspectiveCamera", "illegal fields nearDistance/farDistance");
    nearDistance.value = -1;
    farDistance.value = -1;
  }

  pos_ = (const point3D*) position.value;
  rotangle_ = orientation.angle;  // radians
  rotaxis_ = (const vector3D*) orientation.axis;
  yangle_ = heightAngle.value;  // radians
// vcl_cerr << "perspective camera: position " << *pos_ << ", rotangle " << rotangle_
// << ", rotaxis " << *rotaxis_ << ", heightangle " << yangle_ << vcl_endl;

  camswitch_ = camswitch;
  camswindex_ = camswindex;
#if 0 // commented out
  if (!registered_)
  { vrmlscene_->hasCamera (this, objName->getString ());  // objName non nil
    registered_ = 1;
  }

  ge3dPushIdentity ();
  ge3dRotate (rotaxis_, - rotangle_);
  ge3d_translate (-pos_->x, -pos_->y, -pos_->z);
  ge3d_get_and_pop_matrix (mat_);

  if (vrmlscene_->activePCam () == this)
    ge3dMultMatrix ((const float (*)[4]) mat_);  // set up transformation (as in draw)

  copymatrix (mat_, invmat_);
  invertmatrix (invmat_);
#endif
} // QvPerspectiveCamera


/***** transformations *****/


// assert: point3D/vector3D compatible to float [3]
// (storing accumulated transformations would allow less push/pop calls
// but imply setting the transformation matrix on each primitive; had
// to flag wheter it changed; does not seem worthwile at the moment)


void QvTransform::build (QvState*)
{
  //  vcl_cerr << "QvTransform ignored";
#if 0 // commented out
  const vector3D* tran1 = (const vector3D*) translation.value;
  const vector3D* tran2 = (const vector3D*) center.value;
  const vector3D* rot1 = (const vector3D*) rotation.axis;
  const vector3D* rot2 = (const vector3D*) scaleOrientation.axis;
  const float angle1 = rotation.angle;  // rad
  const float angle2 = scaleOrientation.angle;
  const float* scale = scaleFactor.value;

  ge3dPushIdentity ();
  ge3d_translate (tran1->x + tran2->x, tran1->y + tran2->y, tran1->z + tran2->z);
  ge3dRotate (rot1, angle1);
  ge3dRotate (rot2, angle2);  // just rotates the scaling vector
  ge3dScale (scale);
  ge3dRotate (rot2, - angle2);
  ge3d_translate (-tran2->x, -tran2->y, -tran2->z);  // - tran2
  ge3d_get_and_pop_matrix (mat_);

  ge3dMultMatrix ((const float (*)[4]) mat_);  // set up transformation (as in draw)
#endif

  copymatrix (mat_, invmat_);
  invertmatrix (invmat_);

// vcl_cerr << "inverse transformation matrix:\n";
// for (int i = 0;  i < 4;  i++)
//   printf ("%13f %12f %12f %12f\n", invmat_ [i][0], invmat_ [i][1], invmat_ [i][2], invmat_ [i][3]);

  copymatrix (invmat_, invtranspmat3D_);
  transposematrix (invtranspmat3D_);
  mat4Dto3D (invtranspmat3D_);  // cancel out last row and column
} // QvTransform


void QvRotation::build (QvState*)
{
#if 0 // commented out
  const vector3D* axis = (const vector3D*) rotation.axis;
  float angle = rotation.angle;  // rad

  ge3dPushIdentity ();
  ge3dRotate (axis, angle);  // set up transformation (as in draw)
  ge3d_get_and_pop_matrix (mat_);

  ge3dMultMatrix ((const float (*)[4]) mat_);  // set up transformation (as in draw)
#else
  vcl_cerr << "QvRotation not implemented\n";
#endif

  // rotations produce orthogonal matrices, so the inverse is simply
  // the transposed matrix; also they are always 3x3 essentially
  copymatrix (mat_, invmat_);
  transposematrix (invmat_);
}


void QvMatrixTransform::build (QvState*)
{
  mat_ = (const float (*)[4]) &matrix.value;
  // awf  ge3dMultMatrix (mat_);  // set up transformation (as in draw)

// fprintf (stderr, "matrix elements:\n"
//   "(%10g %10g %10g %10g)\n(%10g %10g %10g %10g)\n"
//   "(%10g %10g %10g %10g)\n(%10g %10g %10g %10g)\n",
//   mat_[0], mat_[1], mat_[2], mat_[3], mat_[4], mat_[5], mat_[6], mat_[7],
//   mat_[8], mat_[9], mat_[10], mat_[11], mat_[12], mat_[13], mat_[14], mat_[15]);

  copymatrix ((ARRAYconst float (*)[4]) mat_, invmat_);  // cast for cxx

  invertmatrix (invmat_);

  copymatrix (invmat_, invtranspmat3D_);
  transposematrix (invtranspmat3D_);
  mat4Dto3D (invtranspmat3D_);  // cancel out last row and column
}


void QvTranslation::build (QvState*)
{
#if 0 // commented out
  vcl_cerr << "QvTranslation ignored\n";
  trans_ = (const vector3D*) translation.value;
  ge3dTranslate (trans_);  // set up transformation (as in draw)
#endif
}


void QvScale::build (QvState*)
{
  scale_ = scaleFactor.value;
  if (!scale_[0] || !scale_[1] || !scale_[2])
    QvDebugError::post ("Scale", "scaling factors must all be non-zero");

  invscale_[0] = (scale_[0] ? 1/scale_[0] : 1);
  invscale_[1] = (scale_[1] ? 1/scale_[1] : 1);
  invscale_[2] = (scale_[2] ? 1/scale_[2] : 1);

  // vcl_cerr << "QvScale ignored\n";

  //ge3dScale (scale_);  // set up transformation (as in draw)
}


/***** shapes *****/


void QvAsciiText::build (QvState*)
{
  // might turn '\n' into multiple lines (or give at least a warning)
#if 0 // commented out
  int family = VRMLScene::fnt_serif;  // default: serif font

  QvElement* attr = state->getTopElement (QvState::FontStyleIndex);
  if (attr)
  {
    QvFontStyle* fontstyle = (QvFontStyle*) attr->data;
    size_ = fontstyle->size.value / 10.0;
    if (size_ == 1.0)
      size_ = 0;
    // if (size_)  vcl_cerr << "scaling font by " << size_ << vcl_endl;
    bold_ = fontstyle->style.value & QvFontStyle::BOLD;
    italic_ = fontstyle->style.value & QvFontStyle::ITALIC;
    switch (fontstyle->family.value)
    {
      case QvFontStyle::SERIF:
        family = VRMLScene::fnt_serif;
      break;
      case QvFontStyle::SANS:
        family = VRMLScene::fnt_sans;
      break;
      case QvFontStyle::TYPEWRITER:
        family = VRMLScene::fnt_fixed;
      break;
    }
  }
  else
  {
    size_ = 0;  // no scale, same as 1.0
    bold_ = italic_ = 0;
    // family = VRMLScene::fnt_serif;
  }

  // require font chars (glyphs), calls buildFont
  if (!requested_)
  {
    requested_ = 1;
    int bld = bold_;  // linewidth always increased (may be in wireframe mode)
    fontchars_ = vrmlscene_->getFontChars (family, bld, italic_);
    if (!fontchars_)
    {
      QvDebugError::post ("AsciiText", "unable to load font file");
      return;
    }
  }

  if (!fontchars_)
    return;

  int numrows = string.num;
  float vspc = spacing.value;

  vrmlscene_->increaseNumPrimitives ();  // might count lines or characters instead


  if (roworigin_)  // row info already built
    return;


  float halign = 0;  // left alignment
  if (justification.value == QvAsciiText::CENTER)
    halign = 0.5;
  else if (justification.value == QvAsciiText::RIGHT)
    halign = 1.0;

  // build info needed per row
  point3D* roworig = roworigin_ = new point3D [numrows];
  point3D* rowmax = rowmax_ = new point3D [numrows];
  float* rowhspc = rowhspc_ = new float [numrows];
  QvString* rowstr = string.values;
  const unsigned char* str;

  // bounding box: union of all row bounding boxes
  emptyBoundingbox (omin_, omax_);
  if (size_)  // row scaling in object coordinates
  { ge3d_push_matrix ();
    ge3d_scale (1.0, 1.0, 1.0, size_);
  }

  for (int i = 0;  i < numrows;  i++, roworig++, rowmax++)
  {
    float rowwidth = 0;
    int numgaps = 0;
    str = (const unsigned char*) rowstr++->getString ();
    // find width of this row
    while (*str)
    { rowwidth += fontchars_ [*str++].width ();
      numgaps++;
    }
    if (numgaps)
      numgaps--;

    rowwidth += numgaps * 0.5;
    // compare rowwidth to width to adjust hspacing
    // add total hspacing to rowwidth
    *rowhspc++ = 0.5;
    // default hspc: 0.5

    init3D (*roworig, - halign * rowwidth, - i * 10 * vspc, 0);  // lower left
    init3D (*rowmax, (1 - halign) * rowwidth, roworig->y + 7, 0);  // upper right
    // 7 means using 70 % of baseline distance (10) as bounding box
    extendBoundingbox (*roworig, *rowmax, omin_, omax_);
  }
  // font scale (multiples of 10.0) handled with scaling transformation

  computeBoundingbox (omin_, omax_, wmin_, wmax_);  // applies current transformation
  hasextent_ = 1;

  if (size_)
    ge3d_pop_matrix ();
#endif
} // QvAsciiText


void QvCube::build (QvState*)
{
  // object coordinates
  omax_.x = (float)vcl_fabs (width.value / 2.0);
  omax_.y = (float)vcl_fabs (height.value / 2.0);
  omax_.z = (float)vcl_fabs (depth.value / 2.0);
  omin_.x = -omax_.x;
  omin_.y = -omax_.y;
  omin_.z = -omax_.z;

  computeBoundingbox (omin_, omax_, wmin_, wmax_);  // applies current transformation
  hasextent_ = 1;

  //vrmlscene_->increaseNumPrimitives ();
}


void QvCone::build (QvState*)
{
#if 0
  int pts = parts.value;
  parts_ = 0;
  if (pts & QvCone::SIDES) parts_ |= cyl_sides;
  if (pts & QvCone::BOTTOM) parts_ |= cyl_bottom;
  // assert: ALL = SIDES | BOTTOM
  // note: QvLib defines enums SIDES/BOTTOM/[TOP] different for Cones and Cylinders

  if (!pts)  // no parts - no extent
    return;

  float r = bottomRadius.value;
  float h = height.value;

  omin_.x = omin_.z = -r;
  omax_.x = omax_.z = r;
  // possibly degenerated cone of bottom only
  if (pts == QvCone::BOTTOM)
    omin_.y = omax_.y = - h/2.0;
  else
    omin_.y = - (omax_.y = h/2.0);

  computeBoundingbox (omin_, omax_, wmin_, wmax_);  // applies current transformation
  hasextent_ = 1;

  vrmlscene_->increaseNumPrimitives ();
#else
  vcl_cerr << "QvCone no implemented\n";
#endif
} // QvCone


void QvCylinder::build (QvState*)
{
#if 0
  int pts = parts.value;
  parts_ = 0;
  if (pts & QvCylinder::SIDES)
    parts_ |= cyl_sides;
  if (pts & QvCylinder::TOP)
    parts_ |= cyl_top;
  if (pts & QvCylinder::BOTTOM)
    parts_ |= cyl_bottom;
  // assert: ALL = SIDES | TOP | BOTTOM

  if (!pts)  // no parts - no extent
    return;

  float r = radius.value;
  float h = height.value;

  omin_.x = omin_.z = -r;
  omax_.x = omax_.z = r;
  // possibly degenerated cylinder of bottom or top only
  if (pts == QvCylinder::TOP)
    omin_.y = omax_.y = h/2.0;
  else if (pts == QvCylinder::BOTTOM)
    omin_.y = omax_.y = - h/2.0;
  else
    omin_.y = - (omax_.y = h/2.0);

  computeBoundingbox (omin_, omax_, wmin_, wmax_);  // applies current transformation
  hasextent_ = 1;

  vrmlscene_->increaseNumPrimitives ();
#else
  vcl_cerr << "QvCylinder not implemented\n";
#endif
} // QvCylinder


void QvSphere::build (QvState*)
{
#if 0
  float r = radius.value;
  omin_.x = omin_.y = omin_.z = -r;
  omax_.x = omax_.y = omax_.z = r;

  computeBoundingbox (omin_, omax_, wmin_, wmax_);  // applies current transformation
  hasextent_ = 1;

  vrmlscene_->increaseNumPrimitives ();
#else
  vcl_cerr << "QvSphere not implemented\n";
#endif
} // QvSphere


void QvIndexedFaceSet::build (QvState* state)
{
//vcl_cerr << "QvIndexedFaceSet::build\n";
  QvElement* attr = state->getTopElement (QvState::Coordinate3Index);
  if (!attr)
  { vcl_cerr << "QvIndexedFaceSet. error: no vertices\n";
    numvertinds_ = nummatinds_ = numnormalinds_ = numtextureinds_ = 0;
    numvertices_ = 0;
    return;
  }

  QvCoordinate3* coords = (QvCoordinate3*) attr->data;
  omin_ = coords->omin_;
  omax_ = coords->omax_;
  computeBoundingbox (omin_, omax_, wmin_, wmax_);
  hasextent_ = 1;

  const QvMFVec3f& verts = coords->point;
  //vcl_cerr << "size of vertex coord list: " << verts.num << "; no. of coord indices: " << coordIndex.num << vcl_endl;

  // TODO: ensure that all indices are in proper range
  vertexlist_ = (const point3D*) verts.values;
  numvertinds_ = coordIndex.num;  // no. of vertex indices
  vertindices_ = (const int*) coordIndex.values;

  // Georg Meszaros
  numvertices_ = verts.num;

#if 0
  // find no. of faces
  unsigned numfaces = 0;
  unsigned nv = numvertinds_;
  const int* cind = vertindices_;
  if (nv && cind [nv-1] >= 0)  // incomplete last face
    numfaces = 1;
  while (nv--)
  { if (*cind++ < 0)
      numfaces++;
  }
  if (!build_font)  // do not count font polygons
    vrmlscene_->increaseNumFaces (numfaces);

  // should store this number to do no update when build is called
  // another time after fetching inline nodes

  // *** face normals ***
  // TODO: range check (poss. incomplete last face)
  if (!facenormals_ && numfaces)  // only once on multiple instances
  {
    facenormals_ = new vector3D [numfaces];
    vector3D* fn = facenormals_;
    nv = numvertinds_;
    cind = vertindices_;
    while (nv)
    {
      // here at the beginning of a new face
      int v0 = cind [0];
      int v1 = cind [1];
      int v2 = cind [2];
      // vcl_cerr << "computing face normal from indices " << v0 << ", " << v1 << ", " << v2 << ": ";
      if (v0 >= 0 && v1 >= 0 && v2 >= 0 && nv > 2)
      {
        if (nv == 3 || cind [3] < 0)  // triangle: simple normal vector calculation of the 3 vertices
          computeNormal (vertexlist_ [v0], vertexlist_ [v1], vertexlist_ [v2], *fn);
        else  // more than 3 vertices
        { // use Newell's method to calculate normal vector
          // first three vertices may be collinear and need not be a convex "ear"
          // see Graphics Gems, Vol. III, V.5, p. 231
          const point3D* vert1, * vert2;
          init3D (*fn, 0, 0, 0);
          vert2 = vertexlist_ + v0;  // v0 kept at vertex 0
          while (nv > 1 && cind [1] >= 0)  // edge cind[0] to cind[1]
          {
            vert1 = vert2;
            vert2 = vertexlist_ + *++cind;
            nv--;
            fn->x += (vert1->y - vert2->y) * (vert1->z + vert2->z);
            fn->y += (vert1->z - vert2->z) * (vert1->x + vert2->x);
            fn->z += (vert1->x - vert2->x) * (vert1->y + vert2->y);
          }
          vert1 = vert2;  // last edge (back to v0)
          vert2 = vertexlist_ + v0;
          fn->x += (vert1->y - vert2->y) * (vert1->z + vert2->z);
          fn->y += (vert1->z - vert2->z) * (vert1->x + vert2->x);
          fn->z += (vert1->x - vert2->x) * (vert1->y + vert2->y);
        }

        // vcl_cerr << *fn << vcl_endl;
        float norm2 = dot3D (*fn, *fn);
        if (norm2 > 0.0)  // normalize
        { norm2 = 1 / vcl_sqrt (norm2);
          scl3D (*fn, norm2);
        }
        // vcl_cerr << *fn << vcl_endl;
      }
      else  // face with less than 3 vertices
        init3D (*fn, 0, 0, 0);

      // goto next face
      fn++;
      while (*cind >= 0 && nv)
        cind++, nv--;
      if (nv)  // skip index -1 (face separator)
        cind++, nv--;
    }
  } // face normals

  float crease_angle = 0.5;  // default for autosmooth


  // *** convexify ***
  attr = state->getTopElement (QvState::ShapeHintsIndex);
  if (attr)
    crease_angle = ((QvShapeHints*) attr->data)->creaseAngle.value;
  if (attr && !numconvexinds_)
  { // convexify on UNKOWN_FACE_TYPE, only once
    int facetype = ((QvShapeHints*) attr->data)->faceType.value;
    if (facetype == QvShapeHints::UNKNOWN_FACE_TYPE)
      vrmlscene_->convexifyFaceSet (this);
  }


  // material binding
  matbinding_ = matb_default;
  attr = state->getTopElement (QvState::MaterialBindingIndex);
  if (attr)
  { int matb = ((QvMaterialBinding*) attr->data)->value.value;
    if (matb >= 0 && matb <= QvMaterialBinding::PER_VERTEX_INDEXED)  // assert: max value
      matbinding_ = ge3dMatBindings [matb];
  }

  // materials
  attr = state->getTopElement (QvState::MaterialIndex);
  if (attr)
  {
    materials_ = ((QvMaterial*) attr->data)->materials_;  // hilit_none
    // current material will be no longer stored with FaceSet to allow for anchor highlighing
    nummatinds_ = materialIndex.num;  // no. of material indices
    matindices_ = (const int*) materialIndex.values;

    // Inventor spec: when _INDEXED material binding lacks material indices use coord indices instead
    if (matbinding_ == matb_pervertexindexed && nummatinds_ == 1 && *matindices_ == -1)
    {
      nummatinds_ = numvertinds_;
      matindices_ = vertindices_;
      // note: doing so only for *vertex* indexed,
      // as face/part indexed would use -1's as material index (TODO: filter them out)
    }
  }
  else
  { materials_ = 0;
    nummatinds_ = 0;
    matindices_ = 0;
  }


  // normal indices
  // do smooth shading on binding default, per vertex, per vertex indexed

  attr = state->getTopElement (QvState::NormalIndex);
  if (!attr)
  { // no normal vectors
    normallist_ = 0;
    // numnormalinds_ = 0;  // unchanged (see constructor/below)
    normalindices_ = 0;

    attr = state->getTopElement (QvState::NormalBindingIndex);
    int normalbinding = QvNormalBinding::DEFAULT;
    if (attr)
      normalbinding = ((QvNormalBinding*) attr->data)->value.value;

    // autosmooth (vertex normal generation)
    // sets normallist_ to newly created normalList_
    // also overwrites normalIndex and sets numnormalinds_ and normalindices_
    if (normalIndex.num == 1 && *normalIndex.values == -1 && crease_angle > 0)
      switch (normalbinding)
      { // autosmooth only on normalbinding default or per vertex [indexed]
        case QvNormalBinding::DEFAULT:
        case QvNormalBinding::PER_VERTEX:
        case QvNormalBinding::PER_VERTEX_INDEXED:
        {
          if (normalList_.num < 2)  // done only once
            vrmlscene_->autosmooth (this, crease_angle);
          else
          { normallist_ = (const vector3D*) normalList_.values;
            normalindices_ = (const int*) normalIndex.values;
            // numnormalinds_ unchanged
          }
        }
      }
    else
    { normallist_ = (const vector3D*) normalList_.values;
      normalindices_ = (const int*) normalIndex.values;
      // numnormalinds_ unchanged
    }
  }
  else
  {
    const QvMFVec3f& normals = ((QvNormal*) attr->data)->vector;
    //vcl_cerr << "size of normal vector list: " << normals.num << "; no. of normal indices: " << normalIndex.num << vcl_endl;

    // normal binding not checked in this case

    normallist_ = (const vector3D*) normals.values;
    // TODO: ensure that all indices are in proper range
    numnormalinds_ = normalIndex.num;  // no. of normal indices
    normalindices_ = (const int*) normalIndex.values;

    // Inventor spec: on normal binding PER_VERTEX coordIndex can be used for normalIndex
    if (numnormalinds_ == 1 && *normalindices_ == -1)
    {
      // TODO: ensure that all indices are in proper range
      numnormalinds_ = numvertinds_;
      normalindices_ = vertindices_;
    }
  }
#endif


  // texture indices (texture binding implicit)
  attr = state->getTopElement (QvState::TextureCoordinate2Index);
  if (!attr)
  { // rarely entering here (default entry of -1)
    // texture indices useless without texture coordinates
    texvertlist_ = 0;
    numtextureinds_ = 0;
    textureindices_ = 0;
    // TODO: automatic texture coordinate generation (based on bounding box)
  }
  else
  {
    const QvMFVec2f& texverts = ((QvTextureCoordinate2*) attr->data)->point;
    //vcl_cerr << "size of tex.vert. list: " << texverts.num << "; no. of texture ind.: " << textureCoordIndex.num << vcl_endl;

    // TODO: ensure that all indices are in proper range
    texvertlist_ = (const point2D*) texverts.values;
    numtextureinds_ = textureCoordIndex.num;  // no. of texture indices
    textureindices_ = (const int*) textureCoordIndex.values;

    // Inventor: when texture vertices but no texture coord indices use coord indices instead
    if (numtextureinds_ == 1 && *textureindices_ == -1)
    {
      numtextureinds_ = numvertinds_;
      textureindices_ = vertindices_;
    }
  }

} // QvIndexedFaceSet


void QvIndexedLineSet::build (QvState* state)
{
//vcl_cerr << "QvIndexedLineSet::build\n";

  QvElement* attr = state->getTopElement (QvState::Coordinate3Index);
  if (!attr)
  { vcl_cerr << "QvIndexedLineSet. error: no vertices\n";
    numvertinds_ = 0;
    return;
  }

  epsilon_ = 0.05f;  // picking tolerance

  QvCoordinate3* coords = (QvCoordinate3*) attr->data;
  omin_ = coords->omin_;
  omax_ = coords->omax_;
  // extend bounding box by epsilon
  vector3D delta;
  init3D (delta, epsilon_, epsilon_, epsilon_);
  inc3D (omax_, delta);  // omax_ += epsilon
  dec3D (omin_, delta);  // omin_ -= epsilon
  computeBoundingbox (omin_, omax_, wmin_, wmax_);
  hasextent_ = 1;
  //vcl_cerr << "world bounding box: " << wmin_ << wmax_ << vcl_endl;

  const QvMFVec3f& verts = coords->point;

  // TODO: ensure that all indices are in proper range
  // TODO: consider current material binding

//vcl_cerr << "number of points - size of vertex list: " << verts.num
//<< "; no. of coord indices: " << coordIndex.num << vcl_endl;

  vertexlist_ = (const point3D*) verts.values;
  numvertinds_ = coordIndex.num;  // no. of vertex indices
  vertindices_ = (const int*) coordIndex.values;

  // no normal and texture indices for lines

} // QvIndexedLineSet


void QvPointSet::build (QvState* state)
{
  QvElement* attr = state->getTopElement (QvState::Coordinate3Index);
  if (!attr)
  { vcl_cerr << "QvPointSet. error: no vertices\n";
    num_ = 0;
    return;
  }

  epsilon_ = 0.05f;  // picking tolerance

  QvCoordinate3* coords = (QvCoordinate3*) attr->data;
  omin_ = coords->omin_;
  omax_ = coords->omax_;
  // extend bounding box by epsilon
  vector3D delta;
  init3D (delta, epsilon_, epsilon_, epsilon_);
  inc3D (omax_, delta);  // omax_ += epsilon
  dec3D (omin_, delta);  // omin_ -= epsilon
  computeBoundingbox (omin_, omax_, wmin_, wmax_);
  hasextent_ = 1;
  // vcl_cerr << "world bounding box: " << wmin_ << wmax_ << vcl_endl;

  const QvMFVec3f& verts = coords->point;

  points_ = (const point3D*) verts.values;
  int start = (int) startIndex.value;
  num_ = (int) numPoints.value;

  if (num_ < 0 || start + num_ > verts.num)  // use all remaining points
    num_ = verts.num - start;

  points_ += start;  // true beginning

// vcl_cerr << "QvPointSet::build: startIndex " << start
// << ", number: " << num_ << " (originally " << numPoints.value << ")\n";

} // QvPointSet


/***** WWW *****/


void QvWWWAnchor::build (QvState* state)
{
  // give warning on invalid fields; reset to sentinel
  if (!(!target.value))
  {
    QvDebugError::post ("WWWAnchor", "illegal field target");
    target.value = "";
  }

  // spec: WWWAnchor must behave like a separator (although it is not derived from it)
  state->push ();  // push attribute stacks
  ge3dPushIdentity ();

  if (!parentURL_.getLength ())
  { // such to handle relative anchors in inline scenes too
    // parentURL_ = scene_->mostRecentURL ();
    //DEBUGNL ("WWWAnchor relative to document [" << parentURL_.getString () << "].");
  }

  QvGroup::build (state);

  ge3d_pop_matrix ();
  state->pop ();  // pop stacks

  if (hasextent_)
    computeBoundingbox (omin_, omax_, wmin_, wmax_);  // applies current transformation

} // QvWWWAnchor


void QvWWWInline::build (QvState* state)
{
  // initially, WWWInline has no children - they get built when the link is fetched

  // bounding box
  const point3D* bsize = (const point3D*) bboxSize.value;
  const point3D* bcenter = (const point3D*) bboxCenter.value;

  if (state_ == s_virgin)
  { // such to handle nested relative inline scenes correctly
    //parentURL_ = scene_->mostRecentURL ();
    //DEBUGNL ("WWWInline for parent [" << parentURL_.getString () << "] encountered");
  }

  if (state_ == s_completed)  // children already read: use their bounding box
  {
    // vcl_cerr << "QvWWWInline::build: children already fetched. using their bounding box.\n";
    hasextent_ = 0;  // possibly no geometry
    QvGroup::build (state);
  }
  else if (bsize->x > 0 || bsize->y > 0 || bsize->z > 0)  // nonempty BB specified
  {
    // DEBUGNL ("WWWInline bounding box size: " << *bsize << ", center: " << *bcenter);

    // object coordinates
    pol3D (*bcenter, -0.5f, *bsize, omin_);  // omin_ = bcenter - bsize/2
    add3D (omin_, *bsize, omax_);  // omax_ = min + bsize

    computeBoundingbox (omin_, omax_, wmin_, wmax_);
    hasextent_ = 1;
  }
  else
  { DEBUGNL ("QvWWWInline::build with empty bounding box");
  }

// vcl_cerr << "QvWWWInline::build. bounding box defined: " << hasextent_ << vcl_endl;
// if (hasextent_)
// vcl_cerr << "object boundings: " << min_ << ", " << max_ << ", world boundings: " << wmin_ << ", " << wmax_ << vcl_endl;

} // QvWWWInline


/***** misc *****/

void QvInfo::build (QvState*)
{
#if 0
  const char* name = objName->getString ();  // objName non nil
  if (!name || !*name)  // unnamed info
    return;

  const char* info = string.value.getString ();
  // vcl_cerr << "Info [" << name << "]: " << info << vcl_endl;
  if (!info || !*info)  // empty info
    return;

  if (handled_)  // only respected on first traversal
    return;

  if (!vcl_strcmp (name, BACKGROUND_INFO))
  {
    colorRGB col;
    if (vcl_sscanf (info, "%f %f %f", &col.R, &col.G, &col.B) == 3)
    { // set new background color
      ge3dBackgroundColor (&col);
      ge3d_clearscreen ();
    }
    else
      QvDebugError::post ("Info", BACKGROUND_INFO " should be given as 3 floats, separated by spaces");
  }
  else if (!vcl_strcmp (name, VIEWER_INFO))
  { // just differ between flip ("examiner") and the other modes ("walk")

    if (!vcl_strcmp (info, "examiner"))
      scene_->navigationHint (Scene3D::nav_flip);
    else if (!vcl_strcmp (info, "walk"))
      scene_->navigationHint (Scene3D::nav_walk);
    else
      QvDebugError::post ("Info", VIEWER_INFO " should be defined as \"walk\" or \"examiner\"");
  } // VIEWER_INFO

  handled_ = 1;

#else
  vcl_cerr << "QvInfo not implemented\n";
#endif
} // QvInfo


void QvUnknownNode::build (QvState* state)  { QvGroup::build (state); }


/***** extensions *****/


void QvLabel::build (QvState*)
{
  if (newinstance_)
  { QvDebugError::post ("Label", "invalid VRML 1.0 node (ignored)\n"
                        "        (further occurances will not be reported)");
    newinstance_ = 0;
  }
}


void QvLightModel::build (QvState*)
{
  if (newinstance_)
  { QvDebugError::post ("LightModel", "warning: extension to VRML 1.0 standard\n"
                        "        (further occurances will not be reported)");
    newinstance_ = 0;
  }

  dolighting_ = model.value != QvLightModel::BASE_COLOR;
}
