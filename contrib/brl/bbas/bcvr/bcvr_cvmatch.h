//---------------------------------------------------------------------
// This is brl/bbas/bcvr/bcvr_cvmatch.h
#ifndef bcvr_cvmatch_h_
#define bcvr_cvmatch_h_
//:
// \file
// \brief open curve matching adapted from original source code of Thomas Sebastian
//        On Aligning Curves.
//        T. B. Sebastian, P. N. Klein, B. B. Kimia
//        IEEE Trans. Pattern Anal. Mach. Intell. 25(1): 116-125 (2003)
//
// \author
//  MingChing Chang
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <bsol/bsol_intrinsic_curve_2d.h>
#include <bsol/bsol_intrinsic_curve_2d_sptr.h>
#include <vcl_cassert.h>

#include "bcvr_cv_cor_sptr.h"

#include <vbl/vbl_ref_count.h>
#define DP_VERY_LARGE_COST 1E10

//value in (-Pi,Pi]
double curve_fixAngleMPiPi (double a);

//Does a1-a2, value in (-Pi,Pi]
double curve_angleDiff (double a1, double a2);

double  curve_maxof (double a, double b, double c);

typedef vcl_vector< vcl_vector<double> >            DPCostType;
typedef vcl_vector< vcl_vector< vcl_pair <int,int> > >  DPMapType;
typedef vcl_vector< vcl_pair<int,int> >            FinalMapType;

class bcvr_cvmatch : public vbl_ref_count
{
 protected:

  bsol_intrinsic_curve_2d_sptr    curve1_;
  bsol_intrinsic_curve_2d_sptr    curve2_;

  DPCostType        DPCost_;          //DPMap of cost: n*m array of double
  DPMapType        DPMap_;          //DPMap of prev point vcl_map: n*m array of vcl_pair of index
  FinalMapType      finalMap_;        //alignment curve
  vcl_vector<double>  finalMapCost_;      //cost on alignment curve
  double          finalCost_;        //final cost

  double  R_;
  bool normalized_stretch_cost_;
  int template_size_;
  vcl_vector<int> XOFFSET;
  vcl_vector<int> YOFFSET;

 public:

  //Result of matching:
  DPCostType*        DPCost()      { return &DPCost_; }
  DPMapType*        DPMap()      { return &DPMap_; }
  FinalMapType*      finalMap()    { return &finalMap_; }
  vcl_vector<double>*  finalMapCost() { return &finalMapCost_; }
  double          finalCost()    { return finalCost_; }

  int getFMapFirst (int i) {
    assert (i>=0);
    return (*finalMap())[i].first;
  }
  int getFMapSecond (int i) {
    assert (i>=0);
    return (*finalMap())[i].second;
  }

  vcl_string    fileName1_, fileName2_;
  bsol_intrinsic_curve_2d_sptr curve1() { return curve1_; }
  bsol_intrinsic_curve_2d_sptr curve2() { return curve2_; }
  void setCurve1 (const bsol_intrinsic_curve_2d_sptr& c1) {
    curve1_ = c1;
    // just in case
    curve1_->computeProperties();
  }
  void setCurve2 (const bsol_intrinsic_curve_2d_sptr& c2) {
    curve2_ = c2;
    // just in case
    curve2_->computeProperties();
  }
  void setFinalMap (FinalMapType map) {
    finalMap_ = map;
  }

  void setFinalMapCost (vcl_vector<double>* map) {
    finalMapCost_ = *map;
  }

  void setFinalMap (int i, int first, int second) {
    finalMap_[i].first = first;
        finalMap_[i].second = second;
  }
  void setStretchCostFlag (bool flag) {
        normalized_stretch_cost_ = flag;
  }
  void setTemplateSize (int size);

  double stretchCost (bsol_intrinsic_curve_2d_sptr curve, int i, int ip) {
      return curve->arcLength(i) - curve->arcLength(ip);}
  double bendCost (bsol_intrinsic_curve_2d_sptr curve, int i, int ip) {
      return curve_angleDiff (curve->angle(i), curve->angle(ip));}

  bcvr_cvmatch ();
  bcvr_cvmatch (bsol_intrinsic_curve_2d_sptr c1, bsol_intrinsic_curve_2d_sptr c2);
  virtual ~bcvr_cvmatch ();

  void initializeDPCosts();
  virtual double computeIntervalCost (int i, int ip, int j, int jp);

  void computeDPCosts ();
  void findDPCorrespondence ();

  void Match ();

  //: prepare and return the instance of container class that saves curve correspondence
  bcvr_cv_cor_sptr get_cv_cor();

  void GetAverageCurveFromAMatch (bsol_intrinsic_curve_2d_sptr AverageCurve);
  void GetSumCurveFromAMatch (bsol_intrinsic_curve_2d_sptr SumCurve);
  void GetMorphingCurvesFromAMatch (bsol_intrinsic_curve_2d_sptr MorphingCurves, int NumMorphs);

  //Debug Print...
  void ListDPTable (void);
  void ListAlignCurve (void);
  void SaveDPTable (void);
  void SaveAlignCurve (void);
};

#endif // bcvr_cvmatch_h_
