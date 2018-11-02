#ifndef mfpf_pose_predictor_builder_h_
#define mfpf_pose_predictor_builder_h_
//:
// \file
// \brief Trains regressor in an mfpf_pose_predictor
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <mfpf/mfpf_pose_predictor.h>
#include <vnl/vnl_random.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Trains regressor in an mfpf_pose_predictor.
//  Object set up with a pose predictor which has been
//  partially initialised so as to allow calculation of the
//  image samples.
//  Given a set of training images, this samples at random
//  displacements and trains a linear regressor to predict
//  the given displacement.
//  The internal pose predictor is just used to do the image
//  sampling (to reduce code duplication).
class mfpf_pose_predictor_builder
{
 private:
  //: Pose predictor object used to do image sampling
  mfpf_pose_predictor sampler_;

  //: Random number generator for offsets
  vnl_random rand_;

  //: Number of random samples to use per example
  unsigned n_per_eg_;

  //: Samples (one per row)
  vnl_matrix<double> samples_;

  //: Pose vectors for each sample (one per row)
  vnl_matrix<double> poses_;

  //: Current index into samples_/poses_
  unsigned ci_;

  //: Define default values
  void set_defaults();

 public:

  // Dflt ctor
  mfpf_pose_predictor_builder();

  // Destructor
  virtual ~mfpf_pose_predictor_builder();

  //: Define sampling region and method
  //  Supplied predictor is partially initialised
  void set_sampling(const mfpf_pose_predictor&);

  //: Define number of samples per training image
  void set_n_per_eg(unsigned n);

  //: Initialise building
  // Must be called before any calls to add_example(...)
  virtual void clear(unsigned n_egs);

  //: Add one example to the model
  virtual void add_example(const vimt_image_2d_of<float>& image,
                           const mfpf_pose& pose);

  //: Build object from the data supplied in add_example()
  virtual void build(mfpf_pose_predictor&);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual std::string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mfpf_pose_predictor_builder* clone() const;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const;

  //: Prints ASCII representation of shape to os
  void print_shape(std::ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: Test equality
  bool operator==(const mfpf_pose_predictor_builder& nc) const;
};

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs,
                 const mfpf_pose_predictor_builder& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs,
                mfpf_pose_predictor_builder& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,
                        const mfpf_pose_predictor_builder& b);

#endif
