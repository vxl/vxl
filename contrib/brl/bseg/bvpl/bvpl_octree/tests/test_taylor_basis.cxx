//:
// \file
// \author Isabel Restrepo
// \date 11-Feb-2011

#include <iostream>
#include <map>
#include <testlib/testlib_test.h>

#include "test_utils.h"

#include <bvpl/kernels/bvpl_taylor_basis_factory.h>
#include <bvpl/bvpl_octree/bvpl_taylor_basis.h>
#include <bvpl/bvpl_octree/sample/bvpl_taylor_basis_sample.h>

#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_sym_matrix.h>

void create_basis(std::string test_dir, std::map<std::string, float> &values)
{
  //generate basis paths
  if (!vul_file::is_directory(test_dir))
    vul_file::make_directory(test_dir);

  vul_file::change_directory(test_dir);

  bvpl_taylor_basis_loader loader(test_dir);

  std::vector<std::string> basis_names;
  loader.files(basis_names);

  //make the basis directories
  for (unsigned i= 0; i<basis_names.size(); i++)
  {
    std::string basis_dir = vul_file::get_cwd()+ "/" + basis_names[i];
    if (!vul_file::is_directory(basis_dir))
      vul_file::make_directory(basis_dir);

    vul_file::change_directory(basis_dir);

    create_scene(1,1,1, true, float(i), "float_response_scene");

    vul_file::change_directory("..");

    values.insert(std::pair<std::string, float>(basis_names[i], float(i)));
  }
}

void test_assemble_basis(bvpl_taylor_scenes_map_sptr taylor_scenes, std::map<std::string, float> &values)
{
  bvpl_taylor_basis::assemble_basis(taylor_scenes, 0, 0, 0);

  //retieve the basis seen and check vectors and matrices
  double I0 = values["I0"];

  vnl_vector<double> G(3);
  G[0] = values["Ix"];
  G[1] = values["Iy"];
  G[2] = values["Iz"];

  double H_as_vector[6];
  H_as_vector[0] = values["Ixx"];
  H_as_vector[1] = values["Ixy"];
  H_as_vector[2] = values["Iyy"];
  H_as_vector[3] = values["Ixz"];
  H_as_vector[4] = values["Iyz"];
  H_as_vector[5] = values["Izz"];
  vnl_sym_matrix<double> H(H_as_vector, 3);

  std::cout << "IO: " << I0 << "\nG: " << G <<"\nH: " << H.as_matrix() << std::endl;

  boxm_scene<boct_tree<short, bvpl_taylor_basis2_sample > >* basis_scene = dynamic_cast<boxm_scene<boct_tree<short, bvpl_taylor_basis2_sample > >* > (taylor_scenes->get_scene("basis").as_pointer());
  basis_scene->load_block(0, 0, 0);

  std::vector<boct_tree_cell<short, bvpl_taylor_basis2_sample >* > basis_leaves = basis_scene->get_block(0, 0, 0)->get_tree()->leaf_cells();

  std::vector<boct_tree_cell<short, bvpl_taylor_basis2_sample >* >::iterator it = basis_leaves.begin();
  bool result = true;
  for (; it!= basis_leaves.end(); it++)
  {
    bvpl_taylor_basis2_sample sample = (*it)->data();
    if (sample.I0!=I0){
      result = false;
      std::cout << sample << std::endl;
    }

    if (!((sample.G.as_vector()).is_equal(G, 1e-7))){
      result = false;
      std::cout << sample << std::endl;
    }

    if (!((sample.H.as_matrix()).is_equal(H.as_matrix(), 1e-7))){
      result = false;
      std::cout << sample << std::endl;
    }
  }

  TEST("Test assemble basis", result, true);
}


void test_taylor_basis()
{
  //generate basis
  std::string basis_dir = "./taylor_basis";
  std::map<std::string, float> values;
  create_basis(basis_dir, values);

  //load map of scenes into memory
  bvpl_taylor_basis_loader loader(vul_file::get_cwd());
  std::cout << vul_file::get_cwd() << std::endl;
  bvpl_taylor_scenes_map_sptr  taylor_scenes = new bvpl_taylor_scenes_map(loader);


  //test assembling
  test_assemble_basis(taylor_scenes, values);

  //test computing error
  boxm_scene<boct_tree<short, float> >* data_scene = create_scene(2,2,2, true, 1.0f, "data_scene");
  boxm_scene<boct_tree<short, float> >* error_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (taylor_scenes->get_scene("error").as_pointer());
  double cell_length = data_scene->finest_cell_length();
  bvpl_taylor_basis::compute_approximation_error(data_scene, taylor_scenes->get_scene("basis"),error_scene,loader,0,0,0, cell_length);

  error_scene->load_block(0,0,0);
  std::cout.precision(15);
  error_scene->get_block(0,0,0)->get_tree()->print();

  unsigned long tree_ncells = error_scene->get_block(0,0,0)->get_tree()->size();
  unsigned long scene_ncells = error_scene->size();
  unsigned long nsamples = scene_ncells*100L;

  //number of samples - 10% of total number of leaf cells
  unsigned long tree_nsamples = (unsigned long)(float(tree_ncells)/scene_ncells*nsamples);
  double error = bvpl_taylor_basis::sum_errors(error_scene,0,0,0, tree_nsamples);

  TEST_NEAR("Test average error", error, 131737.5, 1e-7);
}


TESTMAIN(test_taylor_basis);
