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
//
// Note: this file is a modified version of QvTraverse as part of the
// VRML Qv parser.

//<file>
//
// Name:        wrltraverse.cxx
//
// Purpose:     sample traversal of a VRMLScene, prints scene information
//
// Created:     24 Apr 95   taken from QvTraverse
//
// Changed:      9 Aug 95   Michael Pichler
//
//
//</file>


#if defined(PMAX) || defined (HPUX9)
enum Part { goofyPart };        // cfront confused about QvCone::Part and QvCylinder::Part
enum Binding { goofyBinding };  // cfront confused about QvMaterialBinding/QvNormalBinding::Binding
#endif

#if defined(__KCC)
# define u_long go_away_bloody_u_long
# include <sys/types.h>
# undef  u_long
#endif

#if defined(__SUNPRO_CC)
# include <sys/types.h>
#endif

#include "QvElement.h"
#include "QvNodes.h"
#include "QvExtensions.h"
#include "QvUnknownNode.h"  /* mpichler */
#include "QvState.h"
#include <vcl_iostream.h>

//////////////////////////////////////////////////////////////////////////////
//
// Traversal code for all nodes. The default method (in QvNode) does
// nothing. Because traverse() is defined in the header for ALL node
// classes, each one has an implementation here.
//
//////////////////////////////////////////////////////////////////////////////

// For debugging
static int indent = 0;
static void
announce(const char *className)
{
    for (int i = 0; i < indent; i++)
        vcl_cout << '\t';
    vcl_cout << "Traversing a " << className << vcl_endl;
}
#define ANNOUNCE(className) announce(QV__QUOTE(className))

#define DEFAULT_TRAVERSE(className) \
void                                \
className::traverse(QvState *)      \
{                                   \
    ANNOUNCE(className);            \
}


// Extensions
DEFAULT_TRAVERSE(QvLabel)
DEFAULT_TRAVERSE(QvLightModel)


//////////////////////////////////////////////////////////////////////////////
//
// Groups.
//
//////////////////////////////////////////////////////////////////////////////

void
QvGroup::traverse(QvState *state)
{
    ANNOUNCE(QvGroup);
    indent++;
    for (int i = 0; i < getNumChildren(); i++)
        getChild(i)->traverse(state);
    indent--;
}

void
QvLOD::traverse(QvState *state)
{
    ANNOUNCE(QvLOD);
    indent++;

    // ??? In a real implementation, this would choose a child based
    // ??? on the distance to the eye point.
    if (getNumChildren() > 0)
        getChild(0)->traverse(state);

    indent--;
}

void
QvSeparator::traverse(QvState *state)
{
    ANNOUNCE(QvSeparator);
    state->push();
    indent++;
    for (int i = 0; i < getNumChildren(); i++)
        getChild(i)->traverse(state);
    indent--;
    state->pop();
}

void
QvSwitch::traverse(QvState *state)
{
    ANNOUNCE(QvSwitch);
    indent++;

    int which = (int) whichChild.value;

    if (which == QV_SWITCH_NONE)
        ;

    else if (which == QV_SWITCH_ALL)
        for (int i = 0; i < getNumChildren(); i++)
            getChild(i)->traverse(state);

    else
        if (which < getNumChildren())
            getChild(which)->traverse(state);

    indent--;
}

void
QvTransformSeparator::traverse(QvState *state)
{
    ANNOUNCE(QvTransformSeparator);

    // We need to "push" just the transformation stack. We'll
    // accomplish this by just pushing a no-op transformation onto
    // that stack. When we "pop", we'll restore that stack to its
    // previous state.

    QvElement *markerElt = new QvElement;
    markerElt->data = this;
    markerElt->type = QvElement::NoOpTransform;
    state->addElement(QvState::TransformationIndex, markerElt);

    indent++;
    for (int i = 0; i < getNumChildren(); i++)
        getChild(i)->traverse(state);
    indent--;

    // Now do the "pop"
    while (state->getTopElement(QvState::TransformationIndex) != markerElt)
        state->popElement(QvState::TransformationIndex);
}

//////////////////////////////////////////////////////////////////////////////
//
// Properties.
//
//////////////////////////////////////////////////////////////////////////////

#define DO_PROPERTY(className, stackIndex)                                    \
void                                                                          \
className::traverse(QvState *state)                                           \
{                                                                             \
    ANNOUNCE(className);                                                      \
    QvElement *elt = new QvElement;                                           \
    elt->data = this;                                                         \
    state->addElement(QvState::stackIndex, elt);                              \
}

#define DO_TYPED_PROPERTY(className, stackIndex, eltType)                     \
void                                                                          \
className::traverse(QvState *state)                                           \
{                                                                             \
    ANNOUNCE(className);                                                      \
    QvElement *elt = new QvElement;                                           \
    elt->data = this;                                                         \
    elt->type = QvElement::eltType;                                           \
    state->addElement(QvState::stackIndex, elt);                              \
}

DO_PROPERTY(QvCoordinate3,              Coordinate3Index)
DO_PROPERTY(QvFontStyle,                FontStyleIndex)
DO_PROPERTY(QvMaterial,                 MaterialIndex)
DO_PROPERTY(QvMaterialBinding,          MaterialBindingIndex)
DO_PROPERTY(QvNormal,                   NormalIndex)
DO_PROPERTY(QvNormalBinding,            NormalBindingIndex)
DO_PROPERTY(QvShapeHints,               ShapeHintsIndex)
DO_PROPERTY(QvTextureCoordinate2,       TextureCoordinate2Index)
DO_PROPERTY(QvTexture2,                 Texture2Index)
DO_PROPERTY(QvTexture2Transform,        Texture2TransformationIndex)

DO_TYPED_PROPERTY(QvDirectionalLight,   LightIndex, DirectionalLight)
DO_TYPED_PROPERTY(QvPointLight,         LightIndex, PointLight)
DO_TYPED_PROPERTY(QvSpotLight,          LightIndex, SpotLight)

DO_TYPED_PROPERTY(QvOrthographicCamera, CameraIndex, OrthographicCamera)
DO_TYPED_PROPERTY(QvPerspectiveCamera,  CameraIndex, PerspectiveCamera)

DO_TYPED_PROPERTY(QvTransform,       TransformationIndex, Transform)
DO_TYPED_PROPERTY(QvRotation,        TransformationIndex, Rotation)
DO_TYPED_PROPERTY(QvMatrixTransform, TransformationIndex, MatrixTransform)
DO_TYPED_PROPERTY(QvTranslation,     TransformationIndex, Translation)
DO_TYPED_PROPERTY(QvScale,           TransformationIndex, Scale)

//////////////////////////////////////////////////////////////////////////////
//
// Shapes.
//
//////////////////////////////////////////////////////////////////////////////

static void
printProperties(QvState *state)
{
    vcl_cout << "--------------------------------------------------------------\n";
    state->print();
    vcl_cout << "--------------------------------------------------------------\n";
}

#define DO_SHAPE(className)         \
void                                \
className::traverse(QvState *state) \
{                                   \
    ANNOUNCE(className);            \
    printProperties(state);         \
}

DO_SHAPE(QvAsciiText)
DO_SHAPE(QvCone)
DO_SHAPE(QvCube)
DO_SHAPE(QvCylinder)
DO_SHAPE(QvIndexedFaceSet)
DO_SHAPE(QvIndexedLineSet)
DO_SHAPE(QvPointSet)
DO_SHAPE(QvSphere)

//////////////////////////////////////////////////////////////////////////////
//
// WWW-specific nodes.
//
//////////////////////////////////////////////////////////////////////////////

// ???
DEFAULT_TRAVERSE(QvWWWAnchor)
DEFAULT_TRAVERSE(QvWWWInline)

//////////////////////////////////////////////////////////////////////////////
//
// Default traversal methods. These nodes have no effects during traversal.
//
//////////////////////////////////////////////////////////////////////////////

DEFAULT_TRAVERSE(QvInfo)
DEFAULT_TRAVERSE(QvUnknownNode)

//////////////////////////////////////////////////////////////////////////////

#undef ANNOUNCE
#undef DEFAULT_TRAVERSE
#undef DO_PROPERTY
#undef DO_SHAPE
#undef DO_TYPED_PROPERTY
