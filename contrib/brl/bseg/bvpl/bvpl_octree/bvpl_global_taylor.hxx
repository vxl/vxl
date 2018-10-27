#ifndef bvpl_global_taylor_hxx_
#define bvpl_global_taylor_hxx_
//:
// \file
#include "bvpl_global_taylor.h"
// \author Isabel Restrepo
// \date 11-Apr-2011

#include <boxm/boxm_scene.h>
#include <boxm/boxm_scene_parser.h>

#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>

#include <bvpl/bvpl_octree/bvpl_block_vector_operator.h>
#include <bvpl/functors/bvpl_algebraic_functor.h>
#include <bvpl/kernels/bvpl_taylor_basis_factory.h>
#include <bvpl_octree/bvpl_corner_detector.h>
#include <bvpl_octree/bvpl_corner_functors.h>

#include <vul/vul_file.h>

#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>

//: Constructor  from xml file
template<class T_data, unsigned DIM>
bvpl_global_taylor<T_data, DIM>::bvpl_global_taylor(const std::string &path, const std::string kernel_names[])
{
  std::cout << "Loading taylor info from xml-file" << std::endl;
  path_out_ = path;
  std::ifstream xml_ifs(xml_path().c_str());
  if (!xml_ifs.is_open()) {
    std::cerr << "Error: could not open xml info file: " << xml_path() << '\n';
    throw;
  }
  bxml_document doc = bxml_read(xml_ifs);
  bxml_element query("taylor_global_info");
  bxml_data_sptr root = bxml_find_by_name(doc.root_element(), query);
  if (!root) {
    std::cerr << "Error: bvpl_discover_pca_kernels - could not parse xml root\n";
    throw;
  }

  //Parse scenes
  bxml_element scenes_query("scene");
  std::vector<bxml_data_sptr> scenes_data = bxml_find_all_with_name(root, scenes_query);

  unsigned nscenes=scenes_data.size();
  std::cout << "Number of scenes: " << nscenes <<std::endl;

  scenes_.clear();
  scenes_.resize(nscenes);

  aux_dirs_.clear();
  aux_dirs_.resize(nscenes);

  finest_cell_length_.clear();
  finest_cell_length_.resize(nscenes);

  for (unsigned si = 0; si < nscenes; si++)
  {
    bxml_element* scenes_elm = dynamic_cast<bxml_element*>(scenes_data[si].ptr());
    int id = -1;
    scenes_elm->get_attribute("id", id);
    scenes_elm->get_attribute("path", scenes_[id]);
    scenes_elm->get_attribute("aux_dir", aux_dirs_[id]);
    scenes_elm->get_attribute("cell_length" , finest_cell_length_[id]);

    std::cout << "Scene " << id << " is " << scenes_[id] << '\n';
  }

  //Parse kernels
  bxml_element kernels_query("kernels");
  bxml_data_sptr kernels_data = bxml_find_by_name(root, kernels_query);
  if (kernels_data)
  {
    bxml_element* kernels_elm = dynamic_cast<bxml_element*>(kernels_data.ptr());

    kernels_elm->get_attribute("path", kernels_path_);

    std::cout << "Kernels path is " << kernels_path_ << '\n';
    kernel_vector_ = new bvpl_kernel_vector;

    if (vul_file::exists(kernels_path_) && vul_file::is_directory(kernels_path_)) {
      //read the kernels

      kernel_vector_->kernels_.clear();

      kernel_vector_->kernels_.resize(DIM, nullptr);
      //std::string kernel_names[]={"I0", "Ix", "Iy", "Iz", "Ixx", "Iyy", "Izz", "Ixy", "Ixz", "Iyz"};
      for (unsigned ki = 0; ki<DIM; ki++) {
        std::string filename = kernels_path_ + "/" + kernel_names[ki] + ".txt";
        std::cout << "Reading kernel file : " << filename << std::endl;
        bvpl_taylor_basis_factory factory(filename);
        bvpl_kernel_sptr kernel = new bvpl_kernel(factory.create());
        kernel_vector_->kernels_[ki] = kernel;
      }
    }
  }
  else {
    std::cerr << " In Gobal Taylor, path to kernels does not exist\n";
  }
}

//: Compute the DIM taylor kernels for this scene at current block. The output is saved to the projection scene as a DIM-d vector
template<class T_data, unsigned DIM>
void bvpl_global_taylor<T_data, DIM>::compute_taylor_coefficients(int scene_id, int block_i, int block_j, int block_k)
{
  typedef boct_tree_cell<short,float> float_cell_type;

  typedef boct_tree<short,vnl_vector_fixed<T_data,DIM> > taylor_tree_type;

  boxm_scene_base_sptr data_scene_base =load_scene(scene_id);
  boxm_scene_base_sptr proj_scene_base =load_projection_scene(scene_id);
  boxm_scene_base_sptr valid_scene_base = load_valid_scene(scene_id);

  boxm_scene<boct_tree<short, float> >* data_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*>(data_scene_base.as_pointer());
  boxm_scene<taylor_tree_type>* proj_scene = dynamic_cast<boxm_scene<taylor_tree_type>*>(proj_scene_base.as_pointer());
  boxm_scene<boct_tree<short, bool> >* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());
  if (!(data_scene && proj_scene && valid_scene))
  {
    std::cerr << "Error in bvpl_global_taylor::sample_statistics: Could not cast input scenes\n";
    return;
  }

  //init variables
  data_scene->unload_active_blocks();
  proj_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();

  //operate on scene
  bvpl_block_vector_operator block_oper;
  bvpl_algebraic_functor functor;
  block_oper.operate(*data_scene, functor, kernel_vector_, block_i, block_j, block_k, *proj_scene, *valid_scene, finest_cell_length_[scene_id]);

  //clean memory
  data_scene->unload_active_blocks();
  proj_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
}

template<class T_data, unsigned DIM>
bool bvpl_global_taylor<T_data, DIM>::compute_approximation_error(int scene_id, int block_i, int block_j, int block_k)
{
  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;
  typedef boct_tree<short,vnl_vector_fixed<double,10> > taylor_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > taylor_cell_type;

  boxm_scene_base_sptr proj_scene_base = load_projection_scene(scene_id);
  boxm_scene_base_sptr error_scene_base = load_error_scene(scene_id);
  boxm_scene_base_sptr data_scene_base =load_scene(scene_id);

  //cast the scenes
  boxm_scene<float_tree_type>* data_scene = dynamic_cast<boxm_scene<float_tree_type>* > (data_scene_base.as_pointer());
  boxm_scene<taylor_tree_type>* basis_scene = dynamic_cast<boxm_scene<taylor_tree_type>* > (proj_scene_base.as_pointer());
  boxm_scene<float_tree_type> * error_scene = dynamic_cast<boxm_scene<float_tree_type>* > (error_scene_base.as_pointer());

  if (!data_scene || !basis_scene || !error_scene) {
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

  vgl_point_3d<int> min_point = kernel_vector_->min();
  vgl_point_3d<int> max_point = kernel_vector_->max();
  double cell_length = this->finest_cell_length_[scene_id];
  std::cout << "In computing taylor error, limits are: " <<min_point << " and " <<max_point <<std::endl;

  for (unsigned i =0; i<data_leaves.size(); i++)
  {
    //current cell is the center
    float_cell_type* data_cell = data_leaves[i];
    taylor_cell_type* basis_cell = basis_leaves[i];

    boct_loc_code<short> data_code = data_cell->get_code();
    boct_loc_code<short> basis_code = basis_cell->get_code();

    //check cells are at the same location
    if (! data_code.isequal(basis_code)) {
      std::cerr << "Error in compute_approximation_error: Cells don't have the same location in the tree\n"
               << "Data Code: " << data_code << '\n'
               << "Basis Code: " << basis_code << '\n';

      return false;
    }

    //create a region around the center cell
    vgl_point_3d<double> centroid = data_tree->global_centroid(data_cell);

    //change the coordinates of enpoints to be in global coordinates abd text if they are contained in the scene
    vgl_point_3d<double> min_point_global(centroid.x() + (double)min_point.x()*cell_length,
                                          centroid.y() + (double)min_point.y()*cell_length,
                                          centroid.z() + (double)min_point.z()*cell_length);
    if (! data_scene->locate_point_in_memory(min_point_global))
      continue;
    vgl_point_3d<double> max_point_global(centroid.x() + (double)max_point.x()*cell_length,
                                          centroid.y() + (double)max_point.y()*cell_length,
                                          centroid.z() + (double)max_point.z()*cell_length);
    if (! data_scene->locate_point_in_memory(max_point_global))
      continue;

    //assemble basis
//    double I0 = basis_cell->data().I0;
//    vnl_double_3 G= basis_cell->data().G;
//    vnl_double_3x3 H = basis_cell->data().H;
    vnl_vector_fixed<double,10> taylor_coeff = basis_cell->data();

    //form basis
    double I0 = taylor_coeff[0];
    vnl_double_3 G(taylor_coeff[1], taylor_coeff[2], taylor_coeff[3]);
    vnl_double_3x3 H;
    H.put(0,0,taylor_coeff[4]);
    H.put(0,1,taylor_coeff[7]);
    H.put(0,2,taylor_coeff[8]);
    H.put(1,0,taylor_coeff[7]);
    H.put(1,1,taylor_coeff[5]);
    H.put(1,2,taylor_coeff[9]);
    H.put(2,0,taylor_coeff[8]);
    H.put(2,1,taylor_coeff[9]);
    H.put(2,2,taylor_coeff[6]);

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


//: Threshold non-salient features according to Harris' measure
template<class T_data, unsigned DIM>
void bvpl_global_taylor<T_data, DIM>::threshold_corners(int scene_id, int block_i, int block_j, int block_k, double harris_k)
{
  typedef boct_tree<short,vnl_vector_fixed<double,10> > taylor_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > taylor_cell_type;

  boxm_scene_base_sptr data_scene_base =load_scene(scene_id);
  boxm_scene_base_sptr proj_scene_base =load_projection_scene(scene_id);
  boxm_scene_base_sptr valid_scene_base = load_valid_scene(scene_id);

  boxm_scene<taylor_tree_type>* proj_scene = dynamic_cast<boxm_scene<taylor_tree_type>*>(proj_scene_base.as_pointer());
  boxm_scene<boct_tree<short, bool> >* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());
  boxm_scene<boct_tree<short, float> >* corner_scene =
  new boxm_scene<boct_tree<short, float> >(valid_scene->lvcs(), valid_scene->origin(), valid_scene->block_dim(), valid_scene->world_dim(), valid_scene->max_level(), valid_scene->init_level());
  corner_scene->set_appearance_model(BOXM_FLOAT);
  corner_scene->set_paths(valid_scene->path(), "harris_scene");
  corner_scene->write_scene("/harris_scene.xml");

  if (!( proj_scene && valid_scene && corner_scene ))
  {
    std::cerr << "Error in bvpl_global_taylor::threshold_corners: Could not cast input scenes\n";
    return;
  }

  //init variables
  proj_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();

  //operate on scene
  bvpl_corner_detector corner_detector;
  bvpl_harris_laptev_functor harris_functor(harris_k);

  vgl_point_3d<int> max_neigborhood_idx = kernel_vector_->max();
  vgl_point_3d<int> min_neigborhood_idx = kernel_vector_->min();

  std::cout << "Neighborhood for harris threshhold: " << min_neigborhood_idx << " , " << max_neigborhood_idx << std::endl;

  corner_detector.detect_and_threshold(proj_scene, harris_functor, min_neigborhood_idx, max_neigborhood_idx,
                                       block_i, block_j, block_k, valid_scene, corner_scene, finest_cell_length_[scene_id]);

  //clean memory
  proj_scene->unload_active_blocks();
  valid_scene->unload_active_blocks();
  corner_scene->unload_active_blocks();
}

//: Init auxiliary scenes and smallest cell length values
template<class T_data, unsigned DIM>
void bvpl_global_taylor<T_data, DIM>::init()
{
  for (unsigned i = 0; i < aux_dirs_.size(); i++)
  {
    boxm_scene_base_sptr data_scene_base = load_scene(i);
    boxm_scene<boct_tree<short, float> >* data_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (data_scene_base.as_pointer());
    if (!data_scene) {
      std::cerr << "Error in bvpl_global_taylor::init(): Could not cast data scene\n";
      return;
    }
    double finest_cell_length = data_scene->finest_cell_length();
    finest_cell_length_[i] = finest_cell_length;

    if (!(vul_file::exists(aux_dirs_[i]) && vul_file::is_directory(aux_dirs_[i]))) {
      vul_file::make_directory(aux_dirs_[i]);
    }

    {
      std::stringstream aux_scene_ss;
      aux_scene_ss << "valid_scene_" << i ;
      std::string aux_scene_path = aux_dirs_[i] + "/" + aux_scene_ss.str() + ".xml";
      if (!vul_file::exists(aux_scene_path)) {
        std::cout<< "Scene: " << aux_scene_path << " does not exist, initializing" << std::endl;
        boxm_scene<boct_tree<short, bool> > *aux_scene =
        new boxm_scene<boct_tree<short, bool> >(data_scene->lvcs(), data_scene->origin(), data_scene->block_dim(), data_scene->world_dim(), data_scene->max_level(), data_scene->init_level());
        aux_scene->set_appearance_model(BOXM_BOOL);
        aux_scene->set_paths(aux_dirs_[i], aux_scene_ss.str());
        aux_scene->write_scene("/" + aux_scene_ss.str() +  ".xml");
      }
    }

    {
      std::stringstream proj_scene_ss;
      proj_scene_ss << "proj_taylor_scene_" << i ;
      std::string proj_scene_path = aux_dirs_[i] + "/" + proj_scene_ss.str() + ".xml";
      if (!vul_file::exists(proj_scene_path)) {
        std::cout<< "Scene: " << proj_scene_path << " does not exist, initializing" << std::endl;
        typedef boct_tree<short,vnl_vector_fixed<T_data,DIM> > taylor_tree_type;
        boxm_scene<taylor_tree_type > *proj_scene =
        new boxm_scene<taylor_tree_type >(data_scene->lvcs(), data_scene->origin(), data_scene->block_dim(), data_scene->world_dim(), data_scene->max_level(), data_scene->init_level());
        proj_scene->set_appearance_model(bvpl_taylor_apm<T_data,DIM>());
        proj_scene->set_paths(aux_dirs_[i], proj_scene_ss.str());
        proj_scene->write_scene("/" + proj_scene_ss.str() +  ".xml");
      }
    }
  }

  xml_write();
}


//: Load scene info
template<class T_data, unsigned DIM>
boxm_scene_base_sptr bvpl_global_taylor<T_data, DIM>::load_scene (int scene_id)
{
  if (scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    std::cerr << "Error in bvpl_global_pca::load_scene: Invalid scene id\n";
    return nullptr;
  }
  //load scene
  boxm_scene_base_sptr scene_base = new boxm_scene_base();
  boxm_scene_parser scene_parser;
  scene_base->load_scene(scenes_[scene_id], scene_parser);

  //cast scene
  boxm_scene<boct_tree<short, float > > *scene= new boxm_scene<boct_tree<short, float > >();
  if (scene_base->appearence_model() == BOXM_FLOAT) {
    scene->load_scene(scene_parser);
    scene_base = scene;
  }
  else {
    std::cerr << "Error in bvpl_global_pca::load_scene: Invalid appearance model\n";
    return nullptr;
  }

  return scene_base;
}

//: Load auxiliary scene indicating if a cell is valid. e.g border cells are not valid
template<class T_data, unsigned DIM>
boxm_scene_base_sptr bvpl_global_taylor<T_data, DIM>::load_valid_scene (int scene_id)
{
  if (scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    std::cerr << "Error in bvpl_global_taylor::load_scene: Invalid scene id\n";
    return nullptr;
  }
  //load scene
  boxm_scene_base_sptr aux_scene_base = new boxm_scene_base();
  boxm_scene_parser aux_parser;
  std::stringstream aux_scene_ss;
  aux_scene_ss << aux_dirs_[scene_id] << "/valid_scene_" << scene_id << ".xml";
  aux_scene_base->load_scene(aux_scene_ss.str(), aux_parser);

  //cast scene
  boxm_scene<boct_tree<short, bool > > *aux_scene= new boxm_scene<boct_tree<short, bool > >();
  if (aux_scene_base->appearence_model() == BOXM_BOOL) {
    aux_scene->load_scene(aux_parser);
    aux_scene_base = aux_scene;
  }
  else {
    std::cerr << "Error in bvpl_global_taylor::load_aux_scene: Invalid appearance model\n";
    return nullptr;
  }

  return aux_scene_base;
}


//: Load auxiliary scene info
template<class T_data, unsigned DIM>
boxm_scene_base_sptr bvpl_global_taylor<T_data, DIM>::load_error_scene (int scene_id)
{
  if (scene_id<0 || scene_id>=(int)scenes_.size())
  {
    std::cerr << "Error in bvpl_global_taylor::load_error_scene: Invalid scene id\n";
    return nullptr;
  }

  boxm_scene_base_sptr data_scene_base = load_scene(scene_id);
  boxm_scene<boct_tree<short, float> >* data_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (data_scene_base.as_pointer());
  if (!data_scene) {
    std::cerr << "Error in bvpl_global_pca<feature_dim>::init(): Could not cast data scene\n";
    return nullptr;
  }

  std::stringstream aux_scene_ss;
  aux_scene_ss << "error_taylor_scene_" << scene_id ;
  std::string aux_scene_path = aux_dirs_[scene_id] + "/" + aux_scene_ss.str() + ".xml";
  if (!vul_file::exists(aux_scene_path)) {
    std::cout<< "Scene: " << aux_scene_path << " does not exist, initializing" << std::endl;
    boxm_scene<boct_tree<short, float> > *aux_scene =
    new boxm_scene<boct_tree<short, float> >(data_scene->lvcs(), data_scene->origin(), data_scene->block_dim(), data_scene->world_dim(), data_scene->max_level(), data_scene->init_level());
    aux_scene->set_appearance_model(BOXM_FLOAT);
    aux_scene->set_paths(aux_dirs_[scene_id], aux_scene_ss.str());
    aux_scene->write_scene("/" + aux_scene_ss.str() +  ".xml");
  }

  //load scene
  boxm_scene_base_sptr error_scene_base = new boxm_scene_base();
  boxm_scene_parser error_parser;
  std::stringstream error_scene_ss;
  error_scene_ss << aux_dirs_[scene_id] << "/error_taylor_scene_" << scene_id << ".xml";
  error_scene_base->load_scene(error_scene_ss.str(), error_parser);

  //cast scene
  boxm_scene<boct_tree<short, float> > *error_scene= new boxm_scene<boct_tree<short, float> >();
  if (error_scene_base->appearence_model() == BOXM_FLOAT) {
    error_scene->load_scene(error_parser);
    error_scene_base = error_scene;
  }
  else {
    std::cerr << "Error in bvpl_global_taylor::load_error_scene: Invalid appearance model\n";
    return nullptr;
  }

  return error_scene_base;
}


#if 0
//: Load auxiliary scene indicating if a cell is should be used for training
boxm_scene_base_sptr bvpl_global_taylor::load_train_scene (int scene_id)
{
  if (scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    std::cerr << "Error in bvpl_global_taylor::load_scene: Invalid scene id\n";
    return NULL;
  }
  //load scene
  boxm_scene_base_sptr aux_scene_base = new boxm_scene_base();
  boxm_scene_parser aux_parser;
  std::stringstream aux_scene_ss;
  aux_scene_ss << aux_dirs_[scene_id] << "/train_scene_" << scene_id << ".xml";
  aux_scene_base->load_scene(aux_scene_ss.str(), aux_parser);

  //cast scene
  boxm_scene<boct_tree<short, bool > > *aux_scene= new boxm_scene<boct_tree<short, bool > >();
  if (aux_scene_base->appearence_model() == BOXM_BOOL) {
    aux_scene->load_scene(aux_parser);
    aux_scene_base = aux_scene;
  }
  else {
    std::cerr << "Error in bvpl_global_taylor::load_aux_scene: Invalid appearance model\n";
    return NULL;
  }

  return aux_scene_base;
}
#endif


//: Load auxiliary scene info
template<class T_data, unsigned DIM>
boxm_scene_base_sptr bvpl_global_taylor<T_data, DIM>::load_projection_scene (int scene_id)
{
  if (scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    std::cerr << "Error in bvpl_global_taylor::load_projection_scene: Invalid scene id\n";
    return nullptr;
  }
  //load scene
  boxm_scene_base_sptr proj_scene_base = new boxm_scene_base();
  boxm_scene_parser proj_parser;
  std::stringstream proj_scene_ss;
  proj_scene_ss << aux_dirs_[scene_id] << "/proj_taylor_scene_" << scene_id << ".xml";
  proj_scene_base->load_scene(proj_scene_ss.str(), proj_parser);

  //cast scene
  typedef boct_tree<short,vnl_vector_fixed<T_data,DIM> > taylor_tree_type;
  boxm_scene<taylor_tree_type > *proj_scene= new boxm_scene<taylor_tree_type >();
  if (proj_scene_base->appearence_model() == bvpl_taylor_apm<T_data,DIM>()) {
    proj_scene->load_scene(proj_parser);
    proj_scene_base = proj_scene;
  }
  else {
    std::cerr << "Error in bvpl_global_taylor::load_proj_scene: Invalid appearance model\n";
    return nullptr;
  }

  return proj_scene_base;
}

//: Write this class to xml file
template<class T_data, unsigned DIM>
void bvpl_global_taylor<T_data, DIM>::xml_write()
{
  bxml_document doc;
  bxml_element *root = new bxml_element("taylor_global_info");
  doc.set_root_element(root);
  root->append_text("\n");

  //write the scenes
  for (unsigned i =0; i<scenes_.size(); i++)
  {
    bxml_element* scenes_elm = new bxml_element("scene");
    scenes_elm->append_text("\n");
    scenes_elm->set_attribute("id", i);
    scenes_elm->set_attribute("path", scenes_[i]);
    scenes_elm->set_attribute("aux_dir", aux_dirs_[i]);
    scenes_elm->set_attribute("cell_length", finest_cell_length_[i]);
    root->append_data(scenes_elm);
    root->append_text("\n");
  }

  //write path to kernels
  bxml_element* kernel_elm = new bxml_element("kernels");
  kernel_elm->append_text("\n");
  kernel_elm->set_attribute("path", kernels_path_);
  root->append_data(kernel_elm);
  root->append_text("\n");

  //write to disk
  std::ofstream os(xml_path().c_str());
  bxml_write(os, doc);
  os.close();
}


//: Extract a particular coefficient scene
template<class T_data, unsigned DIM>
void bvpl_global_taylor<T_data, DIM>::extract_coefficient_scene(int scene_id, int coefficient_id, boxm_scene<boct_tree<short, float > > *float_scene)
{
  typedef boct_tree<short, float> float_tree_type;
  typedef boct_tree_cell<short, float> float_cell_type;

  typedef boct_tree<short,vnl_vector_fixed<double,10> > projection_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > projection_cell_type;
  std::vector<vnl_vector_fixed<double,10> > projections;

  boxm_scene_base_sptr projection_scene_base = load_projection_scene(scene_id);

  boxm_scene<projection_tree_type>* projection_scene = dynamic_cast<boxm_scene<projection_tree_type>* >(projection_scene_base.as_pointer());

  if (!(projection_scene && float_scene))
  {
    std::cerr << "Error in bof_util::random_label_for_training: Could not cast scenes\n";
    return;
  }

  //init variables
  projection_scene->unload_active_blocks();
  float_scene->unload_active_blocks();

  //get the cells for these blocks
  boxm_block_iterator<projection_tree_type> it = projection_scene->iterator();
  for (it.begin(); !it.end(); ++it)
  {
    if (!(projection_scene->valid_index(it.index()) && float_scene->valid_index(it.index()))) {
      std::cerr << "In bof_util::random_label_for_training: invalid block\n";
      return;
    }

    projection_scene->load_block(it.index());
    float_scene->load_block(it.index());

    //get the trees
    projection_tree_type* projection_tree = projection_scene->get_block(it.index())->get_tree();
    float_tree_type* float_tree = projection_tree->clone_to_type<float>();

    //get leaf cells
    std::vector<projection_cell_type *> projection_leaves = projection_tree->leaf_cells();
    std::vector<float_cell_type *> float_leaves = float_tree->leaf_cells();

    unsigned int tree_ncells = projection_leaves.size();

    for (unsigned int i =0; i<tree_ncells; ++i)
    {
      float_leaves[i]->set_data(float(projection_leaves[i]->data()[coefficient_id]));
    }
    float_scene->get_block(it.index())->init_tree(float_tree);
    float_scene->write_active_block();
  }

  //release memory
  projection_scene->unload_active_blocks();
  float_scene->unload_active_blocks();
}


#define BVPL_GLOBAL_TAYLOR_INSTANTIATE(T1,T2) \
template class bvpl_global_taylor<T1,T2 >

#endif // bvpl_global_taylor_hxx_
