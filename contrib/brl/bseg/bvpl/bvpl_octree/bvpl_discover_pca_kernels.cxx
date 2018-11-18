//:
// \file
// \author Isabel Restrepo
// \date 8-Nov-2010

#include "bvpl_discover_pca_kernels.h"
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>
#include <boxm/boxm_scene_parser.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bvpl_discover_pca_kernels::bvpl_discover_pca_kernels(const std::string &path)
{
  std::cout << "Loading pca info from xml-file" << std::endl;

  path_out_ = path;
  std::ifstream xml_ifs(xml_path().c_str());

  if (!xml_ifs)
    return;
  bxml_document doc = bxml_read(xml_ifs);
  bxml_element query("pca_info");
  bxml_data_sptr root = bxml_find_by_name(doc.root_element(), query);
  if (!root) {
    std::cout << "bvpl_discover_pca_kernels - could not parse xml root\n";
  }

  //Parse neighborhood bounding box - units are number of voxels
  bxml_element nbbox_query("neighborhood");
  bxml_data_sptr nbbox_data = bxml_find_by_name(root, nbbox_query);
  auto* nbbox_elm = dynamic_cast<bxml_element*>(nbbox_data.ptr());
  int min_x=0, min_y=0, min_z=0, max_x=0, max_y=0, max_z =0;
  nbbox_elm->get_attribute("min_x", min_x);
  nbbox_elm->get_attribute("min_y", min_y);
  nbbox_elm->get_attribute("min_z", min_z);
  nbbox_elm->get_attribute("max_x", max_x);
  nbbox_elm->get_attribute("max_y", max_y);
  nbbox_elm->get_attribute("max_z", max_z);

  nbbox_ = vgl_box_3d<int>(vgl_point_3d<int>(min_x, min_y, min_y), vgl_point_3d<int>(max_x, max_y, max_z));
  std::cout << "Neighborhood: " << nbbox_ << std::endl;

  //Parse Number of samples
  bxml_element nsamples_query("samples");
  bxml_data_sptr nsamples_data = bxml_find_by_name(root, nsamples_query);
  auto* nsamples_elm = dynamic_cast<bxml_element*>(nsamples_data.ptr());
  nsamples_elm->get_attribute("nsamples", nsamples_);
  std::cout << "Number of samples: " << nsamples_ << std::endl;

  //Parse dimension
  bxml_element dim_query("dimension");
  bxml_data_sptr dim_data = bxml_find_by_name(root, dim_query);
  auto* dim_elm = dynamic_cast<bxml_element*>(dim_data.ptr());
  dim_elm->get_attribute("feature_dim", feature_dim_);
  std::cout << "Feature dimension: " << feature_dim_ << std::endl;

  //Parse paths and set matrices
  bxml_element paths_query("paths");
  bxml_data_sptr paths_data = bxml_find_by_name(root, paths_query);
  auto* path_elm = dynamic_cast<bxml_element*>(paths_data.ptr());

  int valid = 0;
  std::string ifs_path;

  path_elm->get_attribute("pc_path", ifs_path);
  if (ifs_path != pc_path())
    valid = -1;
  else{
    std::ifstream ifs(ifs_path.c_str());
    ifs >> pc_;
    if (pc_.size()!=feature_dim_*feature_dim_)
      valid = -2;
  }

  path_elm->get_attribute("weights_path", ifs_path);
  if (ifs_path != weights_path())
    valid = -3;
  else{
    std::ifstream ifs(ifs_path.c_str());
    ifs >> weights_;
    if (weights_.size()!=feature_dim_)
      valid = -4;
  }

  path_elm->get_attribute("sample_mean_path", ifs_path);
  if (ifs_path != sample_mean_path())
    valid = -5;
  else
  {
    std::ifstream ifs(ifs_path.c_str());
    ifs >> sample_mean_feature_ ;
    if (sample_mean_feature_.size()!=feature_dim_)
      valid = -6;
  }

  path_elm->get_attribute("data_path", ifs_path);
  if (ifs_path != data_path())
    valid = -7;

  path_elm->get_attribute("positions_path", ifs_path);
  if (ifs_path != pos_path())
    valid = -8;

  path_elm->get_attribute("scene_path", scene_path_);

  //load scene
  boxm_scene_parser parser;
  data_scene_base_=new boxm_scene_base();
  data_scene_base_->load_scene(scene_path_, parser);

  //cast scene
  auto *scene= new boxm_scene<boct_tree<short, float > >();
  if (data_scene_base_->appearence_model() == BOXM_FLOAT){
    scene->load_scene(parser);
    data_scene_base_ = scene;
  }

  path_elm->get_attribute("data_mean_path", ifs_path);
  if (ifs_path != data_mean_path()){
    compute_mean_feature(scene);
    valid = -9;
    std::cout << "bvpl_discover_pca_kernels - errors parsing data_mean_feature.xml\n";
  }
  else
  {
    std::ifstream ifs(ifs_path.c_str());
    ifs >> data_mean_feature_ ;
    if (data_mean_feature_.size()!=feature_dim_)
      valid = -10;
  }

  //Parse dimension
  bxml_element cell_length_query("finest_cell_length");
  bxml_data_sptr cell_length_data = bxml_find_by_name(root, cell_length_query);
  auto* cell_length_elm = dynamic_cast<bxml_element*>(cell_length_data.ptr());
  bool has_length = cell_length_elm->get_attribute("length", finest_cell_length_);
  if (!has_length || finest_cell_length_ < 0.0){
     std::cout << "Error: Finest cell length read from file: " << finest_cell_length_ << " has length " << has_length << std::endl;
     finest_cell_length_ = scene->finest_cell_length();
     valid = -11;
  }
  std::cout << "Finest cell length: " << finest_cell_length_ << std::endl;

  if (valid<0){
    std::cout << "bvpl_discover_pca_kernels - errors parsing pca_info.xml. Error code: " << valid << std::endl;
    xml_write();
  }
}


//: Set Up the PCA matrix for the given scene
void bvpl_discover_pca_kernels::set_up_pca_svd(boxm_scene<boct_tree<short,float> > *scene)
{
  //zero-mean feature matrix
  vnl_matrix<double> M = this->compute_deviation_matrix(scene);

  vnl_svd<double> A(M);

  //columns of U contain principal components.
  pc_.set_size(feature_dim_, feature_dim_);
  A.U().extract(pc_);
  weights_.set_size(feature_dim_);
  weights_ = A.W().diagonal().extract(feature_dim_);

  weights_ = element_product(weights_, weights_);

#ifdef DEBUG
  std::cerr << "Scatter Matix svd" << M*M.transpose() << '\n'
           << "PC Matix SVD:\n Size: " << pc_.size() << '\n' <<pc_ << '\n'
           << "Evals SVD:\nSize: " << weights_.size() << '\n' << weights_ << '\n';
  bvpl_write_pca(path_out_ + "/pca_matrices", M, weights_, pc_);
#endif
}


//: Set Up the PCA matrix for the given scene
//  This method performs eigen value decomposition on the scatter matrix
//  Use this method when number of samples >> number of dimensions
void bvpl_discover_pca_kernels::set_up_pca_evd(boxm_scene<boct_tree<short,float> > *scene)
{
  //zero-mean feature matrix
  vnl_matrix<double> S = this->compute_scatter_matrix(scene);

  // Compute eigenvectors(principal components) and values of S
  pc_.set_size(feature_dim_,feature_dim_);
  weights_.set_size(feature_dim_);

  vnl_symmetric_eigensystem_compute(S, pc_, weights_);

  pc_.fliplr();
  weights_.flip();

#ifdef DEBUG
  std::cerr << "Scatter Matix" << S << '\n'
           << "PC Matix EVD:\n Size: " << pc_.size() << '\n' <<pc_ << '\n'
           << "EVals EVD:\n Size: " << weights_.size() << '\n' <<weights_ << '\n';
  bvpl_write_pca(path_out_ + "/pca_evd", S, weights_, pc_);
#endif
}


//: Ramndom samples according to octree structure.
//  Sampling is achieved by generating uniform random cell samples.
//  Since there are more cells where the resolution is finer, then these regions get sampled more often
vnl_matrix<double> bvpl_discover_pca_kernels::compute_scatter_matrix( boxm_scene<boct_tree<short, float> > *scene)
{
  //init variables
  double scene_ncells = scene->size();
  scene->unload_active_blocks();
  sample_mean_feature_.fill(0.0f);
  vnl_matrix<double> S(feature_dim_, feature_dim_, 0.0f);
  vnl_random rng;
  std::ofstream pos_ofs(pos_path().c_str());
  std::ofstream data_ofs(data_path().c_str());
  data_ofs.precision(15);

  // 1. Traverse through the blocks/trees
  boxm_block_iterator<boct_tree<short,float> > it = scene->iterator();
  unsigned long long nfeature = 1;
  for (it.begin(); !it.end(); ++it)
  {
    scene->load_block_and_neighbors(it.index());
    boct_tree<short, float>   *tree = (*it)->get_tree();
    assert(tree != nullptr);

    //2. Sample cells from this tree. The number of samples from this tree depends on the portion of scene cells that live in this tree
    std::vector<boct_tree_cell<short, float> *> leaf_cells = tree->leaf_cells();
    int tree_ncells = leaf_cells.size();
    auto tree_nsamples = (unsigned long)((float)tree_ncells/scene_ncells*nsamples_);
#ifdef DEBUG
    std::cout <<"Tree nsamples is: " << tree_nsamples << '\n'
             <<" nsamples is: " << nsamples_ <<std::endl;
#endif
    for (unsigned i=0; i<tree_nsamples; ++i)
    {
      unsigned long sample = rng.lrand32(tree_ncells-1);

      boct_tree_cell<short, float> *center_cell = leaf_cells[sample];
      vgl_point_3d<double> center_cell_centroid = tree->global_centroid(center_cell);

      //CAUTION: the neighborhood box was suppossed to be defined as number of regular neighbors
      //convert neighborhood box to scene coordinates
      vgl_point_3d<int> nmin = nbbox_.min_point();
      vgl_point_3d<int> nmax = nbbox_.max_point();

      vgl_box_3d<double> roi(finest_cell_length_*(double)nmin.x(),finest_cell_length_*(double)nmin.y(),finest_cell_length_*(double)nmin.z(),
                             finest_cell_length_*(double)nmax.x(),finest_cell_length_*(double)nmax.y(),finest_cell_length_*(double)nmax.z());
      roi.set_centroid(center_cell_centroid);

      //if neighborhood is not inclusive we would have missing features
      if (!((scene->get_world_bbox()).contains(roi))){
        --i;
        continue;
      }

      //3. Assemble neighborhood as a feature-vector
      vnl_vector<double> this_feature(feature_dim_, 0.0f);

      unsigned curr_dim = 0;
      for (int z = nbbox_.min_z(); z<=nbbox_.max_z(); ++z)
        for (int y = nbbox_.min_y(); y<=nbbox_.max_y(); ++y)
          for (int x = nbbox_.min_x(); x<=nbbox_.max_x(); ++x)
          {
            vgl_point_3d<double> neighbor_centroid(center_cell_centroid.x() + (double)x*finest_cell_length_,
                                                   center_cell_centroid.y() + (double)y*finest_cell_length_,
                                                   center_cell_centroid.z() + (double)z*finest_cell_length_);

            boct_tree_cell<short,float> *neighbor_cell = scene->locate_point_in_memory(neighbor_centroid);

            assert(neighbor_cell !=nullptr);
            this_feature[curr_dim] = (double)neighbor_cell->data();
            ++curr_dim;
          }

      assert(curr_dim == feature_dim_);

      pos_ofs << center_cell_centroid <<'\n' ;
      data_ofs << this_feature << '\n';

      //increment weights
      double rho = 1.0/(double)nfeature;
      double rho_comp = 1.0 - rho;

      // the difference vector between the sample and the mean
      vnl_vector<double> diff = this_feature - sample_mean_feature_;

      //update the covariance
      S += rho_comp*outer_product(diff,diff);

      //update the mean
      sample_mean_feature_ += (rho*diff);

      ++nfeature;
#ifdef DEBUG
      std::cerr << "Feature EVD: " <<this_feature << '\n'
               << "Mean Feature EVD: " <<mean_feature << '\n';
#endif
    }
  } //end of block-loop

  pos_ofs.close();
  data_ofs.close();
  std::ofstream mean_ofs(sample_mean_path().c_str());
  mean_ofs.precision(15);
  mean_ofs << sample_mean_feature_;
  mean_ofs.close();

  nsamples_ = nfeature-1;

  return S;
}


//: Samples according to octree structure.
//  Sampling is achieved by generating uniform random cell samples.
//  Since there are more cells where the resolution is finer, then these regions get sampled more often
vnl_matrix<double> bvpl_discover_pca_kernels::compute_deviation_matrix( boxm_scene<boct_tree<short, float> > *scene)
{
  //init variables
  unsigned long scene_ncells = scene->size();
  sample_mean_feature_.fill(0.0f);
  vnl_matrix<double> M(feature_dim_, (unsigned int)nsamples_);
  vnl_random rng(9667566ul);
  std::ofstream pos_ofs(pos_path().c_str());

  // 1. Traverse through the blocks/trees
  boxm_block_iterator<boct_tree<short,float> > it = scene->iterator();
  unsigned long feature_col = 0;
  for (it.begin(); !it.end(); ++it)
  {
    scene->load_block_and_neighbors(it.index());
    boct_tree<short, float>   *tree = (*it)->get_tree();
    assert(tree != nullptr);

    //2. Sample cells from this tree. The number of samples from this tree depends on the portion of scene cells that live in this tree
    std::vector<boct_tree_cell<short, float> *> leaf_cells = tree->leaf_cells();
    int tree_ncells = leaf_cells.size();
    auto tree_nsamples = (unsigned long)(tree_ncells*nsamples_/scene_ncells); // possible overflow ...

    for (unsigned int i=0; i<tree_nsamples; ++i)
    {
      unsigned long sample = rng.lrand32(tree_ncells-1);

      boct_tree_cell<short, float> *center_cell = leaf_cells[sample];
      vgl_point_3d<double> center_cell_centroid = tree->global_centroid(center_cell);

      //CAUTION: the neighborhood box was suppossed to be defined as number of regular neighbors
      //convert neighborhood box to scene coordinates
      vgl_point_3d<int> nmin = nbbox_.min_point();
      vgl_point_3d<int> nmax = nbbox_.max_point();

      vgl_box_3d<double> roi(finest_cell_length_*(double)nmin.x(),finest_cell_length_*(double)nmin.y(),finest_cell_length_*(double)nmin.z(),
                             finest_cell_length_*(double)nmax.x(),finest_cell_length_*(double)nmax.y(),finest_cell_length_*(double)nmax.z());
      roi.set_centroid(center_cell_centroid);

      //if neighborhood is not inclusive we would have missing features
      if (!((scene->get_world_bbox()).contains(roi))){
        --i;
        continue;
      }

      //3. Assemble neighborhood as a feature-vector
      vnl_vector<double> this_feature(feature_dim_, 0.0f);

      unsigned curr_dim = 0;
      for (int z = nbbox_.min_z(); z<=nbbox_.max_z(); ++z)
        for (int y = nbbox_.min_y(); y<=nbbox_.max_y(); ++y)
          for (int x = nbbox_.min_x(); x<=nbbox_.max_x(); ++x)
          {
            vgl_point_3d<double> neighbor_centroid(center_cell_centroid.x() + (double)x*finest_cell_length_,
                                                   center_cell_centroid.y() + (double)y*finest_cell_length_,
                                                   center_cell_centroid.z() + (double)z*finest_cell_length_);

            boct_tree_cell<short,float> *neighbor_cell = scene->locate_point_in_memory(neighbor_centroid);

            assert(neighbor_cell !=nullptr);
            this_feature[curr_dim] = (double)neighbor_cell->data();
            ++curr_dim;
          }

      assert(curr_dim == feature_dim_);

      pos_ofs << center_cell_centroid <<'\n' ;

      M.set_column(feature_col,this_feature);
      sample_mean_feature_+=this_feature;
      ++feature_col;
#ifdef DEBUG
      std::cerr << "Feature SVD: " <<this_feature << '\n'
               << "Mean Feature SVD: " <<mean_feature/(double)feature_col << '\n';
#endif
    }
  } //end of block-loop

  //the actual number of samples is different from desired number of smaples because only integer samples are drawn from each tree
  vnl_matrix<double> F(feature_dim_, feature_col);

  M.extract(F);

  //4. Normalize feature vector
  sample_mean_feature_/=feature_col;
#ifdef DEBUG
  std::cerr << "Mean Feature SVD: " <<mean_feature << '\n';
#endif
  std::ofstream mean_ofs(sample_mean_path().c_str());
  mean_ofs.precision(15);
  mean_ofs << sample_mean_feature_;
  mean_ofs.close();

  for (unsigned int j=0; j<feature_col; ++j)
    F.set_column(j,F.get_column(j) - sample_mean_feature_);

  return F;
}


//: Project training samples onto pca space and return error as a function of number of components used
void bvpl_discover_pca_kernels::compute_training_error(vnl_vector<double> &proj_error)
{
  std::ifstream data_ifs(data_path().c_str());
  std::cerr << data_path() << '\n';
  //a vector to keep projection error - first element refers to error when using only first pc,
  //the sencond elemend, to error when projecting on frist 2 components and so on
  proj_error.set_size(feature_dim_);
  proj_error.fill(0.0);

  //For all samples
  for (unsigned long i = 0; i < nsamples_; ++i)
  {
    if (data_ifs.eof())
      continue;

    //retrieve the data from file
    vnl_vector<double> norm_feature(feature_dim_, 0.0);
    data_ifs >> norm_feature;
#ifdef DEBUG
    std::cerr << "Feature: " <<norm_feature << '\n';
#endif
    norm_feature-=sample_mean_feature_;

    //solve for the coefficients
    vnl_vector<double> a(feature_dim_, 0.0);
    a = pc_.transpose() * (norm_feature);

    //project as a function of number of components
    for (unsigned c = 0; c < feature_dim_; ++c)
    {
      //reconstruct
      vnl_vector<double> norm_feature_approx  = pc_.extract(feature_dim_, c+1) * a.extract(c+1);
      vnl_vector<double> rec_feature = pc_*a;
      //compute error
      proj_error[c]+= (norm_feature - norm_feature_approx).squared_magnitude();
    }
  }
}


//: Project training samples onto pca space and return error as a function of number of components used
void bvpl_discover_pca_kernels::compute_normalized_training_error(vnl_vector<double> &proj_error)
{
  theoretical_training_error(proj_error);
  proj_error /= (double)nsamples_;
}

//: Project training samples onto pca space and return error as a function of number of components used
void bvpl_discover_pca_kernels::theoretical_training_error(vnl_vector<double> &proj_error)
{
  //a vector to keep projection error - first element refers to error when using only first pc,
  //the sencond elemend, to error when projecting on frist 2 components and so on
  proj_error.set_size(feature_dim_);
  proj_error.fill(0.0);

  for (unsigned i=0; i<weights_.size(); ++i)
  {
    double error = weights_.extract(weights_.size() - (i+1), i+1).sum(); // * 0.5
    proj_error[i] = error;
  }
}


//: Project test samples onto pca space and return error as a function of number of components used
void bvpl_discover_pca_kernels::compute_testing_error(vnl_vector<double> &proj_error)
{
  //a vector to keep projection error - first element refers to error when using only first pc,
  //the sencond element, to error when projecting on frist 2 components and so on
  proj_error.set_size(feature_dim_);
  proj_error.fill(0.0);

  //cast scene
  auto *scene= dynamic_cast<boxm_scene<boct_tree<short, float > > *> (data_scene_base_.as_pointer());

  compute_mean_feature(scene);

  //iterate through leaves and arrage as vectors.
  boxm_cell_iterator<boct_tree<short, float > > cell_it = scene->cell_iterator(&boxm_scene<boct_tree<short, float> >::load_block_and_neighbors, true);
  cell_it.begin();

  while (!(cell_it.end()))
  {
    //boct_tree_cell<short, float> *center_cell = *cell_it;
    vgl_point_3d<double> center_cell_centroid = cell_it.global_centroid();

    //CAUTION: the neighborhood box was suppossed to be defined as number of regular neighbors
    //convert neighborhood box to scene coordinates
    vgl_point_3d<int> nmin = nbbox_.min_point();
    vgl_point_3d<int> nmax = nbbox_.max_point();

    vgl_box_3d<double> roi(finest_cell_length_*(double)nmin.x(),finest_cell_length_*(double)nmin.y(),finest_cell_length_*(double)nmin.z(),
                           finest_cell_length_*(double)nmax.x(),finest_cell_length_*(double)nmax.y(),finest_cell_length_*(double)nmax.z());
    roi.set_centroid(center_cell_centroid);

    //if neighborhood is not inclusive we would have missing features
    if (!((scene->get_world_bbox()).contains(roi))){
      ++cell_it;
      continue;
    }

    //3. Assemble neighborhood as a feature-vector
    vnl_vector<double> this_feature(feature_dim_, 0.0f);

    unsigned curr_dim = 0;
    for (int z = nbbox_.min_z(); z<=nbbox_.max_z(); ++z)
      for (int y = nbbox_.min_y(); y<=nbbox_.max_y(); ++y)
        for (int x = nbbox_.min_x(); x<=nbbox_.max_x(); ++x)
        {
          vgl_point_3d<double> neighbor_centroid(center_cell_centroid.x() + (double)x*finest_cell_length_,
                                                 center_cell_centroid.y() + (double)y*finest_cell_length_,
                                                 center_cell_centroid.z() + (double)z*finest_cell_length_);

          boct_tree_cell<short,float> *neighbor_cell = scene->locate_point_in_memory(neighbor_centroid);

          assert(neighbor_cell !=nullptr);
          this_feature[curr_dim] = (double)neighbor_cell->data();
          ++curr_dim;
        }

    assert(curr_dim == feature_dim_);

    this_feature-=data_mean_feature_;

    //solve for the coefficients
    vnl_vector<double> a(feature_dim_, 0.0);
    a = pc_.transpose() * (this_feature);

#ifdef DEBUG
    std::cout <<"This feature is " << this_feature <<std::endl;
#endif

    //project as a function of number of components
    for (unsigned c = 0; c < feature_dim_; ++c)
    {
      //reconstruct
      vnl_vector<double> feature_approx  = pc_.extract(feature_dim_, c+1) * a.extract(c+1);
      vnl_vector<double> rec_feature = pc_*a;
#ifdef DEBUG
      std::cout <<"Feature approx at c = " << c << " is " <<  feature_approx <<std::endl;
#endif
      //compute error
      proj_error[c]+= (this_feature - feature_approx).squared_magnitude();
    }

    ++cell_it;
  }
#if 0
  vnl_vector<double> training_error;
  this->theoretical_training_error(training_error);
  proj_error-=training_error;
#endif
}


//: Reconstructions error on testing samples. By block. Error is given as average error per sample
void bvpl_discover_pca_kernels::compute_testing_error(const boxm_scene_base_sptr& error_scene_base, unsigned ncomponents,
                                                      int block_i, int block_j, int block_k)
{
  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;

  //cast the scenes
  auto* data_scene = dynamic_cast<boxm_scene<float_tree_type>* > (data_scene_base_.as_pointer());
  auto * error_scene = dynamic_cast<boxm_scene<float_tree_type>* > (error_scene_base.as_pointer());

  if (!(data_scene &&error_scene)){
    std::cerr << "Error in bvpl_discover_pca_kernels::compute_testing_error: Faild to cast scene\n";
    return;
  }

  //get the cells for this block
  if (!(data_scene->valid_index(block_i, block_j, block_k) && error_scene->valid_index(block_i, block_j, block_k))){
    std::cerr << "In compute_testing_error: Invalid block\n";
    return;
  }
  data_scene->unload_active_blocks();
  error_scene->unload_active_blocks();
  data_scene->load_block_and_neighbors(block_i, block_j, block_k);
  error_scene->load_block(block_i, block_j, block_k);

  //get the trees
  float_tree_type* data_tree = data_scene->get_block(block_i, block_j, block_k)->get_tree();
  float_tree_type* error_tree = data_tree->clone();
  //error is always positive, therefore cells with negative errors can be identified as uninitialized.
  error_tree->init_cells(-1.0f);

  //get the leaves
  std::vector<float_cell_type*> data_leaves = data_tree->leaf_cells();
  std::vector<float_cell_type*> error_leaves = error_tree->leaf_cells();

  //CAUTION: the neighborhood box was suppossed to be defined as number of regular neighbors
  //convert neighborhood box to scene coordinates
  vgl_point_3d<int> min_point = nbbox_.min_point();
  vgl_point_3d<int> max_point = nbbox_.max_point();

  for (unsigned i=0; i<data_leaves.size(); ++i)
  {
    float_cell_type* data_cell = data_leaves[i];

    //create a region around the center cell
    vgl_point_3d<double> centroid = data_tree->global_centroid(data_cell);

    //change the coordinates of enpoints to be in global coordinates abd text if they are contained in the scene
    vgl_point_3d<double> min_point_global(centroid.x() + (double)min_point.x()*finest_cell_length_, centroid.y() + (double)min_point.y()*finest_cell_length_, centroid.z() + (double)min_point.z()*finest_cell_length_);
    vgl_point_3d<double> max_point_global(centroid.x() + (double)max_point.x()*finest_cell_length_, centroid.y() + (double)max_point.y()*finest_cell_length_, centroid.z() + (double)max_point.z()*finest_cell_length_);
    if (!(data_scene->locate_point_in_memory(min_point_global) && data_scene->locate_point_in_memory(max_point_global)))
      continue;

    //3. Assemble neighborhood as a feature-vector
    vnl_vector<double> this_feature(feature_dim_, 0.0f);

    unsigned curr_dim = 0;
    for (int z = nbbox_.min_z(); z<=nbbox_.max_z(); ++z)
      for (int y = nbbox_.min_y(); y<=nbbox_.max_y(); ++y)
        for (int x = nbbox_.min_x(); x<=nbbox_.max_x(); ++x)
        {
          vgl_point_3d<double> neighbor_centroid(centroid.x() + (double)x*finest_cell_length_,
                                                 centroid.y() + (double)y*finest_cell_length_,
                                                 centroid.z() + (double)z*finest_cell_length_);

          boct_tree_cell<short,float> *neighbor_cell = data_scene->locate_point_in_memory(neighbor_centroid);

          if (!neighbor_cell){
            std::cerr << "Error in compute_testing_error\n";
            return;
          }

          this_feature[curr_dim] = (double)neighbor_cell->data();
          ++curr_dim;
        }

    if (curr_dim != feature_dim_){
      std::cerr << "Error in compute_testing_error\n";
      return;
    }
    this_feature-=data_mean_feature_;

    //solve for the coefficients
    vnl_vector<double> a(feature_dim_, 0.0);
    a = pc_.transpose() * (this_feature);

    //project as a function of number of components
    vnl_vector<double> feature_approx  = pc_.extract(feature_dim_, ncomponents) * a.extract(ncomponents);

    //compute error
    error_leaves[i]->set_data((float)((this_feature - feature_approx).squared_magnitude()));
  }

  // write and release memory
  error_scene->get_block(block_i, block_j, block_k)->init_tree(error_tree);
  error_scene->write_active_block();
  data_scene->unload_active_blocks();
  error_scene->unload_active_blocks();
}


#if BVPL_OCTREE_HAS_PTHREADS
void bvpl_discover_pca_kernels::compute_testing_error_thread_safe(boxm_scene<boct_tree<short,float> > * error_scene,
                                                                  unsigned ncomponents,
                                                                  int block_i, int block_j, int block_k)
{
  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;

  //cast the scenes
  auto* data_scene = dynamic_cast<boxm_scene<float_tree_type>* > (data_scene_base_.as_pointer());

  if (!(data_scene &&error_scene)){
    std::cerr << "Error in bvpl_discover_pca_kernels::compute_testing_error: Faild to cast scene\n";
    return;
  }

  if (!(data_scene->load_all_blocks() && error_scene->load_all_blocks())){
    std::cerr << "Must load all blocks into memory before using threads\n";
    return;
  }
  //get the cells for this block
  if (!error_scene->valid_index(block_i, block_j, block_k)){
    std::cerr << "In compute_testing_error: Invalid block\n";
    return;
  }

  //error_scene->load_block_thread_safe(block_i, block_j, block_k);

  //get the trees
  boxm_block<float_tree_type> *data_block = data_scene->get_block(block_i, block_j, block_k);
  if (!data_block)
    return;

  float_tree_type* data_tree = data_block->get_tree();
  float_tree_type* error_tree = error_scene->get_block(block_i, block_j, block_k)->get_tree();
  //error is always positive, therefore cells with negative errors can be identified as uninitialized.
  // error_tree->init_cells(-1.0f);

  //get the leaves
  std::vector<float_cell_type*> data_leaves = data_tree->leaf_cells();
  //std::vector<float_cell_type*> error_leaves = error_tree->leaf_cells();

  //CAUTION: the neighborhood box was suppossed to be defined as number of regular neighbors
  //convert neighborhood box to scene coordinates
  vgl_point_3d<int> min_point = nbbox_.min_point();
  vgl_point_3d<int> max_point = nbbox_.max_point();

  for (auto data_cell : data_leaves)
  {
    //create a region around the center cell
    vgl_point_3d<double> centroid = data_tree->global_centroid(data_cell);

    //change the coordinates of enpoints to be in global coordinates abd text if they are contained in the scene
    vgl_point_3d<double> min_point_global(centroid.x() + (double)min_point.x()*finest_cell_length_, centroid.y() + (double)min_point.y()*finest_cell_length_, centroid.z() + (double)min_point.z()*finest_cell_length_);
    vgl_point_3d<double> max_point_global(centroid.x() + (double)max_point.x()*finest_cell_length_, centroid.y() + (double)max_point.y()*finest_cell_length_, centroid.z() + (double)max_point.z()*finest_cell_length_);
    if (!(data_scene->locate_point_in_memory(min_point_global) && data_scene->locate_point_in_memory(max_point_global)))
      continue;

    //3. Assemble neighborhood as a feature-vector
    vnl_vector<double> this_feature(feature_dim_, 0.0f);

    unsigned curr_dim = 0;
    for (int z = nbbox_.min_z(); z<=nbbox_.max_z(); ++z)
      for (int y = nbbox_.min_y(); y<=nbbox_.max_y(); ++y)
        for (int x = nbbox_.min_x(); x<=nbbox_.max_x(); ++x)
        {
          vgl_point_3d<double> neighbor_centroid(centroid.x() + (double)x*finest_cell_length_,
                                                 centroid.y() + (double)y*finest_cell_length_,
                                                 centroid.z() + (double)z*finest_cell_length_);

          boct_tree_cell<short,float> *neighbor_cell = data_scene->locate_point_in_memory(neighbor_centroid);

          if (!neighbor_cell){
            std::cerr << "Error in compute_testing_error\n";
            return;
          }

          this_feature[curr_dim] = (double)neighbor_cell->data();
          ++curr_dim;
        }

    if (curr_dim != feature_dim_){
      std::cerr << "Error in compute_testing_error\n";
      return;
    }
    this_feature-=data_mean_feature_;

    //solve for the coefficients
    vnl_vector<double> a(feature_dim_, 0.0);
    a = pc_.transpose() * (this_feature);

    //project as a function of number of components
    vnl_vector<double> feature_approx  = pc_.extract(feature_dim_, ncomponents) * a.extract(ncomponents);

    //compute error
    //error_leaves[i]->set_data((float)((this_feature - feature_approx).squared_magnitude()));
  }

  // write and release memory
  error_scene->get_block(block_i, block_j, block_k)->init_tree(error_tree);
  error_scene->write_block_thread_safe(block_i, block_j, block_k);
}
#endif // BVPL_OCTREE_HAS_PTHREADS


//: Compute mean feature of all "leaf-features" in a scene
void bvpl_discover_pca_kernels::compute_mean_feature(boxm_scene<boct_tree<short, float> > *scene)
{
  //a vector to keep projection error - first element refers to error when using only first pc,
  //the sencond element, to error when projecting on frist 2 components and so on
  data_mean_feature_.set_size(feature_dim_);
  data_mean_feature_.fill(0.0);
  unsigned long nfeatures = 0;

  //iterate through leaves and arrage as vectors.
  double cell_length = scene->finest_cell_length();

  boxm_cell_iterator<boct_tree<short, float > > cell_it = scene->cell_iterator(&boxm_scene<boct_tree<short, float> >::load_block_and_neighbors, true);
  cell_it.begin();

  while (!(cell_it.end()))
  {
    //boct_tree_cell<short, float> *center_cell = *cell_it;
    vgl_point_3d<double> center_cell_centroid = cell_it.global_centroid();

    //CAUTION: the neighborhood box was suppossed to be defined as number of regular neighbors
    //convert neighborhood box to scene coordinates
    vgl_point_3d<int> nmin = nbbox_.min_point();
    vgl_point_3d<int> nmax = nbbox_.max_point();

    vgl_box_3d<double> roi(cell_length*(double)nmin.x(),cell_length*(double)nmin.y(),cell_length*(double)nmin.z(),
                           cell_length*(double)nmax.x(),cell_length*(double)nmax.y(),cell_length*(double)nmax.z());
    roi.set_centroid(center_cell_centroid);

    //if neighborhood is not inclusive we would have missing features
    if (!((scene->get_world_bbox()).contains(roi))){
      ++cell_it;
      continue;
    }

    //3. Assemble neighborhood as a feature-vector
    vnl_vector<double> this_feature(feature_dim_, 0.0f);

    unsigned curr_dim = 0;
    for (int z = nbbox_.min_z(); z<=nbbox_.max_z(); ++z)
      for (int y = nbbox_.min_y(); y<=nbbox_.max_y(); ++y)
        for (int x = nbbox_.min_x(); x<=nbbox_.max_x(); ++x)
        {
          vgl_point_3d<double> neighbor_centroid(center_cell_centroid.x() + (double)x*cell_length,
                                                 center_cell_centroid.y() + (double)y*cell_length,
                                                 center_cell_centroid.z() + (double)z*cell_length);

          boct_tree_cell<short,float> *neighbor_cell = scene->locate_point_in_memory(neighbor_centroid);

          assert(neighbor_cell !=nullptr);
          this_feature[curr_dim] = (double)neighbor_cell->data();
          ++curr_dim;
        }

    assert(curr_dim == feature_dim_);

    data_mean_feature_+=this_feature;
    ++nfeatures;
    ++cell_it;
  }
  data_mean_feature_ = data_mean_feature_/(double)nfeatures;

  std::ofstream mean_ofs(data_mean_path().c_str());
  mean_ofs.precision(15);
  mean_ofs << data_mean_feature_;
  mean_ofs.close();
}


#if 0 //too_slow
      //: Compute deviations i.e. zero-mean feature matrix
vnl_matrix<float>& bvpl_discover_pca_kernels::compute_deviation_matrix(boxm_scene<boct_tree<short,float> > *scene)
{
  unsigned long max_nsamples = scene->size();

  //iterate through leaves, keep random samples in memory and arrage as vectors.
  boxm_cell_iterator<boct_tree<short, float > > cell_it = scene->cell_iterator(&boxm_scene<boct_tree<short, float> >::load_block_and_neighbors, true);
  cell_it.begin();

  vnl_random rng(9667566ul);
  unsigned long pos_pre = 0;  //previous position on random samples array

  for (unsigned i=1; i<nsamples_; ++i)
  {
    unsigned long sample = rng.lrand32(max_nsamples);
    vgl_box_3d<double> roi = nbbox_;
    for (unsigned u = 0; u + pos_pre < sample; ++u)
      ++cell_it;
    pos_pre = sample;
    roi.set_centroid(cell_it.global_centroid());
    if (!((scene->get_world_bbox()).contains(roi))){
      --i;
      continue;
    }

    positions_.push_back(sample);
  }

  //sort random numbers for easy access
  positions_.sort();
  if (nsamples_ != positions_.size())
  {
    std::cerr << "Error computing PCA features: Wrong number of random samples generated\n";
    return false;
  }

  cell_it.begin();

  //init cells array to first random element
  std::list<unsigned long>::const_iterator rand_it = positions_.begin();
  cell_it+=(*rand_it - pos_pre);
  pos_pre = *rand_it;

  //init quantities for mean
  unsigned feature_dim = nbbox_.width()*nbbox_.height()* nbbox_.depth();
  vnl_vector<float> mean(feature_dim);
  M.set_size(feature_dim, nsamples_);
  unsigned i=0;

  double cell_length = scene->finest_cell_length();
  while (!(cell_it.end()))
  {
    boct_tree_cell<short, float> *center_cell = *cell_it;
    vgl_point_3d<double> center_cell_centroid = cell_it.global_centroid();

    if (!center_cell){
      std::cerr << "Error is bvpl_discover_pca_kernels: Unexpected NULL cell\n";
      return false;
    }

    //assemble neighborhood as a feature-vector
    vnl_vector<float> this_feature(feature_dim);

    unsigned curr_dims = 0;
    for (int z = nbbox_.min_z(); z<=nbbox_.max_z(); ++z)
      for (int y = nbbox_.min_y(); y<=nbbox_.max_y(); ++y)
        for (int x = nbbox_.min_x(); x<=nbbox_.max_x(); ++x)
        {
          vgl_point_3d<double> neighbor_origin(center_cell_centroid.x() + (double)x*cell_length,
                                               center_cell_centroid.y() + (double)y*cell_length,
                                               center_cell_centroid.z() + (double)z*cell_length);

          boct_tree_cell<short,float> *neighbor_cell = scene->locate_point_in_memory(neighbor_origin);

          if (!neighbor_cell){
            std::cerr << "Error is bvpl_discover_pca_kernels: Unexpected NULL cell\n";
            return false;
          }

          this_feature[x + nbbox_.width()*y + nbbox_.width()*nbbox_.height()*z] = neighbor_cell->data();
          ++curr_dims;
        }

    M.set_column(i,this_feature);
    mean+=this_feature;

    //increment iterators
    cell_it+=(*rand_it - pos_pre);
    pos_pre = *rand_it;
    ++rand_it;
    ++i;
  }

  mean/=nsamples_;

  for (unsigned int j=0; j<nsamples_; ++j)
    M.set_column(j,M.get_column(j) - mean);

  return true;
}
#endif

void bvpl_discover_pca_kernels::xml_write()
{
  bxml_document doc;
  bxml_element *root = new bxml_element("pca_info");
  doc.set_root_element(root);
  root->append_text("\n");

  bxml_element* paths = new bxml_element("paths");
  paths->append_text("\n");
  paths->set_attribute("pc_path", pc_path() );
  paths->set_attribute("data_path", data_path() );
  paths->set_attribute("weights_path", weights_path() );
  paths->set_attribute("positions_path", pos_path()) ;
  paths->set_attribute("sample_mean_path", sample_mean_path());
  paths->set_attribute("data_mean_path", data_mean_path());
  paths->set_attribute("scene_path", scene_path_);
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

  bxml_element* nsamples = new bxml_element("samples");
  nsamples->append_text("\n");
  nsamples->set_attribute("nsamples", nsamples_);
  root->append_data(nsamples);
  root->append_text("\n");

  bxml_element* dim = new bxml_element("dimension");
  dim->append_text("\n");
  dim->set_attribute("feature_dim", feature_dim_);
  root->append_data(dim);
  root->append_text("\n");

  bxml_element* cell_length = new bxml_element("finest_cell_length");
  cell_length->append_text("\n");
  cell_length->set_attribute("length", finest_cell_length_);
  root->append_data(cell_length);
  root->append_text("\n");

  std::ofstream os(xml_path().c_str());
  bxml_write(os, doc);
  os.close();

  //: Write pca main matrices -other matrices aren't class variables and should have been written during computation time

  write_pca_matrices();
}


//: Write a PCA file
bool bvpl_discover_pca_kernels::write_pca_matrices()
{
  std::ofstream pc_ofs(pc_path().c_str());
  pc_ofs.precision(15);
  std::ofstream weights_ofs(weights_path().c_str());
  weights_ofs.precision(15);

  pc_ofs << pc_;
  pc_ofs.close();
  weights_ofs << weights_;
  weights_ofs.close();

  return true;
}
