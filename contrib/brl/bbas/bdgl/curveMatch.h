#ifndef _curve_Match_h_
#define _curve_Match_h_


#define MAX_LEN 2000
#define INFINITY 1e20

#include<vnl\vnl_matrix.h>
#include<vcl_utility.h>


#include "DPMatch.h"
#include "curve.h"

#define SUCCESS 1
#define FAILURE 0

double curveMatch(double &euc_dist,vcl_vector<vcl_pair<double,double> > v1,vcl_vector<vcl_pair<double,double> > v2,
		  vcl_map<int,int> & mapping,vnl_matrix <double> & R_,vnl_matrix <double> & T_,vnl_matrix <double> & Tbar,vcl_vector<int> &tail1,
		  vcl_vector<int> &tail2,double &scale)
{

  double lambda1=1.0;
  double R=8.0;
  int writeFiles=0;
  double sampleSize=0.1;
  int endPointMatch=0;
  int localize=0;
	 
  Curve c1;
  Curve c2;

  c1.readDataFromVector(v1);
  c2.readDataFromVector(v2);

  DPMatch d1(c1,c2);

  if (endPointMatch)
    d1.endPointMatch();
  else
    d1.match();

  vcl_vector< vcl_pair <int,int> > fmap = d1.finalMap();
  vcl_vector <double> fmapCost = d1.finalMapCost();

  //transformed and then computed euclidean distance
  double  totalCost=d1.normfinalCost();
  euc_dist = d1.transformed_euclidean_distance();
 
  R_=d1.R;
  T_=d1.T;
  Tbar=d1.Tbar;	
  scale=d1.scale;
  
  mapping=d1.alignment;
  return totalCost;
}

#endif
