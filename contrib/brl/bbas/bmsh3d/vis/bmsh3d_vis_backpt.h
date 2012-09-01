//:
// \file
// \author 040309 Ming: bmsh3d_vis_backpt.h
// \brief  Derived class of standard OpenInventor object to put backward pointer to our datastructure.

#ifndef bmsh3d_vis_backpt_h_
#define bmsh3d_vis_backpt_h_

#include <bmsh3d/bmsh3d_utils.h>

#include <bmsh3d/bmsh3d_vertex.h>
#include <bmsh3d/bmsh3d_face.h>

#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoCoordinate3.h>

void bmsh3dvis_init_vispt_OpenInventor_classes ();

//: the GUI Element has a backward pointer to the element in the internal data structure.
class bmsh3dvis_gui_elm
{
 protected:
  vispt_elm* _element;

 public:
  bmsh3dvis_gui_elm() {}
  virtual ~bmsh3dvis_gui_elm() {}

  vispt_elm* element() {
    return _element;
  }
};

//----------------- MESH Vertex Sphere -----------------------------
class bmsh3dvis_vertex_SoPointSet : public SoPointSet, public bmsh3dvis_gui_elm
{
  SO_NODE_HEADER( bmsh3dvis_vertex_SoPointSet );

 public:
  bmsh3dvis_vertex_SoPointSet (const bmsh3d_vertex* point=NULL );
  virtual ~bmsh3dvis_vertex_SoPointSet() {}

  static void initClass(void) {
    SO_NODE_INIT_CLASS( bmsh3dvis_vertex_SoPointSet, SoPointSet, "SoPointSet");
  }
  virtual const char * getFileFormatName(void) const {
    return "bmsh3dvis_vertex_SoPointSet";
  }
};

//----------------- MESH Vertex Cube -------------------------------------
class bmsh3dvis_vertex_SoCube : public SoCube, public bmsh3dvis_gui_elm
{
  SO_NODE_HEADER (bmsh3dvis_vertex_SoCube);

 public:
  bmsh3dvis_vertex_SoCube (const bmsh3d_vertex* point=NULL);
  virtual ~bmsh3dvis_vertex_SoCube() {}

  static void initClass(void) {
    SO_NODE_INIT_CLASS( bmsh3dvis_vertex_SoCube, SoCube, "SoCube");
  }
  virtual const char * getFileFormatName(void) const {
    return "bmsh3dvis_vertex_SoCube";
  }
};

//----------------- MESH EDGE -------------------------------------
class bmsh3dvis_edge_SoLineSet : public SoLineSet, public bmsh3dvis_gui_elm
{
  SO_NODE_HEADER (bmsh3dvis_edge_SoLineSet);

 protected:
  //: for online alternation
  SoCoordinate3* _coordinate;

 public:
  bmsh3dvis_edge_SoLineSet (const bmsh3d_edge* edge=NULL);
  virtual ~bmsh3dvis_edge_SoLineSet() {}

  static void initClass() {
    SO_NODE_INIT_CLASS (bmsh3dvis_edge_SoLineSet, SoLineSet, "SoLineSet");
  }
  virtual const char * getFileFormatName() const {
    return "bmsh3dvis_edge_SoLineSet";
  }

  //: for online alternation
  void setCoordinate3 (SoCoordinate3* coordinate) {
    _coordinate = coordinate;
  }
  SoCoordinate3* coordinate3() {
    return _coordinate;
  }
  void update();
};

//----------------- MESH FACE -------------------------------------
class bmsh3dvis_face_SoFaceSet : public SoFaceSet, public bmsh3dvis_gui_elm
{
  SO_NODE_HEADER (bmsh3dvis_face_SoFaceSet);

 public:
  bmsh3dvis_face_SoFaceSet (const bmsh3d_face* face=NULL);
  virtual ~bmsh3dvis_face_SoFaceSet() {}

  static void initClass() {
    SO_NODE_INIT_CLASS (bmsh3dvis_face_SoFaceSet, SoFaceSet, "SoFaceSet");
  }
  virtual const char * getFileFormatName() const {
    return "bmsh3dvis_face_SoFaceSet";
  }
};

#endif // bmsh3d_vis_backpt_h_
