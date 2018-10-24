#ifndef msm_subset_aligner_h_
#define msm_subset_aligner_h_
//:
// \file
// \author Tim Cootes
// \brief Align shapes based on subset of points

#include <msm/msm_aligner.h>
#include <mbl/mbl_cloneable_ptr.h>

class mbl_read_props_type;

//: Align shapes based on subset of points
//  Uses a contained aligner object to estimate transformations based
//  on a subset of the points.
class msm_subset_aligner : public msm_aligner
{
protected:
  //: Aligner to apply to subset of points
  mbl_cloneable_ptr<msm_aligner> aligner_;

  //: List of points to use
  std::vector<unsigned> index_;

  //: Set up subset of points
  // subset_points[i]=all_points[index_[i]];
  void get_subset(const msm_points& all_points,
                  msm_points& subset_points) const;

  //: Set up subset of points
  // subset_wts[i]=all_wts[index_[i]];
  void get_subset_wts(const vnl_vector<double>& all_wts,
                      vnl_vector<double>& subset_wts) const;

  //: Set up subset of points
  // subset_wts[i]=all_wts[index_[i]];
  void get_subset_wts(const std::vector<msm_wt_mat_2d>& all_wts,
                      std::vector<msm_wt_mat_2d>& subset_wts) const;

 public:

  ~msm_subset_aligner() override = default;

  //: Aligner to apply to subset
  const msm_aligner& aligner() const { return aligner_; }

  //: Return number of parameters defining the transformation
  unsigned size() const override;

  //: Compute parameters for inverse transformation
  vnl_vector<double> inverse(const vnl_vector<double>&) const override;

  //: Apply the transformation to the given points
  void apply_transform(const msm_points& points,
                               const vnl_vector<double>& trans,
                               msm_points& new_points) const override;

  //: Return scaling applied by the transform with given parameters.
  double scale(const vnl_vector<double>& trans) const override;

  //: Estimate parameter which best map ref_points to points2
  //  Minimises ||points2-T(ref_points)||^2.
  //  Takes advantage of assumed properties of ref_points (eg CoG=origin,
  //  unit size etc) to perform efficiently.
  //
  //  When used with a shape model of form ref_points+Pb, where the modes P
  //  have certain orthogonality properties with respect to the ref shape,
  //  this can give the optimal transformation into a tangent plane, independent
  //  of the current parameters.  In this case a one-shot method can be used
  //  to compute the optimal shape and pose parameters, rather than an iterative
  //  method which is required where the orthogonality properties do not hold,
  //  or where weights are considered.
  void calc_transform_from_ref(const msm_points& ref_points,
                                       const msm_points& points2,
                                       vnl_vector<double>& trans) const override;

  //: Estimate parameter which best map points1 to points2
  //  Minimises ||points2-T(points1)||^2
  void calc_transform(const msm_points& points1,
                              const msm_points& points2,
                              vnl_vector<double>& trans) const override;

  //: Estimate parameters which map points1 to points2 allowing for weights
  //  Minimises sum of weighted squares error in frame of pts2,
  //  ie sum w_i * ||p2_i - T(p1_i)||
  void calc_transform_wt(const msm_points& points1,
                                 const msm_points& points2,
                                 const vnl_vector<double>& wts,
                                 vnl_vector<double>& trans) const override;

  //: Estimate parameters which map points allowing for anisotropic wts
  //  Errors on point i are weighted by wt_mat[i] in pts2 frame.
  //  i.e. error is sum (p2_i-T(p1_i)'*wt_mat[i]*(p2_i-T(p1_i)
  void calc_transform_wt_mat(const msm_points& points1,
                                     const msm_points& points2,
                                     const std::vector<msm_wt_mat_2d>& wt_mat,
                                     vnl_vector<double>& trans) const override;

  //: Apply transform to weight matrices (ie ignore translation component)
  void transform_wt_mat(const std::vector<msm_wt_mat_2d>& wt_mat,
                                const vnl_vector<double>& trans,
                                std::vector<msm_wt_mat_2d>& new_wt_mat) const override;

  //: Returns params of pose such that pose(x) = pose1(pose2(x))
  vnl_vector<double> compose(const vnl_vector<double>& pose1,
                                     const vnl_vector<double>& pose2) const override;

  //: Apply transform to generate points in reference frame
  //  Translate so that the centre of gravity is at the origin
  //  and scale to a unit size (points.vector().magnitude()==1)
  void normalise_shape(msm_points& points) const override;

  //: Find poses which align a set of points
  //  On exit ref_mean_shape is the mean shape in the reference
  //  frame, pose_to_ref[i] maps points[i] into the reference
  //  frame (ie pose is the mapping from the reference frame to
  //  the target frames).
  // \param pose_source - unused
  // \param average_pose Some estimate of the average mapping
  void align_set(const std::vector<msm_points>& points,
                         msm_points& ref_mean_shape,
                         std::vector<vnl_vector<double> >& pose_to_ref,
                         vnl_vector<double>& average_pose,
                         ref_pose_source pose_source) const override;

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  msm_aligner* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;

  //: Initialise from a text stream.
  // The default implementation is for attribute-less normalisers,
  // and throws if it finds any data in the stream.
  void config_from_stream(std::istream &is) override;

};

#endif // msm_subset_aligner_h_
