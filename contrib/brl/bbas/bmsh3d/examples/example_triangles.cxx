// This is brl/bbas/bmsh3d/examples/example_triangles.cxx

//:
// \file
//
//  This example program demonstrates how to use the bmsh3d mesh library
//  to create a mesh of two triangular faces and save to a file.
//  \author Ming-Ching Chang
//  \date Oct 03, 2006.

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <bmsh3d/bmsh3d_mesh.h>
#include <bmsh3d/algo/bmsh3d_fileio.h>

int main()
{
  std::cout << "This example program demonstrates how to use the bmsh3d mesh library\n"
           << "  to create a mesh of two triangular faces as follows:\n"
           << '\n'
           << "   v2\n"
           << "   o --------o v3          v0: (0,0,0)\n"
           << "   | \\      /              v1: (1,0,0)\n"
           << "   |  \\    /               v2: (0,2,0)\n"
           << "   |   \\  /                v3: (2,2,1)\n"
           << "   o----o                  f0: (v0,v1,v2)\n"
           << "   v0    v1                f1: (v1,v2,v3)\n"
           << '\n'
           << "  In order to maintain topological correctness, vertices and edges\n"
           << "  should be created in prior to the faces.\n"
           << "  In this example, we first create 3 vertices v0, v1, v2,\n"
           << "  and 3 edges e01, e12, e20, and finally add the face f0 to the mesh->\n"
           << "  The second step is to create the vertex v3 and two edges e23 and e13,\n"
           << "  and then add the face f1 to the mesh->\n"
           << "  The result is saved into a PLY2 file example_triangles.ply2." << std::endl;

  auto* mesh = new bmsh3d_mesh ();

  bmsh3d_vertex* v0 = mesh->_new_vertex ();
  v0->set_pt (vgl_point_3d<double> (0, 0, 0));
  mesh->_add_vertex (v0);

  bmsh3d_vertex* v1 = mesh->_new_vertex ();
  v1->set_pt (vgl_point_3d<double> (1, 0, 0));
  mesh->_add_vertex (v1);

  bmsh3d_vertex* v2 = mesh->_new_vertex ();
  v2->set_pt (vgl_point_3d<double> (0, 2, 0));
  mesh->_add_vertex (v2);

  bmsh3d_edge* e01 = mesh->add_new_edge (v0, v1);
  bmsh3d_edge* e12 = mesh->add_new_edge (v1, v2);
  bmsh3d_edge* e20 = mesh->add_new_edge (v2, v0);

  bmsh3d_face* f0 = mesh->_new_face ();
  mesh->_add_face (f0);

  _connect_F_E_end (f0, e01);
  _connect_F_E_end (f0, e12);
  _connect_F_E_end (f0, e20);

  //Now the first face f0 is complete. Continue to create the next face f1
  bmsh3d_vertex* v3 = mesh->_new_vertex ();
  v3->set_pt (vgl_point_3d<double> (2, 2, 1));
  mesh->_add_vertex (v3);

  bmsh3d_edge* e13 = mesh->add_new_edge (v1, v3);
  bmsh3d_edge* e23 = mesh->add_new_edge (v2, v3);

  bmsh3d_face* f1 = mesh->_new_face ();
  mesh->_add_face (f1);

  _connect_F_E_end (f1, e12);
  _connect_F_E_end (f1, e23);
  _connect_F_E_end (f1, e13);

  //We have create the mesh using halfedges which is topologically correct.
  //Now setup the indexed-face-set (IFS) of it to enable visualization and file I/O.
  mesh->build_IFS_mesh ();

  bmsh3d_save_ply2 (mesh, "example_triangles.ply2");

  delete mesh;
}
