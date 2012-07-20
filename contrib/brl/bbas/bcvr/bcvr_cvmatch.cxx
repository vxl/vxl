#include "bcvr_cvmatch.h"
//:
// \file
#include "bcvr_cv_cor.h"

#include <vcl_cmath.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_utility.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>

#include <vnl/vnl_math.h>

//: return value in (-Pi,Pi]
double curve_fixAngleMPiPi (double a)
{
  while (a <= -vnl_math::pi) a += 2*vnl_math::pi;
  while (a >   vnl_math::pi) a -= 2*vnl_math::pi;
  return a;
}

//Does a1-a2, value in (-Pi,Pi]
double curve_angleDiff (double a1, double a2)
{
  return curve_fixAngleMPiPi(a1-a2);
}

double  curve_maxof (double a, double b, double c)
{
  if (a>b && a>c)
    return a;
  else if (b>c && b>a)
    return b;
  else if (c>b && c>a)
    return c;
  else
    return a;
}

bcvr_cvmatch::bcvr_cvmatch()
{
  _R = 10;
  _normalized_stretch_cost = false;
  setTemplateSize(3);
}

bcvr_cvmatch::bcvr_cvmatch (bsol_intrinsic_curve_2d_sptr c1, bsol_intrinsic_curve_2d_sptr c2)
{
  _curve1 = c1;
  _curve2 = c2;
  _R = 10;
  _normalized_stretch_cost = false;
  setTemplateSize(3);
}

bcvr_cvmatch::~bcvr_cvmatch ()
{
}

void bcvr_cvmatch::setTemplateSize (int temp_size)
{
  XOFFSET.clear();
  YOFFSET.clear();

  if (temp_size == 3) { // 3x3 template
    _template_size = 11;

    int dummyX[11] = {-1,-2,-3, 0,-1,-1,-2,-2,-3,-1,-3};
    int dummyY[11] = {-1,-2,-3,-1, 0,-2,-1,-3,-2,-3,-1};

    for (int i = 0; i<11; i++) {
      XOFFSET.push_back(dummyX[i]);
      YOFFSET.push_back(dummyY[i]);
    }
  }
  else if (temp_size == 5) { // 5x5 template
    _template_size = 21;

    int dummyX[21] = {-1, 0,-1,-1,-2,-2,-3,-1,-3, -1,-3,-4,-4, -1,-2,-3,-4,-5,-5,-5,-5};
    int dummyY[21] = {-1,-1, 0,-2,-1,-3,-2,-3,-1, -4,-4,-3,-1, -5,-5,-5,-5,-1,-2,-3,-4};

    for (int i = 0; i<21; i++) {
      XOFFSET.push_back(dummyX[i]);
      YOFFSET.push_back(dummyY[i]);
    }
  }
  else if (temp_size == 11) { // 11x11 template
    _template_size = 93;

    int dummyX[93] = {-1, 0,-1,-1,-2,-2,-3,-1,-3, -1,-3,-4,-4, -1,-2,-3,-4,-5,-5,-5,-5, //5x5
    -1,-5,-6,-6, -1,-2,-3,-4,-5,-6,-7,-7,-7,-7,-7,-7, -1,-3,-5,-7,-8,-8,-8,-8, -1,-2,-4,-5,-7,-8,-9,-9,-9,-9,-9,-9, //9x9
     -1, -3, -7, -9,-10,-10,-10,-10, -1, -2, -3, -4, -5, -6, -7, -8, -9,-10,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11, //11x11
     -1, -5, -7, -11,-12,-12,-12,-12};
    int dummyY[93] = {-1,-1, 0,-2,-1,-3,-2,-3,-1, -4,-4,-3,-1, -5,-5,-5,-5,-1,-2,-3,-4, //5x5
    -6,-6,-5,-1, -7,-7,-7,-7,-7,-7,-1,-2,-3,-4,-5,-6, -8,-8,-8,-8,-1,-3,-5,-7, -9,-9,-9,-9,-9,-9,-1,-2,-4,-5,-7,-8, //9x9
    -10,-10,-10,-10, -1, -3, -7, -9,-11,-11,-11,-11,-11,-11,-11,-11,-11,-11, -1, -2, -3, -4, -5, -6, -7, -8, -9,-10, //11x11
    -12,-12,-12,-12, -1, -5, -7, -11};

    for (int i = 0; i<93; i++) {
      XOFFSET.push_back(dummyX[i]);
      YOFFSET.push_back(dummyY[i]);
    }
  }
}

// ###########################################################
//          MATCH
// ###########################################################

void bcvr_cvmatch::Match ()
{
  initializeDPCosts();
  computeDPCosts ();
  //ListDPTable();
  findDPCorrespondence ();
}

// ###########################################################
//          DP Cost
// ###########################################################

void bcvr_cvmatch::initializeDPCosts()
{
  //Ming: init
  _DPCost.clear();
  _DPMap.clear();
  int _n = _curve1->size();
  int _m = _curve2->size();
  assert (_n>0);
  assert (_m>0);

  for (int i=0;i<_n;i++) {
    vcl_vector<double> tmp1(_m,DP_VERY_LARGE_COST);
    _DPCost.push_back(tmp1);
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector< vcl_pair <int,int> > tmp2(_m,tmp3);
    _DPMap.push_back(tmp2);
  }

  //Cost Matrix Initialization
  _finalCost = DP_VERY_LARGE_COST;
  for (int n=0;n<_n;n++) {
    for (int m=0;m<_m;m++) {
      _DPCost[n][m]=DP_VERY_LARGE_COST;
    }
  }
  _DPCost[0][0]=0.0;
}

// Cost of matching the interval [x(i-1),x(i)]  to [y(k),y(j)].
double bcvr_cvmatch::computeIntervalCost(int i, int ip, int j, int jp)
{
  double ds1 = vcl_fabs(stretchCost (_curve1, i,ip));
  double ds2 = vcl_fabs(stretchCost (_curve2, j,jp));
  double dF;

  if (_normalized_stretch_cost) {
    if (ds1+ds2 > 1E-5)
      dF = vcl_pow(ds1-ds2,2)/(ds1+ds2);
    else dF = 0;
  }
  else
    dF = vcl_fabs(ds1-ds2);

  double dt1 = bendCost (_curve1, i,ip);
  double dt2 = bendCost (_curve2, j,jp);
  double dK = vcl_fabs(dt1-dt2);
#if 0
  double dK = vcl_fabs(curve_angleDiff(dt1, dt2));

  // 1)The bad orientation cost from fix starting tangent!
  double do1 = bendCost (_curve1, i,0);
  double do2 = bendCost (_curve2, j,0);
  double dO = vcl_fabs(do1-do2);

  // 2)The good orientation cost from absolute position
  double dx = _curve1.x(i) - _curve1.x(0);
  double dy = _curve1.y(i) - _curve1.y(0);
  double do1 = vcl_atan2(dy, dx);
  dx = _curve2.x(j) - _curve2.x(0);
  dy = _curve2.y(j) - _curve2.y(0);
  double do2 = vcl_atan2(dy, dx);
  double dO = vcl_fabs(do1-do2);
#endif
  double cost = dF + _R*dK; // + _R*0.2*dO;

  return cost;
}

  //#define TEMPLATE_SIZE 9 //3x3
void bcvr_cvmatch::computeDPCosts ()
{
  int sum,start,i,ip,j,jp,k;
  double cost;

  int _n = _curve1->size();
  int _m = _curve2->size();

  for (sum = 1; sum<_n+_m-1; sum++) {
    start=(int)curve_maxof(0,sum-_m+1,-10000);
    for (i=start;(i<=_n-1 && i<=sum);i++) {
      j=sum-i;
      for (k=0;k<_template_size;k++) { //TEMPLATE_SIZE=9 originally
        ip=i+XOFFSET[k];
        jp=j+YOFFSET[k];
        if (ip >= 0 &&  jp >=0) {
            double incCost=computeIntervalCost(i,ip,j,jp);
            cost =_DPCost[ip][jp]+incCost;
            if (cost < _DPCost[i][j]){
              _DPCost[i][j]=cost;
              _DPMap[i][j].first=ip;
              _DPMap[i][j].second=jp;
            }
        }
      }
    }
  }
  //Kai
  ///vcl_cout<<"computeDPCosts() Number of computation: "<<count<<'\n';
}

// ###########################################################
//          AFTER DP, FIND MATCHING
// ###########################################################

void bcvr_cvmatch::findDPCorrespondence (void)
{
  int i, j, ip, jp;

  _finalMap.clear();          //Clean the table
  _finalMapCost.clear();

  int _n = _curve1->size();
  int _m = _curve2->size();

  _finalCost = _DPCost[_n-1][_m-1];  //The final value of DPMap

  ip = _n-1;
  jp = _m-1;
  i = _n-1;
  j = _m-1;

  vcl_pair <int,int> p(ip,jp);
  _finalMap.push_back(p);
  _finalMapCost.push_back(_DPCost[p.first][p.second]);

  while (ip > 0 || jp > 0) { //Ming: should be &&
    ip=_DPMap[i][j].first;
    jp=_DPMap[i][j].second;
    vcl_pair <int,int> p(ip,jp);
    _finalMap.push_back(p);
    _finalMapCost.push_back(_DPCost[p.first][p.second]);

    i=ip; //Go to the previous point
    j=jp;
  }
}
//################################################################
double getTFromIndex (bsol_intrinsic_curve_2d_sptr curve1, int previ1, int i1, int curi1)
{
  assert (previ1<=curi1 && curi1<=i1);
  int i;

  double total_s = 0, s = 0;
  for (i=previ1+1; i<curi1+1; i++)
    s += curve1->s(i);
  for (i=previ1+1; i<i1+1; i++)
    total_s += curve1->s(i);

  if (total_s==0)
    return 0;

  double t = s/total_s;
  assert (0<=t && t<=1);
  return t;
}

void getMappingPointFromT (bsol_intrinsic_curve_2d_sptr curve2, int previ2, int i2, double t, double& pointx, double& pointy)
{
  if (t==0) {
    pointx = curve2->x(previ2);
    pointy = curve2->y(previ2);
    return;
  }
  else if (t==1) {
    pointx = curve2->x(i2);
    pointy = curve2->y(i2);
    return;
  }

  assert (0<t && t<1);

  if (previ2==i2) {
    pointx = curve2->x(previ2);
    pointy = curve2->y(previ2);
    return;
  }

  double total_s = 0;
  for (int i=previ2+1; i<i2+1; i++)
    total_s += curve2->s(i);

  int curi2 = previ2+1;
  double cur_s = curve2->s(curi2);
  double cur_t = cur_s/total_s;
  while (curi2<i2+1) {
    assert (curi2<i2+1);
    assert (cur_t<=1);
    //test if pt contains in (previ2 to curi2)
    if (cur_t>=t) { //yes
      double diff_t = cur_t-t;
      pointx = curve2->x(curi2-1)*(1-diff_t) + curve2->x(curi2)*diff_t;
      pointy = curve2->y(curi2-1)*(1-diff_t) + curve2->y(curi2)*diff_t;
      return;
    }
    //no, increase curi2
    curi2++;
    cur_s += curve2->s(curi2);
    cur_t = cur_s/total_s;
  }
}

void bcvr_cvmatch::GetAverageCurveFromAMatch (bsol_intrinsic_curve_2d_sptr AverageCurve)
{
  AverageCurve->clear();
  double x, y;

  int size = finalMap()->size();
  int previ1 = getFMapFirst(size-1);
  int previ2 = getFMapSecond(size-1);

  //Add the first point.
  x = (_curve1->x(getFMapFirst(size-1)) + _curve2->x(getFMapSecond(size-1)))/2;
  y = (_curve1->y(getFMapFirst(size-1)) + _curve2->y(getFMapSecond(size-1)))/2;
  AverageCurve->add_vertex (x, y);

  for (int i=size-2; i>=0; i--) {
    int i1 = getFMapFirst(i);
    int i2 = getFMapSecond(i);

    for (int cur_i=previ1+1; cur_i<=i1; cur_i++) {
      double t = getTFromIndex (_curve1, previ1, i1, cur_i);
      double mpointx, mpointy;
      getMappingPointFromT (_curve2, previ2, i2, t, mpointx, mpointy);

      double cpointx = _curve1->x(cur_i);
      double cpointy = _curve1->y(cur_i);

      AverageCurve->add_vertex ((mpointx+cpointx)/2, (mpointy+cpointy)/2);
    }
    previ1 = i1;
    previ2 = i2;
  }

  int na = AverageCurve->size();
  int no = _curve1->size();
  assert (na==no);
}

//The number of sample points of AverageCurve is the same as the one in the reference curve.
//Here the reference curve is always curve1.
void bcvr_cvmatch::GetSumCurveFromAMatch (bsol_intrinsic_curve_2d_sptr SumCurve)
{
  SumCurve->clear();
  double x, y;

  int size = finalMap()->size();
  int previ1 = getFMapFirst(size-1);
  int previ2 = getFMapSecond(size-1);

  //Add the first point.
  x = _curve2->x(getFMapSecond(size-1));
  y = _curve2->y(getFMapSecond(size-1));
  SumCurve->add_vertex (x, y);

  for (int i=size-2; i>=0; i--) {
    int i1 = getFMapFirst(i);
    int i2 = getFMapSecond(i);

    for (int cur_i=previ1+1; cur_i<=i1; cur_i++) {
      double t = getTFromIndex (_curve1, previ1, i1, cur_i);
      double mpointx, mpointy;
      getMappingPointFromT (_curve2, previ2, i2, t, mpointx, mpointy);

      //double cpointx = _curve1.x(cur_i);
      //double cpointy = _curve1.y(cur_i);

      SumCurve->add_vertex (mpointx, mpointy);
    }
    previ1 = i1;
    previ2 = i2;
  }

  int ns = SumCurve->size();
  int no = _curve1->size();
  assert (ns==no);
}

void bcvr_cvmatch::GetMorphingCurvesFromAMatch (bsol_intrinsic_curve_2d_sptr MorphingCurves, int NumMorphs)
{
  double w;

  for (int iMorph=0; iMorph<NumMorphs; iMorph++) {
    w = (double)(iMorph+1) / (NumMorphs+1);

    //MorphingCurves[iMorph]->clear();
    MorphingCurves.ptr()[iMorph].clear();
    double x, y;

    int size = finalMap()->size();
    int previ1 = getFMapFirst(size-1);
    int previ2 = getFMapSecond(size-1);

    //Add the first point.
    x = (_curve1->x(getFMapFirst(size-1)) + _curve2->x(getFMapSecond(size-1)))/2;
    y = (_curve1->y(getFMapFirst(size-1)) + _curve2->y(getFMapSecond(size-1)))/2;
    MorphingCurves.ptr()[iMorph].add_vertex (x, y);

    for (int i=size-2; i>=0; i--) {
      int i1 = getFMapFirst(i);
      int i2 = getFMapSecond(i);

      for (int cur_i=previ1+1; cur_i<=i1; cur_i++) {
        double t = getTFromIndex (_curve1, previ1, i1, cur_i);
        double mpointx, mpointy;
        getMappingPointFromT (_curve2, previ2, i2, t, mpointx, mpointy);

        double cpointx = _curve1->x(cur_i);
        double cpointy = _curve1->y(cur_i);

        MorphingCurves.ptr()[iMorph].add_vertex (mpointx*(1-w)+cpointx*w, mpointy*(1-w)+cpointy*w);
      }
      previ1 = i1;
      previ2 = i2;
    }

    int na = MorphingCurves.ptr()[iMorph].size();
    int no = _curve1->size();
    assert (na==no);
  }//end for iMorph
}

//##################################################
//       DEBUG PRINT
//##################################################
void bcvr_cvmatch::ListDPTable (void)
{
  int n = _curve1->size();
  int m = _curve2->size();

  vcl_cout<< "===================================================\n"
          << "i j _map[i][j].first _map[i][j].second _cost[i][j]\n";
  for (int i=0;i<=n-1; i++){
    for (int j=0;j<=m-1; j++){
      vcl_cout<<i<<' '<<j<<' '<<(*DPMap())[i][j].first<<' '<<(*DPMap())[i][j].second<<' '<<(*DPCost())[i][j]<<'\n';
    }
  }
}

void bcvr_cvmatch::ListAlignCurve (void)
{
  vcl_cout<<"==========================================================\n"
          <<"i, finalMap[i].first, finalMap[i].second, finalMapCost[i]\n";
  for (unsigned int i=0;i<=finalMap()->size()-1; i++){
    vcl_cout<<i<<' '<<(*finalMap())[i].first<<' '<<(*finalMap())[i].second<<' '<<(*finalMapCost())[i]<<'\n';
  }
}

void bcvr_cvmatch::SaveDPTable (void)
{
  vcl_string basefname1 = _fileName1; //getBaseFileName(_fileName1);
  vcl_string basefname2 = _fileName2; //getBaseFileName(_fileName2);
  vcl_string basefname=basefname1+'-'+basefname2;

  //Output AlignCurve File
  vcl_string acfname = basefname;
  acfname += "-ACurve.txt";
  vcl_ofstream outfp2(acfname.c_str());
  for (unsigned int i=0; i<(*finalMap()).size(); i++){
    outfp2<<i<<' '<<(*finalMap())[i].first<<' '<<(*finalMap())[i].second<<' '<<(*finalMapCost())[i]<<'\n';
  }
  outfp2.close();
}

void bcvr_cvmatch::SaveAlignCurve (void)
{
  int n = _curve1->size();
  int m = _curve2->size();

  vcl_string basefname1 = _fileName1; //getBaseFileName(_fileName1);
  vcl_string basefname2 = _fileName2; //getBaseFileName(_fileName2);
  vcl_string basefname=basefname1+'-'+basefname2;

  //Output DPTalbe File
  vcl_string dpfname = basefname;
  dpfname += "-DPMap.txt";
  vcl_ofstream outfp3(dpfname.c_str());
  for (int i=0;i<=n-1; i++){
    for (int j=0;j<=m-1; j++){
      outfp3<<i<<' '<<j<<' '<<(*DPMap())[i][j].first<<' '<<(*DPMap())[i][j].second<<' '<<(*DPCost())[i][j]<<'\n';
    }
  }
  outfp3.close();
}


//: prepare and return the instance of container class that saves curve correspondence
bcvr_cv_cor_sptr bcvr_cvmatch::get_cv_cor()
{
  bcvr_cv_cor_sptr cv_cor = new bcvr_cv_cor(_curve1, _curve2, _finalMap, _curve1->size());
  cv_cor->set_open_curve_matching(true);
  return cv_cor;
}

