// This is brl/bseg/bvpl/bvpl_octree/bvpl_global_pca.h
#ifndef bvpl_global_pca_h
#define bvpl_global_pca_h
//:
// \file
// \brief A class to compute and learn pca features across different boxm_scenes
// \author Isabel Restrepo mir@lems.brown.edu
// \date  14-Mar-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <utility>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>

#include <boxm/boxm_scene.h>


template <unsigned feature_dim>
class bvpl_global_pca: public vbl_ref_count
{
 public:
  //: Constructor
  bvpl_global_pca(std::vector<std::string> scenes, std::vector<std::string> aux_dirs, std::vector<double> finest_cell_length,
                  vgl_box_3d<int> nbbox,  double training_fraction = 0.1, std::string path_out = "./"):
  scenes_(std::move(scenes)), aux_dirs_(std::move(aux_dirs)), finest_cell_length_(std::move(finest_cell_length)), nbbox_(nbbox), training_fraction_(training_fraction),path_out_(std::move(path_out))
  {
    training_scenes_ = std::vector<bool>(scenes_.size(), true);
    nleaves_ =  std::vector<unsigned long>(scenes_.size(), 0);
    nsamples_ = 0;
    training_mean_.fill(0.0);
    pc_.fill(0.0);
    weights_.fill(0.0);
    scatter_.fill(0.0);
    this->xml_write();
  }

  //:Create from xml file
  bvpl_global_pca(const std::string &path);

  //: Init auxiliary scenes and smallest cell length values
  void init(unsigned scene_id);

  //: Computes the scatter matrix and mean vector of a portion of random samples from a block
  bool sample_statistics( int scene_id, int block_i, int block_j, int block_k,
                          vnl_matrix_fixed<double, feature_dim, feature_dim> &S,
                          vnl_vector_fixed<double, feature_dim> &mean,
                          unsigned long &nfeature);

  //: Update mean and scatter, given the mean and scatter of two sets.
  //  Calculation is done according to Chan et al. Updating Formulae and a Pairwise Algorithm for Computing Sample Variances
   static void combine_pairwise_statistics( const vnl_vector_fixed<double,feature_dim>  &mean1,
                                            const vnl_matrix_fixed<double,feature_dim,feature_dim>  &scatter1,
                                            double const n1,
                                            const vnl_vector_fixed<double,feature_dim>  &mean2,
                                            const vnl_matrix_fixed<double,feature_dim,feature_dim>  &scatter2,
                                            double const n2,
                                            vnl_vector_fixed<double,feature_dim> & mean_out,
                                            vnl_matrix_fixed<double,feature_dim,feature_dim> & scatter_out,
                                            double &n_out );

  //: Set total scatter matrix, mean, sample, principal components and weights for this class
  void set_up_pca_evd(const vnl_matrix_fixed<double, feature_dim, feature_dim> &S,
                      const vnl_vector_fixed<double, feature_dim> &mean,
                      const double total_nsamples);

  //: Access the principal components
  const vnl_matrix_fixed<double, feature_dim, feature_dim>& principal_comps() { return pc_; }

  //: Access the coefficients
  const vnl_vector_fixed<double, feature_dim>& weights() { return weights_; }

  //: Access the mean of samples
  const vnl_vector_fixed<double, feature_dim>& mean()  { return training_mean_; }

  bool register_scene(std::string scene_path, std::string aux_path)
  {
    if (scenes_.size()!=aux_dirs_.size())
      return false;
    scenes_.push_back(scene_path);
    aux_dirs_.push_back(aux_path);
    return true;
  }

  void xml_write();

  //: Computes 10-dimensional pca projection at each voxel on the block and saves it
  void project(int scene_id, int block_i, int block_j, int block_k);

  void projection_error(int scene_id, int block_i, int block_j, int block_k);


  std::string path_out() const { return path_out_; }

  inline std::vector<std::string> scenes() const { return scenes_; }
  inline std::vector<std::string> aux_dirs() const { return aux_dirs_; }
  inline std::vector<double> cell_lengths() const { return finest_cell_length_; }
  inline std::vector<bool> training_scenes() const { return training_scenes_; }
  inline vgl_box_3d<int> nbbox() const { return nbbox_; }
  inline unsigned nscenes() const { return scenes_.size(); }
 protected:

  //: A vector to hold scene paths
  std::vector<std::string> scenes_;
  //: A vector to hold paths to keep any kind of auxiliary scene or info(must be in the same order as scenes_)
  std::vector<std::string> aux_dirs_;
  //: A vector to hold the finest cell length of the corresponding scene scene
  std::vector<double> finest_cell_length_;
  //: A vector that indicates whether a scene should be used as train (True) or test(False). Order is equivalent to scenes_
  std::vector<bool> training_scenes_;
  //: A vector to keep the size of each scene
  std::vector<unsigned long> nleaves_;

  //: The size around the central feature to include in PCA vectors
  vgl_box_3d<int> nbbox_;
  //: Number of total samples(leaves) to be drawn
  unsigned long long nsamples_;

  //: Principal components - eigen vectors of the scatter matrix
  vnl_matrix_fixed<double, feature_dim, feature_dim> pc_;
  //: Weights - eigen values of the scatter matrix
  vnl_vector_fixed<double, feature_dim> weights_;
  //: The mean of sample data
  vnl_vector_fixed<double, feature_dim> training_mean_;
  //: Scatter matrix used to compute pca
  vnl_matrix_fixed<double, feature_dim, feature_dim> scatter_;
  //: Fraction of cells used for learning
  double training_fraction_;

  //: Paths for i/o of matrices and vectors
  std::string path_out_;
  std::string pc_path() const { return path_out_ + "/pc.txt" ; }
  std::string weights_path() const { return path_out_ + "/weights.txt"; }
  std::string mean_path() const { return  path_out_ + "/mean.txt"; }
  std::string scatter_path() const { return  path_out_ + "/scatter.txt"; }
  std::string xml_path() const { return path_out_ + "/pca_global_info.xml"; }

  void write_pca_matrices();

 public:
  boxm_scene_base_sptr load_scene (int scene_id);
  boxm_scene_base_sptr load_train_scene (int scene_id);
  boxm_scene_base_sptr load_valid_scene (int scene_id);
  boxm_scene_base_sptr load_projection_scene (int scene_id);
  boxm_scene_base_sptr load_error_scene (int scene_id);
};

typedef vbl_smart_ptr<bvpl_global_pca<125> > bvpl_global_pca_125_sptr;

#endif
