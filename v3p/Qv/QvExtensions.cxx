//<copyright>
// 
// Copyright (c) 1995
// Institute for Information Processing and Computer Supported New Media (IICM),
// Graz University of Technology, Austria.
// 
//</copyright>

//<file>
//
// Name:        QvExtensions.C
//
// Purpose:     QvLib VRML extensions for common Inventor nodes
//
// Created:      8 Aug 95   Michael Pichler
//
// Changed:     31 Aug 95   Michael Pichler
//
//
//</file>



#include "QvExtensions.h"


/*** Label ***/

QV_NODE_SOURCE(QvLabel);
int QvLabel::newinstance_ = 0;

QvLabel::QvLabel()
{
    QV_NODE_CONSTRUCTOR(QvLabel);
    isBuiltIn = TRUE;
    newinstance_ = 1;  // to avoid repetitive warnings

    QV_NODE_ADD_FIELD(label);

    label.value = "<Undefined label>";
}

QvLabel::~QvLabel()
{
}


/*** LightModel ***/

QV_NODE_SOURCE(QvLightModel);
int QvLightModel::newinstance_ = 0;

QvLightModel::QvLightModel ()
{
    QV_NODE_CONSTRUCTOR(QvLightModel);
    isBuiltIn = TRUE;
    newinstance_ = 1;  // to avoid repetitive warnings

    QV_NODE_ADD_FIELD(model);

    model.value = PHONG;

    QV_NODE_DEFINE_ENUM_VALUE(Model, BASE_COLOR);
    QV_NODE_DEFINE_ENUM_VALUE(Model, PHONG);

    QV_NODE_SET_SF_ENUM_TYPE(model, Model);
}

QvLightModel::~QvLightModel()
{
}
