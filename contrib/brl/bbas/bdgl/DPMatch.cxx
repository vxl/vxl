#include "DPMatch.h"
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cmath.h> // for sqrt() and fabs()
#include <vcl_algorithm.h> // for max(int,int)
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_2x2.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <bdgl/bdgl_curve_algs.h>

#include <mvl/HMatrix2DAffineCompute.h>
void DPMatch::detect_tail(vcl_vector<int> &tail1 , vcl_vector<int> &tail2)
{
  int start1,start2;
  int end1,end2;
  tail2.clear();
  tail1.clear();
  vcl_vector<int> tail_start;
  vcl_vector<int> tail_end;

  vcl_vector<int> tail_start1;
  vcl_vector<int> tail_end1;

  start1=finalMap_[0].first;
  start2=finalMap_[0].second;

  end1=finalMap_[finalMap_.size()-1].first;
  end2=finalMap_[finalMap_.size()-1].second;

  for (unsigned int i=0;i+1<finalMap_.size();++i)
  {
    int x1=finalMap_[i].first; 
    int x2=finalMap_[i].second;

    if (x2==start2)
      tail_start.push_back(x1);
  }

  for (int i=finalMap_.size()-1;i>0;--i)
  {
    int x1=finalMap_[i].first;
    int x2=finalMap_[i].second;
    if (x2==end2)
      tail_end.push_back(x1);
  }
  double ratio1=(double)tail_start.size()/(double)curve1_.numPoints();
  double ratio2=(double)tail_end.size()/(double)curve1_.numPoints();
  
  matched_len1=curve1_.length()-tail_start.size()-tail_end.size();

  if (ratio1>0 && ratio1>ratio2)
  {
    for (unsigned int i=0;i<tail_start.size();i++)
      tail1.push_back(tail_start[i]);
  }
  else if (ratio2>0 && ratio2>ratio1)
  {
    for (unsigned int i=0;i<tail_end.size();i++)
      tail1.push_back(tail_end[i]);
  }
  else
    tail1.clear();

  // detecting the tail of the curve 2
  tail_start.clear();
  tail_end.clear();

  for (unsigned int i=0;i+1<finalMap_.size();++i)
  {
    int x1=finalMap_[i].first;
    int x2=finalMap_[i].second;
    if (x1==start1)
      tail_start1.push_back(x2);
  }
  for (int i=finalMap_.size()-1;i>=0;i--)
  {
    int x1=finalMap_[i].first;
    int x2=finalMap_[i].second;
    if (x1==end1)
      tail_end1.push_back(x2);
  }
  ratio1=(double)tail_start1.size()/(double)curve2_.numPoints();
  ratio2=(double)tail_end1.size()/(double)curve2_.numPoints();
  matched_len2=curve2_.length()-tail_start1.size()-tail_end1.size();
  if (ratio1>0 && ratio1>ratio2)
  {
    for (unsigned int i=0;i<tail_start1.size();i++)
      tail2.push_back(tail_start1[i]);
  }
  else if (ratio2>0&& ratio2> ratio1)
  {
    for (unsigned int i=0;i<tail_end1.size();i++)
      tail2.push_back(tail_end1[i]);
  }
  else
    tail2.clear();
}
double DPMatch::normfinalCost()
{   
	double alpha =0;
	double R=0;
	if(curve1_.Tcurvature()>=curve2_.Tcurvature())
			alpha=curve2_.Tcurvature();
	if(curve1_.Tcurvature()<curve2_.Tcurvature())
			alpha=curve1_.Tcurvature();
	
	return finalCost_/((1+R*alpha)*((double)matched_len1+(double)matched_len2))/2;
}
vcl_map <int,int> DPMatch::refine_mapping()
{
    vcl_map<int,int> one_to_one;
	int hist1=0;
	int hist2=0;
	int x1;
	int x2;
	
	for(int i=0;i<finalMap_.size();i++)
	{
	
	  x1=finalMap_[i].first;
	  x2=finalMap_[i].second;
	  if(i>0)
	    {
	      if(x1== finalMap_[i-1].first)
		hist1++;
	      else
		hist1=0;
	      
	      if(x2== finalMap_[i-1].second)
		hist2++;
	      else
		hist2=0;
	      
	      if(hist1==0 && hist2==0)
		{
		  one_to_one[x1]=x2;
		}
	    }
	}
	return one_to_one;

}

double DPMatch::euclidean_distance(vnl_matrix<double> R,vnl_matrix<double> T,double scale)
{

  vcl_map<int,int>:: iterator iter1;
  vcl_vector<double> x1,y1,x2,y2,x1t,y1t;
  double xcen1=0,xcen2=0,ycen1=0,ycen2=0;
  double H[2]={0,0};
  for (iter1 = alignment.begin(); iter1!=alignment.end(); iter1++)
  {
    x1.push_back(curve1_.x((*iter1).first));
    y1.push_back(curve1_.y((*iter1).first));

    x2.push_back(curve2_.x((*iter1).second));
    y2.push_back(curve2_.y((*iter1).second));

    xcen1+=curve1_.x((*iter1).first);
    ycen1+=curve1_.y((*iter1).first);

    xcen2+=curve2_.x((*iter1).second);
    ycen2+=curve2_.y((*iter1).second);
  }
  xcen1/=alignment.size();
  ycen1/=alignment.size();
  xcen2/=alignment.size();
  ycen2/=alignment.size();

  double dist=0;
  double tx=T(0,0),ty=T(1,0);

  for (unsigned int i=0;i<x1.size();i++)
  {
    x1[i]-=xcen1;
    y1[i]-=ycen1;

	x2[i]-=xcen2;
	y2[i]-=ycen2;

    H[0]=x1[i];
    H[1]=y1[i];

    vnl_matrix<double> X (H, 2, 1);
    vnl_matrix<double> Xt=R*X+T;

    dist+=vcl_sqrt((Xt(0,0)+xcen1-x2[i]-xcen2)*(Xt(0,0)+xcen1-x2[i]-xcen2)+
                   (Xt(1,0)+ycen1-y2[i]-ycen2)*(Xt(1,0)+ycen1-y2[i]-ycen2));
  }

  dist/=x1.size();
  return dist;
}

DPMatch::DPMatch()
{
#if 0 // these are automatically initialised by the compiler
  curve1_ = Curve();
  curve2_ = Curve();
  cost_ = vcl_vector<vcl_vector<double> >();
  map_ = vcl_vector<vcl_vector<vcl_pair<int,int> > >();
  finalMap_ = vcl_vector<vcl_pair<int,int> >();
  finalMapCost_ = vcl_vector<double>();
#endif // 0
  finalCost_ = 0;
  m_ = 0;
  n_ = 0;
  R1_=10.0;
}

DPMatch::DPMatch(Curve &c1, Curve &c2)
{
  curve1_ = c1;
  curve2_ = c2;

  R1_=10.0;
  curve1_.computeProperties();
  curve2_.computeProperties();
  n_=curve1_.numPoints();
  m_=curve2_.numPoints();
  for (int n=0;n<n_;n++)
  {
    vcl_vector<double> tmp1(m_,DP_VERY_LARGE_COST);
    cost_.push_back(tmp1);
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector<vcl_pair <int,int> > tmp2(m_,tmp3);
    map_.push_back(tmp2);
  }
  finalMap_.clear();
  finalMapCost_.clear();
  finalCost_ = DP_VERY_LARGE_COST;
}
DPMatch::DPMatch(Curve &c1, Curve &c2,vgl_point_2d<double> & e)
{
  curve1_ = c1;
  curve2_ = c2;
  
  R1_=10.0;
  curve1_.computeProperties();
  curve2_.computeProperties();
  init();
  n_=curve1_.numPoints();
  m_=curve2_.numPoints();
  for (int n=0;n<n_;n++)
  {
    vcl_vector<double> tmp1(m_,DP_VERY_LARGE_COST);
    cost_.push_back(tmp1);
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector<vcl_pair <int,int> > tmp2(m_,tmp3);
    map_.push_back(tmp2);
  }
  finalMap_.clear();
  finalMapCost_.clear();
  finalCost_ = DP_VERY_LARGE_COST;
  ep_pt.set(e.x(),e.y());
  
}

DPMatch::DPMatch(vcl_vector<vcl_pair<double,double> > v1,
          vcl_vector<vcl_pair<double,double> > v2,
		  vnl_double_2 & e)
{
	curve1_.readDataFromVector(v1);
	curve2_.readDataFromVector(v2);
	R1_=10.0;
    init();
  n_=curve1_.numPoints();
  m_=curve2_.numPoints();
  for (int n=0;n<n_;n++)
  {
    vcl_vector<double> tmp1(m_,DP_VERY_LARGE_COST);
    cost_.push_back(tmp1);
    vcl_pair <int,int> tmp3(0,0);
    vcl_vector<vcl_pair <int,int> > tmp2(m_,tmp3);
    map_.push_back(tmp2);
  }
  finalMap_.clear();
  finalMapCost_.clear();
  finalCost_ = DP_VERY_LARGE_COST;
  ep_pt.set(e[1],e[2]);

}
void DPMatch::printCost()
{
  vcl_cout << "Cost Matrix\n";
  for (int i = 0; i<n_; i++)
  {
    for (int j = 0; j<m_; j++)
      vcl_printf(" %6.3f",cost_[i][j]);
    vcl_printf("\n");
  }
}


void DPMatch::initializeDPCosts()
{
  finalCost_=DP_VERY_LARGE_COST;
  for (int n=0;n<n_;n++)
    for (int m=0;m<m_;m++)
      cost_[n][m]=DP_VERY_LARGE_COST;

  cost_[0][0]=0.0;
}
double DPMatch::transformed_euclidean_distance()
{
  vcl_vector<double> x1,y1,x2,y2;
  alignment=refine_mapping();

  // get the points from refined mapping
  vcl_vector<vgl_homg_point_2d<double> > p1, p2;
  for (vcl_map<int,int>::iterator iter=alignment.begin();iter!=alignment.end();iter++)
  {
    x1.push_back(curve1_.x((*iter).first));
    x2.push_back(curve2_.x((*iter).second));
    y1.push_back(curve1_.y((*iter).first));
    y2.push_back(curve2_.y((*iter).second));

    vgl_homg_point_2d<double> point1(curve1_.x((*iter).first),curve1_.y((*iter).first));
    vgl_homg_point_2d<double> point2(curve2_.x((*iter).second),curve2_.y((*iter).second));

    p1.push_back(point1);
    p2.push_back(point2);
  }
  
  double x1_centroid=0,x2_centroid=0,y1_centroid=0,y2_centroid=0;
  unsigned int N=x1.size();
  //computing centroid

  for (unsigned int j=0;j<N;++j)
  {
    x1_centroid+=x1[j];
    x2_centroid+=x2[j];
    y1_centroid+=y1[j];
    y2_centroid+=y2[j];
  }
  x1_centroid=x1_centroid/N;
  x2_centroid=x2_centroid/N;
  y1_centroid=y1_centroid/N;
  y2_centroid=y2_centroid/N;

  // centering the data
  p1.clear();
  p2.clear();
  for (unsigned int j=0;j<N;++j)
  {
    
	x1[j]-=x1_centroid;
    x2[j]-=x2_centroid;
	
    y1[j]-=y1_centroid;
    y2[j]-=y2_centroid;
	vgl_homg_point_2d<double> point1(x1[j],y1[j]);
	vgl_homg_point_2d<double> point2(x2[j],y2[j]);
	p1.push_back(point1);
    p2.push_back(point2);

  }
  /*HMatrix2D Tfn=HMatrix2DAffineCompute::compute(p1,p2);
  vcl_cout<<"\n"<<Tfn.get(0,0)<<"\t"<<Tfn.get(0,1)<<"\t"<<Tfn.get(0,2)
		   <<"\n"<<Tfn.get(1,0)<<"\t"<<Tfn.get(1,1)<<"\t"<<Tfn.get(1,2)
		   <<"\n"<<Tfn.get(2,0)<<"\t"<<Tfn.get(2,1)<<"\t"<<Tfn.get(2,2);
  */
  double H[4]={0,0,0,0};
  //computing covariance matrix
  for (unsigned int j=0;j<N;++j)
  {
    H[0]+=x1[j]*x2[j];
    H[1]+=x1[j]*y2[j];
    H[2]+=y1[j]*x2[j];
    H[3]+=y1[j]*y2[j];
  }
  //computing svd
  vnl_matrix<double> M (H, 2, 2);
  vnl_svd<double> svd(M, 1e-10);
  vnl_matrix<double> U;
  vnl_matrix<double> Ut;
  vnl_matrix<double> V;

  U=svd.U();
  V=svd.V();
  Ut=U.transpose();
  R=V*Ut;
  double tx=0,ty=0; // double theta=vcl_acos(R(0,0));
  double center1[2]={x1_centroid,y1_centroid};
  double center2[2]={x2_centroid,y2_centroid};

  vnl_matrix<double> cen1(center1,2,1);
  vnl_matrix<double> cen2(center2,2,1);

  //computing scale
  scale=1;
  double numerator=0;
  double denominator=0;


  for (unsigned int i=0;i<N;++i)
  {
    double p[2];
    p[0]= x1[i];
    p[1]= y1[i];
    vnl_matrix<double> Point (p, 2, 1);
    vnl_matrix<double> PointT;
    PointT=R*Point;
    numerator+=PointT(0,0)*x2[i]+PointT(1,0)*y2[i];
    denominator+=Point(0,0)*Point(0,0)+Point(1,0)*Point(1,0);
  }
  if (denominator>1e-6)
    scale = numerator/denominator;

  for (unsigned int i=0;i<x1.size();i++)
  {
    vnl_double_2 X(x1[i],y1[i]);
    //double tempX[2]={x1[i],y1[i]};
    //vnl_matrix<double> X(tempX,2,1) ;
    vnl_double_2 Xt=vnl_double_2x2(R)*X;

    tx+=(x2[i]+cen2(0,0)-Xt[0]-cen1(0,0));
    ty+=(y2[i]+cen2(1,0)-Xt[1]-cen1(1,0));
  }
  tx/=x1.size();
  ty/=y1.size();

  double temp[2];
  temp[0]=tx;
  temp[1]=ty;
  vnl_matrix<double> Tavg(temp,2,1);
#if 0
  Tbar=cen2-cen1;
  Tbar(0,0)=tx;
  Tbar(1,0)=ty;
#endif
  Tbar=Tavg;

  //Tbar(0,0)=Tfn.get(0,2);
  //Tbar(1,0)=Tfn.get(1,2);

  //R(0,0)=Tfn.get(0,0);
  //R(0,1)=Tfn.get(0,1);
  //R(1,0)=Tfn.get(1,0);
  //R(1,1)=Tfn.get(1,1);

  T=cen2-/*scale**/R*cen1;
  return euclidean_distance(R,Tbar,scale);
}

void DPMatch::computeDPCosts()
{
  int XOFFSET[9] = {-1, 0,-1,-1,-2,-2,-3,-1,-3};
  int YOFFSET[9] = {-1,-1, 0,-2,-1,-3,-2,-3,-1};

  for (int sum = 2; sum<n_+m_; ++sum)
  {
    int start=vcl_max(0,sum-m_);
    for (int i=start; i<n_ && i<sum; ++i)
    {
      int j=sum-i-1;
      for (int k=0;k<9;k++)
      {
        int ip=i+XOFFSET[k];
        int jp=j+YOFFSET[k];
        if (ip >= 0 &&  jp >=0)
        {
          double incCost=computeIntervalCost(i,ip,j,jp);
          double cost =cost_[ip][jp]+incCost;
          if (cost < cost_[i][j])
          {
            cost_[i][j]=cost;
            map_[i][j].first=ip;
            map_[i][j].second=jp;
          }
        }
      }
    }
  }
}
void DPMatch::init()
{
 vcl_vector<vgl_point_2d<double> > points1;
 vcl_vector<vgl_point_2d<double> > points2;
 vgl_point_2d<double> temp;

 for(int i=0;i<curve1_.numPoints();i++)
 {
  temp.set(curve1_.x(i),curve1_.y(i));
  points1.push_back(temp);
 }
 for(int i=0;i<curve2_.numPoints();i++)
 {
  temp.set(curve2_.x(i),curve2_.y(i));
  points2.push_back(temp);
 }

 dc1=bdgl_curve_algs::create_digital_curves(points1);
 dc2=bdgl_curve_algs::create_digital_curves(points2);

}

double DPMatch::computeEpipolarCost(int i, int ip, int j, int jp)
{
  
  vgl_homg_point_2d<double> e(ep_pt);
  // assuming the variance of a point to be 0.5 pixel
  double sig_point=0.5;

  if(curve1_.numPoints()>0 && curve2_.numPoints()>0)
  {
	vgl_homg_point_2d<double> p1(curve1_.x(i),curve1_.y(i));
	vgl_homg_point_2d<double> p2(curve2_.x(j),curve2_.y(j));
	vgl_homg_point_2d<double> p1pos,p1neg,p2pos,p2neg;
	// checking if thetwo points are same
    if(p1.x()==p2.x() && p1.y()==p2.y())
		return 0;
	else
	{
	  	if(p1.y()-p2.y()==0)
		{
			p1pos.set(p1.x()+sig_point,p1.y());
			p1neg.set(p1.x()-sig_point,p1.y());

			p2pos.set(p2.x()+sig_point,p2.y());
			p2neg.set(p2.x()-sig_point,p2.y());

		}
		else
		{
			double m=(p1.x()-p2.x())/(p2.y()-p1.y());
			p1pos.set(p1.x()+sig_point*m/vcl_sqrt(m*m+1),p1.y()+sig_point/vcl_sqrt(m*m+1));
			p1neg.set(p1.x()-sig_point*m/vcl_sqrt(m*m+1),p1.y()+sig_point/vcl_sqrt(m*m+1));

			p2pos.set(p2.x()+sig_point*m/vcl_sqrt(m*m+1),p2.y()+sig_point/vcl_sqrt(m*m+1));
			p2neg.set(p2.x()-sig_point*m/vcl_sqrt(m*m+1),p2.y()+sig_point/vcl_sqrt(m*m+1));
		}
	vgl_line_2d<double> l1(p1,p2);
	double dist1,dist2;
	if(l1.a()*p1pos.x()+l1.b()*p1pos.y()+l1.c()>0 && (l1.a()*p2pos.x()+l1.b()*p2pos.y()+l1.c()>0))
	{
		vgl_homg_line_2d<double> line1(p1pos,p2neg);
		vgl_homg_line_2d<double> line2(p2pos,p1neg);
		dist1=vgl_homg_operators_2d<double>::perp_dist_squared (e, line1);
		dist2=vgl_homg_operators_2d<double>::perp_dist_squared (e, line2);
	}
	else
	{
		vgl_homg_line_2d<double> line1(p1pos,p2pos);
		vgl_homg_line_2d<double> line2(p2neg,p1neg);
		dist1=vgl_homg_operators_2d<double>::perp_dist_squared (e, line1);
		dist2=vgl_homg_operators_2d<double>::perp_dist_squared (e, line2);
	}
	vgl_homg_line_2d<double> hl1(l1);
	double dist=vgl_homg_operators_2d<double>::perp_dist_squared (e, hl1);
	return dist/vcl_fabs(dist1-dist2);
	}
  }
  return -1;

}
double DPMatch::computeIntervalCost(int i, int ip, int j, int jp)
{
  R1_=10;
  curve1_.stretchCost(i,ip,ds1_);
  curve2_.stretchCost(j,jp,ds2_);
  curve1_.bendCost(i,ip,dt1_);
  curve2_.bendCost(j,jp,dt2_);
  double w=0.2;
  double C=0.01;
  double delta=1;
  double dE=computeEpipolarCost(i,ip, j,  jp);
  double dF = vcl_fabs(ds1_ - ds2_);
  double dK = vcl_fabs(dt1_ - dt2_);
  double cost = dF + R1_*dK +R1_*pow(vcl_sqrt(dE)/delta,5)/(1+pow(vcl_sqrt(dE)/delta,5));//C*vcl_
  if (ip==0 || jp==0)
    cost*=w;
  if (i==n_-1|| j==m_-1)
    cost*=w;
  if(cost<0)
		vcl_cout<<"\t"<<cost<<"\t"<<dE;
  return cost;
}


void DPMatch::findDPCorrespondence()
{
  finalMap_.clear();
  finalMapCost_.clear();
  finalCost_=cost_[n_-1][m_-1];
  finalCost_=finalCost_;

  int ip=n_-1;
  int jp=m_-1;
  int i=n_-1;
  int j=m_-1;
  vcl_pair<int,int> p(ip,jp);
  finalMap_.push_back(p);
  finalMapCost_.push_back(cost_[p.first][p.second]);
  while (ip > 0 || jp > 0)
  {
    ip=map_[i][j].first;
    jp=map_[i][j].second;
	//vcl_cout<<"\n"<<i<<"\t"<<ip<<"\t"<<j<<"\t"<<jp<<"\t"<<vcl_sqrt(computeEpipolarCost2(i,ip, j,  jp))<<"\t"
	//			<<computeIntervalCost(i,ip,j,jp);
    vcl_pair<int,int> p(ip,jp);
    finalMap_.push_back(p);
    finalMapCost_.push_back(cost_[p.first][p.second]);
    i=ip;
    j=jp;
  }
}


void DPMatch::findDPCorrespondence(int n, int m)
{
  finalMap_.clear();
  finalMapCost_.clear();
  finalCost_=cost_[n][m];

  int ip=n;
  int jp=m;
  int i=n;
  int j=m;
  vcl_pair<int,int> p1;
  vcl_pair<int,int> p(ip,jp);
  p1=p;
  vcl_vector<double> pt;
  finalMap_.push_back(p);
  finalMapCost_.push_back(cost_[p.first][p.second]);
  while (ip > 0 || jp > 0)
  {
    ip=map_[i][j].first;
    jp=map_[i][j].second;
    vcl_pair <int,int> p(ip,jp);
    computeIntervalCost(p1.first,p1.second, p.first,p.second);

    finalMap_.push_back(p);
    finalMapCost_.push_back(cost_[p.first][p.second]);
    i=ip;
    j=jp;
  }
}

void DPMatch::findEndPoint()
{
  vcl_cout << "In DP Endpoint\n";

  finalCost_=1E10;
  int endIndex = -1;
  for (int i=0;i<m_;i++)
  {
    if (cost_[n_-1][i] < finalCost_)
    {
      vcl_cout << finalCost_ << ' ' << cost_[n_-1][i] << ' ' << i << '\n';
      finalCost_=cost_[n_-1][i];
      endIndex=i;
    }
  }
  if (endIndex >= 0)
    findDPCorrespondence(n_-1,endIndex);
}

void DPMatch::match()
{
  initializeDPCosts();
  //vcl_cout << "initializeDPCosts done\n";
  computeDPCosts();
  //vcl_cout << "computeDPCosts done\n";
  findDPCorrespondence();
}


void DPMatch::endPointMatch()
{
  //vcl_cout << "in DP Match\n";
  initializeDPCosts();
  //vcl_cout << "initializeDPCosts done\n";
  computeDPCosts();
  //vcl_cout << "computeDPCosts done\n";
  findEndPoint();
  //vcl_cout << "corresp done\n";
}













