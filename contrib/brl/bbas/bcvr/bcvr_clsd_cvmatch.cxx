#include <iostream>
#include <cmath>
#include <cstdio>
#include "bcvr_clsd_cvmatch.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/algo/vgl_fit_lines_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include "bcvr_cv_cor.h"

bcvr_clsd_cvmatch::bcvr_clsd_cvmatch()
{
  n1_ = 0;
  n2_ = 0;
  setTemplateSize(3);
}

bcvr_clsd_cvmatch::bcvr_clsd_cvmatch(const bsol_intrinsic_curve_2d_sptr& c1,
                                     const bsol_intrinsic_curve_2d_sptr& c2,
                                     double R,
                                     int template_size)
{
  int n;

  _curve1 = new bsol_intrinsic_curve_2d(*c1);
  _curve2 = new bsol_intrinsic_curve_2d(*c2);
  _curve1->setOpen(c1->isOpen());
  _curve2->setOpen(c2->isOpen());
  n1_=c1->size();     //No. of points in original curve
  n2_=c2->size();

  // duplicate curve 1 to capture cyclic nature of closed curves
  for (n=0;n<n1_;n++)
    _curve1->add_vertex(c1->x(n), c1->y(n));

  // caller function should not compute properties of curve 1 to avoid duplicate computation
  _curve2->computeProperties();
  _curve1->computeProperties();

  for (n=0;n<2*n1_;n++) {
    std::vector<double> tmp1(n2_,DP_VERY_LARGE_COST);
    _cost.push_back(tmp1);

    std::pair <int,int> tmp3(0,0);
    std::vector< std::pair <int,int> > tmp2(n2_,tmp3);
    _map.push_back(tmp2);

    std::pair <int,int> tmp4(0,0);
    std::vector< std::pair <int,int> > tmp5(n1_+n2_,tmp4);
    _finalMap.push_back(tmp5);
  }

  _leftMask.insert(_leftMask.begin(),n2_,0);
  //_rightMask.insert(_rightMask.begin(),n2_,2*n1_-1);
  _rightMask.insert(_rightMask.begin(),n2_,2*n2_-1);
  _finalCost.insert(_finalCost.begin(),2*n1_,0);

  R_ = R;
  setTemplateSize(template_size);
}

bcvr_clsd_cvmatch::bcvr_clsd_cvmatch(const vsol_polygon_2d_sptr& p1,
                                     const vsol_polygon_2d_sptr& p2,
                                     double R,
                                     double rms,  // fit lines to the input polygon before using
                                     int template_size)
{
  bsol_intrinsic_curve_2d_sptr c1 = new bsol_intrinsic_curve_2d();
  _curve2 = new bsol_intrinsic_curve_2d();
  if (rms > 0) {  // vgl_fit
    int min_fit_length = 2;
    vgl_fit_lines_2d<double> fitter;
    fitter.set_min_fit_length(min_fit_length);
    fitter.set_rms_error_tol(rms);
    std::cout << "original polygon1 size: " << p1->size() << ' ';
    for (unsigned int i = 0; i<p1->size(); i++) {
      vgl_point_2d<double> p = p1->vertex(i)->get_p();
      fitter.add_point(p);
    }
    fitter.fit();
    std::vector<vgl_line_segment_2d<double> >& segs = fitter.get_line_segs();

    c1->add_vertex(segs[0].point1().x(),segs[0].point1().y());
    c1->add_vertex(segs[0].point2().x(),segs[0].point2().y());
    for (unsigned int i = 1; i<segs.size(); i++) {
      c1->add_vertex(segs[i].point2().x(),segs[i].point2().y());
    }
    std::cout << "fitted curve1 size: " << c1->size() << std::endl;

    // do the second polygon
    fitter.clear();
    std::cout << "original polygon2 size: " << p2->size() << ' ';
    for (unsigned int i = 0; i<p2->size(); i++) {
      vgl_point_2d<double> p = p2->vertex(i)->get_p();
      fitter.add_point(p);
    }
    fitter.fit();
    segs = fitter.get_line_segs();

    _curve2->add_vertex(segs[0].point1().x(),segs[0].point1().y());
    _curve2->add_vertex(segs[0].point2().x(),segs[0].point2().y());
    for (unsigned int i = 1; i<segs.size(); i++) {
      _curve2->add_vertex(segs[i].point2().x(),segs[i].point2().y());
    }
    std::cout << "fitted curve2 size: " << _curve2->size() << std::endl;
  }
  else {  // set the original points
    for (unsigned int i = 0; i<p1->size(); i++) {
      c1->add_vertex(p1->vertex(i));
    }

    for (unsigned int i = 0; i<p2->size(); i++) {
      _curve2->add_vertex(p2->vertex(i));
    }
  }

   int n;
  n1_=c1->size();     //No. of points in original curve
  n2_=_curve2->size();

  // duplicate curve 1 to capture cyclic nature of closed curves
  _curve1 = new bsol_intrinsic_curve_2d(*c1);
  _curve1->setOpen(false);
  _curve2->setOpen(false);
  for (n=0;n<n1_;n++)
    _curve1->add_vertex(c1->x(n), c1->y(n));

  // caller function should not compute properties of curve 1 to avoid duplicate computation
  _curve2->computeProperties();
  _curve1->computeProperties();

  for (n=0;n<2*n1_;n++) {
    std::vector<double> tmp1(n2_,DP_VERY_LARGE_COST);
    _cost.push_back(tmp1);

    std::pair <int,int> tmp3(0,0);
    std::vector< std::pair <int,int> > tmp2(n2_,tmp3);
    _map.push_back(tmp2);

    std::pair <int,int> tmp4(0,0);
    std::vector< std::pair <int,int> > tmp5(n1_+n2_,tmp4);
    _finalMap.push_back(tmp5);
  }

  _leftMask.insert(_leftMask.begin(),n2_,0);
  //_rightMask.insert(_rightMask.begin(),n2_,2*n1_-1);
  _rightMask.insert(_rightMask.begin(),n2_,2*n2_-1);
  _finalCost.insert(_finalCost.begin(),2*n1_,0);

  R_ = R;
  setTemplateSize(template_size);
}

void bcvr_clsd_cvmatch::setTemplateSize (int temp_size)
{
  XOFFSET.clear();
  YOFFSET.clear();

  if (temp_size == 1) {  // 1x1 template
    _template_size = 3;

    int dummyX[3] = {-1, 0,-1};
    int dummyY[3] = {-1,-1, 0};

    for (int i = 0; i<3; i++) {
      XOFFSET.push_back(dummyX[i]);
      YOFFSET.push_back(dummyY[i]);
    }
  }
  else if (temp_size == 3) { // 3x3 template
    _template_size = 11;

    int dummyX[11] = {-3,-2,-1, 0,-1,-1,-2,-2,-3,-1,-3};
    int dummyY[11] = {-3,-2,-1,-1, 0,-2,-1,-3,-2,-3,-1};

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

void bcvr_clsd_cvmatch::printCost()
{
  int i,j;
  std::cout << "Cost Matrix" << std::endl;
  for (i = 0; i<n1_; i++) {
    for (j = 0; j<n2_; j++) {
      std::printf("%6.3f ",_cost[i][j]);
    }
    std::printf("\n");
  }
}

void bcvr_clsd_cvmatch::writeCost(const std::string& fname)
{
  std::FILE *fp=std::fopen(fname.c_str(),"w");
  int i,j;
  double c;
  for (i = 0; i<n1_; i++) {
    for (j = 0; j<n2_; j++) {
      c=_cost[i][j];
      std::fwrite(&c,sizeof(double),1,fp);
    }
  }
  std::fclose(fp);
}

void bcvr_clsd_cvmatch::printMap()
{
  int i,j;
  std::printf("Map Matrix\n");
  for (i = 0; i<n1_; i++) {
    for (j = 0; j<n2_; j++) {
      std::printf("(%2d,%2d) ", _map[i][j].first, _map[i][j].second);
    }
    std::printf("\n");
  }
}

void bcvr_clsd_cvmatch::initializeDPMask1()
{
  int ii,jj;

  for (ii=0;ii<2*n1_;ii++) {
    for (jj=0;jj<n2_;jj++) {
      _cost[ii][jj]=DP_VERY_LARGE_COST;
    }
  }
  for (jj=0;jj<n2_;jj++) {
    _leftMask[jj]=0;
    _rightMask[jj]=2*n2_-1;
  }
}

void bcvr_clsd_cvmatch::initializeDPMask2(int s1, int s2)
{
  int ci=0,cj=1;
  int Ni=_finalMap[s1].size(); //Size of optimal paths i and j
  int Nj=_finalMap[s2].size();
  int ii;
  int jj;
  int x1,x2,y1,y2;
  int count;

  //intPair *Pi, *Pj;
  //intPair *PiR, *PjR;

  int NDi;//Size of discrete paths i and j
  int NDj;
  int start,end;
  int n1,n2;

  n1=n1_;
  n2=n2_;

  std::pair <int,int> tmp3(0,0);
  std::vector< std::pair <int,int> > Pi(n1+n2,tmp3);
  std::vector< std::pair <int,int> > Pj(n1+n2,tmp3);
  std::vector< std::pair <int,int> > PiR(n1+n2,tmp3);
  std::vector< std::pair <int,int> > PjR(n1+n2,tmp3);

  count=0;
  for (ii=0;ii<Ni-1;ii++) {
    x1=_finalMap[s1][ii].first;x2=_finalMap[s1][ii+1].first;
    y1=_finalMap[s1][ii].second;y2=_finalMap[s1][ii+1].second;
    PiR[count].first=_finalMap[s1][ii].first;
    PiR[count].second=_finalMap[s1][ii].second;
    count++;
    for (jj=y1+1;jj<y2;jj++) {
      PiR[count].first=x1;
      PiR[count].first=jj;   // ozge: ??
      count++;
    }
    for (jj=x1+1;jj<x2;jj++) {
      PiR[count].first=jj;
      PiR[count].first=y2;
      count++;
    }
  }

  PiR[count].first=_finalMap[s1][Ni-1].first;
  PiR[count].second=_finalMap[s1][Ni-1].second;
  count++;
  NDi=count;
  for (count=NDi-1;count>=0;count--)
    Pi[NDi-count]=PiR[count];


  //Right path
  count=0;
  for (ii=0;ii<Nj-1;ii++) {
    x1=_finalMap[s2][ii].first;x2=_finalMap[s2][ii+1].first;
    y1=_finalMap[s2][ii].second;y2=_finalMap[s2][ii+1].second;
    PjR[count].first=_finalMap[s2][ii].first;
    PjR[count].second=_finalMap[s2][ii].second;
    count++;
    for (jj=x1+1;jj<x2;jj++) {
      PjR[count].first=jj;
      PjR[count].second=y1;
      count++;
    }
    for (jj=y1+1;jj<y2;jj++) {
      PjR[count].first=x2;
      PjR[count].second=jj;
      count++;
    }
  }
  PjR[count].first=_finalMap[s2][Nj-1].first;
  PjR[count].second=_finalMap[s2][Nj-1].second;
  count++;
  NDj=count;
  for (count=NDj-1;count>=0;count--)
    Pj[NDj-count]=PjR[count];
#if 0
  if (s1==3 && s2==6) {
    for (count=0;count<NDi;count++)
      std::printf("%3d %3d\n",Pi[count].first,Pi[count].second);
    for (count=0;count<NDj;count++)
      std::printf("%3d %3d\n",Pj[count].first,Pj[count].second);
  }
#endif

  for (jj=0;jj<n2_;jj++) {
    while (ci <NDi && PiR[ci].second<jj) {
      ci++;
    }
    start = PiR[ci].first;
    while (cj < NDj && PjR[cj].second < jj+1) {
      cj++;
    }
    end = PjR[cj-1].first;
    _leftMask[jj]=start;
    _rightMask[jj]=end;
    //std::cout << jj <<" Start= " << start << " End= " << end << ' ' << cj-1 << std::endl;
    for (ii=0;ii<=2*n1_-1;ii++)
      _cost[ii][jj]=DP_VERY_LARGE_COST;
  }

  Pi.clear();
  Pj.clear();
  PiR.clear();
  PjR.clear();
}

void bcvr_clsd_cvmatch::computeDPCosts(int startPoint)
{
  int start,end,i,ip,j,jp,k;
  double cost;

  _cost[startPoint][0]=0;
  //for (sum = 1; sum<_n+_m-1; sum++) {
  for (j=0;j<n2_;j++) {
    start=(startPoint>_leftMask[j] ? startPoint : _leftMask[j]);
    end=(_rightMask[j]<startPoint+n1_-1 ? _rightMask[j] : startPoint+n1_-1);
    for (i=start;i<=end;i++) {
      for (k=0;k<_template_size;k++) {
              ip=i+XOFFSET[k];
              jp=j+YOFFSET[k];
              if (jp >=0  && ip >= _leftMask[jp] &&  ip <= _rightMask[jp]) {
                cost =_cost[ip][jp]+computeIntervalCost(i,ip,j,jp);
                if (_cost[i][j] > cost) {
                  _cost[i][j]=cost;
                  _map[i][j].first=ip;
                  _map[i][j].second=jp;
                }
              }
      }
    }
  }
}

void bcvr_clsd_cvmatch::findDPCorrespondence(int startPoint)
{
  int i,j,ip,jp;
  int count;

  std::pair <int,int> tmp3(0,0);
  std::vector< std::pair <int,int> > tmpMap(n1_+n2_+1,tmp3);
  //tmpMap=(intPair*)calloc(d->n1+d->n2+1,sizeof(intPair));

  _finalCost[startPoint]=_cost[n1_+startPoint-1][n2_-1];

  ip=n1_+startPoint-1;
  jp=n2_-1;
  i=n1_+startPoint-1;
  j=n2_-1;

  count=0;
  tmpMap[count].first=ip;
  tmpMap[count].second=jp;
  count++;

  while (ip > startPoint || jp > 0) {
    ip=_map[i][j].first;
    jp=_map[i][j].second;
    tmpMap[count].first=ip;
    tmpMap[count].second=jp;
    count++;
    i=ip;
    j=jp;
  }
  _finalMap[startPoint].clear();
  std::pair <int,int> p;
  //_finalMapSize[startPoint]=count;
  for (i=0;i<count;i++) {
    p.first = tmpMap[count-1-i].first;
    p.second = tmpMap[count-1-i].second;
    _finalMap[startPoint].push_back(p);
    //_finalMap[startPoint][i].first=tmpMap[count-1-i].first;
    //_finalMap[startPoint][i].second=tmpMap[count-1-i].second;
  }

  tmpMap.clear();
}

void bcvr_clsd_cvmatch::findOptimalPath(int startPoint)
{
  //std::cout << "Computing optimal path for " << startPoint << std::endl;
  computeDPCosts(startPoint);
  findDPCorrespondence(startPoint);
}

void bcvr_clsd_cvmatch::computeMiddlePaths(int i, int j)
{
  int k=(i+j)/2;
  if (i<k) {
    initializeDPMask2(i,j);
    findOptimalPath(k);
    computeMiddlePaths(i,k);
    computeMiddlePaths(k,j);
  }
}

void bcvr_clsd_cvmatch::Match()
{
  //std::cout << "in DP Match" << std::endl;

  //initialize the mask for the match with starting point 0
  initializeDPMask1();
  //Find optimal Path with starting point 0
  findOptimalPath(0);

  //Copy the starting point match (0) to the match from _n
  int N0=_finalMap[0].size();
  std::pair <int,int> p;
  _finalMap[n1_].clear();
  _finalCost[n1_]=_finalCost[0];
  for (int i=0;i<N0;i++) {
    p.first=_finalMap[0][i].first+n1_;
    p.second=_finalMap[0][i].second;
    _finalMap[n1_].push_back(p);
  }
  std::cout <<  "In Match: Done copying  start point"<< std::endl;
  computeMiddlePaths(0,n1_);

#if 0
  std::ofstream fpoo;
  fpoo.open("D:\\contours\\Mpeg-7\\temp_original.out", std::ios::app);

  fpoo << 2*n1_ << ' ' << n2_ << '\n';
  for (int i = 0; i<2*n1_; i++) {
    for (int j = 0; j<n2_; j++) {   // -1 since (0,0) will go to CD
      fpoo << _curve1->arcLength(i) << ' '
           << _curve2->arcLength(j) << std::endl;
    }
  }

  double minCost = _finalCost[0];
  int i_min = 0;
  for (int i=1; i<n1_ ; i++) {
    if (minCost > _finalCost[i]) {
      minCost = _finalCost[i];
      i_min = i;
    }
  }

  N0=_finalMap[i_min].size();
  fpoo << N0 << '\n';
  for (int i=0; i<N0 ; i++) {
    int ii =_finalMap[i_min][i].first;
    int jj =_finalMap[i_min][i].second;
    fpoo << _curve1->arcLength(ii) << ' '
         << _curve2->arcLength(jj) << std::endl;
  }

  fpoo.close();
#endif
}

double bcvr_clsd_cvmatch::stretchCost (const bsol_intrinsic_curve_2d_sptr& curve, int i, int ip)
{
  return curve->arcLength(i) - curve->arcLength(ip);
}

double bcvr_clsd_cvmatch::bendCost (const bsol_intrinsic_curve_2d_sptr& curve, int i, int ip)
{
  return curve_angleDiff (curve->angle(i), curve->angle(ip));
}

// Cost of matching the interval [x(i-1),x(i)]  to [y(k),y(j)].
double bcvr_clsd_cvmatch::computeIntervalCost(int i, int ip, int j, int jp)
{
  //Here the cost is based on lengths of the segments.
  double ds1 = std::fabs(stretchCost(_curve1, i,ip));
  double ds2 = std::fabs(stretchCost(_curve2, j,jp));

  double dF;
  if (_normalized_stretch_cost) {
    if (ds1+ds2 > 1E-5)
      dF = std::pow(ds1-ds2,2)/(ds1+ds2);
    else dF = 0;
  }
  else dF = std::fabs(ds1-ds2);


  double dt1 = bendCost(_curve1, i,ip);
  double dt2 = bendCost(_curve2, j,jp);
  double dK = std::fabs(dt1-dt2);

  return dF + R_*dK;
#if 0
  s11=d->curve1->arcLength[ip];
  s12=d->curve1->arcLength[i];
  s21=d->curve2->arcLength[jp];
  s22=d->curve2->arcLength[j];
  t11=d->curve1->angle[ip];
  t12=d->curve1->angle[i];
  t21=d->curve2->angle[jp];
  t22=d->curve2->angle[j];
  cost = std::fabs(std::fabs(s12-s11)-std::fabs(s22-s21));
  cost += d->R*std::fabs(angleDiff(t12,t11)-angleDiff(t22,t21));
  return cost;
#endif // 0

#if 0 //The other kinds of costs
  l1=std::fabs(s12-s11);
  l2=std::fabs(s22-s21);
  t1=angleDiff(t12,t11);
  t2=angleDiff(t22,t21);

  cost=0.0;
  if (l1+l2 > 1e-5)
    cost = std::pow(l1-l2,2.0)*std::exp(-(l1+l2));
  cost = std::pow(l1-l2,2.0)/(l1+l2);
  if (std::fabs(t1+t2) > 1e-5)
    cost = d->R*std::pow(t1-t2,2.0)*std::exp(-std::fabs(t1+t2));
  cost += d->R*std::pow(t1-t2,2.0)/(std::fabs(t1+t2));
  return cost;
#endif // 0
}


//: return the best cost around the starting point, but within a restricted arclength range.
//  e.g. if ratio is 1/4 then go L/4 to the right and -L/4 to the left
//  of the starting point and return the best cost in that range
double bcvr_clsd_cvmatch::finalBestCostRestrictedStartingPoint(int &index, double ratio, bool get_normalized_cost)
{
  double L = _curve1->length()/2.0f;
  double allowedLmin = L*ratio;
  double allowedLmax = L-allowedLmin;

  double minCost = _finalCost[0];
  index = 0;

  for (int count=1;count<n1_;count++) {
    double al = _curve1->arcLength(count);
    if (al < allowedLmin || al > allowedLmax)
      if (minCost>_finalCost[count]) {
        minCost=_finalCost[count];
        index=count;
      }
  }

  if (get_normalized_cost)
    return minCost/((_curve1->length()/2.0f)+_curve2->length());
  else
    return minCost;
}

//: return the best cost around the starting point
double bcvr_clsd_cvmatch::finalBestCost(int &index, bool get_normalized_cost)
{
  double minCost = _finalCost[0];
  index = 0;

  for (int count=1;count<n1_;count++) {
    if (minCost>_finalCost[count]) {
      minCost=_finalCost[count];
      index=count;
    }
  }

  if (get_normalized_cost)
    return minCost/((_curve1->length()/2.0f)+_curve2->length());
  else
    return minCost;
}

//: prepare and return the instance of container class that saves curve correspondence
bcvr_cv_cor_sptr bcvr_clsd_cvmatch::get_cv_cor(int minIndex)
{
  return new bcvr_cv_cor(_curve1, _curve2, _finalMap[minIndex], n1_);
}
