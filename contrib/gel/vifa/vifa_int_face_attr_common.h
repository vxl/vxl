// This is gel/vifa/vifa_int_face_attr_common.h
#ifndef VIFA_INT_FACE_ATTR_COMMON_H
#define VIFA_INT_FACE_ATTR_COMMON_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief Computes attribute data about one IntensityFace or a group of IntensityFaces.
//
// Computes attribute data about one IntensityFace or a group of IntensityFaces.
// All attributes which apply to both are referenced in this class.
//
// \author Anthony Hoogs, from DDB in TargetJr
//
// \date Jan 2001
//
// \verbatim
//  Modifications:
//   MPP Mar/Apr 2003, Ported to VXL
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vdgl/vdgl_fit_lines_params.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_sptr.h>
#include <vifa/vifa_coll_lines.h>
#include <vifa/vifa_incr_var.h>
#include <vifa/vifa_int_face_attr_common_params.h>
#include <vifa/vifa_typedefs.h>


class vifa_int_face_attr_common : public vifa_int_face_attr_common_params
{
 protected:
  //: Have the attributes been computed?
  bool      attributes_valid_;

  //: Cache of all edges from face(s).
  edge_2d_list  edges_;

  //: Cache of fitted line segments.
  edge_2d_list  fitted_edges_;

  //: Statistics about fitted edges in cache.
  vifa_incr_var  fitted_edges_stats_;

  //: Cache of collinear lines.
  coll_list    collinear_lines_;
  vifa_incr_var  col_span_;
  vifa_incr_var  col_support_;
  vifa_incr_var  col_contrib_;

  //: Face area, in pixels.
  float      area_;

  //: Ratio of major face moments.
  float      aspect_ratio_;

  //: Edge length^2 / detection area.
  float      complexity_;

  //: Weighted complexity measure.
  float      weighted_complexity_;

  //: Length of boundary.
  float      peri_length_;

  //: Weighted length of boundary.
  float      weighted_peri_length_;

  // Projective parallelism of face boundary.
  float      para_sal_strong_;

  // "Weak" projective parallelism of face boundary.
  float      para_sal_weak_;

 public:
  //: Default constructor
  vifa_int_face_attr_common(void);

  //: Full constructor
  vifa_int_face_attr_common(vdgl_fit_lines_params*    fitter_params,
                            vifa_group_pgram_params*  gpp_s,
                            vifa_group_pgram_params*  gpp_w,
                            vifa_coll_lines_params*   cpp,
                            vifa_norm_params*         np
                           );

  //: Destructor.
  virtual ~vifa_int_face_attr_common();

  // ---
  // Public functional methods
  // ---
  virtual bool  ComputeAttributes() = 0;
  virtual bool  GetAttributes(vcl_vector<float>&  attrs) = 0;
  virtual bool  GetNativeAttributes(vcl_vector<float>&  attrs) = 0;
  virtual bool  valid_p() const { return attributes_valid_; }

  // Data access for non-attributes
  virtual edge_2d_list&  GetEdges() = 0;
  edge_2d_list&          GetFittedEdges();
  double fitted_max() { return fitted_edges_stats_.get_max(); }
  double fitted_min() { return fitted_edges_stats_.get_min(); }
  double fitted_var() { return fitted_edges_stats_.get_var(); }
  double fitted_mean() { return fitted_edges_stats_.get_mean(); }

  coll_list&  get_collinear_lines();
  double col_span_mean() { return col_span_.get_mean(); }
  double col_span_var() { return col_span_.get_var(); }
  double col_support_mean() { return col_support_.get_mean(); }
  double col_support_var() { return col_support_.get_var(); }
  double col_contrib_mean() { return col_contrib_.get_mean(); }
  double col_contrib_var() { return col_contrib_.get_var(); }
  double col_collapse();

  vifa_group_pgram_params_sptr get_strong_group_pgram_params() {return gpp_s_;}
  vifa_group_pgram_params_sptr get_weak_group_pgram_params() { return gpp_w_; }
  void  set_strong_group_pgram_params(const vifa_group_pgram_params&  gp)
    { gpp_s_ = new vifa_group_pgram_params(gp); }
  void  set_weak_group_pgram_params(const vifa_group_pgram_params&  gp)
    { gpp_w_ = new vifa_group_pgram_params(gp); }

  void  set_norm_params(const vifa_norm_params&  np)
    { np_ = new vifa_norm_params(np); }
  float  normalize_intensity(float  raw_intensity)
    { return np_ ? np_->normalize(raw_intensity) : raw_intensity; }

  double      get_contrast_across_edge(vtol_edge_sptr  e,
                                       double          dflt_cont = 0.0);
  vifa_coll_lines_sptr  get_line_along_edge(vtol_edge* edge);

  // Get the face(s) centroid X.
  virtual float  Xo() = 0;

  // Get the face(s) centroid Y.
  virtual float  Yo() = 0;

  virtual float  Area() = 0;
  virtual float  AspectRatio() = 0;
  virtual float  PerimeterLength() = 0;
  virtual float  WeightedPerimeterLength() = 0;
  virtual float  Complexity() = 0;
  virtual float  WeightedComplexity() = 0;

  //: Get proportion of edges with projective parallelism.
  float      StrongParallelSal();

  //: Get proportion of edges with "weak" projective parallelism.
  float      WeakParallelSal();

  virtual float  TwoPeakParallel() = 0;
  virtual float  FourPeakParallel() = 0;
  virtual float  EightyPercentParallel() = 0;

 protected:
  virtual void  init();
  void      fit_lines();
  void      find_collinear_lines();
  bool      find_collinear_match(vtol_edge_2d_sptr edge,
                                 coll_list&        lines,
                                 double            dist_threshold,
                                 coll_iterator&    result
                                );
  float      compute_parallel_sal(vifa_group_pgram_params_sptr  gpp);
};

#endif  // VIFA_INT_FACE_ATTR_COMMON_H
