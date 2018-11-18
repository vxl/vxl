//:
// \file
// \author Isabel Restrepo
// \date 31-Jan-2011

#include "bvpl_taylor_basis.h"
#include <bvpl/bvpl_octree/sample/bvpl_taylor_basis_sample.h>

#include <boxm/boxm_scene.h>

bool bvpl_taylor_basis::compute_approximation_error(const boxm_scene_base_sptr& data_scene_base,
                                                    const boxm_scene_base_sptr& basis_scene_base,
                                                    const boxm_scene_base_sptr& error_scene_base,
                                                    const bvpl_taylor_basis_loader& loader,
                                                    int block_i, int block_j, int block_k, double cell_length)
{
  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;
  typedef boct_tree<short, bvpl_taylor_basis2_sample > taylor_tree_type;
  typedef boct_tree_cell<short, bvpl_taylor_basis2_sample > taylor_cell_type;
  //cast the scenes
  auto* data_scene = dynamic_cast<boxm_scene<float_tree_type>* > (data_scene_base.as_pointer());
  auto* basis_scene = dynamic_cast<boxm_scene<taylor_tree_type>* > (basis_scene_base.as_pointer());
  auto * error_scene = dynamic_cast<boxm_scene<float_tree_type>* > (error_scene_base.as_pointer());

  if (!(data_scene && basis_scene &&error_scene)){
    std::cerr << "Error in compute_approximation_error: Faild to cast scene\n";
    return false;
  }

  //load blocks of interest
  data_scene->load_block_and_neighbors(block_i, block_j, block_k);
  basis_scene->load_block(block_i, block_j, block_k);
  error_scene->load_block(block_i, block_j, block_k);

  //get the leaves
  float_tree_type* data_tree = data_scene->get_block(block_i, block_j, block_k)->get_tree();
  float_tree_type* error_tree = data_tree->clone();

  //error is always positive, therefore cells with negative errors can be identified as uninitialized.
  error_tree->init_cells(-1.0f);
  taylor_tree_type* basis_tree = basis_scene->get_block(block_i, block_j, block_k)->get_tree();

  std::vector<float_cell_type*> data_leaves = data_tree->leaf_cells();
  std::vector<float_cell_type*> error_leaves = error_tree->leaf_cells();
  std::vector<taylor_cell_type*> basis_leaves = basis_tree->leaf_cells();

  vgl_point_3d<int> min_point =  loader.min_point();
  vgl_point_3d<int> max_point =  loader.max_point();


  for (unsigned i =0; i<data_leaves.size(); i++) {
    //current cell is the center
    float_cell_type* data_cell = data_leaves[i];
    taylor_cell_type* basis_cell = basis_leaves[i];

    boct_loc_code<short> data_code = data_cell->get_code();
    boct_loc_code<short> basis_code = basis_cell->get_code();

    //check cells are at the same location
    if (! data_code.isequal(basis_code)){
      std::cerr << "Error in compute_approximation_error: Cells don't have the same location in the tree\n"
               << "Data Code: " << data_code << '\n'
               << "Basis Code: " << basis_code << '\n';

      return false;
    }

    //create a region around the center cell
    vgl_point_3d<double> centroid = data_tree->global_centroid(data_cell);

    //change the coordinates of enpoints to be in global coordinates abd text if they are contained in the scene
    vgl_point_3d<double> min_point_global(centroid.x() + (double)min_point.x()*cell_length, centroid.y() + (double)min_point.y()*cell_length, centroid.z() + (double)min_point.z()*cell_length);
    vgl_point_3d<double> max_point_global(centroid.x() + (double)max_point.x()*cell_length, centroid.y() + (double)max_point.y()*cell_length, centroid.z() + (double)max_point.z()*cell_length);
    if (!(data_scene->locate_point_in_memory(min_point_global) && data_scene->locate_point_in_memory(max_point_global)))
      continue;

    //retrieve basis
    double I0 = basis_cell->data().I0;
    vnl_double_3 G= basis_cell->data().G;
    vnl_double_3x3 H = basis_cell->data().H;
    double error = 0.0;
    for (int x = min_point.x(); x<= max_point.x(); x++) {
      for (int y = min_point.y(); y<=max_point.y(); y++) {
        for (int z = min_point.z(); z<=max_point.z(); z++) {
          vgl_point_3d<double> point2visit(centroid.x()+(double)x*cell_length, centroid.y()+ (double)y*cell_length, centroid.z() + (double)z*cell_length);
          boct_tree_cell<short,float> *this_cell = data_scene->locate_point_in_memory(point2visit);
          if (this_cell) {
            vnl_double_3 X((double)x,(double)y, (double)z);
            double approx = I0 + dot_product(X,G) + 0.5* (dot_product(X,(H*X)));
            error = error + (this_cell->data() - approx)*(this_cell->data() - approx);
            //std::cout << "Taylor Error :\n" << "This centroid: " << this_centroid << ", box_centroid: " <<box_centroid <<std::endl;
            //std::cout << "Taylor Error :\n" << "X: " << X << "\nI0: " << I0 <<"\nG: " << G << "\nH: " << H <<"\nApprox: " <<approx << "\nError: " << error << std::endl;
          }
        }
      }
    }

    error_leaves[i]->set_data((float)error);
  }

  // write and release memory
  error_scene->get_block(block_i, block_j, block_k)->set_tree(error_tree);
  error_scene->write_active_block();
  data_scene->unload_active_blocks();
  basis_scene->unload_active_blocks();

  return true;
}


double bvpl_taylor_basis::sum_errors(const boxm_scene_base_sptr& error_scene_base,
                                     int block_i, int block_j, int block_k, unsigned long tree_nsamples)
{
  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;
  auto * error_scene = dynamic_cast<boxm_scene<float_tree_type>* > (error_scene_base.as_pointer());

  error_scene->load_block(block_i, block_j, block_k);

  //get the leaves
  float_tree_type* error_tree = error_scene->get_block(block_i, block_j, block_k)->get_tree();
  std::vector<float_cell_type*> error_leaves = error_tree->leaf_cells();

  int tree_ncells = error_leaves.size();
  //unsigned long tree_nsamples = (unsigned long)((float)tree_ncells/scene_ncells*nsamples_);

  double error = 0.0;
  vnl_random rng(9667566ul);
  for (unsigned i=0; i<tree_nsamples; i++)
  {
    unsigned long sample = rng.lrand32(tree_ncells-1);

    boct_tree_cell<short, float> *center_cell = error_leaves[sample];
    // vgl_point_3d<double> center_cell_centroid = error_tree->global_centroid(center_cell);

    //if neighborhood is not inclusive we would have missing features
    if ((double)center_cell->data()< -0.5){
      i--;
      continue;
    }
    else {
      error+=(double)center_cell->data();
    }
  }

  return error/tree_nsamples;
}

//: Reads the values of basis responses and assembles them into matrices and vectors
bool bvpl_taylor_basis::assemble_basis(const bvpl_taylor_scenes_map_sptr& taylor_scenes,int block_i, int block_j, int block_k)
{
  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree<short, bvpl_taylor_basis2_sample > taylor_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;
  typedef boct_tree_cell<short, bvpl_taylor_basis2_sample > taylor_cell_type;

  std::cerr << "Caution: Using hardcoded size for taylor reconstruction\n";

  //load output scene
  boxm_scene<taylor_tree_type>* basis_scene = dynamic_cast<boxm_scene<taylor_tree_type>* > (taylor_scenes->get_scene("basis").as_pointer());
  if (!basis_scene)
  {
    std::cerr << "Error in reconstruct: Faild to cast output scene\n";
    return false;
  }

  //retieve basis scenes
  //zeroth derivative scene
  boxm_scene<float_tree_type>* I0_scene = dynamic_cast<boxm_scene<float_tree_type>* > (taylor_scenes->get_scene("I0").as_pointer());
  boxm_scene<float_tree_type>* Ix_scene = dynamic_cast<boxm_scene<float_tree_type>* > (taylor_scenes->get_scene("Ix").as_pointer());
  boxm_scene<float_tree_type>* Iy_scene = dynamic_cast<boxm_scene<float_tree_type>* > (taylor_scenes->get_scene("Iy").as_pointer());
  boxm_scene<float_tree_type>* Iz_scene = dynamic_cast<boxm_scene<float_tree_type>* > (taylor_scenes->get_scene("Iz").as_pointer());
  boxm_scene<float_tree_type>* Ixx_scene = dynamic_cast<boxm_scene<float_tree_type>* > (taylor_scenes->get_scene("Ixx").as_pointer());
  boxm_scene<float_tree_type>* Ixy_scene = dynamic_cast<boxm_scene<float_tree_type>* > (taylor_scenes->get_scene("Ixy").as_pointer());
  boxm_scene<float_tree_type>* Ixz_scene = dynamic_cast<boxm_scene<float_tree_type>* > (taylor_scenes->get_scene("Ixz").as_pointer());
  boxm_scene<float_tree_type>* Iyy_scene = dynamic_cast<boxm_scene<float_tree_type>* > (taylor_scenes->get_scene("Iyy").as_pointer());
  boxm_scene<float_tree_type>* Iyz_scene = dynamic_cast<boxm_scene<float_tree_type>* > (taylor_scenes->get_scene("Iyz").as_pointer());
  boxm_scene<float_tree_type>* Izz_scene = dynamic_cast<boxm_scene<float_tree_type>* > (taylor_scenes->get_scene("Izz").as_pointer());


  if (!(I0_scene && Ix_scene && Iy_scene && Iz_scene && Ixx_scene && Iyy_scene && Izz_scene && Ixy_scene && Ixz_scene && Iyz_scene))
  {
    std::cerr << "Error in assemble_basis: Faild to load taylor scenes\n";
    return false;
  }

  //load the blocks
  basis_scene->load_block(block_i, block_j,block_k);
  I0_scene->load_block(block_i, block_j,block_k);
  Ix_scene->load_block(block_i, block_j,block_k);
  Iy_scene->load_block(block_i, block_j,block_k);
  Iz_scene->load_block(block_i, block_j,block_k);
  Ixx_scene->load_block(block_i, block_j,block_k);
  Ixy_scene->load_block(block_i, block_j,block_k);
  Ixz_scene->load_block(block_i, block_j,block_k);
  Iyy_scene->load_block(block_i, block_j,block_k);
  Iyz_scene->load_block(block_i, block_j,block_k);
  Izz_scene->load_block(block_i, block_j,block_k);


  float_tree_type *treeI0 = I0_scene->get_block(block_i, block_j, block_k)->get_tree();
  taylor_tree_type *basis_tree = treeI0->clone_to_type<bvpl_taylor_basis2_sample>();
  basis_tree->init_cells(bvpl_taylor_basis2_sample());
  std::vector<taylor_cell_type*> leaves_basis = basis_tree->leaf_cells();
  std::vector<float_cell_type* > leaves_I0 = treeI0->leaf_cells();
  std::vector<float_cell_type* > leaves_Ix = Ix_scene->get_block(block_i, block_j, block_k)->get_tree()->leaf_cells();
  std::vector<float_cell_type* > leaves_Iy = Iy_scene->get_block(block_i, block_j, block_k)->get_tree()->leaf_cells();
  std::vector<float_cell_type* > leaves_Iz = Iz_scene->get_block(block_i, block_j, block_k)->get_tree()->leaf_cells();
  std::vector<float_cell_type* > leaves_Ixx =Ixx_scene->get_block(block_i, block_j, block_k)->get_tree()->leaf_cells();
  std::vector<float_cell_type* > leaves_Ixy =Ixy_scene->get_block(block_i, block_j, block_k)->get_tree()->leaf_cells();
  std::vector<float_cell_type* > leaves_Ixz =Ixz_scene->get_block(block_i, block_j, block_k)->get_tree()->leaf_cells();
  std::vector<float_cell_type* > leaves_Iyy =Iyy_scene->get_block(block_i, block_j, block_k)->get_tree()->leaf_cells();
  std::vector<float_cell_type* > leaves_Iyz =Iyz_scene->get_block(block_i, block_j, block_k)->get_tree()->leaf_cells();
  std::vector<float_cell_type* > leaves_Izz =Izz_scene->get_block(block_i, block_j, block_k)->get_tree()->leaf_cells();


  //iterate gathering and assembling the data
  for (unsigned i=0; i<leaves_basis.size(); i++)
  {
    auto I0 = (double)leaves_I0[i]->data();
    vnl_double_3 G((double)leaves_Ix[i]->data(),(double)leaves_Iy[i]->data(),(double)leaves_Iz[i]->data());

    vnl_double_3x3 H;
    H.put(0,0,(double)leaves_Ixx[i]->data());
    H.put(0,1,(double)leaves_Ixy[i]->data());
    H.put(0,2,(double)leaves_Ixz[i]->data());
    H.put(1,0,(double)leaves_Ixy[i]->data());
    H.put(1,1,(double)leaves_Iyy[i]->data());
    H.put(1,2,(double)leaves_Iyz[i]->data());
    H.put(2,0,(double)leaves_Ixz[i]->data());
    H.put(2,1,(double)leaves_Iyz[i]->data());
    H.put(2,2,(double)leaves_Izz[i]->data());

    bvpl_taylor_basis2_sample sample(I0, G, H);

    leaves_basis[i]->set_data(sample);
  }

  //write basis block
  basis_scene->get_block(block_i, block_j, block_k)->set_tree(basis_tree);
  basis_scene->write_active_block();

  //free up memory
  basis_scene->unload_active_blocks();
  I0_scene->unload_active_blocks();
  Ix_scene->unload_active_blocks();
  Iy_scene->unload_active_blocks();
  Iz_scene->unload_active_blocks();
  Ixx_scene->unload_active_blocks();
  Ixy_scene->unload_active_blocks();
  Ixz_scene->unload_active_blocks();
  Iyy_scene->unload_active_blocks();
  Iyz_scene->unload_active_blocks();
  Izz_scene->unload_active_blocks();

  return true;
}
