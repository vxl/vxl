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

#include <vgl/vgl_box_3d.h>

#include <boxm/boxm_scene.h>
#include <bxml/bxml_document.h>


#include <vnl/vnl_matrix.h>
#include <vnl/vnl_random.h>
#include <vnl/algo/vnl_svd.h>

#include <vcl_list.h>

class bvpl_discover_pca_kernels
{
 public:
  //: Constructor from neighborhood bounding boxm, number of samples to be drawn, scene and flag to indicate whether to use evd or svd
  bvpl_discover_pca_kernels(vgl_box_3d<int> nbbox, unsigned long nsamples, boxm_scene<boct_tree<short,float> > *scene, const vcl_string& path_out=".", bool use_evd = true):
  nbbox_(nbbox), nsamples_(nsamples), path_out_(path_out)
  {
    scene_path_ = scene->filename();
    feature_dim_ = (nbbox_.width()+1)*(nbbox_.height()+1)* (nbbox_.depth()+1);
    mean_feature_.set_size(feature_dim_);
    if (use_evd)
      set_up_pca_evd(scene);
    else
      set_up_pca_svd(scene);
  }

  //: Construct from xml file
  bvpl_discover_pca_kernels(const vcl_string &path);

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
  const vnl_vector<double>& mean()  { return mean_feature_; }

  //: Project training samples onto pca space and return error as a function of number of components used
  void compute_training_error(vnl_vector<double> &proj_error);

  //: Reconstrcution error on training samples. Same as above but it is not computed from samples. 
  void theoretical_training_error(vnl_vector<double> &proj_error);
  
  //: Reconstructions error on testing samples. All training samples are excuded!
  void compute_testing_error(vnl_vector<double> &proj_error);
  
  //: Compute the mean feature of all "leaf" samples in a scene
  void compute_mean_feature(boxm_scene<boct_tree<short, float> > *scene, vnl_vector<double>& mean);
  
  //: Return the number of samples used
  unsigned long nsamples() { return nsamples_; }

  //: Return dimension of the features
  unsigned feature_dim() { return feature_dim_; }

  //: Return scene path
  const vcl_string scene_path() { return scene_path_; }

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

  //: Compute deviations i.e. zero-mean feature matrix
  vnl_matrix<double> compute_deviation_matrix(boxm_scene<boct_tree<short,float> > *scene);

  //: Compute the scatter matrix.
  vnl_matrix<double> compute_scatter_matrix(boxm_scene<boct_tree<short,float> > *scene);

  //: Write a PCA file
  bool write_pca_matrices();

  //: The size around the central feature to include in PCA vectors
  vgl_box_3d<int> nbbox_;

  //: Number of total samples(leaves) to be drawn
  unsigned long nsamples_;

  //: Number of dimension of feature vector
  unsigned feature_dim_;

  //: Principal components - eigen vectors of the scatter matrix
  vnl_matrix<double> pc_;
  //: Weights - eigen values of the scatter matrix
  vnl_vector<double> weights_;
  //: The mean of sample
  vnl_vector<double> mean_feature_;

  //: Paths for i/o of matrices and vectors
  vcl_string path_out_;
  vcl_string scene_path_;
  vcl_string pc_path() { return path_out_ + "/pc.txt" ; }
  vcl_string data_path() { return path_out_ + "/data.txt"; }
  vcl_string weights_path() { return path_out_ + "/weights.txt"; }
  vcl_string pos_path() { return path_out_ + "/positions.txt"; }
  vcl_string mean_path() { return  path_out_ + "/mean.txt"; }
  vcl_string xml_path() { return path_out_ + "/pca_info.xml"; }

  //: Tolerance, retain eigenvalues above max(eigenVals)*tol
  double tol_;
};

#endif
