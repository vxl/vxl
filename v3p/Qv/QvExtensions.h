//<copyright>
//
// Copyright (c) 1995
// Institute for Information Processing and Computer Supported New Media (IICM),
// Graz University of Technology, Austria.
//
//</copyright>

//<file>
//
// Name:        QvExtensions.h
//
// Purpose:     QvLib VRML extensions for common Inventor nodes
//
// Created:      8 Aug 95   Michael Pichler
//
// Changed:     31 Aug 95   Michael Pichler
//
//
//</file>



#ifndef  _QV_EXTENSIONS_H_
#define  _QV_EXTENSIONS_H_

#include "QvSFEnum.h"
#include "QvSFString.h"
#include "QvSubNode.h"


// personal remark [mpi]: I hate meta-w-programming,
// but I don't want to break style with QvLib...


class QvLabel: public QvNode
{
  QV_NODE_HEADER(QvLabel);

public:
  // Fields
  QvSFString label;
  static int newinstance_;
};

class QvLightModel: public QvNode
{
  QV_NODE_HEADER(QvLightModel);

public:
  enum Model {
    BASE_COLOR,
    PHONG
  };

  // Fields
  QvSFEnum model;
  static int newinstance_;

  int dolighting_;  // mpichler, 19950809
};

#endif // _QV_EXTENSIONS_H_
