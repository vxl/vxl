// This is brl/bseg/bvpl/bvpl_octree/bvpl_discover_pca_kernels.h
#ifndef bvpl_discover_pca_kernels_h
#define bvpl_discover_pca_kernels_h
//:
// \file
// \brief A class to discover those features that represent the data the best in a least square sense using PCA
// \author Isabel Restrepo mir@lems.brown.edu
// \date  8-Nov-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <list>
#include <iostream>
#include <utility>
#include <vgl/vgl_box_3d.h>

#include <boxm/boxm_scene.h>
#include <bxml/bxml_document.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bvpl/bvpl_octree/sample/bvpl_pca_basis_sample.h>

#if BVPL_OCTREE_HAS_PTHREADS
#include <pthread.h>
#endif

class bvpl_discover_pca_kernels;

typedef vbl_smart_ptr<bvpl_discover_pca_kernels> bvpl_discover_pca_kernels_sptr;

class bvpl_discover_pca_kernels: public vbl_ref_count
{
 public:
  //: Constructor from neighborhood bounding boxm, number of samples to be drawn, scene and flag to indicate whether to use evd or svd
  bvpl_discover_pca_kernels(vgl_box_3d<int> nbbox, unsigned long long nsamples, boxm_scene<boct_tree<short,float> > *scene, std::string  path_out=".", bool use_evd = true):
  nbbox_(nbbox), nsamples_(nsamples), path_out_(std::move(path_out))
  {
    scene_path_ = scene->filename();
    std::cout << "Scene path: " << scene->filename()<< std::endl;;
    data_scene_base_=scene;

    finest_cell_length_ = scene->finest_cell_length();

    feature_dim_ = (nbbox_.width()+1)*(nbbox_.height()+1)* (nbbox_.depth()+1);
    compute_mean_feature(scene);

    sample_mean_feature_.set_size(feature_dim_);

    if (use_evd)
      set_up_pca_evd(scene);
    else
      set_up_pca_svd(scene);

    scene->unload_active_blocks();
  }

  //: Construct from xml file
  bvpl_discover_pca_kernels(const std::string &path);

  //: Constructor from zero-mean matrix
  bvpl_discover_pca_kernels(const vnl_matrix<double>& M)
  {
    vnl_svd<double> A(M, -1e-8);

    //columns of U contain principal components.
    pc_.set_size(A.rank(), M.rows());
    A.U().extract(pc_);
  }

  //: Access the principal components
  const vnl_matrix<double>& principal_comps() { return pc_; }

  //: Access the coefficients
  const vnl_vector<double>& weights() { return weights_; }

  //: Access the mean of samples
  const vnl_vector<double>& mean()  { return sample_mean_feature_; }

  //: Project training samples onto pca space and return error as a function of number of components used
  void compute_training_error(vnl_vector<double> &proj_error);

  //: Project training samples onto pca space and return error as a function of number of components used
  void compute_normalized_training_error(vnl_vector<double> &proj_error);

  //: Reconstrcution error on training samples. Same as above but it is not computed from samples.
  void theoretical_training_error(vnl_vector<double> &proj_error);

  //: Reconstructions error on testing samples. All training samples are excuded!
  void compute_testing_error(vnl_vector<double> &proj_error);

  //: Reconstructions error on testing samples. By block. Error is given as average error per sample
  void compute_testing_error(const boxm_scene_base_sptr& error_scene_base, unsigned ncomponents,
                             int block_i, int block_j, int block_k);

#if BVPL_OCTREE_HAS_PTHREADS

  void compute_testing_error_thread_safe(boxm_scene<boct_tree<short,float> > * error_scene, unsigned ncomponents,
                                         int block_i, int block_j, int block_k);

//  //: Reconstructions error on testing samples. By block. Error is the sse of feature vector approximation
//  bool compute_testing_error_thread_safe(boxm_scene_base_sptr error_scene_base, unsigned ncomponents);
//
//  //: Reconstructions error on testing samples. By block. Error is the sse of feature vector approximation
//  friend void *compute_testing_error_thread( void *ptr);

#endif

  //: Computes pca projection at each voxel on the block and it save the specified number of dimensions (dim)
  template <unsigned DIM>
  void project(boxm_scene<boct_tree<short, bvpl_pca_basis_sample<DIM> > > *proj_scene,
               int block_i, int block_j, int block_k);

  //: Compute the mean feature of all "leaf" samples in the data scene associated with this class
  void compute_mean_feature(boxm_scene<boct_tree<short,float> > *scene);

  //: Return the number of samples used
  unsigned long long nsamples() const { return nsamples_; }

  //: Return dimension of the features
  unsigned feature_dim() const { return feature_dim_; }

  //: Return scene path
  std::string scene_path() const { return scene_path_; }

  //: Return number of blocks in the data scene
  vgl_vector_3d<unsigned> data_scene_dim() const { return data_scene_base_->world_dim(); }

  bool load_all_scene_blocks()
  {
    boxm_scene<boct_tree<short,float> > *scene = dynamic_cast<boxm_scene<boct_tree<short,float> >* > (data_scene_base_.as_pointer());
    return scene->read_all_blocks();
  }

  void unload_all_scene_blocks()
  {
    boxm_scene<boct_tree<short,float> > *scene = dynamic_cast<boxm_scene<boct_tree<short,float> >* > (data_scene_base_.as_pointer());
    scene->unload_active_blocks();
  }

  //: Compute deviations i.e. zero-mean feature matrix
  vnl_matrix<double> compute_deviation_matrix(boxm_scene<boct_tree<short,float> > *scene);

  //: Compute the scatter matrix.
  vnl_matrix<double> compute_scatter_matrix(boxm_scene<boct_tree<short,float> > *scene);

  void xml_write();

 protected:

  //: Set Up the PCA matrix for the given scene.
  //  This method computes the deviation matrix and performs SVD to find the principal components
  //  If the number of samples is very large use set_up_pca_evd
  void set_up_pca_svd(boxm_scene<boct_tree<short,float> > *scene);

  //: Set Up the PCA matrix for the given scene
  //  This method performs eigen value decomposition on the covariance matrix
  //  Use this method when number of samples >> number of dimensions
  void set_up_pca_evd(boxm_scene<boct_tree<short,float> > *scene);

  //: Write a PCA file
  bool write_pca_matrices();

  //: The size around the central feature to include in PCA vectors
  vgl_box_3d<int> nbbox_;
  //: Number of total samples(leaves) to be drawn
  unsigned long long nsamples_;
  //: Number of dimension of feature vector
  unsigned feature_dim_;
  //: Principal components - eigen vectors of the scatter matrix
  vnl_matrix<double> pc_;
  //: Weights - eigen values of the scatter matrix
  vnl_vector<double> weights_;
  //: The mean of sample data
  vnl_vector<double> sample_mean_feature_;
  //: The mean of all features in the scene data
  vnl_vector<double> data_mean_feature_;
  //: Pointer to data scene
  boxm_scene_base_sptr data_scene_base_;
  //: Finest cell length of data scene
  double finest_cell_length_;

  //: Paths for i/o of matrices and vectors
  std::string path_out_;
  std::string scene_path_;
  std::string pc_path() const { return path_out_ + "/pc.txt" ; }
  std::string data_path() const { return path_out_ + "/data.txt"; }
  std::string weights_path() const { return path_out_ + "/weights.txt"; }
  std::string pos_path() const { return path_out_ + "/positions.txt"; }
  std::string sample_mean_path() const { return  path_out_ + "/sample_mean.txt"; }
  std::string data_mean_path() const { return  path_out_ + "/data_mean.txt"; }
  std::string xml_path() const { return path_out_ + "/pca_info.xml"; }

  //: Tolerance, retain eigenvalues above max(eigenVals)*tol
  double tol_;
};


//: Computes pca projection at each voxel on the block and it saves the specified number of coefficientes
template <unsigned DIM>
void bvpl_discover_pca_kernels::project(boxm_scene<boct_tree<short, bvpl_pca_basis_sample<DIM> > > *proj_scene,
                                        int block_i, int block_j, int block_k)
{
  typedef boct_tree<short,float> float_tree_type;
  typedef boct_tree_cell<short,float> float_cell_type;
  typedef boct_tree<short,bvpl_pca_basis_sample<DIM> > pca_tree_type;
  typedef boct_tree_cell<short,bvpl_pca_basis_sample<DIM> > pca_cell_type;

  //cast the scenes
  boxm_scene<float_tree_type>* data_scene = dynamic_cast<boxm_scene<float_tree_type>* > (data_scene_base_.as_pointer());

  if (!(data_scene &&proj_scene)) {
    std::cerr << "Error in bvpl_discover_pca_kernels::compute_testing_error: Faild to cast scene\n";
    return;
  }

  double cell_length = data_scene->finest_cell_length();

  //get the cells for this block
  if (!(data_scene->valid_index(block_i, block_j, block_k) && proj_scene->valid_index(block_i, block_j, block_k))) {
    std::cerr << "In compute_testing_error: Invalid block\n";
    return;
  }

  data_scene->load_block_and_neighbors(block_i, block_j, block_k);
  proj_scene->load_block(block_i, block_j, block_k);

  //get the trees
  float_tree_type* data_tree = data_scene->get_block(block_i, block_j, block_k)->get_tree();
  pca_tree_type* proj_tree = data_tree->template clone_to_type<bvpl_pca_basis_sample<DIM> >();

  //get the leaves
  std::vector<float_cell_type*> data_leaves = data_tree->leaf_cells();
  std::vector<pca_cell_type*> proj_leaves = proj_tree->leaf_cells();

  //CAUTION: the neighborhood box was supposed to be defined as number of regular neighbors
  //convert neighborhood box to scene coordinates
  vgl_point_3d<int> min_point = nbbox_.min_point();
  vgl_point_3d<int> max_point = nbbox_.max_point();

  for (unsigned i =0; i<data_leaves.size(); i++)
  {
    float_cell_type* data_cell = data_leaves[i];

    //create a region around the center cell
    vgl_point_3d<double> centroid = data_tree->global_centroid(data_cell);

    //change the coordinates of enpoints to be in global coordinates abd test if they are contained in the scene
    vgl_point_3d<double> min_point_global(centroid.x() + (double)min_point.x()*cell_length, centroid.y() + (double)min_point.y()*cell_length, centroid.z() + (double)min_point.z()*cell_length);
    vgl_point_3d<double> max_point_global(centroid.x() + (double)max_point.x()*cell_length, centroid.y() + (double)max_point.y()*cell_length, centroid.z() + (double)max_point.z()*cell_length);
    if (!(data_scene->locate_point_in_memory(min_point_global) && data_scene->locate_point_in_memory(max_point_global))) {
      proj_leaves[i]->set_data(bvpl_pca_basis_sample<DIM>());
      continue;
    }

    //3. Assemble neighborhood as a feature-vector
    vnl_vector<double> this_feature(feature_dim_, 0.0f);

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
            std::cerr << "Error in compute_testing_error\n";
            return;
          }

          this_feature[curr_dim] = (double)neighbor_cell->data();
          curr_dim++;
        }

    if (curr_dim != feature_dim_) {
      std::cerr << "Error in compute_testing_error\n";
      return;
    }
    this_feature-=data_mean_feature_;

    //solve for the coefficients
    vnl_vector<double> a(feature_dim_, 0.0);
    a = pc_.transpose() * (this_feature);

    //compute error
    bvpl_pca_basis_sample<DIM> sample(a.extract(DIM), 0);
    proj_leaves[i]->set_data(sample);
  }

  // write and release memory
  proj_scene->get_block(block_i, block_j, block_k)->init_tree(proj_tree);
  proj_scene->write_active_block();
  data_scene->unload_active_blocks();
}

#endif
