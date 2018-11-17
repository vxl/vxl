//---------------------------------------------------------------------
// This is bbas/bcvr/bcvr_clsd_cvmatch.h
#ifndef bcvr_clsd_cvmatch_h_
#define bcvr_clsd_cvmatch_h_
//:
// \file
// \brief closed elastic curve matching algorithm:
//        On Aligning Curves. T. B. Sebastian, P. N. Klein, B. B. Kimia
//        IEEE Trans. Pattern Anal. Mach. Intell. 25(1): 116-125 (2003)
//
//        adapted from original implementation by Huseyin Tek
//
// \author
//  O.C. Ozcanli - October 11, 2004
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <iostream>
#include <ctime>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsol/vsol_polygon_2d_sptr.h>
#include "bcvr_cv_cor_sptr.h"
#include "bcvr_cvmatch.h"

class bcvr_clsd_cvmatch : public vbl_ref_count
{
 protected:
  //Data
  bsol_intrinsic_curve_2d_sptr _curve1;
  bsol_intrinsic_curve_2d_sptr _curve2;

  DPCostType _cost; //Temporary array for each match (2_n x _m)
  std::vector<int> _leftMask;
  std::vector<int> _rightMask;

  DPMapType _map; //Temporary array for each match (2_n x _m)
  DPMapType _finalMap; //Shortest Path for each start point (_n+1 x 1)
  std::vector<double> _finalCost; //Final cost for each start point (_n+1 x 1)

  int n1_;
  int n2_;
  double R_;
  bool _normalized_stretch_cost;
  int _template_size;
  std::vector<int> XOFFSET;
  std::vector<int> YOFFSET;

  //Functions
  void initializeDPMask1();
  void initializeDPMask2(int s1, int s2);
  void findOptimalPath(int startPoint);
  void computeMiddlePaths(int i, int j);
  void computeDPCosts(int startPoint);
  void findDPCorrespondence(int startPoint);
  virtual double computeIntervalCost(int i, int ip, int j, int jp);

  double stretchCost (const bsol_intrinsic_curve_2d_sptr& curve, int i, int ip);
  double bendCost (const bsol_intrinsic_curve_2d_sptr& curve, int i, int ip);

 public:
  bcvr_clsd_cvmatch();
  bcvr_clsd_cvmatch(const bsol_intrinsic_curve_2d_sptr& c1,
                    const bsol_intrinsic_curve_2d_sptr& c2,
                    double R,
                    int template_size=3);

  bcvr_clsd_cvmatch(const vsol_polygon_2d_sptr& p1,
                    const vsol_polygon_2d_sptr& p2,
                    double R,
                    double rms = 0.0f,  // fit lines to the input polygon before using
                    int template_size = 3);

  ~bcvr_clsd_cvmatch() override = default;;

  //access functions
  double finalCost(int index) {return _finalCost[index];};
  std::vector<double> finalCost() {return _finalCost;};
  FinalMapType finalMap(int index) {return _finalMap[index];};
  int n1() {return n1_;};
  int n2() {return n2_;};

  //: return the best cost around the starting point, but within a restricted arclength range
  //  E.g. if ratio is 1/4 then go L/4 to the right and -L/4 to the left
  //       of the starting point and return the best cost in that range
  double finalBestCostRestrictedStartingPoint(int &index, double ratio, bool get_normalized_cost=true);

  //: return the best cost around the starting point
  double finalBestCost(int &index, bool get_normalized_cost=true);

  //: prepare and return the instance of container class that saves curve correspondence
  bcvr_cv_cor_sptr get_cv_cor(int minIndex);

  void setStretchCostFlag (bool flag) {
        _normalized_stretch_cost = flag;
  }
  void setTemplateSize (int size);

  //display functions (debug)
  void printCost();
  void writeCost(const std::string& f);
  void printMap();

  //: Match() corresponds to closedCurveDPMatch(ClosedDPMatch *d) in original source code
  void Match();
  //void Match(int startPoint);

  bsol_intrinsic_curve_2d_sptr curve1() { return _curve1; }
  bsol_intrinsic_curve_2d_sptr curve2() { return _curve2; }
};

#endif // bcvr_clsd_cvmatch_h_
