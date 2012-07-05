#ifndef _bcvr_cvmatch_h
#define _bcvr_cvmatch_h
//---------------------------------------------------------------------
// This is brcv/rec/dbcvr/bcvr_cvmatch.h
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

  bsol_intrinsic_curve_2d_sptr    _curve1;
  bsol_intrinsic_curve_2d_sptr    _curve2;

  DPCostType        _DPCost;          //DPMap of cost: n*m array of double
  DPMapType        _DPMap;          //DPMap of prev point vcl_map: n*m array of vcl_pair of index
  FinalMapType      _finalMap;        //alignment curve
  vcl_vector<double>  _finalMapCost;      //cost on alignment curve
  double          _finalCost;        //final cost

  double  _R;
  bool _normalized_stretch_cost;
  int _template_size;
  vcl_vector<int> XOFFSET;
  vcl_vector<int> YOFFSET;

public:

  //Result of matching:
  DPCostType*        DPCost()      { return &_DPCost; }
  DPMapType*        DPMap()      { return &_DPMap; }
  FinalMapType*      finalMap()    { return &_finalMap; }
  vcl_vector<double>*  finalMapCost() { return &_finalMapCost; }
  double          finalCost()    { return _finalCost; }

  int getFMapFirst (int i) {
    assert (i>-1);
    return (*finalMap())[i].first;
  }
  int getFMapSecond (int i) {
    assert (i>-1);
    return (*finalMap())[i].second;
  }

  vcl_string    _fileName1, _fileName2;
  bsol_intrinsic_curve_2d_sptr curve1() { return _curve1; }
  bsol_intrinsic_curve_2d_sptr curve2() { return _curve2; }
  void setCurve1 (const bsol_intrinsic_curve_2d_sptr& c1) {
    _curve1 = c1;
    //: just in case
    _curve1->computeProperties();
  }
  void setCurve2 (const bsol_intrinsic_curve_2d_sptr& c2) {
    _curve2 = c2;
    //: just in case
    _curve2->computeProperties();
  }
  void setFinalMap (FinalMapType map) {
    _finalMap = map;
  }

  void setFinalMapCost (vcl_vector<double>* map) {
    _finalMapCost = *map;
  }

  void setFinalMap (int i, int first, int second) {
    _finalMap[i].first = first;
        _finalMap[i].second = second;
  }
  void setStretchCostFlag (bool flag) {
        _normalized_stretch_cost = flag;
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

#endif

