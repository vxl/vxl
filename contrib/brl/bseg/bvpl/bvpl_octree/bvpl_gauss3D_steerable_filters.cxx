//:
// \file
// \author Isabel Restrepo
// \date 12-Aug-2011

#include "bvpl_gauss3D_steerable_filters.h"
#include <vgl/vgl_vector_3d.h>

//: Constructor
//  Initializes the order separable filter tabs should be applied for eac bases
bvpl_gauss3D_steerable_filters::bvpl_gauss3D_steerable_filters()
{
  axis_.clear();
  axis_.emplace_back(1,0,0);
  axis_.emplace_back(0,1,0);
  axis_.emplace_back(0,0,1);

  //Initialize from lists - waiting for VXL to support TR1!
  basis_.clear();
  basis_names_.clear();

  //zerth order - that is gaussian functio
  std::string G0a[3] = {"f01", "f02", "f02"};
  basis_.emplace_back(&G0a[0], &G0a[3]);
  basis_names_.emplace_back("G0a");

  //first order derivatives
  std::string G1a[3] = {"f11", "f12", "f12"};
  std::string G1b[3] = {"f12", "f11", "f12"};
  std::string G1c[3] = {"f12", "f12", "f11"};
  basis_.emplace_back(&G1a[0], &G1a[3]);
  basis_.emplace_back(&G1b[0], &G1b[3]);
  basis_.emplace_back(&G1c[0], &G1c[3]);
  basis_names_.emplace_back("G1a");
  basis_names_.emplace_back("G1b");
  basis_names_.emplace_back("G1c");

  //second order derivatives
  std::string G2a[3] = {"f21", "f22", "f22"};
  std::string G2b[3] = {"f23", "f24", "f22"};
  std::string G2c[3] = {"f22", "f21", "f22"};
  std::string G2d[3] = {"f23", "f22", "f24"};
  std::string G2e[3] = {"f22", "f23", "f24"};
  std::string G2f[3] = {"f22", "f22", "f21"};
  basis_.emplace_back(&G2a[0], &G2a[3]);
  basis_.emplace_back(&G2b[0], &G2b[3]);
  basis_.emplace_back(&G2c[0], &G2c[3]);
  basis_.emplace_back(&G2d[0], &G2d[3]);
  basis_.emplace_back(&G2e[0], &G2e[3]);
  basis_.emplace_back(&G2f[0], &G2f[3]);
  basis_names_.emplace_back("G2a");
  basis_names_.emplace_back("G2b");
  basis_names_.emplace_back("G2c");
  basis_names_.emplace_back("G2d");
  basis_names_.emplace_back("G2e");
  basis_names_.emplace_back("G2f");
}

//: Computes  the responses to separable filter taps.
//  There are 10 basis, corresponding to gaussians  zeroth, first and second derivatives
//  Each basis is made up of 3 1-d filter taps (x, y, z)
//  "scene" should be initialized with a dim-dimensional vector of the real input scene.
//  the taps have size 5, this could be changed later on
bool bvpl_gauss3D_steerable_filters::basis_response_at_leaves(boxm_scene<boct_tree<short, vnl_vector_fixed<float, bvpl_gauss3D_steerable_filters::DIM_> > > *scene,
                                                              boxm_scene<boct_tree<short, bool> > *valid_scene, double cell_length)
{
  this->assemble_basis_size_5();

  bvpl_algebraic_functor  functor;

  // each basis function, should have 3 1-d taps (one in x, one in y, one in z)
  unsigned max_ntaps = 3;

  std::cout << "bvpl_block_kernel_operator: Operating on cells of length: " << cell_length << std::endl;
  typedef boct_tree<short, vnl_vector_fixed<float,DIM_> > tree_type;
  typedef boct_tree_cell<short, vnl_vector_fixed<float,DIM_> > cell_type;

  // Load input and output blocks
  scene->clone_blocks_to_type(*valid_scene, true);

  //create and init a temporary scene to hold the intermediate 1-d output.
  boxm_scene<tree_type> *temp_scene =
  new boxm_scene<tree_type>(scene->lvcs(), scene->origin(), scene->block_dim(), scene->world_dim(), scene->max_level(), scene->init_level());
  temp_scene->set_paths(".", "temp_gsf_scene");
  temp_scene->set_appearance_model(VNL_FLOAT_10);
  temp_scene->write_scene("temp_gsf_scene.xml");
  scene->clone_blocks(*temp_scene);

  for (unsigned tap_i=0; tap_i<max_ntaps; tap_i++)
  {
    //Assemple tabs into a map
    std::vector<vnl_vector_fixed<float,5> > one_d_taps;

    for (unsigned dim=0; dim< DIM_; dim++){
      std::string tap_name = basis_[dim][tap_i];
      one_d_taps.push_back(separable_taps_[tap_name]);
    }

    //Traverse applying the appropriate 1-d taps
    boxm_cell_iterator<tree_type> cell_it = scene->cell_iterator(&boxm_scene<tree_type>::load_block_and_neighbors);
    cell_it.begin();

    boxm_cell_iterator<tree_type> temp_cell_it = temp_scene->cell_iterator(&boxm_scene<tree_type>::load_block);
    temp_cell_it.begin();

    boxm_cell_iterator<boct_tree<short, bool> > valid_cell_it = valid_scene->cell_iterator(&boxm_scene<boct_tree<short, bool> >::load_block);
    valid_cell_it.begin();

    while ( !(cell_it.end() || temp_cell_it.end() || valid_cell_it.end()) )
    {
      cell_type *temp_center_cell = *temp_cell_it;
      boct_tree_cell<short, bool> *valid_center_cell = *valid_cell_it;

      if (!valid_center_cell->data())
      {
        ++cell_it; ++temp_cell_it; ++valid_cell_it;
        continue;
      }

      bool valid = true;

      vgl_point_3d<double> center_cell_centroid = cell_it.global_centroid();

      vnl_vector_fixed<float, DIM_> responses(0.0);

      for (unsigned response_dim=0; response_dim<=DIM_; response_dim++)
      {
        vnl_vector_fixed<float,5> this_tap = one_d_taps[response_dim];

        for (int i =-2; i<=2; i++)
        {
          vgl_vector_3d<int> kernel_idx = i*axis_[tap_i];

          vgl_point_3d<double> kernel_cell_centroid(center_cell_centroid.x() + (double)kernel_idx.x()*cell_length,
                                                    center_cell_centroid.y() + (double)kernel_idx.y()*cell_length,
                                                    center_cell_centroid.z() + (double)kernel_idx.z()*cell_length);

          cell_type *this_cell = scene->locate_point_in_memory(kernel_cell_centroid);

          if (this_cell) {
            bvpl_kernel_dispatch d(this_tap[i+2]);
            float val =(float)(this_cell->data()[response_dim]);
            functor.apply(val, d);
          }
          else {
            valid = false;
            break;
          }
        }

        if (!valid)
          break;

        responses[response_dim] = (float)(functor.result());
      }

      temp_center_cell->set_data(responses);
      valid_center_cell->set_data(valid);
      ++cell_it; ++temp_cell_it; ++valid_cell_it;
    }

    //copy temp cells into cells
    cell_it.begin();
    temp_cell_it.begin();

    while ( !(cell_it.end() || temp_cell_it.end()) )
    {
      (*cell_it)->set_data((*temp_cell_it)->data());
      ++cell_it; ++temp_cell_it;
    }
 }

  //clear memory
  scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  temp_scene->unload_active_blocks();
  delete temp_scene;
  return true;
}


//: Compute basis response at all levels - assumes the intermediate cells of the tree have ben filled with meaningful info, otherwise the result could be garbage
bool bvpl_gauss3D_steerable_filters::multiscale_basis_response(boxm_scene<boct_tree<short, vnl_vector_fixed<float, bvpl_gauss3D_steerable_filters::DIM_> > > *scene,
                                                               boxm_scene<boct_tree<short, bool> > *valid_scene, unsigned resolution_level)
{
  this->assemble_basis_size_5();

  bvpl_algebraic_functor  functor;

  // each basis function, should have 3 1-d taps (one in x, one in y, one in z)
  unsigned max_ntaps = 3;

  typedef boct_tree<short, vnl_vector_fixed<float,DIM_> > tree_type;
  typedef boct_tree_cell<short, vnl_vector_fixed<float,DIM_> > cell_type;

  // Load input and output blocks
  scene->clone_blocks_to_type(*valid_scene, true);

  //create and init a temporary scene to hold the intermediate 1-d output.
  boxm_scene<tree_type> *temp_scene =
  new boxm_scene<tree_type>(scene->lvcs(), scene->origin(), scene->block_dim(), scene->world_dim(), scene->max_level(), scene->init_level());
  temp_scene->set_paths(".", "temp_gsf_scene");
  temp_scene->set_appearance_model(VNL_FLOAT_10);
  temp_scene->write_scene("temp_gsf_scene.xml");
  scene->clone_blocks(*temp_scene);

  for (unsigned tap_i=0; tap_i<max_ntaps; tap_i++)
  {
    //Assemple tabs into a map
    std::vector<vnl_vector_fixed<float,5> > one_d_taps;

    for (unsigned dim=0; dim< DIM_; dim++){
      std::string tap_name = basis_[dim][tap_i];
      one_d_taps.push_back(separable_taps_[tap_name]);
    }

    //Traverse applying the appropriate 1-d taps
    boxm_cell_iterator<tree_type> cell_it = scene->cell_iterator(&boxm_scene<tree_type>::load_block_and_neighbors);
    cell_it.begin(true);

    boxm_cell_iterator<tree_type> temp_cell_it = temp_scene->cell_iterator(&boxm_scene<tree_type>::load_block);
    temp_cell_it.begin(true);

    boxm_cell_iterator<boct_tree<short, bool> > valid_cell_it = valid_scene->cell_iterator(&boxm_scene<boct_tree<short, bool> >::load_block);
    valid_cell_it.begin(true);

    while ( !(cell_it.end() || temp_cell_it.end() || valid_cell_it.end()) )
    {
      cell_type *temp_center_cell = *temp_cell_it;
      boct_tree_cell<short, bool> *valid_center_cell = *valid_cell_it;

      if (!valid_center_cell->data())
      {
        ++cell_it; ++temp_cell_it; ++valid_cell_it;
        continue;
      }

      bool valid = true;

      vgl_point_3d<double> center_cell_centroid = cell_it.global_centroid();
      double cell_length = cell_it.length();

      vnl_vector_fixed<float, DIM_> responses(0.0);

      for (unsigned response_dim=0; response_dim<=DIM_; response_dim++)
      {
        vnl_vector_fixed<float,5> this_tap = one_d_taps[response_dim];

        for (int i =-2; i<=2; i++)
        {
          vgl_vector_3d<int> kernel_idx = i*axis_[tap_i];

          vgl_point_3d<double> kernel_cell_centroid(center_cell_centroid.x() + (double)kernel_idx.x()*cell_length,
                                                    center_cell_centroid.y() + (double)kernel_idx.y()*cell_length,
                                                    center_cell_centroid.z() + (double)kernel_idx.z()*cell_length);

          cell_type *this_cell = scene->locate_point_in_memory(kernel_cell_centroid, resolution_level);

          if (this_cell) {
            bvpl_kernel_dispatch d(this_tap[i+2]);
            float val =(float)(this_cell->data()[response_dim]);
            functor.apply(val, d);
          }
          else {
            valid = false;
            break;
          }
        }

        if (!valid)
          break;

        responses[response_dim] = (float)(functor.result());
      }

      temp_center_cell->set_data(responses);
      valid_center_cell->set_data(valid);
      ++cell_it; ++temp_cell_it; ++valid_cell_it;
    }

    //copy temp cells into cells
    cell_it.begin();
    temp_cell_it.begin();

    while ( !(cell_it.end() || temp_cell_it.end()) )
    {
      (*cell_it)->set_data((*temp_cell_it)->data());
      ++cell_it; ++temp_cell_it;
    }
  }

  //clear memory
  scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  temp_scene->unload_active_blocks();
  delete temp_scene;
  return true;
}

#if 0
//: Computes  the responses to separable filter taps.
//  There are 10 basis, corresponding to gaussians  zeroth, first and second derivatives
//  Each basis is made up of 3 1-d filter taps (x, y, z)
//  "scene" should be initialized with a dim-dimensional vector of the real input scene.
//  the taps have size 5, this could be changed later on
bool bvpl_gauss3D_steerable_filters::basis_response(boxm_scene<boct_tree<short, vnl_vector_fixed<float, bvpl_gauss3D_steerable_filters::DIM_> > > *scene,
                                                    int block_i, int block_j, int block_k,
                                                    boxm_scene<boct_tree<short, bool> > *valid_scene, double cell_length)
{
  this->assemble_basis_size_5();

  bvpl_algebraic_functor  functor;

  // each basis function, should have 3 1-d taps (one in x, one in y, one in z)
  unsigned max_ntaps = 3;

  std::cout << "bvpl_block_kernel_operator: Operating on cells of length: " << cell_length << std::endl;
  typedef boct_tree<short, vnl_vector_fixed<float,DIM_> > tree_type;
  typedef boct_tree_cell<short, vnl_vector_fixed<float,DIM_> > cell_type;

  // Load input and output blocks
  scene->load_block_and_neighbors(block_i,block_j,block_k);
  valid_scene->load_block(block_i,block_j,block_k);

  tree_type *tree = scene->get_block(block_i, block_j, block_k)->get_tree();
  tree_type *temp_tree = tree->clone(); //a temporary tree to store partial results
  boct_tree<short, bool> *valid_tree = tree->clone_to_type<bool>();
  valid_tree->init_cells(true);

  std::vector<cell_type* > cells = tree->leaf_cells();
  std::vector<cell_type* > temp_cells = temp_tree->leaf_cells();
  std::vector<boct_tree_cell<short, bool> * > valid_cells = valid_tree->leaf_cells();

  std::cout << scene->locate_point_in_memory(test_point)->data()<< std::endl;

  for (unsigned tap_i=0; tap_i<max_ntaps; tap_i++)
  {
    //Assemple tabs into a map
    std::vector<vnl_vector_fixed<float,5> > one_d_taps;

    for (unsigned dim=0; dim< DIM_; dim++){
      std::string tap_name = basis_[dim][tap_i];
      one_d_taps.push_back(separable_taps_[tap_name]);
    }

    //Traverse applying the appropriate 1-d taps
    std::vector<cell_type* >::iterator cells_it = cells.begin();
    std::vector<cell_type* >::iterator temp_it = temp_cells.begin();
    std::vector<boct_tree_cell<short, bool> * >::iterator valid_it = valid_cells.begin();
    for (; (cells_it!=cells.end())&&(temp_it!=temp_cells.end())&&(valid_it!=valid_cells.end()); cells_it++, valid_it++, temp_it++)
    {
      cell_type *center_cell = *cells_it;
      cell_type *temp_center_cell = *temp_it;
      boct_tree_cell<short, bool> *valid_center_cell = *valid_it;

      if (!valid_center_cell->data())
        continue;

      bool valid = true;

      vgl_point_3d<double> center_cell_centroid = tree->global_centroid(center_cell);

      vnl_vector_fixed<float, DIM_> responses(0.0);

      for (unsigned response_dim=0; response_dim<=DIM_; response_dim++)
      {
        vnl_vector_fixed<float,5> this_tap = one_d_taps[response_dim];

        for (int i =-2; i<=2; i++)
        {
          vgl_vector_3d<int> kernel_idx = i*axis_[tap_i];

          vgl_point_3d<double> kernel_cell_centroid(center_cell_centroid.x() + (double)kernel_idx.x()*cell_length,
                                                    center_cell_centroid.y() + (double)kernel_idx.y()*cell_length,
                                                    center_cell_centroid.z() + (double)kernel_idx.z()*cell_length);

          cell_type *this_cell = scene->locate_point_in_memory(kernel_cell_centroid);

          if (this_cell) {
            bvpl_kernel_dispatch d(this_tap[i+2]);
            float val =(float)(this_cell->data()[response_dim]);
            functor.apply(val, d);
          }
          else {
            valid = false;
            break;
          }
        }

        if (!valid)
          break;

        responses[response_dim] = (float)(functor.result());
      }

      temp_center_cell->set_data(responses);
      valid_center_cell->set_data(valid);
    }

    //copy temp cells into cells
    cells_it = cells.begin();
    temp_it = temp_cells.begin();
    for (; (cells_it!=cells.end())&&(temp_it!=temp_cells.end()); cells_it++, temp_it++)
      (*cells_it)->set_data((*temp_it)->data());

    std::cout << scene->locate_point_in_memory(test_point)->data()<< std::endl;;
  }

  //write the output block
  scene->get_block(block_i, block_j, block_k)->init_tree(tree);
  scene->write_active_block();
  valid_scene->get_block(block_i, block_j, block_k)->init_tree(valid_tree);
  valid_scene->write_active_block();

  //clear memory
  scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  delete temp_tree;
  return true;
}
#endif


bool bvpl_gauss3D_steerable_filters::rotation_invariant_interpolation(boxm_scene<boct_tree<short, vnl_vector_fixed<float, bvpl_gauss3D_steerable_filters::DIM_> > > *scene,
                                                                      boxm_scene<boct_tree<short, bool> > *valid_scene)
{
  typedef boct_tree<short, vnl_vector_fixed<float,DIM_> > tree_type;
  typedef boct_tree_cell<short, vnl_vector_fixed<float,DIM_> > cell_type;

  //Traverse all cells
  boxm_cell_iterator<tree_type> cell_it = scene->cell_iterator(&boxm_scene<tree_type>::load_block);
  cell_it.begin();
  boxm_cell_iterator<boct_tree<short, bool> > valid_cell_it = valid_scene->cell_iterator(&boxm_scene<boct_tree<short, bool> >::load_block, true);
  valid_cell_it.begin();

  while ( !(cell_it.end() || valid_cell_it.end()) )
  {
    cell_type *center_cell = *cell_it;
    boct_tree_cell<short, bool> *valid_center_cell = *valid_cell_it;

    if (!valid_center_cell->data()){
      ++cell_it; ++valid_cell_it;
      continue;
    }

    vnl_vector_fixed<float, DIM_> responses=center_cell->data();

    vgl_vector_3d<float> grad(responses[1], responses[2],responses[3]);
    normalize(grad); //after normalizing the gradient the direction cosines are the cartesian coordinates of grad

    //rotate the responses according to the gradient
    vnl_vector_fixed<float, DIM_> rotation_weigths(1.0f);
    rotation_weigths[0] = 1;
    rotation_weigths[1] = grad.x(); rotation_weigths[2] = grad.y();     rotation_weigths[3] = grad.z();
    rotation_weigths[4] = grad.x()* grad.x(); rotation_weigths[5] = 2.0f* grad.x()*grad.y();
    rotation_weigths[6] = grad.y()* grad.y(); rotation_weigths[7] = 2.0f* grad.x()*grad.z();
    rotation_weigths[8] = 2.0f * grad.y()* grad.z(); rotation_weigths[9] = grad.z()*grad.z();

    responses = element_product(responses, rotation_weigths);

    center_cell->set_data(responses);
    ++cell_it; ++valid_cell_it;
  }

  //clear memory
  scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  return true;
}


void bvpl_gauss3D_steerable_filters::assemble_basis_size_5()
{
  separable_taps_.clear();

  //Zeroth Derivative G0
  //Separable basis taps: f01, f02
  float f01_data[5] = {0.000402093f, 0.162216f, 1.19862f, 0.162216f, 0.000402093f};
  vnl_vector_fixed<float, 5> f01(f01_data);
  separable_taps_.insert(std::pair<std::string, vnl_vector_fixed<float, 5> >("f01", f01));

  float f02_data[5] = {0.000335463f, 0.135335f, 1.0f, 0.135335f, 0.000335463f};
  vnl_vector_fixed<float,5> f02(f02_data);
  separable_taps_.insert(std::pair<std::string, vnl_vector_fixed<float, 5> >("f02", f02));

  //First Derivative Basis: G1 = alpha*G1a + beta*G1b
  //Separable basis functions: f11, f12
  float f11_data[5] = {0.00227458f, 0.458816f, 0.0f, -0.458816f, -0.00227458f};
  vnl_vector_fixed<float,5> f11(f11_data);
  separable_taps_.insert(std::pair<std::string, vnl_vector_fixed<float, 5> >("f11", f11));
  float f12_data[5] = {0.000335463f, 0.135335f, 1.0f, 0.135335f, 0.000335463f};
  vnl_vector_fixed<float,5> f12(f12_data);
  separable_taps_.insert(std::pair<std::string, vnl_vector_fixed<float, 5> >("f12", f12));

  //Second derivative basis G2 = alpha^2*G2a + 2*alpha*beta*G2b + beta^2*G2c + 2*alpha*gamma*G2d + 2*beta*gamma*G2e + gamma^2*G2f
  //Separable basis taps = f21, f22, f23, f24
  float f21_data[5] = {0.00696448f, 0.561934f, -1.38405f, 0.561934f, 0.00696448f};
  vnl_vector_fixed<float,5> f21(f21_data);
  separable_taps_.insert(std::pair<std::string, vnl_vector_fixed<float, 5> >("f21", f21));

  float f22_data[5] = {0.000335463f, 0.135335f, 1.0f, 0.135335f, 0.000335463f};
  vnl_vector_fixed<float,5> f22(f22_data);
  separable_taps_.insert(std::pair<std::string, vnl_vector_fixed<float, 5> >("f22", f22));

  float f23_data[5] = {-0.00643349f, -1.29773f, 0.0f, 1.29773f, 0.00643349f};
  vnl_vector_fixed<float,5> f23(f23_data);
  separable_taps_.insert(std::pair<std::string, vnl_vector_fixed<float, 5> >("f23", f23));

  float f24_data[5] = {-0.000670925f, -0.135335f, 0.0f, 0.135335f, 0.000670925f};
  vnl_vector_fixed<float,5> f24(f24_data);
  separable_taps_.insert(std::pair<std::string, vnl_vector_fixed<float, 5> >("f24", f24));
}
