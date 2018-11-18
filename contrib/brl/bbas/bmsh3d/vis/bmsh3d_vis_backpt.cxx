//: 050223 MingChing Chang

#include "bmsh3d_vis_backpt.h"

// ==========================================================
//    The Homemade vis_pointer OpenInventor Classes
// ==========================================================
SO_NODE_SOURCE (bmsh3dvis_vertex_SoPointSet);
SO_NODE_SOURCE (bmsh3dvis_vertex_SoCube);
SO_NODE_SOURCE (bmsh3dvis_edge_SoLineSet);
SO_NODE_SOURCE (bmsh3dvis_face_SoFaceSet);


void bmsh3dvis_init_vispt_OpenInventor_classes ()
{
  bmsh3dvis_vertex_SoPointSet::initClass();
  bmsh3dvis_vertex_SoCube::initClass();
  bmsh3dvis_edge_SoLineSet::initClass();
  bmsh3dvis_face_SoFaceSet::initClass();
}

// ==========================================================

bmsh3dvis_vertex_SoPointSet::bmsh3dvis_vertex_SoPointSet (const bmsh3d_vertex* point)
{
  SO_NODE_CONSTRUCTOR (bmsh3dvis_vertex_SoPointSet);
  this->isBuiltIn = true;

  _element = (vispt_elm*) point; // casting away const !!!
  _element->set_vis_pointer ((void*)this);
}

bmsh3dvis_vertex_SoCube::bmsh3dvis_vertex_SoCube (const bmsh3d_vertex* point)
{
  SO_NODE_CONSTRUCTOR (bmsh3dvis_vertex_SoCube);
  this->isBuiltIn = true;

  _element = (vispt_elm*) point; // casting away const !!!
  _element->set_vis_pointer ((void*)this);
}

bmsh3dvis_edge_SoLineSet::bmsh3dvis_edge_SoLineSet (const bmsh3d_edge* edge)
{
  SO_NODE_CONSTRUCTOR (bmsh3dvis_edge_SoLineSet);
  this->isBuiltIn = true;

  _element = (vispt_elm*) edge; // casting away const !!!
  _element->set_vis_pointer ((void*)this);
}

bmsh3dvis_face_SoFaceSet::bmsh3dvis_face_SoFaceSet (const bmsh3d_face* face)
{
  SO_NODE_CONSTRUCTOR (bmsh3dvis_face_SoFaceSet);
  this->isBuiltIn = true;

  _element = (vispt_elm*) face; // casting away const !!!
  _element->set_vis_pointer ((void*)this);
}
