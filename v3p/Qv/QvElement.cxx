#include "QvElement.h"

const char *QvElement::nodeTypeNames[QvElement::NumNodeTypes] = {
    "Unknown",
    "OrthographicCamera",
    "PerspectiveCamera",
    "DirectionalLight",
    "PointLight",
    "SpotLight",
    "NoOpTransform",
    "MatrixTransform",
    "Rotation",
    "Scale",
    "Transform",
    "Translation",
};

QvElement::QvElement()
{
  // These will be set to something real when the element is
  // added to the state
  depth = -1;
  next = NULL;

  // Presumably, the caller will set these
  data = NULL;
  type = Unknown;
}

QvElement::~QvElement()
{
}

void
QvElement::print()
{
  vcl_printf("\t\tElement of type %s\n", nodeTypeNames[type]);
}
