//-*-c++-*--------------------------------------------------------------
//
// polygon_mesh.h: a class for polygon meshes
//
// Copyright (c) 2001 Brendan McCane University of Otago, Dunedin, New
// Zealand Reproduction rights limited as described in the COPYRIGHT
// file.
//----------------------------------------------------------------------

#ifndef OTAGO_polygon_mesh__h_INCLUDED
#define OTAGO_polygon_mesh__h_INCLUDED

#include <vgl/vgl_point_3d.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector_fixed.h>

class PolygonMesh
{
 public:
  typedef vgl_point_3d<double> DPoint ;
  typedef vnl_vector_fixed<double, 3> DVector3D;
  typedef vcl_vector<int> Face; // a face is a list of vertices
  typedef vcl_vector<DPoint> Polygon;
 private:
  vcl_vector<DPoint> vertex_list;
  vcl_vector<Face> face_list;

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
  inline int num_faces(){return face_list.size();}
  inline int num_vertices(){return vertex_list.size();}
};

vcl_ostream &operator <<(vcl_ostream &os, PolygonMesh &pmesh);
// add these when I can access iostream docs
vcl_istream &operator >>(vcl_istream &is, PolygonMesh &pmesh);

#endif // OTAGO_polygon_mesh__h_INCLUDED
