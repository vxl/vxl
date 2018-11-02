// This is brl/bbas/bmsh3d/bmsh3d_textured_face_mc.h
//---------------------------------------------------------------------
#ifndef bmsh3d_textured_face_mc_h_
#define bmsh3d_textured_face_mc_h_
//:
// \file
// \brief A Multiply Connected Face with an associated texture map.
//
//        Note that it is assumed the vertices are of type bmsh3d_textured_vertex
//
// \author
//  Daniel Crispell March 14, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <iostream>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "bmsh3d_face_mc.h"
#include <vgl/vgl_point_2d.h>

class bmsh3d_textured_face_mc : public bmsh3d_face_mc
{
 protected:
  //: URI of texture map file
  std::string tex_map_uri_;

  //: Should be one tex_coord per face vertex, corresponding to the ifs_face vertices.
  std::map<int, vgl_point_2d<double> > tex_coords_;

  // ====== Constructors/Destructor ======
 public:
  bmsh3d_textured_face_mc (const int id): bmsh3d_face_mc(id){}

  //: converts an untextured face to a textured face
  bmsh3d_textured_face_mc (const bmsh3d_face* face, std::string tex_map_uri);

  std::string tex_map_uri() const {return tex_map_uri_;}
  void set_tex_map_uri(std::string uri){tex_map_uri_ = uri;}

#if 0
  //: copy constructor
  bmsh3d_textured_face_mc (const bmsh3d_textured_face_mc& face);
#endif

  ~bmsh3d_textured_face_mc () override {tex_coords_.clear();}

  // ====== Data access functions ======

  vgl_point_2d<double> tex_coords (const unsigned int i) {
    ///assert (i < vertices_.size());
    return tex_coords_[i];
  }

  std::map<int, vgl_point_2d<double> >&  tex_coords() {
    return tex_coords_;
  }

  void set_tex_coord(int id, vgl_point_2d<double> pt) {
    tex_coords_[id] = pt;
  }

  // ====== Connectivity Modification Functions ======

  void _add_vertex(bmsh3d_vertex* vertex) {
    vgl_point_2d<double> tex_coord(0.0, 0.0);
    _add_vertex(vertex,tex_coord);
  }

  void _add_vertex (bmsh3d_vertex* vertex, vgl_point_2d<double> tex_coord) {
    vertices_.push_back (vertex);
    tex_coords_[vertex->id()] = tex_coord;
  }

  void print();
};

#endif
