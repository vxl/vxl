// This is brl/bbas/imesh/imesh_fileio.h
#ifndef imesh_fileio_h_
#define imesh_fileio_h_
//:
// \file
// \brief Functions for reading mesh files
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date May 2, 2008

#include <vcl_iostream.h>
#include <vcl_string.h>
#include "imesh_mesh.h"


//: Read a mesh from a file, determine type from extension
bool imesh_read(const vcl_string& filename, imesh_mesh& mesh);

//: Read a mesh from a PLY2 stream
bool imesh_read_ply2(vcl_istream& is, imesh_mesh& mesh);

//: Read a mesh from a PLY2 file
bool imesh_read_ply2(const vcl_string& filename, imesh_mesh& mesh);

//: Read a mesh from a PLY file
bool imesh_read_ply(const vcl_string& filename, imesh_mesh& mesh);

//: Read a mesh from a PLY file
bool imesh_read_ply(vcl_istream& is, imesh_mesh& mesh);

//: Write a mesh to a PLY2 stream
void imesh_write_ply2(vcl_ostream& os, const imesh_mesh& mesh);

//: Write a mesh to a PLY2 file
void imesh_write_ply2(const vcl_string& filename, const imesh_mesh& mesh);

//: Read texture coordinates from a UV2 stream
bool imesh_read_uv2(vcl_istream& is, imesh_mesh& mesh);

//: Read texture coordinates from a UV2 file
bool imesh_read_uv2(const vcl_string& filename, imesh_mesh& mesh);

//: Read a mesh from a wavefront OBJ stream
bool imesh_read_obj(vcl_istream& is, imesh_mesh& mesh);

//: Read a mesh from a wavefront OBJ file
bool imesh_read_obj(const vcl_string& filename, imesh_mesh& mesh);

//: Write a mesh to a wavefront OBJ stream
void imesh_write_obj(vcl_ostream& os, const imesh_mesh& mesh);

//: Write a mesh to a wavefront OBJ file
void imesh_write_obj(const vcl_string& filename, const imesh_mesh& mesh);

//: Write a mesh into a kml file
void imesh_write_kml(vcl_ostream& os, const imesh_mesh& mesh);

//: Write a mesh into a kml collada file
void imesh_write_kml_collada(vcl_ostream& os, const imesh_mesh& mesh);

//: Write a mesh into a vrml file
void imesh_write_vrml(vcl_ostream& os, const imesh_mesh& mesh);

#endif // imesh_fileio_h_
