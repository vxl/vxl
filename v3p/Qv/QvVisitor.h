// This is v3p/Qv/QvVisitor.h
#ifndef QvVisitor_h_
#define QvVisitor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//
// .NAME    QvVisitor - Traverse a VRML 1.0 file
// .LIBRARY vview
// .HEADER  Oxford Package
// .INCLUDE vview/QvVisitor.h
// .FILE    QvVisitor.cxx
//
// .SECTION Description
//    QvVisitor is the best way to read a VRML 1.0 file.
//    Derive your class from QvVisitor, and use QvFile to traverse it.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Jan 99
//
// .SECTION Modifications:
//   990109 AWF Initial version.
//
//-----------------------------------------------------------------------------

class QvNode;
class QvGroup;
class QvAsciiText;
class QvCone;
class QvCoordinate3;
class QvCube;
class QvCylinder;
class QvDirectionalLight;
class QvFontStyle;
class QvIndexedFaceSet;
class QvIndexedLineSet;
class QvInfo;
class QvLOD;
class QvMaterial;
class QvMaterialBinding;
class QvMatrixTransform;
class QvNormal;
class QvNormalBinding;
class QvOrthographicCamera;
class QvPerspectiveCamera;
class QvPointLight;
class QvPointSet;
class QvRotation;
class QvScale;
class QvSeparator;
class QvShapeHints;
class QvSphere;
class QvSpotLight;
class QvSwitch;
class QvTexture2;
class QvTexture2Transform;
class QvTextureCoordinate2;
class QvTransform;
class QvTransformSeparator;
class QvTranslation;
class QvWWWAnchor;
class QvWWWInline;
class QvUnknownNode;
class QvLabel;
class QvLightModel;

class QvVisitor
{
 public:
  QvVisitor();
  virtual ~QvVisitor();

  // Non-leaf nodes
  virtual bool Visit(QvNode*);
  virtual bool Visit(QvGroup*);

  virtual bool Visit(QvAsciiText *);
  virtual bool Visit(QvCone *);
  virtual bool Visit(QvCoordinate3 *);
  virtual bool Visit(QvCube *);
  virtual bool Visit(QvCylinder *);
  virtual bool Visit(QvDirectionalLight *);
  virtual bool Visit(QvFontStyle *);
  virtual bool Visit(QvIndexedFaceSet *);
  virtual bool Visit(QvIndexedLineSet *);
  virtual bool Visit(QvInfo *);
  virtual bool Visit(QvLOD *);
  virtual bool Visit(QvMaterial *);
  virtual bool Visit(QvMaterialBinding *);
  virtual bool Visit(QvMatrixTransform *);
  virtual bool Visit(QvNormal *);
  virtual bool Visit(QvNormalBinding *);
  virtual bool Visit(QvOrthographicCamera *);
  virtual bool Visit(QvPerspectiveCamera *);
  virtual bool Visit(QvPointLight *);
  virtual bool Visit(QvPointSet *);
  virtual bool Visit(QvRotation *);
  virtual bool Visit(QvScale *);
  virtual bool Visit(QvSeparator *);
  virtual bool Visit(QvShapeHints *);
  virtual bool Visit(QvSphere *);
  virtual bool Visit(QvSpotLight *);
  virtual bool Visit(QvSwitch *);
  virtual bool Visit(QvTexture2 *);
  virtual bool Visit(QvTexture2Transform *);
  virtual bool Visit(QvTextureCoordinate2 *);
  virtual bool Visit(QvTransform *);
  virtual bool Visit(QvTransformSeparator *);
  virtual bool Visit(QvTranslation *);
  virtual bool Visit(QvWWWAnchor *);
  virtual bool Visit(QvWWWInline *);
  virtual bool Visit(QvUnknownNode *);
  virtual bool Visit(QvLabel *);
  virtual bool Visit(QvLightModel *);
 private:
  int indent;
};

#endif // QvVisitor_h_
