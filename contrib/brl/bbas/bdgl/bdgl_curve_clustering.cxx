//bdgl_curve_clustering.cxx : implementation of bdgl_curve_clustering class

#include "bdgl_curve_clustering.h"
#include <bdgl/bdgl_tracker_curve.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_point_2d.h>
#include <mbl/mbl_histogram.h>

bdgl_curve_cluster::bdgl_curve_cluster()
{
  curve_cluster_.clear();
  cluster_id_=-1;
  scale_=1;
}


void bdgl_curve_clustering::compute_transformation
         (vcl_vector<vgl_point_2d<double> > orig_curve,
          vcl_vector<vgl_point_2d<double> > & transformed_curve,
          vnl_matrix<double> R,vnl_matrix<double> T,double s)
{
  vcl_vector<double> x;
  vcl_vector<double> y;
  transformed_curve.clear();

  for (unsigned int i=0; i<orig_curve.size(); ++i)
  {
    x.push_back(orig_curve[i].x());
    y.push_back(orig_curve[i].y());
  }

  double x_cen=compute_mean(x);
  double y_cen=compute_mean(y);

  for (unsigned int i=0; i<orig_curve.size(); ++i)
  {
    double tempx=orig_curve[i].x()-x_cen;
    double tempy=orig_curve[i].y()-y_cen;

    double xt=s*R(0,0)*tempx + s*R(0,1)*tempy + T(0,0)+x_cen;
    double yt=s*R(1,0)*tempx + s*R(1,1)*tempy + T(1,0)+y_cen;

    vgl_point_2d<double> temp(xt,yt);
    transformed_curve.push_back(temp);
  }
}

double bdgl_curve_clustering::compute_euclidean_dist(int i,int j)
{
        // computing the euclidean distance
        // by transforming j using transformation of i
        vnl_matrix<double> Ri,Ti;
        Ri=clusters_[i].get_R();
        Ti=clusters_[i].get_T();
        double si=clusters_[i].get_scale();
        double cost=0;
        vcl_map<int,int>::iterator iter;
        vcl_vector<vgl_point_2d<double> > curve1;
        vcl_vector<vgl_point_2d<double> > tcurve1;
        vcl_vector<vgl_point_2d<double> > curve2;

        for (iter=clusters_[j].prototype->get_best_match_prev()->mapping_.begin();
             iter!=clusters_[j].prototype->get_best_match_prev()->mapping_.end(); ++iter)
        {
          double tempx1=clusters_[j].prototype->get_best_match_prev()->match_curve_set[0]->desc->points_[(*iter).first].x();
          double tempy1=clusters_[j].prototype->get_best_match_prev()->match_curve_set[0]->desc->points_[(*iter).first].y();
          vgl_point_2d<double> point1(tempx1,tempy1);
          curve1.push_back(point1);
          double tempx2=clusters_[j].prototype->desc->points_[(*iter).second].x();
          double tempy2=clusters_[j].prototype->desc->points_[(*iter).second].y();
          vgl_point_2d<double> point2(tempx2,tempy2);
          curve2.push_back(point2);
        }
        compute_transformation(curve1,tcurve1,Ri,Ti,si);
        for (unsigned int i=0; i<tcurve1.size(); ++i)
        {
          double dist=vcl_sqrt((tcurve1[i].x()-curve2[i].x())*(tcurve1[i].x()-curve2[i].x())
                              +(tcurve1[i].y()-curve2[i].y())*(tcurve1[i].y()-curve2[i].y()));
          cost+=dist;
        }
        cost/=clusters_[j].prototype->get_best_match_prev()->mapping_.size();

        //double cost1=compute_hausdorff_distance(tcurve1,curve2);
        return cost;
}

double bdgl_curve_clustering::compute_hausdorff_distance(vcl_vector<vgl_point_2d<double> > curve1,
                                                         vcl_vector<vgl_point_2d<double> > curve2)
{
  double maxdAB=-1;
  for (int i=0;i<curve1.size();i++)
  {
    double mindxB=1e6;
    for (unsigned int j=0; j<curve2.size(); ++j)
    {
      double dxB=vcl_sqrt((curve1[i].x()-curve2[j].x())*(curve1[i].x()-curve2[j].x())
                          +(curve1[i].y()-curve2[j].y())*(curve1[i].y()-curve2[j].y()));
      if (dxB<mindxB)
        mindxB=dxB;
    }
    if (mindxB>maxdAB)
      maxdAB=mindxB;
  }

  double maxdBA=-1;
  for (unsigned int i=0; i<curve2.size(); ++i)
  {
    double mindxA=1e6;
    for (unsigned int j=0; j<curve1.size(); ++j)
    {
      double dxA=vcl_sqrt((curve2[i].x()-curve1[j].x())*(curve2[i].x()-curve1[j].x())
                  +(curve2[i].y()-curve1[j].y())*(curve2[i].y()-curve1[j].y()));
      if (dxA<mindxA)
        mindxA=dxA;
    }
    if (mindxA>maxdBA)
      maxdBA=mindxA;
  }

  if (maxdBA>=maxdAB)
    return maxdBA;
  if (maxdAB>maxdBA)
    return maxdAB;

  return -1;
}

double bdgl_curve_clustering::compute_vector_distance(int i,int j)
{
  vcl_pair<bdgl_tracker_curve_sptr,bdgl_tracker_curve_sptr> p;
  vcl_pair<bdgl_tracker_curve_sptr,bdgl_tracker_curve_sptr> pt;
  if (clusters_[i].curve_cluster_.size()==1 &&
      clusters_[j].curve_cluster_.size()==1)
  {
    p.first=clusters_[i].curve_cluster_[0];
    p.second=clusters_[j].curve_cluster_[0];
    double cost1=distance_table[p];

    p.first=clusters_[i].curve_cluster_[0];
    p.second=clusters_[j].curve_cluster_[0];
    double cost2=distance_table[p];

    double cost= cost1>cost2 ? cost1:cost2;
    return cost;
  }
  vcl_vector<double> min_distances;
  for (unsigned int k=0; k<clusters_[i].curve_cluster_.size(); ++k)
  {
    min_distances.clear();
    vcl_vector<double> distances;
    for (unsigned int l=0; l<clusters_[j].curve_cluster_.size(); ++l)
    {
      distances.clear();
      double dist=0;
      for (unsigned int m=0; m<clusters_[j].curve_cluster_.size(); ++m)
      {
        if (l!=m)
        {
          p.first=clusters_[j].curve_cluster_[l];
          p.second=clusters_[j].curve_cluster_[m];
          double i_cost1=distance_table[p];

          p.first=clusters_[j].curve_cluster_[m];
          p.second=clusters_[j].curve_cluster_[l];
          double i_cost2=distance_table[p];

          double i_cost=i_cost2>i_cost1 ? i_cost2:i_cost1;

          pt.first=clusters_[i].curve_cluster_[k];
          pt.second=clusters_[j].curve_cluster_[m];
          double e_cost1=distance_table[p];

          pt.first=clusters_[j].curve_cluster_[m];
          pt.second=clusters_[i].curve_cluster_[k];
          double e_cost2=distance_table[p];

          double e_cost=e_cost2>e_cost1 ? e_cost2:e_cost1;

          dist+=(i_cost-e_cost)*(i_cost-e_cost);
        }
      }
      if (dist>0)
        distances.push_back(dist);
    }
    min_distances.push_back(min(distances));
  }
  return max(min_distances);
}

double bdgl_curve_clustering::compute_likelihood_distance(int i,int j)
{
  vcl_pair<bdgl_tracker_curve_sptr,bdgl_tracker_curve_sptr> p;
  vcl_pair<bdgl_tracker_curve_sptr,bdgl_tracker_curve_sptr> pt;
  vcl_vector<double> inter;
  vcl_vector<double> intra;
  vcl_vector<double> clusteri,clusterj;
  double mu_i,mu_j,sigma_i,sigma_j;
  inter.clear();
  if (clusters_[i].curve_cluster_.size()==1 &&
      clusters_[j].curve_cluster_.size()==1)
  {
    p.first=clusters_[i].curve_cluster_[0];
    p.second=clusters_[j].curve_cluster_[0];
    double cost1=distance_table[p];

    p.first=clusters_[i].curve_cluster_[0];
    p.second=clusters_[j].curve_cluster_[0];
    double cost2=distance_table[p];

    double cost= cost1>cost2 ? cost1:cost2;
    return vcl_exp(-cost);
  }

  // computing sigma and mu of the cluster i
  if (clusters_[i].curve_cluster_.size()>1)
  {
    for (unsigned int k=0; k+1<clusters_[i].curve_cluster_.size(); ++k)
    {
      for (unsigned int l=k+1;l<clusters_[i].curve_cluster_.size(); l++)
      {
        p.first=clusters_[i].curve_cluster_[k];
        p.second=clusters_[i].curve_cluster_[l];
        double cost1=distance_table[p];
        p.first=clusters_[i].curve_cluster_[l];
        p.second=clusters_[i].curve_cluster_[k];
        double cost2=distance_table[p];

        double cost=cost1>cost2 ? cost1:cost2;
        clusteri.push_back(cost);
      }
    }
    mu_i=compute_mean(clusteri);
    sigma_i=compute_std(clusteri);
  }
  else
  {
    mu_i=0;
    sigma_i=0;
  }
  clusters_[i].mu=mu_i;
  clusters_[i].sigma=sigma_i;
  // computing sigma and mu of the cluster j
  if (clusters_[j].curve_cluster_.size()>1)
  {
    for (unsigned int k=0; k+1<clusters_[j].curve_cluster_.size(); ++k)
    {
      for (unsigned int l=k+1; l<clusters_[j].curve_cluster_.size(); ++l)
      {
        p.first=clusters_[j].curve_cluster_[k];
        p.second=clusters_[j].curve_cluster_[l];
        double cost1=distance_table[p];
        p.first=clusters_[j].curve_cluster_[l];
        p.second=clusters_[j].curve_cluster_[k];
        double cost2=distance_table[p];
        double cost=cost1>cost2 ? cost1:cost2;
        clusterj.push_back(cost);
      }
    }
    mu_j=compute_mean(clusterj);
    sigma_j=compute_std(clusterj);
  }
  else
  {
    mu_j=0;
    sigma_j=0;
  }
  clusters_[j].mu=mu_j;
  clusters_[j].sigma=sigma_j;

  double sum1=0;
  double sum2=0;
  // cluster sizes are greater than 1;
  if (clusters_[j].curve_cluster_.size()>1)
  {
    for (unsigned int k=0; k<clusters_[i].curve_cluster_.size(); ++k)
    {
      intra.clear();
      for (unsigned int l=0; l<clusters_[j].curve_cluster_.size(); ++l)
      {
        p.first=clusters_[i].curve_cluster_[k];
        p.second=clusters_[j].curve_cluster_[l];
        double cost1=distance_table[p];
        pt.first=p.second;
        pt.second=p.first;
        double cost2=distance_table[pt];
        // maximum of the two distances A->B and B->A
        double cost=cost1>cost2 ? cost1:cost2;
        sum1+=(cost-mu_j)*(cost-mu_j);
     }
   }
   sum1=sum1/(2*sigma_j*sigma_j);
   sum1=vcl_exp(-sum1);
  }
  else
    sum1=0;
  if (clusters_[i].curve_cluster_.size()>1)
  {
    for (unsigned int k=0; k<clusters_[j].curve_cluster_.size(); ++k)
    {
      intra.clear();
      for (unsigned int l=0; l<clusters_[i].curve_cluster_.size(); ++l)
      {
        p.first=clusters_[j].curve_cluster_[k];
        p.second=clusters_[i].curve_cluster_[l];
        double cost1=distance_table[p];
        pt.first=p.second;
        pt.second=p.first;
        double cost2=distance_table[pt];
        // maximum of the two distances A->B and B->A
        double cost=cost1>cost2 ? cost1:cost2;

        sum2+=(cost-mu_i)*(cost-mu_i);
      }
    }
    sum2=sum2/(2*sigma_i*sigma_i);
    sum2=vcl_exp(-sum2);
  }
  else
    sum2=0;
  return sum2>sum1 ? sum2:sum1;
}

double bdgl_curve_clustering::compute_cluster_dist(int i,int j)
{
  // simple clustering method using median of the distances as the
  // distance between two clusters
  vcl_pair<bdgl_tracker_curve_sptr,bdgl_tracker_curve_sptr> p;
  vcl_pair<bdgl_tracker_curve_sptr,bdgl_tracker_curve_sptr> pt;
  vcl_vector<double> inter;
  vcl_vector<double> intra;
  inter.clear();

  for (unsigned int k=0; k<clusters_[i].curve_cluster_.size(); ++k)
  {
    intra.clear();

    for (unsigned int l=0; l<clusters_[j].curve_cluster_.size(); ++l)
    {
      p.first=clusters_[i].curve_cluster_[k];
      p.second=clusters_[j].curve_cluster_[l];
      double cost1=distance_table[p];
      pt.first=p.second;
      pt.second=p.first;
      double cost2=distance_table[pt];
      // maximum of the two distances A->B and B->A
      if (cost1>cost2)
        intra.push_back(cost1);
      else
        intra.push_back(cost2);
    }

    inter.push_back(median(intra));
  }
  return median(inter);
}

//initialize the clusters
void bdgl_curve_clustering::init_clusters(vcl_vector<bdgl_tracker_curve_sptr> * curve_sets)
{
  for (unsigned int i=0; i<(*curve_sets).size(); ++i)
  {
    bdgl_curve_cluster temp_cluster;
    if ((*curve_sets)[i]->get_best_match_prev())
    {
      if ((*curve_sets)[i]->get_best_match_prev()->euc_<5 && (*curve_sets)[i]->isreal_ &&
          vcl_fabs(vcl_asin((*curve_sets)[i]->get_best_match_prev()->R_(0,1)))<0.1)
      {
        (*curve_sets)[i]->group_id_=i;
        temp_cluster.curve_cluster_.push_back((*curve_sets)[i]);
        temp_cluster.prototype=(*curve_sets)[i];

        // assigning R , T and scale to the clusters
        temp_cluster.set_R((*curve_sets)[i]->get_best_match_prev()->R_);
        temp_cluster.set_T((*curve_sets)[i]->get_best_match_prev()->Tbar);
        temp_cluster.set_scale((*curve_sets)[i]->get_best_match_prev()->scale_);
        clusters_.push_back(temp_cluster);
      }
    }
  }
}

void bdgl_curve_clustering::write_distance_table(vcl_string filename,vcl_vector<bdgl_tracker_curve_sptr> * curve_sets)
{
  vcl_ofstream ofile(filename.c_str());
  if (!ofile)
    return;
  vcl_pair<bdgl_tracker_curve_sptr,bdgl_tracker_curve_sptr> p;
  for (unsigned int i=0; i<(*curve_sets).size(); ++i)
  {
    bdgl_tracker_curve_sptr c1=(*curve_sets)[i];

    if ((*curve_sets)[i]->get_best_match_prev())
    {
      if ((*curve_sets)[i]->get_best_match_prev()->euc_<5 && (*curve_sets)[i]->isreal_ &&
          vcl_fabs(vcl_asin((*curve_sets)[i]->get_best_match_prev()->R_(0,1)))<0.1)
      {
        ofile<<c1->get_id()<<'\t';
        for (unsigned int j=0; j<(*curve_sets).size(); ++j)
        {
          bdgl_tracker_curve_sptr c2=(*curve_sets)[j];
          if ((*curve_sets)[j]->get_best_match_prev())
          {
            if ((*curve_sets)[j]->get_best_match_prev()->euc_<5 && (*curve_sets)[j]->isreal_ &&
                vcl_fabs(vcl_asin((*curve_sets)[j]->get_best_match_prev()->R_(0,1)))<0.1)
            {
              p.first=c1;
              p.second=c2;
              double cost1=distance_table[p];
              p.first=c2;
              p.second=c1;
              double cost2=distance_table[p];
              double cost=cost2>cost1 ? cost2 : cost1;
              ofile<<cost<<'\t';
            }
          }
        }
        ofile<<'\n';
      }
    }
  }
  ofile.close();
}

void bdgl_curve_clustering::cluster_curves(vcl_vector<bdgl_tracker_curve_sptr> * curve_sets)
{
  // initialize the clusters
  init_clusters(curve_sets);
  //cluster the curves
  clustering();
}

void bdgl_curve_clustering::clustering()
{
  // computing distance betwen each pair of curve using their effect of transforms
  distance_table.clear();
  vcl_ofstream f("c:\\plot_cost_distribution.txt");
  for (unsigned int i=0; i<clusters_.size(); ++i)
  {
    mbl_histogram histogramx(0,50,25);
    for (unsigned int j=0; j<clusters_.size(); ++j)
    {
      double cost=compute_euclidean_dist(i,j);

      vcl_pair<bdgl_tracker_curve_sptr,bdgl_tracker_curve_sptr> p;
      p.first=clusters_[i].curve_cluster_[0];
      p.second=clusters_[j].curve_cluster_[0];
      distance_table[p]=cost;
      histogramx.obs(cost);
    }
    vcl_vector<int> freqx=histogramx.frequency();
    f<<clusters_[i].curve_cluster_[0]->get_id();
    for (unsigned int i=0; i<freqx.size(); ++i)
    {
      f<<'\t'<<freqx[i];
    }
    f<<'\n';
  }
  f.close();
  // some temporary variables
  double min_cost=0;
  //double likelihood=0;
  int mini,minj;

  int clustering_=1;
  while (clusters_.size()>no_of_clusters_ && clustering_)
  {
    min_cost=1e6;
    for (unsigned int i=0; i<clusters_.size(); ++i)
    {
      for (unsigned int j=i; j<clusters_.size(); ++j)
      {
        if (i!=j)
        {
          double cost1=compute_cluster_dist(i,j);
          //double cost1=compute_likelihood_distance( i, j);
          if (min_cost>cost1)
          //if (likelihood>cost1)
          {
            min_cost=cost1;
            //likelihood=cost1;
            mini=i;
            minj=j;
          }
        }
      }
    }
    if (min_cost<min_cost_threshold_)
    {
      vcl_cout<< "\n the cost is "<<min_cost<<'\t'
              << clusters_[mini].mu<<'\t'<<clusters_[mini].sigma;
      merge_clusters(mini,minj);
    }
    else
      clustering_=0;
  }
  // assigning new ids and filtering out groups with one member
  for (unsigned int i=0; i<clusters_.size(); ++i)
  {
    if (clusters_[i].curve_cluster_.size()==1)
      clusters_[i].curve_cluster_[0]->group_id_=-1;
    else
    {
      for (unsigned int j=0; j<clusters_[i].curve_cluster_.size(); ++j)
      {
        clusters_[i].curve_cluster_[j]->group_id_=i;
      }
    }
  }
}

double bdgl_curve_clustering::median(vcl_vector<double> vec)
{
  unsigned int size = vec.size();
  if (size == 0 ) { vcl_cout<<"median of an empty vector\n"; return -1; }
  vcl_sort(vec.begin(), vec.end());
  unsigned int mid = size/2;
  return size % 2 == 0 ? (vec[mid] + vec[mid-1]) / 2 : vec[mid];
}

double bdgl_curve_clustering::min(vcl_vector<double> vec)
{
  unsigned int size = vec.size();
  if (size == 0 )
  { vcl_cout<<"minimum of an empty vector"; return -1; }
  vcl_sort(vec.begin(), vec.end());
  return vec[0];
}

double bdgl_curve_clustering::max(vcl_vector<double> vec)
{
  unsigned int size = vec.size();
  if (size == 0 )
  { vcl_cout<<"minimum of an empty vector"; return -1; }
  vcl_sort(vec.begin(), vec.end());
  return vec[vec.size()-1];
}

double bdgl_curve_clustering::compute_mean(vcl_vector<double> t)
{
  double sum=0;
  for (unsigned int i=0; i<t.size(); ++i)
  {
    sum+=t[i];
  }
  sum=sum/t.size();
  return sum;
}

double bdgl_curve_clustering::compute_std(vcl_vector<double> t)
{
  double sum=0;
  double std=0;
  for (unsigned int i=0; i<t.size(); ++i)
  {
    sum+=t[i];
  }
  sum=sum/t.size();
  for (unsigned int i=0; i<t.size(); ++i)
  {
    std+=(t[i]-sum)*(t[i]-sum);
  }
  std=vcl_sqrt(std/t.size());
  return std;
}

// merge cluster j into i
void bdgl_curve_clustering::merge_clusters(int i,int j)
{
  unsigned int sizeofi=clusters_[i].curve_cluster_.size();
  unsigned int sizeofj=clusters_[j].curve_cluster_.size();

  for (unsigned int k=0; k<sizeofi; ++ k)
  { clusters_[i].curve_cluster_[k]->group_id_=i; }

  for (unsigned int k=0; k<sizeofj; ++k)
  {
    clusters_[j].curve_cluster_[k]->group_id_=i; //clusters_[i].prototype->group_id_;
    clusters_[i].curve_cluster_.push_back(clusters_[j].curve_cluster_[k]);
  }

  clusters_.erase(clusters_.begin()+j);
}

//  function to obtain curves on the moving object
void bdgl_curve_clustering::get_moving_objects(int frame_no,
                                               vcl_vector<vcl_vector<bdgl_tracker_curve_sptr> >& curves_on_objects)
{
  vcl_vector<bdgl_tracker_curve_sptr> temp;
  vcl_ofstream ofile("c:\\temp-clusters.txt");
  for (unsigned int i=0; i<clusters_.size(); ++i)
  {
    double sum=0,std=0;
    if (clusters_[i].curve_cluster_.size()>1)
    {
      for (unsigned int j=0; j<clusters_[i].curve_cluster_.size(); ++j)
      {
        vnl_matrix<double> T=clusters_[i].curve_cluster_[j]->get_best_match_prev()->Tbar;
        double tx=T(0,0);
        double ty=T(1,0);

        sum+=vcl_sqrt(tx*tx+ty*ty);
      }
      sum/=clusters_[i].curve_cluster_.size();

      for (unsigned int j=0; j<clusters_[i].curve_cluster_.size(); ++j)
      {
        vnl_matrix<double> T=clusters_[i].curve_cluster_[j]->get_best_match_prev()->T_;
        double tx=T(0,0);
        double ty=T(1,0);
        std+=vcl_sqrt((vcl_sqrt(tx*tx+ty*ty)-sum)*(vcl_sqrt(tx*tx+ty*ty)-sum));
      }
      std/=clusters_[i].curve_cluster_.size();
      ofile<<"cluster no "<<i<<'\n';
      for (unsigned int j=0; j<clusters_[i].curve_cluster_.size(); ++j)
      {
        clusters_[i].curve_cluster_[j]->group_id_=i;
        ofile<< clusters_[i].curve_cluster_[j]->get_id()<<'\t'
             << clusters_[i].curve_cluster_[j]->get_best_match_prev()->T_(0,0)
             << '\t'<<clusters_[i].curve_cluster_[j]->get_best_match_prev()->T_(1,0)<<'\n';
      }
      ofile<<'\n';

      if (sum>foreg_backg_threshold_ && sum-std > 0)
      {
        for (unsigned int j=0; j<clusters_[i].curve_cluster_.size(); ++j)
        {
          clusters_[i].curve_cluster_[j]->group_id_=i;
          temp.push_back(clusters_[i].curve_cluster_[j]);
        }
        ofile<<"--------------------\n";
      }
    }
  }
  curves_on_objects.push_back(temp);

  ofile.close();
}

