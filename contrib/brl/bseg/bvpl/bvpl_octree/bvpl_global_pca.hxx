#ifndef bvpl_global_pca_hxx_
#define bvpl_global_pca_hxx_
//:
// \file
// \author Isabel Restrepo
// \date 14-Mar-2011

#include "bvpl_global_pca.h"
#include <bvpl/bvpl_octree/sample/bvpl_pca_basis_sample.h>


#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>

#include <boxm/boxm_scene_parser.h>

#include <vnl/vnl_random.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

#include <vul/vul_file.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//#define DEBUG_PROJ

//: Create from xml_file
template <unsigned feature_dim>
bvpl_global_pca<feature_dim>::bvpl_global_pca(const std::string &path)
{
  std::cout << "Loading pca info from xml-file" << std::endl;
  int valid = 0;
  path_out_ = path;
  std::ifstream xml_ifs(xml_path().c_str());
  if (!xml_ifs.is_open()) {
    std::cerr << "Error: bvpl_discover_pca_kernels - could not open xml info file: " << xml_path() << '\n';
    throw;
  }
  bxml_document doc = bxml_read(xml_ifs);
  bxml_element query("pca_global_info");
  bxml_data_sptr root = bxml_find_by_name(doc.root_element(), query);
  if (!root) {
    std::cerr << "Error: bvpl_discover_pca_kernels - could not parse xml root\n";
    throw;
  }

  //Parse neighborhood bounding box - units are number of voxels
  bxml_element nbbox_query("neighborhood");
  bxml_data_sptr nbbox_data = bxml_find_by_name(root, nbbox_query);
  bxml_element* nbbox_elm = dynamic_cast<bxml_element*>(nbbox_data.ptr());
  int min_x, min_y, min_z, max_x, max_y, max_z =0;
  nbbox_elm->get_attribute("min_x", min_x);
  nbbox_elm->get_attribute("min_y", min_y);
  nbbox_elm->get_attribute("min_z", min_z);
  nbbox_elm->get_attribute("max_x", max_x);
  nbbox_elm->get_attribute("max_y", max_y);
  nbbox_elm->get_attribute("max_z", max_z);

  nbbox_ = vgl_box_3d<int>(vgl_point_3d<int>(min_x, min_y, min_y), vgl_point_3d<int>(max_x, max_y, max_z));
  std::cout << "Neighborhood: " << nbbox_ << std::endl;

  //Parse Number of samples
  bxml_element properties_query("properties");
  bxml_data_sptr properties_data = bxml_find_by_name(root, properties_query);
  bxml_element* properties_elm = dynamic_cast<bxml_element*>(properties_data.ptr());
  properties_elm->get_attribute("nsamples", nsamples_);
  unsigned temp_dim;
  properties_elm->get_attribute("feature_dim", temp_dim);
  if (temp_dim!=feature_dim) {
    valid = -10;
  }
  properties_elm->get_attribute("training_fraction", training_fraction_);

  unsigned nscenes = 0;
  properties_elm->get_attribute("nscenes", nscenes);

  std::cout << "Number of samples: " << nsamples_ << '\n'
           << "Feature dimension: " << temp_dim << '\n'
           << "Number of scenes: " << nscenes <<std::endl;

  //Parse scenes
  bxml_element scenes_query("scene");
  std::vector<bxml_data_sptr> scenes_data = bxml_find_all_with_name(root, scenes_query);

  if (nscenes !=scenes_data.size()) {
    std::cerr<<"Wrong number of scenes\n";
    throw;
  }

  scenes_.clear();
  scenes_.resize(nscenes);

  aux_dirs_.clear();
  aux_dirs_.resize(nscenes);

  finest_cell_length_.clear();
  finest_cell_length_.resize(nscenes);

  nleaves_.clear();
  nleaves_.resize(nscenes);

  for (unsigned si = 0; si < nscenes; si++)
  {
    bxml_element* scenes_elm = dynamic_cast<bxml_element*>(scenes_data[si].ptr());
    int id = -1;
    scenes_elm->get_attribute("id", id);
    scenes_elm->get_attribute("path", scenes_[id]);
    scenes_elm->get_attribute("aux_dir", aux_dirs_[id]);
    scenes_elm->get_attribute("cell_length" , finest_cell_length_[id]);
    scenes_elm->get_attribute("nleaves", nleaves_[id]);

    std::cout << "Scene " << id << " is " << scenes_[id] << '\n';
  }

  //Parse training scenes
  bxml_element train_query("training_scenes");
  bxml_element* train_elm = dynamic_cast<bxml_element*>(bxml_find_by_name(root, train_query).ptr());
  unsigned n_train_scenes = 0;
  train_elm->get_attribute("nscenes", n_train_scenes);
  training_scenes_.clear();
  training_scenes_.resize(nscenes, false);

  //read out the scenes
  for (bxml_element::const_data_iterator s_it = train_elm->data_begin(); s_it != train_elm->data_end(); s_it++) {
    if ((*s_it)->type() == bxml_data::TEXT) {
      bxml_text* t = dynamic_cast<bxml_text*>((*s_it).ptr());
      std::stringstream text_d(t->data()); std::string buf;
      std::vector<std::string> tokens;
      while (text_d >> buf) {
        tokens.push_back(buf);
      }
      if (tokens.size() != n_train_scenes)
        continue;
      for (unsigned i = 0; i < n_train_scenes; i++) {
        std::stringstream ss2(tokens[i]); int s_type_id;
        ss2 >> s_type_id;
        std::cout << "Scene: " << s_type_id << " is used for training\n";
        training_scenes_[s_type_id]=true;
      }
      break;
    }
  }

  //Parse paths and set matrices
  bxml_element paths_query("paths");
  bxml_data_sptr paths_data = bxml_find_by_name(root, paths_query);
  if (paths_data)
  {
    bxml_element* path_elm = dynamic_cast<bxml_element*>(paths_data.ptr());

    std::string ifs_path;

    path_elm->get_attribute("pc_path", ifs_path);
    if (ifs_path != pc_path())
      valid = -1;
    else{
      std::ifstream ifs(ifs_path.c_str());
      ifs >> pc_;
      ifs.close();
      if (pc_.size()!=feature_dim*feature_dim)
        valid = -2;
    }

    path_elm->get_attribute("weights_path", ifs_path);
    if (ifs_path != weights_path())
      valid = -3;
    else{
      std::ifstream ifs(ifs_path.c_str());
      ifs >> weights_;
      ifs.close();
      if (weights_.size()!=feature_dim)
        valid = -4;
    }

    path_elm->get_attribute("mean_path", ifs_path);
    if (ifs_path != mean_path())
      valid = -5;
    else
    {
      if ( vul_file::exists(ifs_path))
      {
        std::ifstream ifs(ifs_path.c_str());
        ifs >> training_mean_;
        ifs.close();
      }
      else{
        std::cerr << " Warning: Mean file is empty\n";
        training_mean_.fill(0.0);
      }

      path_elm->get_attribute("scatter_path", ifs_path);
      if (ifs_path != scatter_path())
        valid = -6;
      else if (vul_file::exists(ifs_path)) {
        std::ifstream(ifs_path);
        ifs_path >> scatter_;
        ifs_path.close();
      }
      else{
        std::cerr << " Warning: Scatter file is empty\n";
        scatter_.fill(0.0);
      }

      if (valid<0) {
        std::cout << "bvpl_discover_pca_kernels - errors parsing pca_info.xml. Error code: " << valid << std::endl;
        xml_write();
      }
    }
  }

  xml_ifs.close();
}

//: Init auxiliary scenes and smallest cell length values
template <unsigned feature_dim>
void bvpl_global_pca<feature_dim>::init(unsigned scene_id)
{
  boxm_scene_base_sptr data_scene_base = load_scene(scene_id);
  boxm_scene<boct_tree<short, float> >* data_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (data_scene_base.as_pointer());
  if (!data_scene) {
    std::cerr << "Error in bvpl_global_pca<feature_dim>::init(): Could not cast data scene\n";
    return;
  }
  double finest_cell_length = data_scene->finest_cell_length();
  finest_cell_length_[scene_id] = finest_cell_length;
  nleaves_[scene_id] = data_scene->size();

  if (!(vul_file::exists(aux_dirs_[scene_id]) && vul_file::is_directory(aux_dirs_[scene_id]))) {
    vul_file::make_directory(aux_dirs_[scene_id]);
  }

  {
    std::stringstream aux_scene_ss;
    aux_scene_ss << "train_scene_" << scene_id ;
    std::string aux_scene_path = aux_dirs_[scene_id] + "/" + aux_scene_ss.str() + ".xml";
    if (!vul_file::exists(aux_scene_path)) {
      std::cout<< "Scene: " << aux_scene_path << " does not exist, initializing" << std::endl;
      boxm_scene<boct_tree<short, bool> > *aux_scene =
      new boxm_scene<boct_tree<short, bool> >(data_scene->lvcs(), data_scene->origin(), data_scene->block_dim(), data_scene->world_dim(), data_scene->max_level(), data_scene->init_level());
      aux_scene->set_appearance_model(BOXM_BOOL);
      aux_scene->set_paths(aux_dirs_[scene_id], aux_scene_ss.str());
      aux_scene->write_scene("/" + aux_scene_ss.str() +  ".xml");
    }
  }
  {
    std::stringstream aux_scene_ss;
    aux_scene_ss << "valid_scene_" << scene_id ;
    std::string aux_scene_path = aux_dirs_[scene_id] + "/" + aux_scene_ss.str() + ".xml";
    if (!vul_file::exists(aux_scene_path)) {
      std::cout<< "Scene: " << aux_scene_path << " does not exist, initializing" << std::endl;
      boxm_scene<boct_tree<short, bool> > *aux_scene =
      new boxm_scene<boct_tree<short, bool> >(data_scene->lvcs(), data_scene->origin(), data_scene->block_dim(), data_scene->world_dim(), data_scene->max_level(), data_scene->init_level());
      aux_scene->set_appearance_model(BOXM_BOOL);
      aux_scene->set_paths(aux_dirs_[scene_id], aux_scene_ss.str());
      aux_scene->write_scene("/" + aux_scene_ss.str() +  ".xml");
    }
  }

  {
    std::stringstream proj_scene_ss;
    proj_scene_ss << "proj_pca_scene_" << scene_id ;
    std::string proj_scene_path = aux_dirs_[scene_id] + "/" + proj_scene_ss.str() + ".xml";
    if (!vul_file::exists(proj_scene_path)) {
      std::cout<< "Scene: " << proj_scene_path << " does not exist, initializing" << std::endl;
      typedef boct_tree<short,vnl_vector_fixed<double,10> > pca_tree_type;
      boxm_scene<pca_tree_type > *proj_scene =
      new boxm_scene<pca_tree_type >(data_scene->lvcs(), data_scene->origin(), data_scene->block_dim(), data_scene->world_dim(), data_scene->max_level(), data_scene->init_level());
      proj_scene->set_appearance_model(VNL_DOUBLE_10);
      proj_scene->set_paths(aux_dirs_[scene_id], proj_scene_ss.str());
      proj_scene->write_scene("/" + proj_scene_ss.str() +  ".xml");
    }
  }

  xml_write();
}

//: Compute the scatter matrix of the specified block. A random number of cells are drawn for the calculation
//  The mask block indicates, whether a cell was used for learning the scatter matrix and mean
//  Ramndom samples according to octree structure.
//  Sampling is achieved by generating uniform random cell samples.
//  Since there are more cells where the resolution is finer, then these regions get sampled more often
template <unsigned feature_dim>
bool bvpl_global_pca<feature_dim>::sample_statistics( int scene_id, int block_i, int block_j, int block_k,
                                                      vnl_matrix_fixed<double, feature_dim, feature_dim> &S,
                                                      vnl_vector_fixed<double, feature_dim> &mean,
                                                      unsigned long &nfeature)
{
  if (!training_scenes_[scene_id]) {
    std::cout << "Skiping scene: " << scene_id <<", labeled for testing\n";
    return false;
  }

  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;

  typedef boct_tree<short,bool> bool_tree_type;
  typedef boct_tree_cell<short,bool> bool_cell_type;

  boxm_scene_base_sptr data_scene_base =load_scene(scene_id);
  boxm_scene_base_sptr mask_scene_base =load_train_scene(scene_id);

  boxm_scene<boct_tree<short, float> >* data_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*>(data_scene_base.as_pointer());
  boxm_scene<boct_tree<short, bool> >* mask_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*>(mask_scene_base.as_pointer());

  if (!(data_scene))
  {
    std::cerr << "Error in bvpl_global_pca<feature_dim>::sample_statistics: Could not cast data scenes\n";
    return false;
  }
  if (!(mask_scene))
  {
    std::cerr << "Error in bvpl_global_pca<feature_dim>::sample_statistics: Could not cast mask scenes\n";
    return false;
  }

  //init variables
  data_scene->unload_active_blocks();
  mask_scene->unload_active_blocks();

  mean.fill(0.0);
  S.fill(0.0);

  vnl_random rng;
  //vnl_random rng(9667566ul);

  //get the cells for this block
  if (!(data_scene->valid_index(block_i, block_j, block_k) && mask_scene->valid_index(block_i, block_j, block_k))) {
    std::cerr << "In compute_testing_error: Invalid block\n";
    return false;
  }

  data_scene->load_block_and_neighbors(block_i, block_j, block_k);
  mask_scene->load_block(block_i, block_j, block_k);

  //get the trees
  float_tree_type* data_tree = data_scene->get_block(block_i, block_j, block_k)->get_tree();
  bool_tree_type* mask_tree = data_tree->template clone_to_type<bool>();
  mask_tree->init_cells(false);

  nfeature = 1;

  //2. Sample cells from this tree. The number of samples from this tree depends on the portion of scene cells that live in this tree
  std::vector<float_cell_type *> leaf_cells = data_tree->leaf_cells();
  std::vector<bool_cell_type*> mask_leaves = mask_tree->leaf_cells();

  int tree_ncells = leaf_cells.size();
  unsigned long tree_nsamples = (unsigned long)(tree_ncells*training_fraction_);
  double cell_length = finest_cell_length_[scene_id];
  //CAUTION: the neighborhood box was suppossed to be defined as number of regular neighbors
  //convert neighborhood box to scene coordinates
  vgl_point_3d<int> nmin = nbbox_.min_point();
  vgl_point_3d<int> nmax = nbbox_.max_point();

  std::cout <<" In block (" << block_i <<", " << block_j << ", " << block_k << "), number of nsamples is: " << tree_nsamples << ", cell length is: " << cell_length << std::endl;

  for (unsigned long i=0; i<tree_nsamples; i++)
  {
    unsigned long sample = rng.lrand32(tree_ncells-1);

    boct_tree_cell<short, float> *center_cell = leaf_cells[sample];
    vgl_point_3d<double> center_cell_centroid = data_tree->global_centroid(center_cell);

    vgl_box_3d<double> roi(cell_length*(double)nmin.x(),cell_length*(double)nmin.y(),cell_length*(double)nmin.z(),
                           cell_length*(double)nmax.x(),cell_length*(double)nmax.y(),cell_length*(double)nmax.z());
    roi.set_centroid(center_cell_centroid);

    //if neighborhood is not inclusive we would have missing features
    if (!((data_scene->get_world_bbox()).contains(roi))) {
      i--;
      continue;
    }

    //3. Assemble neighborhood as a feature-vector
    vnl_vector_fixed<double, feature_dim> this_feature(0.0);

    unsigned curr_dim = 0;
    for (int z = nbbox_.min_z(); z<=nbbox_.max_z(); z++)
      for (int y = nbbox_.min_y(); y<=nbbox_.max_y(); y++)
        for (int x = nbbox_.min_x(); x<=nbbox_.max_x(); x++)
        {
          vgl_point_3d<double> neighbor_centroid(center_cell_centroid.x() + (double)x*cell_length,
                                                 center_cell_centroid.y() + (double)y*cell_length,
                                                 center_cell_centroid.z() + (double)z*cell_length);

          boct_tree_cell<short,float> *neighbor_cell = data_scene->locate_point_in_memory(neighbor_centroid);

          assert(neighbor_cell !=nullptr);
          this_feature[curr_dim] = (double)neighbor_cell->data();
          curr_dim++;
        }

    assert(curr_dim == feature_dim);

    mask_leaves[sample]->set_data(true);

    //increment weights
    double rho = 1.0/(double)nfeature;
    double rho_comp = 1.0 - rho;

    // the difference vector between the sample and the mean
    vnl_vector_fixed<double, feature_dim> diff = this_feature - mean;

    //update the covariance
    S += rho_comp*outer_product(diff,diff);

    //update the mean
    mean += (rho*diff);

    ++nfeature;
#if 0
    std::cerr << "Feature EVD: " <<this_feature << '\n'
             << "Mean Feature EVD: " <<mean_feature << '\n';
#endif
  }

  --nfeature;
  // write and release memory
  mask_scene->get_block(block_i, block_j, block_k)->init_tree(mask_tree);
  mask_scene->write_active_block();

  data_scene->unload_active_blocks();
  mask_scene->unload_active_blocks();

  return true;
}


//: Update mean and scatter, given the mean and scatter of two sets.
//  Calculation is done according to Chan et al. Updating Formulae and a Pairwise Algorithm for Computing Sample Variances
template <unsigned feature_dim>
void bvpl_global_pca<feature_dim>::combine_pairwise_statistics( const vnl_vector_fixed<double,feature_dim> &mean1,
                                                                const vnl_matrix_fixed<double,feature_dim,feature_dim> &scatter1,
                                                                double const n1,
                                                                const vnl_vector_fixed<double,feature_dim>  &mean2,
                                                                const  vnl_matrix_fixed<double,feature_dim,feature_dim> &scatter2,
                                                                double const n2,
                                                                vnl_vector_fixed<double,feature_dim> & mean_out,
                                                                vnl_matrix_fixed<double,feature_dim,feature_dim> & scatter_out,
                                                                double &n_out )
{
  n_out = n1+n2;
  mean_out = (n1*mean1 + n2*mean2)* (1.0/n_out);
  vnl_vector_fixed<double, feature_dim> d = mean2 - mean1;
  scatter_out = scatter1 + scatter2 + (n1*n2/n_out)*outer_product(d,d);
}

//: Set total scatter matrix, mean, sample, principal components and weights for this class
template <unsigned feature_dim>
void bvpl_global_pca<feature_dim>::set_up_pca_evd(const vnl_matrix_fixed<double, feature_dim, feature_dim> &S,
                                                  const vnl_vector_fixed<double, feature_dim> &mean,
                                                  const double total_nsamples)
{
  scatter_ = S;
  training_mean_ = mean;
  nsamples_ = (long unsigned int)total_nsamples;

  // Compute eigenvectors(principal components) and values of S

  vnl_matrix<double> pc_temp;
  vnl_vector<double> w_temp;

  vnl_symmetric_eigensystem_compute(scatter_.as_ref(), pc_temp, w_temp);
  pc_ = pc_temp.fliplr();
  weights_=w_temp.flip();

  //save the newly set vaeiables
  this->xml_write();
}

//: Computes 10-dimensional pca projection at each voxel on the block and saves it
template <unsigned feature_dim>
void bvpl_global_pca<feature_dim>::project(int scene_id, int block_i, int block_j, int block_k)
{
  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;

  typedef boct_tree<short,vnl_vector_fixed<double,10> > pca_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > pca_cell_type;

  boxm_scene_base_sptr data_scene_base =load_scene(scene_id);
  boxm_scene_base_sptr proj_scene_base =load_projection_scene(scene_id);
  boxm_scene_base_sptr aux_scene_base = load_valid_scene(scene_id);

  boxm_scene<boct_tree<short, float> >* data_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*>(data_scene_base.as_pointer());
  boxm_scene<pca_tree_type>* proj_scene = dynamic_cast<boxm_scene<pca_tree_type>*>(proj_scene_base.as_pointer());
  boxm_scene<boct_tree<short, bool> >* aux_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (aux_scene_base.as_pointer());
  if (!(data_scene && proj_scene && aux_scene))
  {
    std::cerr << "Error in bvpl_global_pca<feature_dim>::sample_statistics: Could not cast input scenes\n";
    return;
  }

  //init variables
  data_scene->unload_active_blocks();
  proj_scene->unload_active_blocks();
  aux_scene->unload_active_blocks();

  //get the cells for this block
  if (!(data_scene->valid_index(block_i, block_j, block_k) && proj_scene->valid_index(block_i, block_j, block_k) && aux_scene->valid_index(block_i, block_j, block_k))) {
    std::cerr << "In compute_testing_error: Invalid block\n";
    return;
  }

  data_scene->load_block_and_neighbors(block_i, block_j, block_k);
  proj_scene->load_block(block_i, block_j, block_k);
  aux_scene->load_block(block_i, block_j, block_k);

  //get the trees
  float_tree_type* data_tree = data_scene->get_block(block_i, block_j, block_k)->get_tree();
  pca_tree_type* proj_tree = data_tree->template clone_to_type<vnl_vector_fixed<double,10> >();
  boct_tree<short, bool>* aux_tree = data_tree->template clone_to_type<bool>();
  aux_tree->init_cells(true);

  //get leaf cells
  std::vector<float_cell_type *> data_leaves = data_tree->leaf_cells();
  std::vector<pca_cell_type *> proj_leaves = proj_tree->leaf_cells();
  std::vector<boct_tree_cell<short, bool> *> aux_leaves = aux_tree->leaf_cells();

  double cell_length = finest_cell_length_[scene_id];

  //CAUTION: the neighborhood box was suppossed to be defined as number of regular neighbors
  //convert neighborhood box to scene coordinates
  vgl_point_3d<int> nmin = nbbox_.min_point();
  vgl_point_3d<int> nmax = nbbox_.max_point();
#ifdef DEBUG_PROJ
  double error = 0.0;
  unsigned long n_valid_cells= 0;
#endif

  for (unsigned long i =0; i<data_leaves.size(); i++)
  {
    float_cell_type* data_cell = data_leaves[i];

    //create a region around the center cell
    vgl_point_3d<double> centroid = data_tree->global_centroid(data_cell);

    //change the coordinates of enpoints to be in global coordinates abd test if they are contained in the scene
    vgl_point_3d<double> min_point_global(centroid.x() + (double)nmin.x()*cell_length, centroid.y() + (double)nmin.y()*cell_length, centroid.z() + (double)nmin.z()*cell_length);
    vgl_point_3d<double> max_point_global(centroid.x() + (double)nmax.x()*cell_length, centroid.y() + (double)nmax.y()*cell_length, centroid.z() + (double)nmax.z()*cell_length);
    if (!(data_scene->locate_point_in_memory(min_point_global) && data_scene->locate_point_in_memory(max_point_global))) {
      proj_leaves[i]->set_data(vnl_vector_fixed<double,10>(0.0));
      aux_leaves[i]->set_data(false);
      continue;
    }

    //3. Assemble neighborhood as a feature-vector
    vnl_vector_fixed<double, feature_dim> this_feature(0.0f);

    unsigned curr_dim = 0;
    for (int z = nbbox_.min_z(); z<=nbbox_.max_z(); z++)
      for (int y = nbbox_.min_y(); y<=nbbox_.max_y(); y++)
        for (int x = nbbox_.min_x(); x<=nbbox_.max_x(); x++)
        {
          vgl_point_3d<double> neighbor_centroid(centroid.x() + (double)x*cell_length,
                                                 centroid.y() + (double)y*cell_length,
                                                 centroid.z() + (double)z*cell_length);

          boct_tree_cell<short,float> *neighbor_cell = data_scene->locate_point_in_memory(neighbor_centroid);

          if (!neighbor_cell) {
            std::cerr << "Error in bvpl_global_pca<feature_dim>::project\n";
            return;
          }

          this_feature[curr_dim] = (double)neighbor_cell->data();
          curr_dim++;
        }

    if (curr_dim != feature_dim) {
      std::cerr << "Error in bvpl_global_pca<feature_dim>::project\n";
      return;
    }
    this_feature-=training_mean_;

    //solve for the coefficients
    vnl_vector_fixed<double, feature_dim> a(0.0);
    a = pc_.transpose() * (this_feature);
    vnl_vector_fixed<double,10> sample(a.extract(10));
    proj_leaves[i]->set_data(sample);

#ifdef DEBUG_PROJ
    //project as a function of number of components
    vnl_vector_fixed<double, feature_dim> feature_approx  = pc_.extract(feature_dim, 10) * sample;

    //compute error
    error+=(float)((this_feature - feature_approx).squared_magnitude());
    n_valid_cells++;
#endif
  }

  // write and release memory
  proj_scene->get_block(block_i, block_j, block_k)->init_tree(proj_tree);
  proj_scene->write_active_block();
  proj_scene->unload_active_blocks();
  aux_scene->get_block(block_i, block_j, block_k)->init_tree(aux_tree);
  aux_scene->write_active_block();
  aux_scene->unload_active_blocks();
  data_scene->unload_active_blocks();

#ifdef DEBUG_PROJ
  std::cout << "Total error in this block: " << error/(double)n_valid_cells << std::endl;
#endif
}

//: Computes the projection error (as square magniture) given 10-dimensional pca projection at each voxel on the block
template <unsigned feature_dim>
void bvpl_global_pca<feature_dim>::projection_error(int scene_id, int block_i, int block_j, int block_k)
{
  if (training_scenes_[scene_id]) {
    std::cout << "Skipping training scene: " <<scene_id << std::endl;
    return;
  }
  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;

  typedef boct_tree<short,vnl_vector_fixed<double,10> > pca_tree_type;
  typedef boct_tree_cell<short,vnl_vector_fixed<double,10> > pca_cell_type;

  boxm_scene_base_sptr proj_scene_base = load_projection_scene(scene_id);
  boxm_scene_base_sptr error_scene_base = load_error_scene(scene_id);
  boxm_scene_base_sptr data_scene_base =load_scene(scene_id);

  boxm_scene<float_tree_type>* data_scene = dynamic_cast<boxm_scene<float_tree_type>*>(data_scene_base.as_pointer());
  boxm_scene<pca_tree_type>* proj_scene = dynamic_cast<boxm_scene<pca_tree_type>*>(proj_scene_base.as_pointer());
  boxm_scene<float_tree_type >* error_scene = dynamic_cast<boxm_scene<float_tree_type >*>(error_scene_base.as_pointer());

  if (!(data_scene && error_scene && proj_scene))
  {
    std::cerr << "Error in bvpl_global_pca<feature_dim>::sample_statistics: Could not cast input scenes\n";
    return;
  }

  //init variables
  data_scene->unload_active_blocks();
  proj_scene->unload_active_blocks();
  error_scene->unload_active_blocks();

  //get the cells for this block
  if (!(data_scene->valid_index(block_i, block_j, block_k) && proj_scene->valid_index(block_i, block_j, block_k) && error_scene->valid_index(block_i, block_j, block_k))) {
    std::cerr << "In compute_testing_error: Invalid block\n";
    return;
  }

  data_scene->load_block_and_neighbors(block_i, block_j, block_k);
  proj_scene->load_block(block_i, block_j, block_k);
  error_scene->load_block(block_i, block_j, block_k);

  //get the trees
  float_tree_type* data_tree = data_scene->get_block(block_i, block_j, block_k)->get_tree();
  pca_tree_type* proj_tree = proj_scene->get_block(block_i, block_j, block_k)->get_tree();
  float_tree_type* error_tree = data_tree->clone();
  error_tree->init_cells(-1.0f);

  //get leaf cells
  std::vector<float_cell_type *> data_leaves = data_tree->leaf_cells();
  std::vector<pca_cell_type *> proj_leaves = proj_tree->leaf_cells();
  std::vector<float_cell_type *> error_leaves = error_tree->leaf_cells();

  double cell_length = finest_cell_length_[scene_id];

  //CAUTION: the neighborhood box was suppossed to be defined as number of regular neighbors
  //convert neighborhood box to scene coordinates
  vgl_point_3d<int> nmin = nbbox_.min_point();
  vgl_point_3d<int> nmax = nbbox_.max_point();

  for (unsigned i =0; i<data_leaves.size(); i++)
  {
    float_cell_type* data_cell = data_leaves[i];

    //create a region around the center cell
    vgl_point_3d<double> centroid = data_tree->global_centroid(data_cell);

    //change the coordinates of enpoints to be in global coordinates aad test if they are contained in the scene
    vgl_point_3d<double> min_point_global(centroid.x() + (double)nmin.x()*cell_length, centroid.y() + (double)nmin.y()*cell_length, centroid.z() + (double)nmin.z()*cell_length);
    vgl_point_3d<double> max_point_global(centroid.x() + (double)nmax.x()*cell_length, centroid.y() + (double)nmax.y()*cell_length, centroid.z() + (double)nmax.z()*cell_length);
    if (!(data_scene->locate_point_in_memory(min_point_global) && data_scene->locate_point_in_memory(max_point_global))) {
      error_leaves[i]->set_data(-1.0f);
      continue;
    }

    //3. Assemble neighborhood as a feature-vector
    vnl_vector_fixed<double, feature_dim> this_feature(0.0f);

    unsigned curr_dim = 0;
    for (int z = nbbox_.min_z(); z<=nbbox_.max_z(); ++z)
      for (int y = nbbox_.min_y(); y<=nbbox_.max_y(); ++y)
        for (int x = nbbox_.min_x(); x<=nbbox_.max_x(); ++x)
        {
          vgl_point_3d<double> neighbor_centroid(centroid.x() + (double)x*cell_length,
                                                 centroid.y() + (double)y*cell_length,
                                                 centroid.z() + (double)z*cell_length);

          boct_tree_cell<short,float> *neighbor_cell = data_scene->locate_point_in_memory(neighbor_centroid);

          if (!neighbor_cell) {
            std::cerr << "Error in bvpl_global_pca<feature_dim>::project\n";
            return;
          }

          this_feature[curr_dim] = (double)neighbor_cell->data();
          curr_dim++;
        }

    if (curr_dim != feature_dim) {
      std::cerr << "Error in bvpl_global_pca<feature_dim>::project\n";
      return;
    }
    this_feature-=training_mean_;

    //get the coefficients
    vnl_vector_fixed<double, 10> a = proj_leaves[i]->data();

    //project
    vnl_vector_fixed<double, feature_dim> feature_approx  = pc_.extract(feature_dim, 10) * a;

    //compute error
    error_leaves[i]->set_data((float)((this_feature - feature_approx).squared_magnitude()));
  }

  // write and release memory
  error_scene->get_block(block_i, block_j, block_k)->init_tree(error_tree);
  error_scene->write_active_block();
  error_scene->unload_active_blocks();
  data_scene->unload_active_blocks();
  proj_scene->unload_active_blocks();
}


//: Load scene info
template <unsigned feature_dim>
boxm_scene_base_sptr bvpl_global_pca<feature_dim>::load_scene (int scene_id)
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
template <unsigned feature_dim>
boxm_scene_base_sptr bvpl_global_pca<feature_dim>::load_valid_scene (int scene_id)
{
  if (scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    std::cerr << "Error in bvpl_global_pca::load_scene: Invalid scene id\n";
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
    std::cerr << "Error in bvpl_global_pca::load_aux_scene: Invalid appearance model\n";
    return nullptr;
  }

  return aux_scene_base;
}

//: Load auxiliary scene indicating if a cell is should be used for training
template <unsigned feature_dim>
boxm_scene_base_sptr bvpl_global_pca<feature_dim>::load_train_scene (int scene_id)
{
  if (scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    std::cerr << "Error in bvpl_global_pca::load_scene: Invalid scene id\n";
    return nullptr;
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
    std::cerr << "Error in bvpl_global_pca::load_aux_scene: Invalid appearance model\n";
    return nullptr;
  }

  return aux_scene_base;
}


//: Load auxiliary scene info
template <unsigned feature_dim>

boxm_scene_base_sptr bvpl_global_pca<feature_dim>::load_projection_scene (int scene_id)
{
  if (scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    std::cerr << "Error in bvpl_global_pca::load_projection_scene: Invalid scene id\n";
    return nullptr;
  }
  //load scene
  boxm_scene_base_sptr proj_scene_base = new boxm_scene_base();
  boxm_scene_parser proj_parser;
  std::stringstream proj_scene_ss;
  proj_scene_ss << aux_dirs_[scene_id] << "/proj_pca_scene_" << scene_id << ".xml";
  proj_scene_base->load_scene(proj_scene_ss.str(), proj_parser);

  //cast scene
  typedef boct_tree<short,vnl_vector_fixed<double,10> > pca_tree_type;
  boxm_scene<pca_tree_type > *proj_scene= new boxm_scene<pca_tree_type >();
  if (proj_scene_base->appearence_model() == VNL_DOUBLE_10) {
    proj_scene->load_scene(proj_parser);
    proj_scene_base = proj_scene;
  }
  else {
    std::cerr << "Error in bvpl_global_pca::load_proj_scene: Invalid appearance model\n";
    return nullptr;
  }

  return proj_scene_base;
}


//: Load auxiliary scene info
template <unsigned feature_dim>
boxm_scene_base_sptr bvpl_global_pca<feature_dim>::load_error_scene (int scene_id)
{
  if (scene_id<0 || scene_id>((int)scenes_.size() -1))
  {
    std::cerr << "Error in bvpl_global_pca::load_error_scene: Invalid scene id\n";
    return nullptr;
  }

  boxm_scene_base_sptr data_scene_base = load_scene(scene_id);
  boxm_scene<boct_tree<short, float> >* data_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (data_scene_base.as_pointer());
  if (!data_scene) {
    std::cerr << "Error in bvpl_global_pca<feature_dim>::init(): Could not cast data scene\n";
    return nullptr;
  }

  std::stringstream aux_scene_ss;
  aux_scene_ss << "error_pca_scene_" << scene_id ;
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
  error_scene_ss << aux_dirs_[scene_id] << "/error_pca_scene_" << scene_id << ".xml";
  error_scene_base->load_scene(error_scene_ss.str(), error_parser);

  //cast scene
  boxm_scene<boct_tree<short, float> > *error_scene= new boxm_scene<boct_tree<short, float> >();
  if (error_scene_base->appearence_model() == BOXM_FLOAT) {
    error_scene->load_scene(error_parser);
    error_scene_base = error_scene;
  }
  else {
    std::cerr << "Error in bvpl_global_pca::load_error_scene: Invalid appearance model\n";
    return nullptr;
  }

  return error_scene_base;
}


//: Write this class to xml file
template <unsigned feature_dim>
void bvpl_global_pca<feature_dim>::xml_write()
{
  bxml_document doc;
  bxml_element *root = new bxml_element("pca_global_info");
  doc.set_root_element(root);
  root->append_text("\n");

  bxml_element* paths = new bxml_element("paths");
  paths->append_text("\n");
  paths->set_attribute("pc_path", pc_path() );
  paths->set_attribute("weights_path", weights_path() );
  paths->set_attribute("mean_path", mean_path());
  paths->set_attribute("scatter_path", scatter_path());
  root->append_data(paths);
  root->append_text("\n");

  bxml_element* neighborhood = new bxml_element("neighborhood");
  neighborhood->append_text("\n");
  neighborhood->set_attribute("min_x", nbbox_.min_point().x());
  neighborhood->set_attribute("min_y", nbbox_.min_point().y());
  neighborhood->set_attribute("min_z", nbbox_.min_point().z());
  neighborhood->set_attribute("max_x", nbbox_.max_point().x());
  neighborhood->set_attribute("max_y", nbbox_.max_point().y());
  neighborhood->set_attribute("max_z", nbbox_.max_point().z());
  root->append_data(neighborhood);
  root->append_text("\n");

  bxml_element* properties = new bxml_element("properties");
  properties->append_text("\n");
  properties->set_attribute("nsamples", nsamples_);
  properties->set_attribute("feature_dim", feature_dim);
  //properties->set_attribute("finest_cell_length", finest_cell_length_);
  properties->set_attribute("nscenes", scenes_.size());
  properties->set_attribute("training_fraction", training_fraction_);

  root->append_data(properties);
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
    scenes_elm->set_attribute("nleaves", nleaves_[i]);
    root->append_data(scenes_elm);
    root->append_text("\n");
  }

  //write training scenes
  bxml_element* train_elm = new bxml_element("training_scenes");
  train_elm->append_text("\n");

  std::stringstream ss;
  unsigned ts = 0;

  for (unsigned i = 0; i< training_scenes_.size(); i++) {
    if (training_scenes_[i]) {
      ss << i << ' ';
      ts++;
    }
  }

  train_elm->set_attribute("nscenes", ts);
  train_elm->append_text(ss.str());
  train_elm->append_text("\n");
  root->append_data(train_elm);
  root->append_text("\n");

  //write to disk
  std::ofstream os(xml_path().c_str());
  bxml_write(os, doc);
  os.close();

  // Write pca main matrices -other matrices aren't class variables and should have been written during computation time
  write_pca_matrices();
}

//: Write a PCA file
template <unsigned feature_dim>
void bvpl_global_pca<feature_dim>::write_pca_matrices()
{
  std::ofstream pc_ofs(pc_path().c_str());
  pc_ofs.precision(15);
  std::ofstream weights_ofs(weights_path().c_str());
  weights_ofs.precision(15);
  std::ofstream mean_ofs(mean_path().c_str());
  mean_ofs.precision(15);
  std::ofstream scatter_ofs(scatter_path().c_str());
  scatter_ofs.precision(15);

  pc_ofs << pc_;
  pc_ofs.close();
  weights_ofs << weights_;
  weights_ofs.close();
  mean_ofs << training_mean_;
  mean_ofs.close();
  scatter_ofs << scatter_;
  scatter_ofs.close();
}


#define BVPL_GLOBAL_PCA(feature_dim) \
template class bvpl_global_pca<feature_dim>;


#endif // bvpl_global_pca_hxx_
