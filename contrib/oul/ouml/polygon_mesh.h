// This is oul/ouml/polygon_mesh.h
// Copyright (c) 2001 Brendan McCane University of Otago, Dunedin, New
// Zealand Reproduction rights limited as described in the COPYRIGHT file.
#ifndef OTAGO_polygon_mesh__h_INCLUDED
#define OTAGO_polygon_mesh__h_INCLUDED
//:
// \file
// \brief a class for polygon meshes
//
//----------------------------------------------------------------------

#include <vector>
#include <iostream>
#include <iosfwd>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class PolygonMesh
{
 public:
  typedef vgl_point_3d<double> DPoint ;
  typedef vnl_vector_fixed<double, 3> DVector3D;
  typedef std::vector<int> Face; // a face is a list of vertices
  typedef std::vector<DPoint> Polygon;
 private:
  std::vector<DPoint> vertex_list;
  std::vector<Face> face_list;

 public:
  PolygonMesh(int num_vertices=100, int num_faces=100):
    vertex_list(num_vertices), face_list(num_faces){}
  int add_vertex(DPoint &pt);
  int set_vertex(int index, DPoint &pt);
  int add_face(Face &fc);
  int set_face(int index, Face &fc);
  DPoint get_vertex(int index);
  Polygon get_face(int index);
  DVector3D get_face_normal(int face_index,
                            int vertex_index);
  bool read_file(char *filename);
  inline int num_faces() const {return face_list.size();}
  inline int num_vertices() const {return vertex_list.size();}
};

std::ostream &operator <<(std::ostream &os, PolygonMesh &pmesh);
// add these when I can access iostream docs
std::istream &operator >>(std::istream &is, PolygonMesh &pmesh);

#endif // OTAGO_polygon_mesh__h_INCLUDED
