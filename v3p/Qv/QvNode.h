//<file>
//
// Name:        QvNode.h
//
// Purpose:     declaration basis for all QvNodes
//
// Created:     24 Apr 95   Michael Pichler; taken from QvSubNode.h
//
// Changed:      8 Feb 96   Alexander Nussbaumer (NodeType)
//
// Changed:      7 Jun 96   Michael Pichler
//
//</file>

#ifndef  _QV_NODE_
#define  _QV_NODE_

#include "QvString.h"

#include <vcl_iosfwd.h>

#include "vectors.h"

class QvChildList;
class QvDict;
class QvFieldData;
class QvInput;
class QvNodeList;
class QvState;

class Scene3D;  // mpichler
class VRMLScene;
class BSPTree;  // gmes
class QvGroup;


class QvNodeType
{ // name conflicts when defined inside QvNode
public:
  enum {  // anuss: each node can identify itself
    QvAsciiText, QvCone, QvCoordinate3, QvCube, QvCylinder,
    QvDirectionalLight, QvFontStyle, QvGroup, QvIndexedFaceSet,
    QvIndexedLineSet, QvInfo, QvLOD, QvMaterial, QvMaterialBinding,
    QvMatrixTransform, QvNormal, QvNormalBinding, QvOrthographicCamera,
    QvPerspectiveCamera, QvPointLight, QvPointSet, QvRotation, QvScale,
    QvSeparator, QvShapeHints, QvSphere, QvSpotLight, QvSwitch,
    QvTexture2, QvTexture2Transform, QvTextureCoordinate2, QvTransform,
    QvTransformSeparator, QvTranslation, QvWWWAnchor, QvWWWInline,
    QvUnknownNode, QvLabel, QvLightModel,
    unsetType = -1
  };
};

class QvNode
{
public:
  enum Stage {
    FIRST_INSTANCE,             // First real instance being constructed
    PROTO_INSTANCE,             // Prototype instance being constructed
    OTHER_INSTANCE              // Subsequent instance being constructed
  };

  QvFieldData   *fieldData;
  QvChildList   *children;
  QvBool        isBuiltIn;

  QvName                *objName;
  QvNode();
  virtual ~QvNode();

  // Reference counting:
  mutable long  refCount;
  void  ref() const;            // Adds reference
  void  unref() const;          // Removes reference, deletes if now 0
  void  unrefNoDelete() const;  // Removes reference, never deletes

  const QvName &        getName() const;
  void                  setName(const QvName &name);

  static void           init();
  static QvBool         read(QvInput *in, QvNode *&node);

  virtual QvFieldData * getFieldData() = 0;

  int nodeType ()  // anuss
  { return nodetype_; }

  int isGroupNode ();  // returns non-zero iff node derived from QvGroup

  // traversal functions (mpichler)
  virtual void traverse(QvState *state) = 0;  // print node information

  virtual void build (QvState* state) = 0;  // preprocessing step

  virtual void draw ();  // draw the node (base routine just draws selection)

  virtual int pick (int depth) = 0;  // picking

  virtual void buildBSP (BSPTree* bsp_root) = 0;  // Georg Meszaros, 19960310

  virtual void save (QvDict* dict) = 0;  // saving (anuss, 19960118)
  static void saveAll (const QvNode *root, vcl_ostream& os, float version);

  virtual void pass (QvGroup* parent, int childindex);  // traversal

  void select ()  { selected_ = 1; }            // select node
  void unselect ()  { selected_ = 0;  }         // unselect node
  int selected () const  { return selected_; }  // get selected flag

  // pointers back to the current (VRML)Scene (mpichler)
  static int curdrawmode_;                      // current drawing mode
  static Scene3D* scene_;                       // scene (management) class
  static VRMLScene* vrmlscene_;                 // vrml scene (data)
  static matrix4D selectedTransformation_;      // transformation of selected object

  int hasextent_;                       // flag if extent (if not, wmin_/wmax_ unset)
  point3D wmin_, wmax_;                 // bounding box (world coordinates)
  point3D omin_, omax_;                 // bounding box (object coordinates)

protected:
  virtual QvBool        readInstance(QvInput *in);
  int nodetype_;                        // anuss: QvNodeType
  int selected_;                        // mpichler: flag whether selected

private:
  static QvDict *nameDict;

  static void           addName(QvNode *, const char *);
  static void           removeName(QvNode *, const char *);
  static QvNode *       readReference(QvInput *in);
  static QvBool readNode(QvInput *in, QvName &className,QvNode *&node);
  static QvBool readNodeInstance(QvInput *in, const QvName &className,
                                 const QvName &refName, QvNode *&node);
  static QvNode *       createInstance(QvInput *in, const QvName &className);
  static QvNode *       createInstanceFromName(const QvName &className);
  static void           flushInput(QvInput *in);
};

#endif /* _QV_NODE_ */
