// This is brl/bbas/bmsh3d/examples/bmsh3d_mesh2ply.cxx

//:
// \file
// \brief
// \date September 11, 2006

#include <vcl_iostream.h>
#include <vul/vul_timer.h>

#include <bmsh3d/algo/bmsh3d_fileio.h>
#include <bmsh3d/algo/bmsh3d_mesh_triangulate.h>

int main( int argc, char **argv )
{
  if (argc < 3)
  {
    vcl_cerr << "Invalid parameters. Program syntax:\n"
             << argv[0] << "input_meshfile output_mesh.ply\n";
    return 1;
  }

  // parse the arguments

  // mesh file to be transformed
  vcl_string input_mesh_file(argv[1]);
  vcl_string output_mesh_file(argv[2]);

  // i. Load the meshes
  bmsh3d_mesh mesh;

  vcl_cout << "i. Load input mesh file:\n";
  vul_timer timer;
  timer.mark();

  // load the meshes
  vcl_cout << "Loading mesh ...";
  bmsh3d_load(&mesh, input_mesh_file.c_str());

  vcl_cout << "done.\n";

  vcl_cout << "Total time taken =\n";
  timer.print(vcl_cout);

  vcl_cout << "Triangulating the mesh before saving...";
  bmsh3d_mesh* tri_mesh = generate_tri_mesh(&mesh);

  //Save the mesh to ply format
  vcl_cout << "Saving the mesh in PLY format ...\n";
  bmsh3d_save_ply(tri_mesh, output_mesh_file.c_str(), false);

  delete tri_mesh;
  vcl_cout << "Done.\n";
  return 0;
}
