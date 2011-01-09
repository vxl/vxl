//:
// \file
// \author Isabel Restrepo
// \date 8-Nov-2010

#include "bvpl_discover_pca_kernels.h"
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>
#include <vcl_cassert.h>

bvpl_discover_pca_kernels::bvpl_discover_pca_kernels(const vcl_string &path)
{
  path_out_ = path;
  vcl_ifstream xml_ifs(xml_path().c_str());

  if (!xml_ifs)
    return;
  bxml_document doc = bxml_read(xml_ifs);
  bxml_element query("pca_info");
  bxml_data_sptr root = bxml_find_by_name(doc.root_element(), query);
  if (!root) {
    vcl_cout << "bvpl_discover_pca_kernels - could not parse xml root\n";
  }

  //Parse Number of samples
  bxml_element nsamples_query("samples");
  bxml_data_sptr nsamples_data = bxml_find_by_name(root, nsamples_query);
  bxml_element* nsamples_elm = dynamic_cast<bxml_element*>(nsamples_data.ptr());
  nsamples_elm->get_attribute("nsamples", nsamples_);

  //Parse dimension
  bxml_element dim_query("dimension");
  bxml_data_sptr dim_data = bxml_find_by_name(root, dim_query);
  bxml_element* dim_elm = dynamic_cast<bxml_element*>(dim_data.ptr());
  dim_elm->get_attribute("feature_dim", feature_dim_);

  //Parse paths and set matrices
  bxml_element paths_query("paths");
  bxml_data_sptr paths_data = bxml_find_by_name(root, paths_query);
  bxml_element* path_elm = dynamic_cast<bxml_element*>(paths_data.ptr());

  bool valid = true;
  vcl_string ifs_path;

  path_elm->get_attribute("pc_path", ifs_path);
  if (ifs_path != pc_path())
    valid = false;
  else{
    vcl_ifstream ifs(ifs_path.c_str());
    ifs >> pc_;
    if (pc_.size()!=feature_dim_*feature_dim_)
      valid = false;
  }

  path_elm->get_attribute("weights_path", ifs_path);
  if (ifs_path != weights_path())
    valid = false;
  else{
    vcl_ifstream ifs(ifs_path.c_str());
    ifs >> weights_;
    if (weights_.size()!=feature_dim_)
      valid = false;
  }

  path_elm->get_attribute("mean_path", ifs_path);
  if (ifs_path != mean_path())
    valid = false;
  else
  {
    vcl_ifstream ifs(ifs_path.c_str());
    ifs >> mean_feature_ ;
    if (mean_feature_.size()!=feature_dim_)
      valid = false;
  }

  path_elm->get_attribute("data_path", ifs_path);
  if (ifs_path != data_path())
    valid = false;

  path_elm->get_attribute("positions_path", ifs_path);
  if (ifs_path != pos_path())
    valid = false;

  path_elm->get_attribute("scene_path", scene_path_);

  if (!valid)
    vcl_cout << "bvpl_discover_pca_kernels - could not parse matrices\n";
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
  vcl_cerr << "Scatter Matix svd " << M*M.transpose() << '\n'
           << "PC Matix SVD:\n Size: " << pc_.size() << '\n'
           << pc_ << '\n'
           << "EVals SVD:\nSize: " << weights_.size() << '\n'
           << weights_ << '\n';
  bvpl_write_pca(path_out_ + "/pca_matrices", M, weights_, pc_);
#endif // DEBUG
}

//: Set Up the PCA matrix for the given scene
//  This method performs eigen value decomposition on the covariance matrix
//  Use this method when number of samples >> number of dimensions
void bvpl_discover_pca_kernels::set_up_pca_evd(boxm_scene<boct_tree<short,float> > *scene)
{
  //zero-mean feature matrix
  vnl_matrix<double> S = this->compute_covariance_matrix(scene);

  // Compute eigenvectors(principal components) and values of S
  pc_.set_size(feature_dim_,feature_dim_);
  weights_.set_size(feature_dim_);

  vnl_symmetric_eigensystem_compute(S, pc_, weights_);

  pc_.fliplr();
  weights_.flip();
#ifdef DEBUG
  vcl_cerr << "Scatter Matix " << S <<'\n'
           << "PC Matix EVD:\n Size: " << pc_.size() << '\n'
           << pc_ <<'\n'
           << "EVals EVD:\n Size: " << weights_.size() << '\n'
           << weights_ << '\n';
  bvpl_write_pca(path_out_ + "/pca_evd", S, weights_, pc_);
#endif // DEBUG
}

//: Samples according to octree structure.
//  Sampling is achieved by generating uniform random cell samples.
//  Since there are more cells where the resolution is finer, then these regions get sampled more often
vnl_matrix<double> bvpl_discover_pca_kernels::compute_covariance_matrix( boxm_scene<boct_tree<short, float> > *scene)
{
  //init variables
  double scene_ncells = scene->size();
  double cell_length = scene->finest_cell_length();
  scene->unload_active_blocks();
  mean_feature_.fill(0.0f);
  vnl_matrix<double> S(feature_dim_, feature_dim_, 0.0f);
  vnl_random rng(9667566ul);
  vcl_ofstream pos_ofs(pos_path().c_str());
  vcl_ofstream data_ofs(data_path().c_str());
  data_ofs.precision(15);

  // 1. Traverse through the blocks/trees
  boxm_block_iterator<boct_tree<short,float> > it = scene->iterator();
  unsigned long nfeature = 1;
  for (it.begin(); !it.end(); ++it)
  {
    scene->load_block_and_neighbors(it.index());
    boct_tree<short, float>   *tree = (*it)->get_tree();
    assert(tree != NULL);

    //2. Sample cells from this tree. The number of samples from this tree depends on the portion of scene cells that live in this tree
    vcl_vector<boct_tree_cell<short, float> *> leaf_cells = tree->leaf_cells();
    float tree_ncells = leaf_cells.size();
    unsigned long tree_nsamples = (tree_ncells/scene_ncells)*(float)nsamples_;

    for (unsigned i=0; i<tree_nsamples; i++)
    {
      unsigned long sample = rng.lrand32(tree_ncells-1);

      boct_tree_cell<short, float> *center_cell = leaf_cells[sample];
      vgl_point_3d<double> center_cell_centroid = tree->global_centroid(center_cell);

      //CAUTION: the neighborhood box was suppossed to be defined as number of regular neighbors
      //convert neighborhood box to scene coordinates
      vgl_point_3d<int> nmin = nbbox_.min_point();
      vgl_point_3d<int> nmax = nbbox_.max_point();

      vgl_box_3d<double> roi(cell_length*(double)nmin.x(),cell_length*(double)nmin.y(),cell_length*(double)nmin.z(),
                             cell_length*(double)nmax.x(),cell_length*(double)nmax.y(),cell_length*(double)nmax.z());
      roi.set_centroid(center_cell_centroid);

      //if neighborhood is not inclusive we would have missing features
      if (!((scene->get_world_bbox()).contains(roi))) {
        --i;
        continue;
      }

      //3. Assemble neighborhood as a feature-vector
      vnl_vector<double> this_feature(feature_dim_, 0.0f);

      unsigned curr_dim = 0;
      for (int z = nbbox_.min_z(); z<=nbbox_.max_z(); z++)
        for (int y = nbbox_.min_y(); y<=nbbox_.max_y(); y++)
          for (int x = nbbox_.min_x(); x<=nbbox_.max_x(); x++)
          {
            vgl_point_3d<double> neighbor_centroid(center_cell_centroid.x() + (double)x*cell_length,
                                                   center_cell_centroid.y() + (double)y*cell_length,
                                                   center_cell_centroid.z() + (double)z*cell_length);

            boct_tree_cell<short,float> *neighbor_cell = scene->locate_point_in_memory(neighbor_centroid);

            assert(neighbor_cell !=NULL);
            this_feature[curr_dim] = (double)neighbor_cell->data();
            curr_dim++;
          }

      assert(curr_dim == feature_dim_);

      pos_ofs << center_cell_centroid <<'\n' ;
      data_ofs << this_feature << '\n';

      //increment weights
      double rho = 1.0/(double)nfeature;
      double rho_comp = 1.0 - rho;

      // the difference vector between the sample and the mean
      vnl_vector<double> diff = this_feature - mean_feature_;

      //update the covariance
      S += rho_comp*outer_product(diff,diff);

      //update the mean
      mean_feature_ += (rho*diff);

      ++nfeature;
#ifdef DEBUG
      vcl_cerr << "Feature EVD: " <<this_feature << '\n'
               << "Mean Feature EVD: " <<mean_feature << '\n';
#endif
    }
  } //end of block-loop

  pos_ofs.close();
  data_ofs.close();
  vcl_ofstream mean_ofs(mean_path().c_str());
  mean_ofs.precision(15);
  mean_ofs << mean_feature_;
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
  float scene_ncells = scene->size();
  double cell_length = scene->finest_cell_length();
  scene->unload_active_blocks();
  mean_feature_.fill(0.0f);
  vnl_matrix<double> M(feature_dim_, nsamples_);
  vnl_random rng(9667566ul);
  vcl_ofstream pos_ofs(pos_path().c_str());

  // 1. Traverse through the blocks/trees
  boxm_block_iterator<boct_tree<short,float> > it = scene->iterator();
  unsigned long feature_col = 0;
  for (it.begin(); !it.end(); ++it)
  {
    scene->load_block_and_neighbors(it.index());
    boct_tree<short, float>   *tree = (*it)->get_tree();
    assert(tree != NULL);

    //2. Sample cells from this tree. The number of samples from this tree depends on the portion of scene cells that live in this tree
    vcl_vector<boct_tree_cell<short, float> *> leaf_cells = tree->leaf_cells();
    float tree_ncells = leaf_cells.size();
    unsigned long tree_nsamples = (tree_ncells/scene_ncells)*(float)nsamples_;

    for (unsigned i=0; i<tree_nsamples; i++) {
      unsigned long sample = rng.lrand32(tree_ncells-1);

      boct_tree_cell<short, float> *center_cell = leaf_cells[sample];
      vgl_point_3d<double> center_cell_centroid = tree->global_centroid(center_cell);

      //CAUTION: the neighborhood box was suppossed to be defined as number of regular neighbors
      //convert neighborhood box to scene coordinates
      vgl_point_3d<int> nmin = nbbox_.min_point();
      vgl_point_3d<int> nmax = nbbox_.max_point();

      vgl_box_3d<double> roi(cell_length*(double)nmin.x(),cell_length*(double)nmin.y(),cell_length*(double)nmin.z(),
                             cell_length*(double)nmax.x(),cell_length*(double)nmax.y(),cell_length*(double)nmax.z());
      roi.set_centroid(center_cell_centroid);

      //if neighborhood is not inclusive we would have missing features
      if (!((scene->get_world_bbox()).contains(roi))) {
        --i;
        continue;
      }

      //3. Assemble neighborhood as a feature-vector
      vnl_vector<double> this_feature(feature_dim_, 0.0f);

      unsigned curr_dim = 0;
      for (int z = nbbox_.min_z(); z<=nbbox_.max_z(); z++)
        for (int y = nbbox_.min_y(); y<=nbbox_.max_y(); y++)
          for (int x = nbbox_.min_x(); x<=nbbox_.max_x(); x++)
          {
            vgl_point_3d<double> neighbor_centroid(center_cell_centroid.x() + (double)x*cell_length,
                                                   center_cell_centroid.y() + (double)y*cell_length,
                                                   center_cell_centroid.z() + (double)z*cell_length);

            boct_tree_cell<short,float> *neighbor_cell = scene->locate_point_in_memory(neighbor_centroid);

            assert(neighbor_cell !=NULL);
            this_feature[curr_dim] = (double)neighbor_cell->data();
            curr_dim++;
          }

      assert(curr_dim == feature_dim_);

      pos_ofs << center_cell_centroid <<'\n' ;

      M.set_column(feature_col,this_feature);
      mean_feature_+=this_feature;
      ++feature_col;
#ifdef DEBUG
      vcl_cerr << "Feature SVD: " <<this_feature << '\n'
               << "Mean Feature SVD: " <<mean_feature/(double)feature_col << '\n';
#endif
    }
  } //end of block-loop

  //the actual number of samples is different from desired number of smaples because only integer samples are drawn from each tree
  vnl_matrix<double> F(feature_dim_, feature_col);

  M.extract(F);

  //4. Normalize feature vector
  mean_feature_/=feature_col;
#ifdef DEBUG
  vcl_cerr << "Mean Feature SVD: " <<mean_feature << '\n';
#endif
  vcl_ofstream mean_ofs(mean_path().c_str());
  mean_ofs.precision(15);
  mean_ofs << mean_feature_;
  mean_ofs.close();

  for (unsigned int j=0; j<feature_col; ++j)
    F.set_column(j,F.get_column(j) - mean_feature_);

  return F;
}

//: Project training samples onto pca space and return error as a function of number of components used
void bvpl_discover_pca_kernels::compute_training_error(vnl_vector<double> &proj_error)
{
  vcl_ifstream data_ifs(data_path().c_str());

  //a vector to keep projection error - first element refers to error when using only first pc,
  //the sencond elemend, to error when projecting on frist 2 components and so on
  proj_error.set_size(feature_dim_);
  proj_error.fill(0.0);

  //For all samples
  for ( unsigned long i = 0; i < nsamples_; i++)
  {
    if (data_ifs.eof())
      continue;

    //retrieve the data from file
    vnl_vector<double> norm_feature(feature_dim_, 0.0);
    data_ifs >> norm_feature;
#ifdef DEBUG
    vcl_cerr << "Feature: " <<norm_feature << '\n';
#endif

    norm_feature-=mean_feature_;

    //solve fo the coefficients
    vnl_vector<double> a(feature_dim_, 0.0);
    a = pc_.transpose() * (norm_feature);

    //project as a function of number of components
    for ( unsigned c = 0; c < feature_dim_; c++)
    {
      //reconstruct
      vnl_vector<double> norm_feature_approx  = pc_.extract(feature_dim_, c+1) * a.extract(c+1);

      //compute error
      proj_error[c]+= (norm_feature - norm_feature_approx).two_norm();
    }
  }
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

  for (unsigned i=1; i<nsamples_; i++) {
    unsigned long sample = rng.lrand32(max_nsamples);
    vgl_box_3d<double> roi = nbbox_;
    for (unsigned u = 0; u < sample - pos_pre; ++u)
      ++cell_it;
    pos_pre = sample;
    roi.set_centroid(cell_it.global_centroid());
    if (!((scene->get_world_bbox()).contains(roi))) {
      i--;
      continue;
    }

    positions_.push_back(sample);
  }

  //sort random numbers for easy access
  positions_.sort();
  if (nsamples_ != positions_.size())
  {
    vcl_cerr << "Error computing PCA features: Wrong number of random samples generated\n";
    return false;
  }

  cell_it.begin();

  //init cells array to first random element
  vcl_list<unsigned long>::const_iterator rand_it = positions_.begin();
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

    if (!center_cell) {
      vcl_cerr << "Error is bvpl_discover_pca_kernels: Unexpected NULL cell\n";
      return false;
    }

    //assemble neighborhood as a feature-vector
    vnl_vector<float> this_feature(feature_dim);

    unsigned curr_dims = 0;
    for (int z = nbbox_.min_z(); z<=nbbox_.max_z(); z++)
      for (int y = nbbox_.min_y(); y<=nbbox_.max_y(); y++)
        for (int x = nbbox_.min_x(); x<=nbbox_.max_x(); x++)
        {
          vgl_point_3d<double> neighbor_origin(center_cell_centroid.x() + (double)x*cell_length,
                                               center_cell_centroid.y() + (double)y*cell_length,
                                               center_cell_centroid.z() + (double)z*cell_length);

          boct_tree_cell<short,float> *neighbor_cell = scene->locate_point_in_memory(neighbor_origin);

          if (!neighbor_cell) {
            vcl_cerr << "Error is bvpl_discover_pca_kernels: Unexpected NULL cell\n";
            return false;
          }

          this_feature[x + nbbox_.width()*y + nbbox_.width()*nbbox_.height()*z] = neighbor_cell->data();
          curr_dims++;
        }

    M.set_column(i,this_feature);
    mean+=this_feature;

    //increment iterators
    cell_it+=(*rand_it - pos_pre);
    pos_pre = *rand_it;
    ++rand_it;
    i++;
  }

  mean/=nsamples_;

  for (unsigned int j=0; j<nsamples_; ++j)
    M.set_column(j,M.get_column(j) - mean);

  return true;
}
#endif // 0 (too slow)

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
  paths->set_attribute("mean_path", mean_path());
  paths->set_attribute("scene_path", scene_path_);
  root->append_data(paths);
  root->append_text("\n");

  bxml_element* nsamples = new bxml_element("samples");
  nsamples-> append_text("\n");
  nsamples->set_attribute("nsamples", nsamples_);
  root->append_data(nsamples);
  root->append_text("\n");

  bxml_element* dim = new bxml_element("dimension");
  dim->append_text("\n");
  dim->set_attribute("feature_dim", feature_dim_);
  root->append_data(dim);
  root->append_text("\n");

  vcl_ofstream os(xml_path().c_str());
  bxml_write(os, doc);
  os.close();

  //: Write pca main matrices -other matrices aren't class variables and should have been written during computation time

  write_pca_matrices();
}

//: Write a PCA file
bool bvpl_discover_pca_kernels::write_pca_matrices()
{
  vcl_ofstream pc_ofs(pc_path().c_str());
  pc_ofs.precision(15);
  vcl_ofstream weights_ofs(weights_path().c_str());
  weights_ofs.precision(15);

  pc_ofs << pc_;
  pc_ofs.close();
  weights_ofs << weights_;
  weights_ofs.close();

  return true;
}

