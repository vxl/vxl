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
#include <sdet/sdet_fit_lines_params.h>
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
  bool      _attributes_valid;

  //: Cache of all edges from face(s).
  edge_2d_list  _edges;

  //: Cache of fitted line segments.
  edge_2d_list  _fitted_edges;

  //: Statistics about fitted edges in cache.
  vifa_incr_var  _fitted_edges_stats;

  //: Cache of collinear lines.
  coll_list    _collinear_lines;
  vifa_incr_var  _col_span;
  vifa_incr_var  _col_support;
  vifa_incr_var  _col_contrib;

  //: Face area, in pixels.
  float      _area;

  //: Ratio of major face moments.
  float      _aspect_ratio;

  //: Edge length^2 / detection area.
  float      _complexity;

  //: Weighted complexity measure.
  float      _weighted_complexity;

  //: Length of boundary.
  float      _peri_length;

  //: Weighted length of boundary.
  float      _weighted_peri_length;

  // Projective parallelism of face boundary.
  float      _para_sal_strong;

  // "Weak" projective parallelism of face boundary.
  float      _para_sal_weak;

 public:
  //: Default constructor
  vifa_int_face_attr_common(void);

  //: Full constructor
  vifa_int_face_attr_common(sdet_fit_lines_params*    fitter_params,
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
  virtual bool  valid_p() const
    { return _attributes_valid; }

  // Data access for non-attributes
  virtual edge_2d_list&  GetEdges() = 0;
  edge_2d_list&      GetFittedEdges();
  double    fitted_max()
    { return _fitted_edges_stats.get_max(); }
  double    fitted_min()
    { return _fitted_edges_stats.get_min(); }
  double    fitted_var()
    { return _fitted_edges_stats.get_var(); }
  double    fitted_mean()
    { return _fitted_edges_stats.get_mean(); }

  coll_list&  get_collinear_lines();
  double    col_span_mean()
    { return _col_span.get_mean(); }
  double    col_span_var()
    { return _col_span.get_var(); }
  double    col_support_mean()
    { return _col_support.get_mean(); }
  double    col_support_var()
    { return _col_support.get_var(); }
  double    col_contrib_mean()
    { return _col_contrib.get_mean(); }
  double    col_contrib_var()
    { return _col_contrib.get_var(); }
  double    col_collapse();

  vifa_group_pgram_params_sptr  get_strong_group_pgram_params()
    { return _gpp_s; }
  vifa_group_pgram_params_sptr  get_weak_group_pgram_params()
    { return _gpp_w; }
  void  set_strong_group_pgram_params(const vifa_group_pgram_params&  gp)
    { _gpp_s = new vifa_group_pgram_params(gp); }
  void  set_weak_group_pgram_params(const vifa_group_pgram_params&  gp)
    { _gpp_w = new vifa_group_pgram_params(gp); }

  void  set_norm_params(const vifa_norm_params&  np)
    { _np = new vifa_norm_params(np); }
  float  normalize_intensity(float  raw_intensity)
    { return (_np) ? _np->normalize(raw_intensity) : raw_intensity; }

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
