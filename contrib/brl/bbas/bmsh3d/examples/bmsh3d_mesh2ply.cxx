// This is contrib/knee_cartilage/cmd/pt_set_correspondence

//:
// \file
// \brief A program to compute morphology of knee cartilage
// \date September 11, 2006


#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_file_matrix.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_xform.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_triangulate.h>

//: 
int main( int argc, char **argv )
{
  if (argc < 3)
  {
    vcl_cerr << "Invalid parameters. Program syntax:\n";
    vcl_cerr << argv[0] << "input_meshfile output_mesh.ply\n";
    return 1;
  }

  // parse the arguments

  // mesh file to be transformed
  vcl_string input_mesh_file(argv[1]);
  vcl_string output_mesh_file(argv[2]);
  
  // i. Load the meshes
  dbmsh3d_mesh mesh;

  vcl_cout << "i. Load input mesh file: \n";
  vul_timer timer;
  timer.mark();

  // load the meshes
  vcl_cout << "Loading mesh ...";
  dbmsh3d_load(&mesh, input_mesh_file.c_str());

  vcl_cout << "done.\n";

  vcl_cout << "Total time taken = \n";
  timer.print(vcl_cout);

  

  vcl_cout << "Triangulating the mesh before saving...";
  dbmsh3d_mesh* tri_mesh = generate_tri_mesh(&mesh);

  //Save the mesh to ply format
  vcl_cout << "Saving the mesh in PLY format ... \n";
  dbmsh3d_save_ply(tri_mesh, output_mesh_file.c_str(), false);

  delete tri_mesh;
  vcl_cout << "Done.\n";
  return 0;
};


