#ifndef curve_Match_h_
#define curve_Match_h_


#define MAX_LEN 2000
//#define INFINITY 1e20

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_double_2.h>
#include <vcl_utility.h>


#include "DPMatch.h"
#include "curve.h"

#define SUCCESS 1
#define FAILURE 0

inline
double curveMatch(double &euc_dist,                        //!< out
                  vcl_vector<vcl_pair<double,double> > v1, //!< in
                  vcl_vector<vcl_pair<double,double> > v2, //!< in
                  vcl_map<int,int> & mapping,              //!< out
                  vnl_matrix <double> & R_,                //!< out
                  vnl_matrix <double> & T_,                //!< out
                  vnl_matrix <double> & Tbar,              //!< out
                  vcl_vector<int> &tail1,
                  vcl_vector<int> &tail2,
                  double &scale,vgl_point_2d<double>  & e)                           //!< out
{
#if 0 // these variables are not used
  double lambda1=1.0;
  double R=8.0;
  int writeFiles=0;
  double sampleSize=0.1;
  int localize=0;
#endif // 0
  bool endPointMatch=false;

  Curve c1; c1.readDataFromVector(v1);
  Curve c2; c2.readDataFromVector(v2);

 
  DPMatch d1(c1,c2,e);
  //DPMatch d1(c1,c2);
	
  if (endPointMatch)
    d1.endPointMatch();
  else
    d1.match();

  vcl_vector< vcl_pair <int,int> > fmap = d1.finalMap();
  vcl_vector <double> fmapCost = d1.finalMapCost();
  
    //transformed and then computed euclidean distance
  euc_dist = d1.transformed_euclidean_distance();
  mapping  = d1.alignment;
  R_       = d1.R;
  T_       = d1.T;
  d1.detect_tail(tail1,tail2);
  int n1=c1.numPoints();
  int n2=c2.numPoints();
  Tbar     = d1.Tbar;
  scale    = d1.scale;
  return d1.normfinalCost();
}

#endif
