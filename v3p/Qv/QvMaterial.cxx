#include "QvMaterial.h"

QV_NODE_SOURCE(QvMaterial);

QvMaterial::QvMaterial()
{
    QV_NODE_CONSTRUCTOR(QvMaterial);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(ambientColor);
    QV_NODE_ADD_FIELD(diffuseColor);
    QV_NODE_ADD_FIELD(specularColor);
    QV_NODE_ADD_FIELD(emissiveColor);
    QV_NODE_ADD_FIELD(shininess);
    QV_NODE_ADD_FIELD(transparency);

    ambientColor.values[0]=ambientColor.values[1]=ambientColor.values[2] = 0.2f;
    diffuseColor.values[0]=diffuseColor.values[1]=diffuseColor.values[2] = 0.2f;
    specularColor.values[0]=specularColor.values[1]=specularColor.values[2]=0.;
    emissiveColor.values[0]=emissiveColor.values[1]=emissiveColor.values[2]=0.;
    shininess.values[0] = 0.2f;
    transparency.values[0] = 0.0;

    // mpichler, 19951109: initialise materials
    int i;
    materialsGE3D* mat;
    for (i = 0, mat = materials_;  i < hilit_num;  i++, mat++)
      initmtl3D (mat);
}

QvMaterial::~QvMaterial()
{
    // mpichler, 19951109: destroy arrays in all but first material
    // (which has pointers into member data structures)
    // moreover only diffuse and emissive are deleted, because all
    // others are pointers to the unhilit material components
    // and rgb_base is always a pointer to rgb_diffuse or rgb_emissive

    int i;
    materialsGE3D* mat;
    for (i = 1, mat = materials_ + 1;  i < hilit_num;  i++, mat++)
    {
      delete mat->rgb_diffuse;
      delete mat->rgb_emissive;
    }
}
