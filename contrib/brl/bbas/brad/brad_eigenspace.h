// This is brl/bbas/brad/brad_eigenspace.h
#ifndef brad_eigenspace_h
#define brad_eigenspace_h
//:
// \file
// \brief Process image regions to produce an eigensystem from feature vectors
// \author J.L. Mundy
// \date June 30, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//
//
// Each input resource is decomposed into a grid of blocks. A feature
// vector is extracted from the block, such as a histogram. A covariance
// matrix of the feature vectors is computed as well as the associated
// eigensystem. This class can be stored as binary to retain the
// eigenvector projection needed for classifying new images. The current
// implementation assumes that only the top three eigenvectors are needed
// by classifiers. This choice is convenient for display as color images
// and 3-d histograms.
//
// * nib - number of columns in a block
// * njb - number of rows in a block
// * funct - a functor class that computes the feature vector
//           the functor must return a vnl_vector<float> from
//           its (vil_image_view<float> const&) operator
//           and has a size() method to indicate the number of vector elements
//
#include <vector>
#include <iostream>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <bsta/bsta_joint_histogram_3d.h>
#include <brad/brad_eigenspace_base.h>

template <class T>
class brad_eigenspace : public brad_eigenspace_base
{
 public:
  brad_eigenspace()
  : nib_(0), njb_(0), covar_valid_(false), eigensystem_valid_(false) {}

  //: constructor with block size and functor
  brad_eigenspace(unsigned nib, unsigned njb, T funct)
  : funct_(funct), nib_(nib), njb_(njb), covar_valid_(false), eigensystem_valid_(false) {}

  ~brad_eigenspace() override = default;

  std::string feature_vector_type() override {return funct_.type();}

  //: compute the covariance matrix from image resources
  bool compute_covariance_matrix(std::vector<vil_image_resource_sptr> const& rescs);
    //: compute the covariance matrix from a random fraction of image resources
  bool compute_covariance_matrix_rand(std::vector<vil_image_resource_sptr> const& rescs, double frac = 1.0, unsigned nit=256, unsigned njt=256);
  //: compute the covariance matrix using blocked image cache
  bool compute_covariance_matrix_blocked(std::vector<vil_image_resource_sptr> const& rescs, unsigned nit=256, unsigned njt=256);
  //: compute the eigensystem from the covariance matrix
  bool compute_eigensystem();

  //: projection of image blocks onto the top three eigenvectors
  bool compute_eigenimage(vil_image_resource_sptr const& resc,
                          std::string const& output_path);

  //: projection of image blocks onto the top three eigenvectors, per pixel. assumes input and output can fit in memory
  bool compute_eigenimage_pixel(vil_image_view<float> const& input,
                                vil_image_view<float>& eignimage);

  //: create and update an initial histogram of feature vectors projected onto the first three eigenvectors.Defines the min, max range of the histogram cells.
  // note, does not update the histogram counts
  bool init_histogram(vil_image_resource_sptr const& resc, unsigned nbins,
                      bsta_joint_histogram_3d<float>& hist);

  //: update an existing histogram from an image resource
  bool update_histogram(vil_image_resource_sptr const& resc,
                        bsta_joint_histogram_3d<float>& hist);

  //: init from a set of images
  bool init_histogram(std::vector<vil_image_resource_sptr> const& rescs,
                      unsigned nbins,
                      bsta_joint_histogram_3d<float>& hist);
  //: update from a set of images
  bool update_histogram(std::vector<vil_image_resource_sptr> const& rescs,
                        bsta_joint_histogram_3d<float>& hist);

  //: init from a set of images, random selection of tiles
  bool init_histogram_rand(std::vector<vil_image_resource_sptr> const& rescs,
                           unsigned nbins,
                           bsta_joint_histogram_3d<float>& hist,
                           double frac = 1.0, unsigned nit=256,
                           unsigned njt=256);

  //: update from a set of images, random selection of tiles
  bool update_histogram_rand(std::vector<vil_image_resource_sptr> const& rescs,
                             bsta_joint_histogram_3d<float>& hist,
                             double frac = 1.0, unsigned nit=256,
                             unsigned njt=256);
    //: init from a set of images, using a blocked cache
  bool init_histogram_blocked(std::vector<vil_image_resource_sptr> const& rescs,
                              unsigned nbins,
                              bsta_joint_histogram_3d<float>& hist,
                              unsigned nit=256, unsigned njt=256);


  //: update from a set of images, using a blocked cache
  bool update_histogram_blocked(std::vector<vil_image_resource_sptr> const& rescs,
                             bsta_joint_histogram_3d<float>& hist,
                             unsigned nit=256, unsigned njt=256);
  //: classify an image as to atmospheric content, nit, njt are block size
  bool classify_image(vil_image_resource_sptr const& resc,
                      bsta_joint_histogram_3d<float> const& no_atmos,
                      bsta_joint_histogram_3d<float> const& atmos,
                      unsigned nit, unsigned njt,
                      std::string const& output_path);
  bool classify_image(vil_image_resource_sptr const& resc,
                      bsta_joint_histogram_3d<float> const& no_atmos,
                      bsta_joint_histogram_3d<float> const& atmos,
                      unsigned nit, unsigned njt,
                      vil_image_resource_sptr& out_resc,
                      vil_image_resource_sptr& out_resc_orig_size);

  //: classify an image as to atmospheric content, nit, njt are block size, assumes the resulting image can fit in memory. uses a sliding window to classify each pixel.
  bool classify_image_pixel(vil_image_view<float> const& image,
                            bsta_joint_histogram_3d<float> const& no_atmos,
                            bsta_joint_histogram_3d<float> const& atmos,
                            float prob_ratio,
                            vil_image_view<float>& class_image);

  //: print sizes and other info (bool for calling consistency)
  bool print(std::ostream& os = std::cout) const;

  //: accessors, setters
  unsigned nib() const {return nib_;}
  unsigned njb() const {return njb_;}
  vnl_vector<double> mean() const {return mean_;}
  vnl_matrix<double> covariance() const {return covar_;}
  vnl_vector<double> eigenvalues() const {return eigenvalues_;}
  vnl_matrix<double> eigenvectors() const {return eigenvectors_;}
  T functor() const {return funct_;}
  void set_nib(unsigned nib) {nib_=nib;}
  void set_njb(unsigned njb) {njb_=njb;}
  void set_mean_covar(vnl_vector<double> const& mean,
                      vnl_matrix<double> const& covar)
  { mean_ = mean; covar_ = covar; covar_valid_ = true; }
  void set_eigensystem(vnl_vector<double> const& eigv,
                       vnl_matrix<double> const& evecs)
  { eigenvalues_=eigv; eigenvectors_=evecs; eigensystem_valid_ = true; }
  void set_functor(T funct) {funct_ = funct;}

 private:
  //: An instance of the functor
  T funct_;
  //: number of cols in view block
  unsigned nib_;
  //: number of rows in view block
  unsigned njb_;
  //: feature vector mean
  vnl_vector<double> mean_;
  //: feature vector covariance matrix
  vnl_matrix<double> covar_;
  //: eigenvalues of the covariance matrix
  vnl_vector<double> eigenvalues_;
  //: eigenvectors of the covariance matrix
  vnl_matrix<double> eigenvectors_;
  //: flags
  bool covar_valid_;
  bool eigensystem_valid_;
};

//: Binary save brad_eigenspace to stream.
template <class T>
void vsl_b_write(vsl_b_ostream &os, const brad_eigenspace<T>& ep);


//: Binary load brad_eigenspace from stream.
template <class T>
void vsl_b_read(vsl_b_istream &is, brad_eigenspace<T>& fv);


//: Print summary
template <class T>
void
vsl_print_summary(std::ostream &os, const brad_eigenspace<T>& fv);

#include <brad/brad_eigenspace_sptr.h>

#endif // brad_eigenspace_h
