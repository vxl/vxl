// This is gel/vifa/vifa_int_face_attr.h
#ifndef VIFA_INT_FACE_ATTR_H_
#define VIFA_INT_FACE_ATTR_H_

//-----------------------------------------------------------------------------
//:
// \file
// \brief Collects attribute information about an individual intensity face.
//
// \author Roddy Collins, from DDB in TargetJr
//
// \verbatim
//  Modifications:
//   MPP May 2003, Ported to VXL
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vbl/vbl_smart_ptr.h>
#include <vdgl/vdgl_fit_lines_params.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vifa/vifa_histogram.h>
#include <vifa/vifa_int_face_attr_common.h>
#include <vifa/vifa_parallel.h>
#include <vifa/vifa_typedefs.h>


class vifa_int_face_attr: public vifa_int_face_attr_common
{
 protected:

  //: The face whose attributes we store
  vtol_intensity_face_sptr  face_;

  float            cached_min_;
  float            cached_max_;
  float            cached_mean_;
  float            cached_var_;
  float            cached_2_parallel_;
  float            cached_4_parallel_;
  int              cached_80_parallel_;
  vifa_parallel*   npobj_;

 public:

  vifa_int_face_attr(vdgl_fit_lines_params*    fitter_params = NULL,
                     vifa_group_pgram_params*  gpp = NULL,
                     vifa_group_pgram_params*  gpp_w = NULL,
                     vifa_norm_params*         np = NULL
                    );
  vifa_int_face_attr(vtol_intensity_face_sptr  f,
                     vdgl_fit_lines_params*    fitter_params = NULL,
                     vifa_group_pgram_params*  gpp = NULL,
                     vifa_group_pgram_params*  gpp_w = NULL,
                     vifa_norm_params*         np = NULL
                    );
  virtual ~vifa_int_face_attr();

  // ---
  // Public functional methods
  // ---

  bool         ComputeAttributes();
  bool         GetAttributes(vcl_vector<float>&  attrs);
  static void  GetAttributeNames(vcl_vector<vcl_string>&  names);
  bool         GetNativeAttributes(vcl_vector<float>&  attrs);

  // Data access for non-attributes
  vtol_intensity_face_sptr  GetFace() const { return face_; }
  void                      SetFace(vtol_intensity_face_sptr  f);
  edge_2d_list&             GetEdges();

  //: Centroid X
  float  Xo() { return face_->Xo(); }

  //: Centroid Y
  float  Yo() { return face_->Yo(); }

  //: Centroid Z
  float  Zo() { return face_->Zo(); }

  // ---
  // Intensity attribute computations
  // ---

  //: Min intensity
  float  IntMin() { return cached_min_; }

  //: Max intensity
  float  IntMax() { return cached_max_; }

  //: Mean intensity
  float  IntMean() { return cached_mean_; };

  //: Intensity variance
  float  IntVar() { return cached_var_; }

  // ---
  // Geometric attribute computations
  // ---

  //: Area
  float  Area() { return (float)(GetFace() ? face_->Npix() : -1); }

  //: Ratio of major moments
  float  AspectRatio();

  //: Length of boundary, in pixels
  float  PerimeterLength();

  float  WeightedPerimeterLength();
  float  Complexity();

  //: Edge length^2 / detection area
  float  WeightedComplexity();

  float  TwoPeakParallel();
  float  FourPeakParallel();
  float  EightyPercentParallel();

 protected:

  void  ComputeCacheValues();
  void  SetNP();
};

typedef vbl_smart_ptr<vifa_int_face_attr>  vifa_int_face_attr_sptr;

typedef vcl_vector<vifa_int_face_attr_sptr>            attr_list;
typedef vcl_vector<vifa_int_face_attr_sptr>::iterator  attr_iterator;


#endif  // VIFA_INT_FACE_ATTR_H_
