// This is gel/vifa/vifa_int_faces_attr.h
#ifndef VIFA_INT_FACES_ATTR_H
#define VIFA_INT_FACES_ATTR_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief Collect statistics about a group of regions.
//
// Collects statistics about a group of regions.  (For region groups
// formed around a seed region, see IntFacesAdjAttr.)
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
#include <vifa/vifa_int_face_attr.h>
#include <vifa/vifa_int_face_attr_factory.h>
#include <vifa/vifa_parallel.h>
#include <vifa/vifa_typedefs.h>


// Create the index for histogrammed attributes.  Must indicate the
// position in attr_get_funcs[].
#define NUM_HIST_ATTRIBUTES 15

// type needed for attr_get_funcs.
typedef float (vifa_int_face_attr::*AttrFuncPtr)();


class vifa_int_faces_attr : public vifa_int_face_attr_common
{
 protected:
  iface_list     faces_;

  // should be a map, but until operator<() is const in smart_ptr.h...
  //  map<vifa_intensity_face_sptr, vifa_int_face_attr_sptr, less<vifa_intensity_face_sptr> > attr_map_;
  attr_list      attr_map_;

  vcl_vector<float>  centroid_;      // cache for region group center-of-mass
  float          perimeter_;
  float          weighted_perimeter_;

  // Vector of vifa_int_face_attr methods to get an attribute val for one
  // face.  Defines the histogrammed attribute vector.  Length must be
  // NUM_HIST_ATTRIBUTES.
  static AttrFuncPtr attr_get_funcs[];
  static const char* const attr_names[];
  static float attr_min_vals[];

  // attribute slots

  // Histogrammed attributes are grouped together into parallel
  // vectors.  Histograms compute mean and standard deviation.
  // Attributes defined by attr_get_funcs.
  //vcl_vector<vifa_histogram_sptr> attr_hists_;
  incr_var_list  attr_vec_;

  float cached_2_parallel_;
  float cached_4_parallel_;
  float cached_80_parallel_;
  vifa_parallel* npobj_;
  vifa_int_face_attr_factory* factory_;
 public:
  vifa_int_faces_attr(vdgl_fit_lines_params*       fitter_params = NULL,
                      vifa_group_pgram_params*     gpp_s = NULL,
                      vifa_group_pgram_params*     gpp_w = NULL,
                      vifa_coll_lines_params*      cpp = NULL,
                      vifa_norm_params*            np = NULL,
                      vifa_int_face_attr_factory*  factory = NULL
                     );
  vifa_int_faces_attr(iface_list&                  v,
                      vdgl_fit_lines_params*       fitter_params = NULL,
                      vifa_group_pgram_params*     gpp_s = NULL,
                      vifa_group_pgram_params*     gpp_w = NULL,
                      vifa_coll_lines_params*      cpp = NULL,
                      vifa_norm_params*            np = NULL,
                      vifa_int_face_attr_factory*  factory = NULL
                     );

  virtual ~vifa_int_faces_attr();

  // ---
  // Public functional methods
  // ---

  virtual bool  ComputeAttributes();
  virtual bool  GetAttributes(vcl_vector<float>& attrs);
  static void    GetAttributeNames(vcl_vector<vcl_string>& names);
  virtual bool  GetNativeAttributes(vcl_vector<float>& attrs);

  // Data access for non-attributes
  virtual iface_list&  GetFaces()
    { return faces_; };
  virtual void    SetFaces(iface_list& v);
  edge_2d_list&    GetEdges();

  // hmm, need to think about this

  vifa_int_face_attr_sptr factory_new_attr(vtol_intensity_face_sptr face);

  //: centroid X
  float Xo();
  //: centroid Y
  float Yo();

  // ---
  // Attribute computation
  // ---

  // Histogrammed attributes.  See attr_get_funcs in .C file for
  // attribute index.  These are not weighted by area size.
  float GetMeanAttr(int attr_index); // mean of indexed histogrammed attribute
  float GetSDAttr(int attr_index); // sd of indexed histogrammed attribute
  float GetMinAttr(int attr_index); // min of indexed histogrammed attribute
  float GetMaxAttr(int attr_index); // max of indexed histogrammed attribute
  int NumHistAttributes()
    { return NUM_HIST_ATTRIBUTES; }

  // non-histogrammed attributes (one value per vifa_int_faces_attr).
  float Area();                 // area of all faces
  float AspectRatio();          // ratio of major moments
  float PerimeterLength();      // length of boundary, in pixels
  float Complexity();           // edge length^2 / detection area
  float WeightedComplexity();
  float WeightedPerimeterLength();

  float TwoPeakParallel();
  float FourPeakParallel();
  float EightyPercentParallel();

  // a hack for weird linking problems on Windows with attr_get_funcs[]
  static float  CallAttrFunction(vifa_int_face_attr* seed, int i);

  static const char*  GetBaseAttrName(int i);

 protected:
  virtual void  init();
  void          SetNP();
  void          ComputeCentroid();
  virtual bool  ComputeSingleFaceAttributes(bool forceP=false);

  // Caller must delete returned edge_list when done
  edge_list*    GetPerimeterEdges();
  vifa_histogram_sptr  MakeAttrHist(vcl_vector<float>& attr_vals);
};

typedef vbl_smart_ptr<vifa_int_faces_attr>  vifa_int_faces_attr_sptr;


#endif  // VIFA_INT_FACES_ATTR_H
