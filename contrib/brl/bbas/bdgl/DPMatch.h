//#include "ShockMatchInclude.h"
#ifndef __TBS_DPMATCH_DEF__
#define __TBS_DPMATCH_DEF__
#include <vcl_iostream.h>
#include <vcl_map.h>
#include <vcl_vector.h> 

#include <vcl_utility.h>
#include <bdgl\utils.h>

#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_det.h"
#include "mvl/HMatrix2DEuclideanCompute.h"
#include "mvl/HMatrix2DSimilarityCompute.h"
#include "mvl/HMatrix2DComputeLinear.h"
#include "mvl/HomgPoint2D.h"
#include "mvl/HMatrix2D.h"



#define DP_VERY_LARGE_COST 1E10
#include "Curve.h"

class DPMatch{
 public:
  DPMatch();
  DPMatch(Curve &c1, Curve &c2);
  ~DPMatch(){};
  //access functions
  double finalCost(){return finalCost_;}
  double normfinalCost(){return finalCost_/((curve1_.length()+curve2_.length())/2);}
  double finalCost(double cost){finalCost_=cost; return finalCost_;};
  vcl_vector <vcl_pair <int,int> > finalMap(){return finalMap_;};
  vcl_vector< double > finalMapCost(){return finalMapCost_;};
  int n(){return n_;};
  int m(){return m_;};


  void lambda(vcl_vector<double> l){lambda_=l;};

  void printCost();
  void writeCost(vcl_string f);
  void printMap();
  void printFinalMap();

  Curve curve1(){return curve1_;};
  Curve curve2(){return curve2_;};

  void match();
  void endPointMatch();
  void detect_tail(vcl_vector<int> &tail1,vcl_vector<int> &tail2);
  vnl_matrix<double> R;
  vnl_matrix<double> T;
  vnl_matrix<double> Tbar;
  double scale;
  vcl_map<int,int> alignment;
  vcl_map<int,int> refine_mapping();

  double euclidean_distance(vnl_matrix<double> R,vnl_matrix<double> T,double s);
  double transformed_euclidean_distance();

  void deformation_cost();
  double ds1_;
  double ds2_;
  double dt1_;
  double dt2_;

 protected:
  //Data
  Curve curve1_;
  Curve curve2_;
  vcl_vector< vcl_vector<double> > cost_;
  vcl_vector< vcl_vector< vcl_pair <int,int> > > map_;
  vcl_vector< vcl_pair <int,int> > finalMap_;
  double finalCost_;
  vcl_vector< double > finalMapCost_;
  int n_;
  int m_;
  bool flip_;

  int numCostElems_;
  double R1_;
  double R2_;
  vcl_vector<double> lambda_;

  //Functions
  void initializeDPCosts();
  void computeDPCosts();
  void findDPCorrespondence();
  void findDPCorrespondence(int n, int m);
  void findEndPoint();
  double computeIntervalCost(int i, int ip, int j, int jp);
};



#endif



