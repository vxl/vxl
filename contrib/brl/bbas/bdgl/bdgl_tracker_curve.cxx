#include <bdgl/bdgl_tracker_curve.h>
#include <bdgl/bdgl_curve_algs.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vcl_cmath.h>
match_data::match_data(bdgl_tracker_curve_sptr c,match_data_sptr m)
{
  match_curve_set.push_back(c);
  curve_set=0;
  R_=m->R_;
  T_=m->T_;
  scale_=m->scale_;
  Tbar=m->Tbar;
  energy_=m->energy_;
  euc_=m->euc_;
  cost_=m->euc_;
  mapping_=m->mapping_;
  tail1_=m->tail1_;
  tail2_=m->tail2_;
}

bdgl_tracker_curve  :: bdgl_tracker_curve()
{
  id_=-1;
  match_id_=id_;
  group_id_=id_;
  track_id_=-1;
  ismatchedprev_=false;
  ismatchednext_=false;
  isreliable_=false;
  frame_number=-1;
}

void bdgl_tracker_curve  ::init_set(vtol_edge_2d_sptr const& c,int id)
{
  c_=c;
  id_=id;

  match_id_=id_;
  group_id_=id_;

  next_.clear();
  prev_.clear();

  best_match_next_=0;
  best_match_prev_=0;

  isreal_=true;
  ismatchedprev_=false;
  ismatchednext_=false;
  isreliable_=false;

  vdgl_edgel_chain_sptr  ec;
  vdgl_edgel_chain_sptr ec_sub=new vdgl_edgel_chain;

  ec=c->curve()->cast_to_digital_curve()->get_interpolator()->get_edgel_chain();

  //subsampling the curves so as to cut the computational time short
  for (unsigned int i=0; i<ec->size(); i+=2)
    ec_sub->add_edgel(ec->edgel(i));
  desc= new bdgl_curve_description(ec_sub);
  ec_=ec_sub;
}

void bdgl_tracker_curve  ::init_set(vcl_vector<vgl_point_2d<double> > p,int id)
{
  c_=0;
  id_=id;
  match_id_=id_;
  group_id_=id_;
  next_.clear();
  prev_.clear();
  best_match_next_=0;
  best_match_prev_=0;

  ismatchedprev_=false;
  ismatchednext_=false;
  isreliable_=false;
  isreal_=true;
  vdgl_digital_curve_sptr dc;
  vdgl_edgel_chain_sptr  ec;

  dc=bdgl_curve_algs::create_digital_curves(p);
  ec=dc->get_interpolator()->get_edgel_chain();

  desc= new bdgl_curve_description(ec);
}

void bdgl_tracker_curve  ::init_set(bdgl_tracker_curve_sptr c,vcl_vector<int> ks,int id)
{
  c_=0;
  id_=id;
  match_id_=id_;
  group_id_=id_;
  next_.clear();
  prev_.clear();
  best_match_next_=0;
  best_match_prev_=0;

  ismatchedprev_=false;
  ismatchednext_=false;
  isreliable_=false;
  isreal_=false;

  vdgl_edgel_chain_sptr  ec;
  vdgl_edgel_chain_sptr ec_sub=new vdgl_edgel_chain;
  vtol_edge_2d_sptr vc=c->get_curve();
  ec=vc->curve()->cast_to_digital_curve()->get_interpolator()->get_edgel_chain();
  for (unsigned int i=0; i<ks.size(); ++i)
  {
    ec_sub->add_edgel(c->ec_->edgel(ks[i]));
  }
  desc= new bdgl_curve_description(ec_sub);
}

double bdgl_tracker_curve::compute_mean(vcl_vector<double> t)
{
  double sum=0;
  for (unsigned int i=0; i<t.size(); ++i)
    sum+=t[i];
  sum=sum/t.size();
  return sum;
}

void bdgl_tracker_curve
        ::compute_transformation(vcl_vector<vgl_point_2d<double> > p,
                                 vcl_vector<vgl_point_2d<double> > & transformed_curve,
                                 vnl_matrix<double> R,vnl_matrix<double> T)
{
  vcl_vector<double> x;
  vcl_vector<double> y;

  for (unsigned int i=0; i<p.size(); ++i)
  {
    x.push_back(p[i].x());
    y.push_back(p[i].y());
  }

  double x_cen=compute_mean(x);
  double y_cen=compute_mean(y);

  for (unsigned int i=0; i<p.size(); ++i)
  {
    double tempx=p[i].x()-x_cen;
    double tempy=p[i].y()-y_cen;

    double tx=R(0,0)*tempx + R(0,1)*tempy + T(0,0)+x_cen;
    double ty=R(1,0)*tempx + R(1,1)*tempy + T(1,0)+y_cen;

    vgl_point_2d<double> temp(tx,ty);
    transformed_curve.push_back(temp);
  }
}

double bdgl_tracker_curve ::compute_euclidean_distance(vnl_matrix<double> R,vnl_matrix<double> T,double /* s */)
{
  if (get_best_match_prev())
  {
    double cost=0;
    vcl_map<int,int>::iterator iter;
    vcl_vector<vgl_point_2d<double> > curve1;
    vcl_vector<vgl_point_2d<double> > tcurve1;
    vcl_vector<vgl_point_2d<double> > curve2;
    for (iter=get_best_match_prev()->mapping_.begin();
         iter!=get_best_match_prev()->mapping_.end();
         iter++)
    {
      double tempx1=get_best_match_prev()->match_curve_set[0]->desc->points_[(*iter).first].x();
      double tempy1=get_best_match_prev()->match_curve_set[0]->desc->points_[(*iter).first].y();
      vgl_point_2d<double> point1(tempx1,tempy1);
      curve1.push_back(point1);

      double tempx2=desc->points_[(*iter).second].x();
      double tempy2=desc->points_[(*iter).second].y();
      vgl_point_2d<double> point2(tempx2,tempy2);
      curve2.push_back(point2);
    }
    compute_transformation(curve1,tcurve1,R,T);
    for (unsigned int i=0; i<tcurve1.size(); ++i)
    {
      double min_dist=1e6;
      // int min_index=0;
      for (unsigned int j=0; j<curve2.size(); ++j)
      {
        double dist=vcl_sqrt((tcurve1[i].x()-curve2[j].x())*(tcurve1[i].x()-curve2[j].x())
                  +(tcurve1[i].y()-curve2[j].y())*(tcurve1[i].y()-curve2[j].y()));
        if (min_dist>dist)
        {
          min_dist=dist;
          // min_index=j;
        }
      }
      if (min_dist<1e6)
        cost+=min_dist;
    }
    cost/=get_best_match_prev()->mapping_.size();
    return cost;
  }
  else
    return -1;
}

double bdgl_tracker_curve ::compute_euclidean_distance_next(vnl_matrix<double> R,vnl_matrix<double> T,double /* s */)
{
  if (get_best_match_next())
  {
    double x1,y1,x2,y2;
    double dist=0;
    vcl_map<int,int> alignment= get_best_match_next()->mapping_;
    vcl_map<int,int>::iterator iter1;
    double H[2]={0,0};
    for (iter1 = alignment.begin(); iter1!=alignment.end(); iter1++)
    {
      H[0]=desc->points_[(*iter1).first].x();
      H[1]=desc->points_[(*iter1).first].y();
      vnl_matrix<double> X (H, 2, 1);
      vnl_matrix<double> Xt=R*X+T;

      x1=Xt(0,0);
      y1=Xt(1,0);

      x2=get_best_match_next()->match_curve_set[0]->desc->points_[(*iter1).second].x();
      y2=get_best_match_next()->match_curve_set[0]->desc->points_[(*iter1).second].y();

       dist+=vcl_sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
     }

    dist/=alignment.size();
    return dist;
  }
  else
    return -1.0;
}
