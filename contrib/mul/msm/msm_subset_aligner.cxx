//:
// \file
// \author Tim Cootes
// \brief Align shapes based on subset of points

#include <iostream>
#include <cstddef>
#include <sstream>
#include "msm_subset_aligner.h"
#include <vnl/vnl_vector.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_parse_int_list.h>
#include <mbl/mbl_exception.h>

//: Set up subset of points
// subset_points[i]=all_points[index_[i]];
void msm_subset_aligner::get_subset(const msm_points& all_points,
                msm_points& subset_points) const
{
  // Assumes all index_ values are valid.
  subset_points.set_size(index_.size());
  for (unsigned i=0;i<index_.size();++i)
    subset_points.set_point(i,all_points[index_[i]]);
}

//: Set up subset of points
// subset_wts[i]=all_wts[index_[i]];
void msm_subset_aligner::get_subset_wts(const vnl_vector<double>& all_wts,
                    vnl_vector<double>& subset_wts) const
{
  // Assumes all index_ values are valid.
  subset_wts.set_size(index_.size());
  for (unsigned i=0;i<index_.size();++i)
    subset_wts[i]=all_wts[index_[i]];
}

//: Set up subset of points
// subset_wts[i]=all_wts[index_[i]];
void msm_subset_aligner::get_subset_wts(const std::vector<msm_wt_mat_2d>& all_wts,
                      std::vector<msm_wt_mat_2d>& subset_wts) const
{
  // Assumes all index_ values are valid.
  subset_wts.resize(index_.size());
  for (unsigned i=0;i<index_.size();++i)
    subset_wts[i]=all_wts[index_[i]];
}


//: Return number of parameters defining the transformation
unsigned msm_subset_aligner::size() const
{
  return aligner().size();
}

  //: Compute parameters for inverse transformation
vnl_vector<double> msm_subset_aligner::inverse(const vnl_vector<double>& t) const
{
  return aligner().inverse(t);
}

  //: Apply the transformation to the given points
void msm_subset_aligner::apply_transform(const msm_points& points,
                                              const vnl_vector<double>& trans,
                                              msm_points& new_points) const
{
  aligner().apply_transform(points,trans,new_points);
}

//: Return scaling applied by the transform with given parameters.
double msm_subset_aligner::scale(const vnl_vector<double>& trans) const
{
  return aligner().scale(trans);
}

//: Estimate parameter which best map ref_points to points2
void msm_subset_aligner::calc_transform_from_ref(const msm_points& ref_pts,
                                                      const msm_points& pts2,
                                                      vnl_vector<double>& trans) const
{
  msm_points subset_pts,subset_pts2;
  get_subset(ref_pts,subset_pts);
  get_subset(pts2,subset_pts2);
  aligner().calc_transform(subset_pts,subset_pts2,trans);
}

  //: Estimate parameter which best map points1 to points2
  //  Minimises ||points2-T(points1)||^2
void msm_subset_aligner::calc_transform(const msm_points& pts1,
                                             const msm_points& pts2,
                                             vnl_vector<double>& trans) const
{
  msm_points subset_pts,subset_pts2;
  get_subset(pts1,subset_pts);
  get_subset(pts2,subset_pts2);
  aligner().calc_transform(subset_pts,subset_pts2,trans);
}

  //: Estimate parameters which map points1 to points2 allowing for weights
  //  Minimises sum of weighted squares error in frame of pts2,
  //  ie sum w_i * ||p2_i - T(p1_i)||
void msm_subset_aligner::calc_transform_wt(const msm_points& pts1,
                                                const msm_points& pts2,
                                                const vnl_vector<double>& wts,
                                                vnl_vector<double>& trans) const
{
  msm_points subset_pts,subset_pts2;
  get_subset(pts1,subset_pts);
  get_subset(pts2,subset_pts2);
  vnl_vector<double> subset_wts;
  get_subset_wts(wts,subset_wts);
  aligner().calc_transform_wt(subset_pts,subset_pts2,subset_wts,trans);
}

  //: Estimate parameters which map points allowing for anisotropic wts
//  Errors on point i are weighted by wt_mat[i] in pts2 frame.
//  ie error is sum (p2_i-T(p1_i)'*wt_mat[i]*(p2_i-T(p1_i)
void msm_subset_aligner::calc_transform_wt_mat(const msm_points& pts1,
                                                    const msm_points& pts2,
                                                    const std::vector<msm_wt_mat_2d>& wt_mat,
                                                    vnl_vector<double>& trans) const
{
  msm_points subset_pts,subset_pts2;
  get_subset(pts1,subset_pts);
  get_subset(pts2,subset_pts2);
  std::vector<msm_wt_mat_2d> subset_wts;
  get_subset_wts(wt_mat,subset_wts);
  aligner().calc_transform_wt_mat(subset_pts,subset_pts2,subset_wts,trans);
}

  //: Apply transform to weight matrices (ie ignore translation component)
void msm_subset_aligner::transform_wt_mat(const std::vector<msm_wt_mat_2d>& wt_mat,
                                               const vnl_vector<double>& trans,
                                               std::vector<msm_wt_mat_2d>& new_wt_mat) const
{
  aligner().transform_wt_mat(wt_mat,trans,new_wt_mat);
}

//: Returns params of pose such that pose(x) = pose1(pose2(x))
vnl_vector<double> msm_subset_aligner::compose(
                         const vnl_vector<double>& pose1,
                         const vnl_vector<double>& pose2) const
{
  return aligner().compose(pose1,pose2);
}

//: Apply transform to generate points in some reference frame
//  For instance, depending on transform, may translate so the
//  centre of gravity is at the origin and scale to a unit size.
void msm_subset_aligner::normalise_shape(msm_points& points) const
{
  // Note: Currently apply normalisation to whole shape
  aligner().normalise_shape(points);
}


//: Find poses which align a set of points
//  On exit ref_mean_shape is the mean shape in the reference
//  frame, pose_to_ref[i] maps points[i] into the reference
//  frame (ie pose is the mapping from the reference frame to
//  the target frames).
// \param average_pose Average mapping from ref to target frame
void msm_subset_aligner::align_set(const std::vector<msm_points>& points,
                                        msm_points& ref_mean_shape,
                                        std::vector<vnl_vector<double> >& pose_to_ref,
                                        vnl_vector<double>& average_pose,
                                        ref_pose_source) const
{
  std::size_t n_shapes = points.size();
  assert(n_shapes>0);
  pose_to_ref.resize(n_shapes);

  // Use first shape as initial reference
  ref_mean_shape = points[0];
  normalise_shape(ref_mean_shape);

  vnl_vector<double> pose_from_ref;

  average_pose.set_size(2);
  average_pose.fill(0);

  for (unsigned i=0;i<n_shapes;++i)
  {
    calc_transform_from_ref(ref_mean_shape,points[i],pose_from_ref);
    pose_to_ref[i]=inverse(pose_from_ref);
    average_pose+=pose_from_ref;
  }

  mean_of_transformed(points,pose_to_ref,ref_mean_shape);
  average_pose/=n_shapes;
}

//=======================================================================

std::string msm_subset_aligner::is_a() const
{
  return std::string("msm_subset_aligner");
}

//: Create a copy on the heap and return base class pointer
msm_aligner* msm_subset_aligner::clone() const
{
  return new msm_subset_aligner(*this);
}

//: Print class to os
void msm_subset_aligner::print_summary(std::ostream& os) const
{
  os<<" { aligner: ";
  if (aligner_.isDefined()) os<<*aligner_;
  else                      os<<"-";
  os<<" subset of "<<index_.size()<<" points. } ";
}

//: Save class to binary file stream
void msm_subset_aligner::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,short(1));  // Version number
  vsl_b_write(bfs,aligner_);
  vsl_b_write(bfs,index_);
}


//: Load class from binary file stream
void msm_subset_aligner::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,aligner_);
      vsl_b_read(bfs,index_);
      break;
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << std::endl;
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

//: Initialise from a text stream.
void msm_subset_aligner::config_from_stream(std::istream &is)
{
  std::string s = mbl_parse_block(is);
  std::istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  std::istringstream aligner_str(props.get_required_property("aligner"));
  aligner_=msm_aligner::create_from_stream(aligner_str)->clone();

  index_.resize(0);
  std::istringstream index_str(props.get_required_property("index"));
  mbl_parse_int_list(index_str,std::back_inserter(index_),unsigned());

  mbl_read_props_look_for_unused_props(
      "msm_subset_aligner::config_from_stream", props, mbl_read_props_type());
}
