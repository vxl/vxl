//bdgl_curve_clustering.cxx : implementation of bdgl_curve_clustering class

#include <bdgl/bdgl_curve_clustering.h>
#include <vcl_cmath.h>
#include <vcl_map.h>
#include <vcl_algorithm.h>


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
	
	for(int i=0;i<orig_curve.size();i++)
	{
		x.push_back(orig_curve[i].x());
		y.push_back(orig_curve[i].y());
	}

	double x_cen=compute_mean(x);
	double y_cen=compute_mean(y);
	
	for(int i=0;i<orig_curve.size();i++)
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
	double x2,y2;
	double cost=0;
	vcl_map<int,int>::iterator iter;
	vcl_vector<vgl_point_2d<double> > curve1;
	vcl_vector<vgl_point_2d<double> > tcurve1;
	vcl_vector<vgl_point_2d<double> > curve2;
	for(iter=clusters_[j].prototype->get_best_match_prev()->mapping_.begin();
		iter!=clusters_[j].prototype->get_best_match_prev()->mapping_.end();
		iter++)
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
	for(int i=0;i<tcurve1.size();i++)
	{
		double dist=vcl_sqrt((tcurve1[i].x()-curve2[i].x())*(tcurve1[i].x()-curve2[i].x())
							+(tcurve1[i].y()-curve2[i].y())*(tcurve1[i].y()-curve2[i].y()));
		cost+=dist;

	}
	cost/=clusters_[j].prototype->get_best_match_prev()->mapping_.size();
	return cost;
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
	for(int k=0;k<clusters_[i].curve_cluster_.size();k++)
	{
		intra.clear();
		for(int l=0;l<clusters_[j].curve_cluster_.size();l++)
		{
			p.first=clusters_[i].curve_cluster_[k];
			p.second=clusters_[j].curve_cluster_[l];
			double cost1=distance_table[p];
			pt.first=p.second;
			pt.second=p.first;
			double cost2=distance_table[pt];
			// maximum of the two distances A->B and B->A
			if(cost1>cost2)
				intra.push_back(cost1);
			else
				intra.push_back(cost2);
		}
		inter.push_back(median(intra));
	}
	return median(inter);
}
//intialize the clusters
void bdgl_curve_clustering::init_clusters(vcl_vector<bdgl_tracker_curve_sptr> * curve_sets)
{
	for(int i=0;i<(*curve_sets).size();i++)
	{
		bdgl_curve_cluster temp_cluster;
		if((*curve_sets)[i]->get_best_match_prev())
		{
			if((*curve_sets)[i]->get_best_match_prev()->euc_<5
				&& vcl_fabs(vcl_asin((*curve_sets)[i]->get_best_match_prev()->R_(0,1)))<0.1)
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


void bdgl_curve_clustering::cluster_curves(vcl_vector<bdgl_tracker_curve_sptr> * curve_sets)
{
	// intialize the clusters
	init_clusters(curve_sets);
	//cluster the curves
	clustering();
}

void bdgl_curve_clustering::clustering()
{
 
	// computing distance betwen each pair of curve using their effect of transforms
	distance_table.clear();
	for(int i=0;i<clusters_.size();i++)
	{
		for(int j=0;j<clusters_.size();j++)
		{
			double cost=compute_euclidean_dist(i,j);
			vcl_pair<bdgl_tracker_curve_sptr,bdgl_tracker_curve_sptr> p;
			p.first=clusters_[i].curve_cluster_[0];
			p.second=clusters_[j].curve_cluster_[0];
			distance_table[p]=cost;
		}
	}
	// some temporary variables
	double min_cost=0;
	int mini,minj;

	int clustering_=1;
	while(clusters_.size()>no_of_clusters_ && clustering_)
	{
		min_cost=1e6;
		for(int i=0;i<clusters_.size();i++)
		{
			for(int j=i;j<clusters_.size();j++)
			{
				if(i!=j)
				{
					double cost1=compute_cluster_dist(i,j);
					if(min_cost>cost1)
					{
						min_cost=cost1;
						mini=i;
						minj=j;
					}
				}
			}
		}
		if(min_cost<min_cost_threshold_)
			merge_clusters(mini,minj);
		else
			clustering_=0;
	}
	// assigning new ids and filtering out groups with one member
	for(int i=0;i<clusters_.size();i++)
	{
		if(clusters_[i].curve_cluster_.size()==1)
			clusters_[i].curve_cluster_[0]->group_id_=-1;
		else
		{
			for(int j=0;j<clusters_[i].curve_cluster_.size();j++)
			{
				clusters_[i].curve_cluster_[j]->group_id_=i;
			}
		}
	}

}

double bdgl_curve_clustering::median(vcl_vector<double> vec)
{
   int size = vec.size();
   if(size == 0 )
     vcl_cout<<"median of an empty vector";
   vcl_sort(vec.begin(), vec.end());
   int mid = (int)size/2;
   return size % 2 == 0 ? (vec[mid] + vec[mid-1]) / 2 : vec[mid]; 
}
double bdgl_curve_clustering::min(vcl_vector<double> vec)
{
   int size = vec.size();
   if(size == 0 )
   {vcl_cout<<"median of an empty vector";return -1;}
   vcl_sort(vec.begin(), vec.end());
   return vec[0];
}
double bdgl_curve_clustering::compute_mean(vcl_vector<double> t)
{
	double sum=0;
	for(int i=0;i<t.size();i++)
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
	for(int i=0;i<t.size();i++)
	{
		sum+=t[i];
	}
	sum=sum/t.size();
	for(int i=0;i<t.size();i++)
	{
		std+=(t[i]-sum)*(t[i]-sum);
	}
	std=vcl_sqrt(std/t.size());
	return std;
}

// merge cluster j into i
void bdgl_curve_clustering::merge_clusters(int i,int j)
{
	int sizeofi,sizeofj;
	vcl_vector<bdgl_tracker_curve_sptr>::iterator iter;
	vcl_vector<bdgl_tracker_curve_sptr>::iterator citer;

	
	sizeofi=clusters_[i].curve_cluster_.size();
	sizeofj=clusters_[j].curve_cluster_.size();
    
	for(int k=0;k<sizeofi;k++)
	{clusters_[i].curve_cluster_[k]->group_id_=i;}
	
	for(int k=0;k<sizeofj;k++)
	{
		clusters_[j].curve_cluster_[k]->group_id_=i;//clusters_[i].prototype->group_id_;
		clusters_[i].curve_cluster_.push_back(clusters_[j].curve_cluster_[k]);
	}

	clusters_.erase(clusters_.begin()+j);

}
//  function to obtain curves on the moving object
void bdgl_curve_clustering::get_moving_objects(int frame_no,vcl_vector<vcl_vector<bdgl_tracker_curve_sptr> > & curves_on_objects)
{
	for(int i=0;i<clusters_.size();i++)
	{
	 double sum=0,std=0;
	 if(clusters_[i].curve_cluster_.size()>1)
	 {
		for(int j=0;j<clusters_[i].curve_cluster_.size();j++)
		{
			
			vnl_matrix<double> T=clusters_[i].curve_cluster_[j]->get_best_match_prev()->Tbar;
			double tx=T(0,0);
			double ty=T(1,0);

			sum+=vcl_sqrt(tx*tx+ty*ty);

		}
		sum/=clusters_[i].curve_cluster_.size();
		
		for(int j=0;j<clusters_[i].curve_cluster_.size();j++)
		{
			vnl_matrix<double> T=clusters_[i].curve_cluster_[j]->get_best_match_prev()->T_;
			double tx=T(0,0);
			double ty=T(1,0);
			std+=vcl_sqrt((vcl_sqrt(tx*tx+ty*ty)-sum)*(vcl_sqrt(tx*tx+ty*ty)-sum));
		}
		std/=clusters_[i].curve_cluster_.size();

		if(sum>foreg_backg_threshold_ && sum-std > 0)
		{
			vcl_vector<bdgl_tracker_curve_sptr> temp;
			for(int j=0;j<clusters_[i].curve_cluster_.size();j++)
			{
				clusters_[i].curve_cluster_[j]->group_id_=i;
				temp.push_back(clusters_[i].curve_cluster_[j]);
			}
			curves_on_objects.push_back(temp);
		}
		
		}
	}
}
