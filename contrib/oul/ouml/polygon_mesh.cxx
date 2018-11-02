// This is oul/ouml/polygon_mesh.cxx
#include <iostream>
#include <cstdio>
#include "polygon_mesh.h"
//:
// \file
// \brief polygon_mesh.cc: a class for polygon meshes
// \author
// Copyright (c) 2001 Brendan McCane University of Otago, Dunedin, New
// Zealand Reproduction rights limited as described in the COPYRIGHT
// file.
//----------------------------------------------------------------------

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vnl/vnl_cross.h>

//----------------------------------------------------------------------
//: add_vertex
//
// Add a vertex to the end of the list of vertices
//
// \param pt  The location of the vertex
// \return    The position of the added vertex
//
// \author Brendan McCane
//----------------------------------------------------------------------
int PolygonMesh::add_vertex(DPoint &pt)
{
  vertex_list.push_back(pt);
  return vertex_list.size()-1;
}


//----------------------------------------------------------------------
//: add_vertex
//
// Add a vertex to the prescribed position in the list of vertices
//
// \param pt    The location of the vertex
// \param index Where to add the vertex
// \return      The position of the added vertex
//
// \author Brendan McCane
//----------------------------------------------------------------------

int PolygonMesh::set_vertex(int index, DPoint &pt)
{
  assert(index>=0);
  if (vertex_list.capacity()<=(unsigned int)index)
    vertex_list.reserve(index+100);
  vertex_list[index] = pt;
  return index;
}

//----------------------------------------------------------------------
//: add_face
//
// Add a face to the end of the list of faces
//
// \param fc The vertices associated with the face
// \return   The position of the added face
//
// \author Brendan McCane
//----------------------------------------------------------------------

int PolygonMesh::add_face(Face &fc)
{
  face_list.push_back(fc);
  return face_list.size()-1;
}

//----------------------------------------------------------------------
//: add_face
//
// Add a face to the prescribed position in the list of faces
//
// \param fc    The vertices associated with the face
// \param index Where to add the face
// \return      The position of the added face
//
// \author Brendan McCane
//----------------------------------------------------------------------

int PolygonMesh::set_face(int index, Face &fc)
{
  assert(index>=0);
  if (face_list.capacity()<=(unsigned int)index)
    face_list.reserve(index+100);
  face_list[index] = fc;
  return index;
}

//----------------------------------------------------------------------
//: get_vertex
//
// Returns a vertex at the specified location
//
// \param index the position to return
// \return      the position of the vertex
//
// \author Brendan McCane
//----------------------------------------------------------------------

PolygonMesh::DPoint PolygonMesh::get_vertex(int index)
{
  assert(index >= 0);
  if ((unsigned int)index>=vertex_list.size())
  {
    std::cerr << "Warning: vertex doesn't exist\n";
    return {};
  }
  return vertex_list[index];
}

//----------------------------------------------------------------------
//: get_face
//
// Returns a face at the specified location
//
// \param index  the position to return
// \return       a list of vertices
//
// \author Brendan McCane
//----------------------------------------------------------------------

PolygonMesh::Polygon PolygonMesh::get_face(int index)
{
  assert(index >= 0);
  Polygon poly;
  if ((unsigned int)index>=face_list.size())
  {
    std::cerr << "Warning: face doesn't exist\n";
    return poly;
  }
  Face& face = face_list[index];
  for (int i : face)
    poly.push_back(vertex_list[i]);
  return poly;
}


//----------------------------------------------------------------------
//: read_file
//
// read a mesh in from a file
//
// \param filename  the name of the file
// \return          whether or not the file was successfully read
//
// \author Brendan McCane
//----------------------------------------------------------------------

bool PolygonMesh::read_file(char *filename)
{
  char start[100];
  FILE *fp=std::fopen(filename, "r");
  if (!fp) return false;

  while (std::fscanf(fp, "%s", start)!=EOF)
  {
    if (!std::strcmp(start, "Vertex")) // read in a vertex
    {
      int index;
      double x, y, z;
      int ret = std::fscanf(fp, "%d %lf %lf %lf", &index, &x, &y, &z); if (ret<4) return false;
      // add the vertex
      DPoint pt(x,y,z);
      //std::printf("Vertex: %d %f %f %f\n", index, x, y, z);
      set_vertex(index, pt);
    }
    else if (!std::strcmp(start, "Face")) // read in a face
    {
      int index;
      int vertex;
      Face fc;
      int ret = std::fscanf(fp, "%d", &index); if (ret<1) return false;
      //std::printf("Face: %d ", index);
      // read in all the vertex indices
      while ((std::fscanf(fp, "%d", &vertex)))
      {
        //std::printf("%d ", vertex);
        fc.push_back(vertex);
      }
      //std::printf("\n");
      set_face(index, fc);
    }
    else if (!std::strcmp(start, "End")) // end of mesh
      break;
    else // assume comment, read to eoln
    {
      char c;
      do {
        int ret = std::fscanf(fp, "%c", &c); if (ret<1) return false;
      } while (c!='\n');
    }
  }

  return true;
}

//----------------------------------------------------------------------
//: get_face_normal
//
// return the normal vector for the given vertex of a given face.
//
// \param face_index    which face
//
// \param vertex_index  which vertex in the face (ie not the
// vertex index in the entire list of vertices, but the vertex index
// within the face).
//
// \return the normalised normal vector
//
// \author Brendan McCane
//----------------------------------------------------------------------

PolygonMesh::DVector3D PolygonMesh::get_face_normal
(
  int face_index,
  int /*vertex_index*/
)
{
  assert(face_index>=0);
  assert((unsigned int)face_index<face_list.size());
  // in this version, I'm just returning the same vector for all vertices - TODO
  // Therefore I need to find the cross-product of two vectors lying
  // on the plane of the polygon.
  Polygon face = get_face(face_index);
  DVector3D v1(face[0].x()-face[1].x(), face[0].y()-face[1].y(),
               face[0].z()-face[1].z());
  DVector3D v2(face[0].x()-face[2].x(), face[0].y()-face[2].y(),
               face[0].z()-face[2].z());
  DVector3D cross = vnl_cross_3d(v1, v2);
  cross.normalize();
  return cross;
}
