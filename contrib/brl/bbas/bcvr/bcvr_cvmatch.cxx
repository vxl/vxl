#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include "bcvr_cvmatch.h"
//:
// \file
#include "bcvr_cv_cor.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <cassert>

#include <vnl/vnl_math.h>

//: return value in (-Pi,Pi]
double curve_fixAngleMPiPi (double a)
{
  return vnl_math::angle_minuspi_to_pi(a);
}

//Does a1-a2, value in (-Pi,Pi]
double curve_angleDiff (double a1, double a2)
{
  return vnl_math::angle_minuspi_to_pi(a1-a2);
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
  R_ = 10;
  normalized_stretch_cost_ = false;
  setTemplateSize(3);
}

bcvr_cvmatch::bcvr_cvmatch (const bsol_intrinsic_curve_2d_sptr& c1, const bsol_intrinsic_curve_2d_sptr& c2)
{
  curve1_ = c1;
  curve2_ = c2;
  R_ = 10;
  normalized_stretch_cost_ = false;
  setTemplateSize(3);
}

bcvr_cvmatch::~bcvr_cvmatch ()
= default;

void bcvr_cvmatch::setTemplateSize (int temp_size)
{
  XOFFSET.clear();
  YOFFSET.clear();

  if (temp_size == 3) { // 3x3 template
    template_size_ = 11;

    int dummyX[11] = {-1,-2,-3, 0,-1,-1,-2,-2,-3,-1,-3};
    int dummyY[11] = {-1,-2,-3,-1, 0,-2,-1,-3,-2,-3,-1};

    for (int i = 0; i<11; i++) {
      XOFFSET.push_back(dummyX[i]);
      YOFFSET.push_back(dummyY[i]);
    }
  }
  else if (temp_size == 5) { // 5x5 template
    template_size_ = 21;

    int dummyX[21] = {-1, 0,-1,-1,-2,-2,-3,-1,-3, -1,-3,-4,-4, -1,-2,-3,-4,-5,-5,-5,-5};
    int dummyY[21] = {-1,-1, 0,-2,-1,-3,-2,-3,-1, -4,-4,-3,-1, -5,-5,-5,-5,-1,-2,-3,-4};

    for (int i = 0; i<21; i++) {
      XOFFSET.push_back(dummyX[i]);
      YOFFSET.push_back(dummyY[i]);
    }
  }
  else if (temp_size == 11) { // 11x11 template
    template_size_ = 93;

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
  DPCost_.clear();
  DPMap_.clear();
  int n_ = curve1_->size();
  int m_ = curve2_->size();
  assert (n_>0);
  assert (m_>0);

  for (int i=0;i<n_;i++) {
    std::vector<double> tmp1(m_,DP_VERY_LARGE_COST);
    DPCost_.push_back(tmp1);
    std::pair <int,int> tmp3(0,0);
    std::vector< std::pair <int,int> > tmp2(m_,tmp3);
    DPMap_.push_back(tmp2);
  }

  //Cost Matrix Initialization
  finalCost_ = DP_VERY_LARGE_COST;
  for (int n=0;n<n_;n++) {
    for (int m=0;m<m_;m++) {
      DPCost_[n][m]=DP_VERY_LARGE_COST;
    }
  }
  DPCost_[0][0]=0.0;
}

// Cost of matching the interval [x(i-1),x(i)]  to [y(k),y(j)].
double bcvr_cvmatch::computeIntervalCost(int i, int ip, int j, int jp)
{
  double ds1 = std::fabs(stretchCost (curve1_, i,ip));
  double ds2 = std::fabs(stretchCost (curve2_, j,jp));
  double dF;

  if (normalized_stretch_cost_) {
    if (ds1+ds2 > 1E-5)
      dF = std::pow(ds1-ds2,2)/(ds1+ds2);
    else dF = 0;
  }
  else
    dF = std::fabs(ds1-ds2);

  double dt1 = bendCost (curve1_, i,ip);
  double dt2 = bendCost (curve2_, j,jp);
  double dK = std::fabs(dt1-dt2);
#if 0
  double dK = std::fabs(curve_angleDiff(dt1, dt2));

  // 1)The bad orientation cost from fix starting tangent!
  double do1 = bendCost (curve1_, i,0);
  double do2 = bendCost (curve2_, j,0);
  double dO = std::fabs(do1-do2);

  // 2)The good orientation cost from absolute position
  double dx = curve1_.x(i) - curve1_.x(0);
  double dy = curve1_.y(i) - curve1_.y(0);
  double do1 = std::atan2(dy, dx);
  dx = curve2_.x(j) - curve2_.x(0);
  dy = curve2_.y(j) - curve2_.y(0);
  double do2 = std::atan2(dy, dx);
  double dO = std::fabs(do1-do2);
#endif
  double cost = dF + R_*dK; // + R_*0.2*dO;

  return cost;
}

  //#define TEMPLATE_SIZE 9 //3x3
void bcvr_cvmatch::computeDPCosts ()
{
  int sum,start,i,ip,j,jp,k;
  double cost;

  int n_ = curve1_->size();
  int m_ = curve2_->size();

  for (sum = 1; sum<n_+m_-1; sum++) {
    start=(int)curve_maxof(0,sum-m_+1,-10000);
    for (i=start;(i<=n_-1 && i<=sum);i++) {
      j=sum-i;
      for (k=0;k<template_size_;k++) { //TEMPLATE_SIZE=9 originally
        ip=i+XOFFSET[k];
        jp=j+YOFFSET[k];
        if (ip >= 0 &&  jp >=0) {
            double incCost=computeIntervalCost(i,ip,j,jp);
            cost =DPCost_[ip][jp]+incCost;
            if (cost < DPCost_[i][j]) {
              DPCost_[i][j]=cost;
              DPMap_[i][j].first=ip;
              DPMap_[i][j].second=jp;
            }
        }
      }
    }
  }
  //Kai
  ///std::cout<<"computeDPCosts() Number of computation: "<<count<<'\n';
}

// ###########################################################
//          AFTER DP, FIND MATCHING
// ###########################################################

void bcvr_cvmatch::findDPCorrespondence (void)
{
  int i, j, ip, jp;

  finalMap_.clear();          //Clean the table
  finalMapCost_.clear();

  int n_ = curve1_->size();
  int m_ = curve2_->size();

  finalCost_ = DPCost_[n_-1][m_-1];  //The final value of DPMap

  ip = n_-1;
  jp = m_-1;
  i = n_-1;
  j = m_-1;

  std::pair <int,int> p(ip,jp);
  finalMap_.push_back(p);
  finalMapCost_.push_back(DPCost_[p.first][p.second]);

  while (ip > 0 || jp > 0) { //Ming: should be &&
    ip=DPMap_[i][j].first;
    jp=DPMap_[i][j].second;
    std::pair <int,int> p(ip,jp);
    finalMap_.push_back(p);
    finalMapCost_.push_back(DPCost_[p.first][p.second]);

    i=ip; //Go to the previous point
    j=jp;
  }
}
//################################################################
double getTFromIndex (const bsol_intrinsic_curve_2d_sptr& curve1, int previ1, int i1, int curi1)
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

void getMappingPointFromT (const bsol_intrinsic_curve_2d_sptr& curve2, int previ2, int i2, double t, double& pointx, double& pointy)
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

void bcvr_cvmatch::GetAverageCurveFromAMatch (const bsol_intrinsic_curve_2d_sptr& AverageCurve)
{
  AverageCurve->clear();
  double x, y;

  int size = finalMap()->size();
  int previ1 = getFMapFirst(size-1);
  int previ2 = getFMapSecond(size-1);

  //Add the first point.
  x = (curve1_->x(getFMapFirst(size-1)) + curve2_->x(getFMapSecond(size-1)))/2;
  y = (curve1_->y(getFMapFirst(size-1)) + curve2_->y(getFMapSecond(size-1)))/2;
  AverageCurve->add_vertex (x, y);

  for (int i=size-2; i>=0; i--) {
    int i1 = getFMapFirst(i);
    int i2 = getFMapSecond(i);

    for (int cur_i=previ1+1; cur_i<=i1; cur_i++) {
      double t = getTFromIndex (curve1_, previ1, i1, cur_i);
      double mpointx, mpointy;
      getMappingPointFromT (curve2_, previ2, i2, t, mpointx, mpointy);

      double cpointx = curve1_->x(cur_i);
      double cpointy = curve1_->y(cur_i);

      AverageCurve->add_vertex ((mpointx+cpointx)/2, (mpointy+cpointy)/2);
    }
    previ1 = i1;
    previ2 = i2;
  }

  assert (AverageCurve->size() == curve1_->size());
}

//The number of sample points of AverageCurve is the same as the one in the reference curve.
//Here the reference curve is always curve1.
void bcvr_cvmatch::GetSumCurveFromAMatch (const bsol_intrinsic_curve_2d_sptr& SumCurve)
{
  SumCurve->clear();
  double x, y;

  int size = finalMap()->size();
  int previ1 = getFMapFirst(size-1);
  int previ2 = getFMapSecond(size-1);

  //Add the first point.
  x = curve2_->x(getFMapSecond(size-1));
  y = curve2_->y(getFMapSecond(size-1));
  SumCurve->add_vertex (x, y);

  for (int i=size-2; i>=0; i--) {
    int i1 = getFMapFirst(i);
    int i2 = getFMapSecond(i);

    for (int cur_i=previ1+1; cur_i<=i1; cur_i++) {
      double t = getTFromIndex (curve1_, previ1, i1, cur_i);
      double mpointx, mpointy;
      getMappingPointFromT (curve2_, previ2, i2, t, mpointx, mpointy);

      //double cpointx = curve1_.x(cur_i);
      //double cpointy = curve1_.y(cur_i);

      SumCurve->add_vertex (mpointx, mpointy);
    }
    previ1 = i1;
    previ2 = i2;
  }

  assert(SumCurve->size() == curve1_->size());
}

void bcvr_cvmatch::GetMorphingCurvesFromAMatch (const bsol_intrinsic_curve_2d_sptr& MorphingCurves, int NumMorphs)
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
    x = (curve1_->x(getFMapFirst(size-1)) + curve2_->x(getFMapSecond(size-1)))/2;
    y = (curve1_->y(getFMapFirst(size-1)) + curve2_->y(getFMapSecond(size-1)))/2;
    MorphingCurves.ptr()[iMorph].add_vertex (x, y);

    for (int i=size-2; i>=0; i--) {
      int i1 = getFMapFirst(i);
      int i2 = getFMapSecond(i);

      for (int cur_i=previ1+1; cur_i<=i1; cur_i++) {
        double t = getTFromIndex (curve1_, previ1, i1, cur_i);
        double mpointx, mpointy;
        getMappingPointFromT (curve2_, previ2, i2, t, mpointx, mpointy);

        double cpointx = curve1_->x(cur_i);
        double cpointy = curve1_->y(cur_i);

        MorphingCurves.ptr()[iMorph].add_vertex (mpointx*(1-w)+cpointx*w, mpointy*(1-w)+cpointy*w);
      }
      previ1 = i1;
      previ2 = i2;
    }

    assert(MorphingCurves.ptr()[iMorph].size() == curve1_->size());
  }//end for iMorph
}

//##################################################
//       DEBUG PRINT
//##################################################
void bcvr_cvmatch::ListDPTable (void)
{
  int n = curve1_->size();
  int m = curve2_->size();

  std::cout<< "===================================================\n"
          << "i j _map[i][j].first _map[i][j].second _cost[i][j]\n";
  for (int i=0;i<=n-1; i++) {
    for (int j=0;j<=m-1; j++) {
      std::cout<<i<<' '<<j<<' '<<(*DPMap())[i][j].first<<' '<<(*DPMap())[i][j].second<<' '<<(*DPCost())[i][j]<<'\n';
    }
  }
}

void bcvr_cvmatch::ListAlignCurve (void)
{
  std::cout<<"==========================================================\n"
          <<"i, finalMap[i].first, finalMap[i].second, finalMapCost[i]\n";
  for (unsigned int i=0;i<=finalMap()->size()-1; i++) {
    std::cout<<i<<' '<<(*finalMap())[i].first<<' '<<(*finalMap())[i].second<<' '<<(*finalMapCost())[i]<<'\n';
  }
}

void bcvr_cvmatch::SaveDPTable (void)
{
  std::string basefname1 = fileName1_; //getBaseFileName(fileName1_);
  std::string basefname2 = fileName2_; //getBaseFileName(fileName2_);
  std::string basefname=basefname1+'-'+basefname2;

  //Output AlignCurve File
  std::string acfname = basefname;
  acfname += "-ACurve.txt";
  std::ofstream outfp2(acfname.c_str());
  for (unsigned int i=0; i<(*finalMap()).size(); i++) {
    outfp2<<i<<' '<<(*finalMap())[i].first<<' '<<(*finalMap())[i].second<<' '<<(*finalMapCost())[i]<<'\n';
  }
  outfp2.close();
}

void bcvr_cvmatch::SaveAlignCurve (void)
{
  int n = curve1_->size();
  int m = curve2_->size();

  std::string basefname1 = fileName1_; //getBaseFileName(fileName1_);
  std::string basefname2 = fileName2_; //getBaseFileName(fileName2_);
  std::string basefname=basefname1+'-'+basefname2;

  //Output DPTalbe File
  std::string dpfname = basefname;
  dpfname += "-DPMap.txt";
  std::ofstream outfp3(dpfname.c_str());
  for (int i=0;i<=n-1; i++) {
    for (int j=0;j<=m-1; j++) {
      outfp3<<i<<' '<<j<<' '<<(*DPMap())[i][j].first<<' '<<(*DPMap())[i][j].second<<' '<<(*DPCost())[i][j]<<'\n';
    }
  }
  outfp3.close();
}


//: prepare and return the instance of container class that saves curve correspondence
bcvr_cv_cor_sptr bcvr_cvmatch::get_cv_cor()
{
  bcvr_cv_cor_sptr cv_cor = new bcvr_cv_cor(curve1_, curve2_, finalMap_, curve1_->size());
  cv_cor->set_open_curve_matching(true);
  return cv_cor;
}
