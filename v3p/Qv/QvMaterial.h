#ifndef  _QV_MATERIAL_
#define  _QV_MATERIAL_

#include "QvMFColor.h"
#include "QvMFFloat.h"
#include "QvSubNode.h"

#include "mtl.h"

class QvMaterial : public QvNode
{
  QV_NODE_HEADER(QvMaterial);

public:
  // Fields
  QvMFColor    ambientColor;   // Ambient color
  QvMFColor    diffuseColor;   // Diffuse color
  QvMFColor    specularColor;  // Specular color
  QvMFColor    emissiveColor;  // Emissive color
  QvMFFloat    shininess;      // Shininess
  QvMFFloat    transparency;   // Transparency

  // mpichler, 19951109
  enum {
    hilit_none,                // unchanged material
    hilit_bright,              // material brighted up
    hilit_dark,                // material dimmed down
    hilit_colshade,            // shade of one color
    hilit_greyshade,           // shade of grey
    hilit_num                  // no. of material hilits
  };

  // mpichler, 19950629
  materialsGE3D materials_ [hilit_num]; // arrays of material components
};

#endif // _QV_MATERIAL_
