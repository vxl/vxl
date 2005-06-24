// This is brl/bseg/sdet/sdet_fit_conics.h
#ifndef sdet_fit_conics_h_
#define sdet_fit_conics_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for fitting conic segments to edges
//
//  This version does incremental 
//  fitting to a chain of points and produces a 
//  vcl_vector<vgl_conic_segments_2d>. The actual fitting algorithm is 
//  in vgl/algo and therefore should be of wider applicability.
// 
// \author
//  J.L. Mundy - June 22, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vgl/algo/vgl_fit_conics_2d.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <sdet/sdet_fit_conics_params.h>

class sdet_fit_conics : public sdet_fit_conics_params
{
 public:
  //:Constructors/destructor
  sdet_fit_conics(sdet_fit_conics_params& rpp);

  ~sdet_fit_conics();
  //: Process methods
  bool fit_conics();
  void clear();
  //: Accessors
  void set_edges(vcl_vector<vtol_edge_2d_sptr> const& edges);
  vcl_vector<vsol_conic_2d_sptr>& get_conic_segs();
 protected:
  //:protected methods

  //:members
  bool segs_valid_;      //process state flag
  vcl_vector<vtol_edge_2d_sptr> edges_;// the input edges
  vgl_fit_conics_2d<double> fitter_;//the fitting class
  vcl_vector<vsol_conic_2d_sptr> conic_segs_;
};

#endif // sdet_fit_conics_h_
