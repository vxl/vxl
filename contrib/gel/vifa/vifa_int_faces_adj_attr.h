// This is gel/vifa/vifa_int_faces_adj_attr.h
#ifndef VIFA_INT_FACES_ADJ_ATTR_H
#define VIFA_INT_FACES_ADJ_ATTR_H

//-----------------------------------------------------------------------------
//:
// \file
// \brief Collects statistics about a group of faces.
// Collects statistics about a group of faces that have been grown from
// a seed face.  Will either take a seed and grow the regions itself or
// take a collection of faces on faith.
//
// *** WARNING *** Probably does not calculate neighborhoods correctly
// for depths > 1 (probably includes the seed face back into the neighborhood.)
//
// \author Roddy Collins, from DDB in TargetJr
//
// \verbatim
//  Modifications:
//   MPP May 2003, Ported to VXL
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vdgl/vdgl_fit_lines_params.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vifa/vifa_coll_lines_params.h>
#include <vifa/vifa_group_pgram_params.h>
#include <vifa/vifa_int_faces_attr.h>
#include <vifa/vifa_typedefs.h>


class vifa_int_faces_adj_attr: public vifa_int_faces_attr
{
 protected:
  enum
  {
    BAD_DEPTH = -1
  };

  bool            closure_valid_;

  vtol_intensity_face_sptr  seed_;
  vifa_int_face_attr_sptr    seed_attr_;
  int              depth_;
  int              size_filter_;

  float            junk_area_percentage_;
  int              junk_count_;
  float            junk_percent_;
  float            junk_area_ratio_;

  // attribute slots

  // ratios of seed attr to neighborhood mean
  vcl_vector<float>      mean_ratios_;
  vcl_vector<float>      min_ratios_;

 public:
  vifa_int_faces_adj_attr();
  vifa_int_faces_adj_attr(vtol_intensity_face_sptr  seed,
                          int              depth,
                          int              size_filter = -1,
                          vdgl_fit_lines_params*    fitter_params = NULL,
                          vifa_group_pgram_params*  gpp_s = NULL,
                          vifa_group_pgram_params*  gpp_w = NULL,
                          vifa_coll_lines_params*    cpp = NULL,
                          vifa_norm_params*      np = NULL,
                          vifa_int_face_attr_factory*  factory = NULL,
                          float          junk_area_percentage = 0.2
                         );

  vifa_int_faces_adj_attr(vtol_intensity_face_sptr  seed,
                          int                       depth,
                          iface_list&               neighborhood,
                          int                       size_filter  =  -1,
                          vdgl_fit_lines_params*    fitter_params  =  NULL,
                          vifa_group_pgram_params*  gpp_s  =  NULL,
                          vifa_group_pgram_params*  gpp_w  =  NULL,
                          vifa_coll_lines_params*   cpp  =  NULL,
                          vifa_norm_params*         np  =  NULL,
                          vifa_int_face_attr_factory*    factory  =  NULL,
                          float                     junk_area_percentage  =  0.2
                         );

  virtual iface_list&    GetFaces();
  iface_list*        GetFaceList();

  virtual bool  ComputeAttributes();
  virtual bool  GetAttributes(vcl_vector<float>&  attrs);
  static void    GetAttributeNames(vcl_vector<vcl_string>&  names);
  virtual bool  GetNativeAttributes(vcl_vector<float>&  attrs);

  vtol_intensity_face_sptr  GetSeed() const
    { return seed_; }
  void            SetSeed(vtol_intensity_face_sptr  seed);

  int    GetDepth() const
    { return depth_; }
  void  SetDepth(int depth)
    { depth_ = depth; closure_valid_ = false; }

  int    NeighborhoodSize();

  int    GetJunkCount()
    { return junk_count_; }
  float  GetJunkPercent()
    { return junk_percent_; }
  float  GetJunkAreaRatio()
    { return junk_area_ratio_; }

  bool  compute_closure();
  float  Collinearity();

  //
  // attribute gets here
  //

  // Ratio of seed to neighbors
  float  GetRatioAttr(int  attr_index);

  // Ratio of seed to min of neighbors
  float  GetMinRatioAttr(int  attr_index);

 protected:
  virtual void  init();
  bool  add_unique_face(iface_list&               facelist,
                        vtol_intensity_face_sptr  face,
                        int                       size_filter
                       );
  void  compute_closure_step(int                       current_depth,
                             vtol_intensity_face_sptr  seed
                            );

  // Retrieve the iface adjacent to a given iface at an edge (if available)
  vtol_intensity_face_sptr
    get_adjacent_face_at_edge(vtol_intensity_face_sptr&  known_face,
                              vtol_edge_2d*              e);

  // Retrieve all ifaces adjacent to a given face
  iface_list*  get_adjacent_faces(vtol_intensity_face_sptr&  known_face);
};


#endif  // VIFA_INT_FACES_ADJ_ATTR_H
