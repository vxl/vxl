// This is v3p/Qv/QvVisitor.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "QvVisitor.h"

#include <vcl_iostream.h>
#include <vcl_string.h>

#include "QvString.h"
#include "QvInput.h"
#include "QvState.h"
#include "QvNode.h"

#include "QvGroup.h"
#include "QvSeparator.h"
#include "QvTransformSeparator.h"
#include "QvSwitch.h"

static bool verbose = false;
static const char* typenames[] = {
  "AsciiText",
  "Cone",
  "Coordinate3",
  "Cube",
  "Cylinder",
  "DirectionalLight",
  "FontStyle",
  "Group",
  "IndexedFaceSet",
  "IndexedLineSet",
  "Info",
  "LOD",
  "Material",
  "MaterialBinding",
  "MatrixTransform",
  "Normal",
  "NormalBinding",
  "OrthographicCamera",
  "PerspectiveCamera",
  "PointLight",
  "PointSet",
  "Rotation",
  "Scale",
  "Separator",
  "ShapeHints",
  "Sphere",
  "SpotLight",
  "Switch",
  "Texture2",
  "Texture2Transform",
  "TextureCoordinate2",
  "Transform",
  "TransformSeparator",
  "Translation",
  "WWWAnchor",
  "WWWInline",
  "UnknownNode",
  "Label",
  "LightModel",
};

struct QvVisitorIndenter {
  int* indent_;
  QvVisitorIndenter(int* indent):
    indent_(indent) {
    ++*indent_;
  }
  ~QvVisitorIndenter() {
    --*indent_;
  }
};

vcl_ostream& operator<<(vcl_ostream& s, const QvName& name)
{
  if (name.getLength())
    s << "[name `" << name.getString() << "']";
  return s;
}

/////////////////////////////////////////////////////////////////////////////

QvVisitor::QvVisitor()
{
  indent = 0;
}

QvVisitor::~QvVisitor()
{
}

bool QvVisitor::Visit(QvNode* node)
{
  vcl_string is(indent * 4, ' ');

  int type = node->nodeType ();

  if (verbose) {
    vcl_cerr << is << "Node type " << typenames[type] << vcl_endl;
    // Print fields
    QvFieldData * fields = node->getFieldData();
    int nfields = fields->getNumFields();
    for (int f = 0; f < nfields; ++f)
      vcl_cerr << is << "Field " << fields->getFieldName(f).getString() << vcl_endl;
  }

  QvVisitorIndenter incdecindent(&indent);

  switch (type) {
#define DO_VISIT(T) case QvNodeType::T: { return Visit((T*)node); }
    DO_VISIT(QvGroup)
      DO_VISIT(QvSeparator)
      DO_VISIT(QvSwitch)
      DO_VISIT(QvWWWAnchor)
      DO_VISIT(QvUnknownNode)
      DO_VISIT(QvTransformSeparator)

      DO_VISIT(QvAsciiText)
      DO_VISIT(QvCone)
      DO_VISIT(QvCoordinate3)
      DO_VISIT(QvCube)
      DO_VISIT(QvCylinder)
      DO_VISIT(QvDirectionalLight)
      DO_VISIT(QvFontStyle)
      DO_VISIT(QvIndexedFaceSet)
      DO_VISIT(QvIndexedLineSet)
      DO_VISIT(QvInfo)
      DO_VISIT(QvLOD)
      DO_VISIT(QvMaterial)
      DO_VISIT(QvMaterialBinding)
      DO_VISIT(QvMatrixTransform)
      DO_VISIT(QvNormal)
      DO_VISIT(QvNormalBinding)
      DO_VISIT(QvOrthographicCamera)
      DO_VISIT(QvPerspectiveCamera)
      DO_VISIT(QvPointLight)
      DO_VISIT(QvPointSet)
      DO_VISIT(QvRotation)
      DO_VISIT(QvScale)
      DO_VISIT(QvShapeHints)
      DO_VISIT(QvSphere)
      DO_VISIT(QvSpotLight)
      DO_VISIT(QvTexture2)
      DO_VISIT(QvTexture2Transform)
      DO_VISIT(QvTextureCoordinate2)
      DO_VISIT(QvTransform)
      DO_VISIT(QvTranslation)
      DO_VISIT(QvLabel)
      DO_VISIT(QvLightModel)
#undef DO_VISIT
    default: {
      vcl_cerr << "Unknown node, type " << typenames[type] << " " <<  node->getName() << vcl_endl;
      return false;
    }
  }
}

bool QvVisitor::Visit(QvGroup* node) {
  int n = node->getNumChildren();
  for (int child_index = 0; child_index < n; ++child_index)
    Visit(node->getChild(child_index));
  return true;
}

bool QvVisitor::Visit(QvSeparator* node) {
  int n = node->getNumChildren();
  for (int child_index = 0; child_index < n; ++child_index)
    Visit(node->getChild(child_index));
  return true;
}

bool QvVisitor::Visit(QvSwitch* node) {
  int n = node->getNumChildren();
  int child_index = node->whichChild.value;
  if (child_index == QV_SWITCH_ALL)
    for (int c = 0; c < n; ++c)
      Visit(node->getChild(c));
  else if (child_index != QV_SWITCH_NONE) {
    if (child_index >= 0 && child_index < n)
      Visit(node->getChild(child_index));
    else
      vcl_cerr << "QvVisitor: whichChild " << child_index << "out of range [0.."<<(n-1)<<"]\n";
  }

  return true;
}

bool QvVisitor::Visit(QvTransformSeparator* node)
{
  int n = node->getNumChildren();
  for (int child_index = 0; child_index < n; ++child_index)
    Visit(node->getChild(child_index));
  return true;
}

bool QvVisitor::Visit(QvWWWInline* node)
{
  vcl_cerr << "QvVisitor: WARNING: Can't handle WWWInline nodes\n";
  return true;
}

static const bool warn = false;

#define NOWARN_IMPLEMENTATION(T)  bool QvVisitor::Visit(T*) { return false; }
#define DEFAULT_IMPLEMENTATION(T) bool QvVisitor::Visit(T*) { \
  if (warn) vcl_cerr << "QvVisitor: ignoring " << #T << vcl_endl; return false; }

NOWARN_IMPLEMENTATION(QvInfo);
NOWARN_IMPLEMENTATION(QvCoordinate3);
NOWARN_IMPLEMENTATION(QvTextureCoordinate2);

DEFAULT_IMPLEMENTATION(QvAsciiText);
DEFAULT_IMPLEMENTATION(QvCone);
DEFAULT_IMPLEMENTATION(QvCube);
DEFAULT_IMPLEMENTATION(QvCylinder);
DEFAULT_IMPLEMENTATION(QvDirectionalLight);
DEFAULT_IMPLEMENTATION(QvFontStyle);
DEFAULT_IMPLEMENTATION(QvIndexedFaceSet);
DEFAULT_IMPLEMENTATION(QvIndexedLineSet);
DEFAULT_IMPLEMENTATION(QvLOD);
DEFAULT_IMPLEMENTATION(QvMaterial);
DEFAULT_IMPLEMENTATION(QvMaterialBinding);
DEFAULT_IMPLEMENTATION(QvMatrixTransform);
DEFAULT_IMPLEMENTATION(QvNormal);
DEFAULT_IMPLEMENTATION(QvNormalBinding);
DEFAULT_IMPLEMENTATION(QvOrthographicCamera);
DEFAULT_IMPLEMENTATION(QvPerspectiveCamera);
DEFAULT_IMPLEMENTATION(QvPointLight);
DEFAULT_IMPLEMENTATION(QvPointSet);
DEFAULT_IMPLEMENTATION(QvRotation);
DEFAULT_IMPLEMENTATION(QvScale);
DEFAULT_IMPLEMENTATION(QvShapeHints);
DEFAULT_IMPLEMENTATION(QvSphere);
DEFAULT_IMPLEMENTATION(QvSpotLight);
DEFAULT_IMPLEMENTATION(QvTexture2);
DEFAULT_IMPLEMENTATION(QvTexture2Transform);
DEFAULT_IMPLEMENTATION(QvTransform);
DEFAULT_IMPLEMENTATION(QvTranslation);
DEFAULT_IMPLEMENTATION(QvWWWAnchor);
DEFAULT_IMPLEMENTATION(QvUnknownNode);
DEFAULT_IMPLEMENTATION(QvLabel);
DEFAULT_IMPLEMENTATION(QvLightModel);
