//-*-c++-*--------------------------------------------------------------
//
// polygon_mesh.cc: a class for polygon meshes
//
// Copyright (c) 2001 Brendan McCane University of Otago, Dunedin, New
// Zealand Reproduction rights limited as described in the COPYRIGHT
// file.
//----------------------------------------------------------------------

#include "polygon_mesh.h"

//----------------------------------------------------------------------
//: add_vertex
//
// Add a vertex to the end of the list of vertices
//
// .param DPoint &pt: The location of the vertex
// .ret int: the position of the added vertex 
//
//.author Brendan McCane
//----------------------------------------------------------------------
int PolygonMesh::add_vertex(DPoint &pt)
{
	vertex_list.push_back(pt);
	return (vertex_list.size()-1);
}


//----------------------------------------------------------------------
//: add_vertex
//
// Add a vertex to the prescribed position in the list of vertices
//
// .param DPoint &pt: The location of the vertex
// .param int index: where to add the vertex
// .ret int: the position of the added vertex 
//
//.author Brendan McCane
//----------------------------------------------------------------------

int PolygonMesh::set_vertex(int index, DPoint &pt)
{
	assert(index>=0);
	if (vertex_list.capacity()<=index)
		vertex_list.reserve(index+100);
	vertex_list[index] = pt;
	return(index);
}

//----------------------------------------------------------------------
//: add_face
//
// Add a face to the end of the list of faces
//
// .param Face &fc: The vertices associated with the face
// .ret int: the position of the added face 
//
//.author Brendan McCane
//----------------------------------------------------------------------

int PolygonMesh::add_face(Face &fc)
{
	face_list.push_back(fc);
	return(face_list.size()-1);
}

//----------------------------------------------------------------------
//: add_face
//
// Add a face to the prescribed position in the list of faces
//
// .param Face &fc: The vertices associated with the face
// .param int index: where to add the face
// .ret int: the position of the added face 
//
//.author Brendan McCane
//----------------------------------------------------------------------

int PolygonMesh::set_face(int index, Face &fc)
{
	assert(index>=0);
	if (face_list.capacity()<=index)
		face_list.reserve(index+100);
	face_list[index] = fc;
	return(index);
}

//----------------------------------------------------------------------
//: get_vertex
//
// Returns a vertex at the specified location
//
// .param int index: the position to return
// .ret DPoint: the position of the vertex
//
//.author Brendan McCane
//----------------------------------------------------------------------

PolygonMesh::DPoint PolygonMesh::get_vertex(int index)
{
	DPoint empty;

	if (index>=vertex_list.size())
	{
		cerr << "Warning: vertex doesn't exist" << endl;
		return(empty);
	}
	return(vertex_list[index]);
}

//----------------------------------------------------------------------
//: get_face
//
// Returns a face at the specified location
//
// .param int index: the position to return
// .ret Polygon: a list of vertices
//
//.author Brendan McCane
//----------------------------------------------------------------------

PolygonMesh::Polygon PolygonMesh::get_face(int index)
{
	Polygon poly;
	if (index>=face_list.size())
	{
		cerr << "Warning: face doesn't exist" << endl;
		return(poly);
	}
	Face *face=&(face_list[index]);
	for (int i=0; i<face->size(); i++)
		poly.push_back(vertex_list[(*face)[i]]);
	return(poly);
}


//----------------------------------------------------------------------
//: read_file
//
// read a mesh in from a file
//
// .param char *filename: the name of the file
// .ret bool: whether or not the file was successfully read
//
//.author Brendan McCane
//----------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

bool PolygonMesh::read_file(char *filename)
{
	char start[100];
	FILE *fp=fopen(filename, "r");
	if (!fp) return(false);

	while (fscanf(fp, "%s", start)!=EOF)
	{
		if (!strcmp(start, "Vertex")) // read in a vertex
		{
			int index;
			DPoint pt;
			double x, y, z;
			fscanf(fp, "%d %lf %lf %lf", &index, &x, &y, &z);
			// add the vertex
			pt.set_x(x); pt.set_y(y), pt.set_z(z);
			//printf("Vertex: %d %f %f %f\n", index, x, y, z);
			set_vertex(index, pt);
		}
		else if (!strcmp(start, "Face")) // read in a face
		{
			int index;
			int vertex;
			Face fc;
			fscanf(fp, "%d", &index);
			//printf("Face: %d ", index);
			// read in all the vertex indices
			while ((fscanf(fp, "%d", &vertex)))
			{
				//printf("%d ", vertex);
				fc.push_back(vertex);
			}
			//printf("\n");
			set_face(index, fc);
		}
		else if (!strcmp(start, "End")) // end of mesh
			break;
		else // assume comment, read to eoln
		{
			char c;
			do {
				fscanf(fp, "%c", &c);
			} while (c!='\n');
		}
	}
	
	return(true);
}

//----------------------------------------------------------------------
//: get_face_normal
//
// return the normal vector for the given vertex of a given face.
//
// .param int face_index: which face 
//
// .param int vertex_index: which vertex in the face (ie not the
// vertex index in the entire list of vertices, but the vertex index
// within the face).
//
// .ret vnl_vector_fixed<double, 3>: the normalised normal vector
//
//.author Brendan McCane
//----------------------------------------------------------------------

PolygonMesh::DVector3D PolygonMesh::get_face_normal
(
	int face_index,
	int vertex_index
)
{
	assert(face_index<face_list.size());
	// in this version, I'm just returning the same vector for all
	// vertices. Therefore I need to find the cross-product of two
	// vectors lying on the plane of the polygon.
	Polygon face = get_face(face_index);
	DVector3D v1(face[0].x()-face[1].x(), face[0].y()-face[1].y(),
				 face[0].z()-face[1].z());
	DVector3D v2(face[0].x()-face[2].x(), face[0].y()-face[2].y(),
				 face[0].z()-face[2].z());
	DVector3D cross = cross_3d(v1, v2);
	cross.normalize();
	return(cross);
}
