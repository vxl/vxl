#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include <bdgl/bdgl_curve_matching.h>
#include <bdgl/bdgl_tracker_curve_sptr.h>
#include <bdgl/bdgl_tracker_curve.h>

#include "curveMatch.h"

bdgl_curve_matching ::bdgl_curve_matching()
{
 motion_in_pixels_=0;
 no_of_top_choices_=0;
}

struct less_cost
{
  bool operator()(match_data_sptr x, match_data_sptr y)
  { return x->cost_ < y->cost_; }
};

void bdgl_curve_matching :: initialize_matrix(vcl_vector<bdgl_tracker_curve> new_curves,
                                              vcl_vector<bdgl_tracker_curve> old_curves)
{
  for (unsigned int i=0;i<new_curves.size();++i)
  {
    for (unsigned int j=0;j<old_curves.size();++j)
    {
      match_data_sptr temp;
      temp=new match_data;
      temp->cost_=-1;
      temp->euc_=-1;
      temp->match_curve_set.push_back(&old_curves[j]);
      new_curves[i].prev_.push_back(temp);
    }
  }
}


void bdgl_curve_matching :: match_prev_tail_curve(bdgl_tracker_curve_sptr parent_curve,
                                                  bdgl_tracker_curve_sptr &tail_curve,
                                                  vcl_vector<bdgl_tracker_curve_sptr> * new_curves)
{
  vnl_matrix<double> R1,T1,Tbar;
  double matching_cost;
  for (unsigned int i=0;i<(*new_curves).size();++i)
  {
    match_data_sptr temp;
    if (!(*new_curves)[i]->ismatchedprev_)
      if (bounding_box_intersection((*new_curves)[i]->desc->box_, parent_curve->desc->box_))
      {
        temp=new match_data;
        temp->cost_=0;
        temp->match_curve_set.push_back((*new_curves)[i]);
        tail_curve->next_.push_back(temp);
      }
  }
  for (unsigned int i=0;i<tail_curve->next_.size();++i)
  {
    double euc,scale;
    vcl_map<int,int> mapping;
    vcl_vector<int> tail1,tail2;

    matching_cost=match_DP(tail_curve->desc,
                           tail_curve->next_[i]->match_curve_set[0]->desc,
                           mapping,euc,R1,T1,Tbar,scale,tail1,tail2);

    tail_curve->next_[i]->cost_=euc;
    tail_curve->next_[i]->energy_=matching_cost;
    tail_curve->next_[i]->euc_=euc;
    tail_curve->next_[i]->mapping_=mapping;
    tail_curve->next_[i]->R_=R1;
    tail_curve->next_[i]->T_=T1;
    tail_curve->next_[i]->tail1_=tail1;
    tail_curve->next_[i]->tail2_=tail2;
  }
  vcl_sort(tail_curve->next_.begin(),tail_curve->next_.end(),less_cost());
}

void bdgl_curve_matching :: match_next_tail_curve(bdgl_tracker_curve_sptr parent_curve,
                                                  bdgl_tracker_curve_sptr &tail_curve,
                                                  vcl_vector<bdgl_tracker_curve_sptr> * old_curves)
{
  vnl_matrix<double> R1,T1,Tbar;
  double matching_cost;
  for (unsigned int i=0;i<(*old_curves).size();++i)
  {
    match_data_sptr temp;
    if (!(*old_curves)[i]->ismatchednext_)
      if (bounding_box_intersection((*old_curves)[i]->desc->box_, parent_curve->desc->box_))
      {
        temp=new match_data;
        temp->cost_=0;
        temp->match_curve_set.push_back((*old_curves)[i]);
        tail_curve->prev_.push_back(temp);
      }
  }
  for (unsigned int i=0;i<tail_curve->prev_.size();++i)
  {
    double euc,scale=1.0;
    vcl_map<int,int> mapping;
    vcl_vector<int> tail1,tail2;

    euc=match_DP(tail_curve->prev_[i]->match_curve_set[0]->desc,
                 tail_curve->desc,mapping,matching_cost,R1,T1,Tbar,scale,tail1,tail2);

    tail_curve->prev_[i]->cost_=euc;
    tail_curve->prev_[i]->euc_=euc;
    tail_curve->prev_[i]->R_=R1;
    tail_curve->prev_[i]->T_=T1;
    tail_curve->prev_[i]->tail1_=tail1;
    tail_curve->prev_[i]->tail2_=tail2;
    tail_curve->prev_[i]->mapping_=mapping;
    tail_curve->prev_[i]->energy_=matching_cost;
  }
  vcl_sort(tail_curve->prev_.begin(),tail_curve->prev_.end(),less_cost());
}

double bdgl_curve_matching :: coarse_match_DP(bdgl_curve_description * desc1,
                                              bdgl_curve_description * desc2)
{
  vcl_pair<double,double> coordinate;
  vcl_vector<vcl_pair<double,double> > v1,v2;

  double dist;
  double dist1;
  double dist2;

  double euc1;
  double euc2;
  double scale;

  vnl_matrix<double> R1,T1,Tbar1;
  vnl_matrix<double> R2,T2,Tbar2;
  vcl_vector<int> tail_old1;
  vcl_vector<int> tail_old2;
  vcl_vector<int> tail_new1;
  vcl_vector<int> tail_new2;

  vcl_map<int,int> alignment1;
  vcl_map<int,int> alignment2;

  vcl_map<int,int>::iterator iter;
  for (unsigned int i=0;i<desc1->points_.size(); i+=5)
    {
      coordinate.first=desc1->points_[i].x();
      coordinate.second=desc1->points_[i].y();
      v1.push_back(coordinate);
    }
  for (unsigned int i=0;i<desc2->points_.size(); i+=5)
    {
      coordinate.first=desc2->points_[i].x();
      coordinate.second=desc2->points_[i].y();
      v2.push_back(coordinate);
    }
  dist1 = curveMatch(euc1,v1,v2,alignment1,R1,T1,Tbar1,tail_old1,tail_new1,scale);
  vcl_reverse(v2.begin(),v2.end());
  dist2 = curveMatch(euc2,v1,v2,alignment2,R2,T2,Tbar2,tail_old2,tail_new2,scale);
  if (dist1<dist2)
    dist=dist1;
  else
    dist=dist2;
  return dist;
}

double bdgl_curve_matching :: match_DP(bdgl_curve_description * desc1,
                                       bdgl_curve_description * desc2,vcl_map<int,int> &alignment,
                                       double & cost, vnl_matrix<double> &R,vnl_matrix<double> &T,
                                       vnl_matrix<double> & Tbar,double & scale,vcl_vector<int> & tail1,
                                       vcl_vector<int> & tail2)
{
  double dist;
  double dist1;
  double dist2;
  double euc1;
  double euc2;
  double euc;

  double scale1,scale2;
  vnl_matrix<double> R1,T1,Tbar1;
  vnl_matrix<double> R2,T2,Tbar2;

  vcl_vector<int> tail_old1;
  vcl_vector<int> tail_old2;

  vcl_vector<int> tail_new1;
  vcl_vector<int> tail_new2;

  vcl_pair<double,double> coordinate;
  vcl_vector<vcl_pair<double,double> > v1,v2;
  vcl_map<int,int>::iterator iter;

  // getting points from curves into vectors
  for (unsigned int i=0;i<desc1->points_.size();++i)
  {
    coordinate.first=desc1->points_[i].x();
    coordinate.second=desc1->points_[i].y();
    v1.push_back(coordinate);
  }
  for (unsigned int i=0;i<desc2->points_.size();++i)
    {
      coordinate.first=desc2->points_[i].x();
      coordinate.second=desc2->points_[i].y();
      v2.push_back(coordinate);
    }
  double dx1,dx2,dy1,dy2,theta1,theta2;
  double sign11,sign12,sign21,sign22;
  vcl_map<int,int> alignment1;
  vcl_map<int,int> alignment2;
  vcl_map<int,int> alignment3;
  vcl_map<int,int> alignment4;

  if (v1.size()>3)
  {
    dx1=v1[1].first-v1[0].first;
    dy1=v1[1].second-v1[0].second;
    theta1=desc1->angles_[0]*vnl_math::pi/180;
    sign11=dx1*vcl_sin(theta1)-dy1*vcl_cos(theta1);

    dx2=v1[v1.size()-2].first-v1[v1.size()-1].first;
    dy2=v1[v1.size()-2].second-v1[v1.size()-1].second;
    theta2=desc1->angles_[v1.size()-1]*vnl_math::pi/180;
    sign12=dx2*vcl_sin(theta2)-dy2*vcl_cos(theta2);
  }
  if (v2.size()>3)
  {
    dx1=v2[1].first-v2[0].first;
    dy1=v2[1].second-v2[0].second;
    theta1=desc2->angles_[0]*vnl_math::pi/180;
    sign21=dx1*vcl_sin(theta1)-dy1*vcl_cos(theta1);

    dx2=v2[v2.size()-2].first-v2[v2.size()-1].first;
    dy2=v2[v2.size()-2].second-v2[v2.size()-1].second;
    theta2=desc2->angles_[v2.size()-1]*vnl_math::pi/180;
    sign22=dx2*vcl_sin(theta2)-dy2*vcl_cos(theta2);
    //assert(sign22*sign21<=0);
  }
  if (sign11*sign12<0 && sign21*sign22<0)
  {
    if (sign11*sign21>0)
    {
      dist = curveMatch(euc,v1,v2,alignment,R,T,Tbar,tail1,tail2,scale);
    }
    if (sign11*sign22>0)
    {
      vcl_reverse(v2.begin(),v2.end());
      dist = curveMatch(euc,v1,v2,alignment2,R,T,Tbar,tail1,tail2,scale);
      unsigned int sizeofv2=v2.size();
      for (iter=alignment2.begin();iter!=alignment2.end();iter++)
      {
        alignment4[(*iter).first]=sizeofv2-(*iter).second-1;
      }
      alignment=alignment4;
    }
  }
  else
  {
    dist1 = curveMatch(euc1,v1,v2,alignment1,R1,T1,Tbar1,tail_old1,tail_new1,scale1);
    vcl_reverse(v2.begin(),v2.end());
    dist2 = curveMatch(euc2,v1,v2,alignment2,R2,T2,Tbar2,tail_old2,tail_new2,scale2);

    if (dist1<dist2)
    {
      alignment=alignment1;
      euc=euc1;
      dist=dist1;
      R=R1;
      T=T1;
      Tbar=Tbar1;
      tail1=tail_old1;
      tail2=tail_new1;
      scale=scale1;
    }
    else
    {
      euc=euc2;
      dist=dist2;
      unsigned int sizeofv2=v2.size();
      for (iter=alignment2.begin();iter!=alignment2.end();iter++)
      {
        alignment4[(*iter).first]=sizeofv2-(*iter).second-1;
      }
      alignment=alignment4;
      R=R2;
      T=T2;
      Tbar=Tbar2;
      tail1=tail_old2;
      tail2=tail_new2;
      scale=scale2;
    }
  }
  cost=euc;
  vcl_cout<<".";
  return dist;
}

double bdgl_curve_matching :: match_stat(bdgl_curve_description * desc1,
                                         bdgl_curve_description  * desc2)
{
  double dist=0;
  double image_scale=1;
  double angle_scale=.31416;
  double grad_scale=10.0;

  dist = 1.0*vcl_sqrt( vnl_math_sqr( (desc1->center_.x()-desc2->center_.x())/image_scale )
                     +vnl_math_sqr( (desc1->center_.y()-desc2->center_.y())/image_scale ) );
  dist+= 0.5*vnl_math_abs( (desc1->length_-desc2->length_)/image_scale );
  dist+= 0.5*vnl_math_abs( (desc1->curvature_-desc2->curvature_)/image_scale );

  dist+= 1.0*vnl_math_abs( (desc1->gradient_mean_val_-desc2->gradient_mean_val_)/grad_scale);
  dist+= 1.0*vnl_math_abs( (desc1->gradient_mean_dir_-desc2->gradient_mean_dir_)/angle_scale );
  dist+= 1.0*vnl_math_abs( (desc1->gradient_std_val_-desc2->gradient_std_val_)/grad_scale);
  dist+= 1.0*vnl_math_abs( (desc1->gradient_std_dir_-desc2->gradient_std_dir_)/angle_scale );

  return dist;
}

bool bdgl_curve_matching :: bounding_box_intersection(vsol_box_2d_sptr box1,
                                                      vsol_box_2d_sptr box2)
{
  return box2->get_min_x()<=(box1->get_max_x()+motion_in_pixels_) &&
         box2->get_max_x()>=(box1->get_min_x()-motion_in_pixels_) &&
         box2->get_min_y()<=(box1->get_max_y()+motion_in_pixels_) &&
         box2->get_max_y()>=(box1->get_min_y()-motion_in_pixels_);
}

void bdgl_curve_matching :: match(vcl_vector<bdgl_tracker_curve_sptr> * new_curves,
                                  vcl_vector<bdgl_tracker_curve_sptr> * old_curves)
{
  double matching_cost;
  vcl_vector<bdgl_tracker_curve_sptr> merged_new_curves;

  for (unsigned int i=0;i<(*new_curves).size();++i)
  {
    for (unsigned int j=0;j<(*old_curves).size();++j)
    {
      if (bounding_box_intersection((*new_curves)[i]->desc->box_,(*old_curves)[j]->desc->box_))
      {
        matching_cost=coarse_match_DP((*new_curves)[i]->desc,(*old_curves)[j]->desc);
        match_data_sptr temp=new match_data;
        temp->cost_=matching_cost;
        temp->match_curve_set.push_back((*old_curves)[j]);
        (*new_curves)[i]->prev_.push_back(temp);
      }
      (*old_curves)[j]->next_.clear();
    }

    // sorting the matches w.r.t elastic energy cost
    vcl_sort((*new_curves)[i]->prev_.begin(),(*new_curves)[i]->prev_.end(),less_cost());
    if (no_of_top_choices_>0)
    {
      vcl_vector<match_data_sptr>::iterator piter=(*new_curves)[i]->prev_.begin();
      if ((*new_curves)[i]->prev_.size()>=(unsigned int)no_of_top_choices_)
      {
        piter+=no_of_top_choices_;
        (*new_curves)[i]->prev_.erase(piter,(*new_curves)[i]->prev_.end());
      }
    }
  }

  vnl_matrix<double> R1,T1,Tbar;

  for (unsigned int i=0;i<(*new_curves).size();++i)
  {
    double euc,scale;
    vcl_map<int,int> mapping;
    vcl_vector<int> tail_old;
    vcl_vector<int> tail_new;

    if ((*new_curves)[i]->prev_.size()> 0)
    {
      vcl_vector<match_data_sptr>::iterator piter=(*new_curves)[i]->prev_.begin();
      for (unsigned int j=0;j<(*new_curves)[i]->prev_.size(); ++j,++piter)
      {
        matching_cost=match_DP((*new_curves)[i]->prev_[j]->match_curve_set[0]->desc,
                               (*new_curves)[i]->desc,mapping,euc,R1,T1,Tbar,scale,tail_old,tail_new);

        (*new_curves)[i]->prev_[j]->energy_=matching_cost;
        (*new_curves)[i]->prev_[j]->cost_=matching_cost;
        (*new_curves)[i]->prev_[j]->euc_=euc;
        (*new_curves)[i]->prev_[j]->mapping_=mapping;
        (*new_curves)[i]->prev_[j]->R_=R1;
        (*new_curves)[i]->prev_[j]->T_=T1;
        (*new_curves)[i]->prev_[j]->Tbar=Tbar;
        (*new_curves)[i]->prev_[j]->tail1_=tail_old;
        (*new_curves)[i]->prev_[j]->tail2_=tail_new;
        (*new_curves)[i]->prev_[j]->scale_=scale;

        match_data_sptr temp=new match_data;
        temp->euc_=euc;
        temp->cost_=matching_cost;
        temp->energy_=matching_cost;
        temp->mapping_=mapping;
        temp->R_=R1;
        temp->T_=T1;
        temp->Tbar=Tbar;
        temp->scale_=scale;
        temp->match_curve_set.push_back((*new_curves)[i]);

        //insering the next match information
        (*piter)->match_curve_set[0]->next_.push_back(temp);
      }

    vcl_sort((*new_curves)[i]->prev_.begin(),(*new_curves)[i]->prev_.end(),less_cost());
    }
  }
  // sorting the next matches of the curves
  for (unsigned int i=0;i<(*old_curves).size();++i)
  {
    vcl_sort((*old_curves)[i]->next_.begin(),(*old_curves)[i]->next_.end(),less_cost());
  }
  // computing the first best match
  best_matches(new_curves,old_curves);
}

void bdgl_curve_matching::best_matches(vcl_vector<bdgl_tracker_curve_sptr> * new_curves,
                                       vcl_vector<bdgl_tracker_curve_sptr> * old_curves)
{
  vcl_vector<match_data_sptr>::iterator minpiter;
  vcl_vector<bdgl_tracker_curve_sptr>::iterator iter_new;
  vcl_vector<match_data_sptr>::iterator iter_old;
  // simply choosing best corresponding matches
  // giving prefernce to low cost matches
  bool selection=true;
  while (selection)
  {
    double min_cost=1e6;
    vcl_vector<bdgl_tracker_curve_sptr>::iterator min_iter_new;
    bdgl_tracker_curve_sptr min_iter_old;
    for (iter_new=(*new_curves).begin();iter_new!=(*new_curves).end();iter_new++)
    {
      if (!(*iter_new)->ismatchedprev_)
        for (iter_old=(*iter_new)->prev_.begin();iter_old!=(*iter_new)->prev_.end();iter_old++)
        {
          if (!(*iter_old)->match_curve_set[0]->ismatchednext_)
            if (min_cost>(*iter_old)->cost_)
            {
              min_cost=(*iter_old)->cost_;
              min_iter_new=iter_new;
              min_iter_old=(*iter_old)->match_curve_set[0];
              minpiter=iter_old;
            }
          }
        }

        if (min_iter_new == (*new_curves).end() || ! ((bool)min_iter_old) )
        {
          selection=false;
        }
        else
        {
          // check if the tail exists
          selection=false;
          // tail in the previous frame

          double s;
          if ((*minpiter)->tail1_.size()>0 && (*minpiter)->tail1_.size()>(*minpiter)->tail2_.size())
          {
            vcl_vector<vgl_point_2d<double> > points;
            for (unsigned int k=0;k<(*minpiter)->tail1_.size();++k)
            {
              vgl_point_2d<double> point;
              point.set((*minpiter)->match_curve_set[0]->desc->points_[(*minpiter)->tail1_[k]].x(),
                        (*minpiter)->match_curve_set[0]->desc->points_[(*minpiter)->tail1_[k]].y());

              points.push_back(point);
            }

            match_data_sptr  best_match_for_tail;
            bdgl_tracker_curve_sptr  prev_tail_curve=new bdgl_tracker_curve;
            prev_tail_curve->init_set(points,0);
            vnl_matrix<double> R1,T1;
            // matching the tail
            match_prev_tail_curve((*minpiter)->match_curve_set[0], prev_tail_curve,new_curves);
            if (prev_tail_curve->next_[0]->match_curve_set[0]==(*min_iter_new))
            {
              if (prev_tail_curve->next_.size()>1)
                best_match_for_tail=prev_tail_curve->next_[1];
              else
                best_match_for_tail=NULL;
            }
            else
            {
              best_match_for_tail=prev_tail_curve->next_[0];
            }
            // check the best match of the tail
            if (best_match_for_tail)
              if (best_match_for_tail->match_curve_set[0]->prev_.size()>0 )
                if (best_match_for_tail->cost_ <
                    best_match_for_tail->match_curve_set[0]->prev_[0]->cost_)
                {
                  R1=(*minpiter)->R_;
                  T1=(*minpiter)->T_;
                  s=(*minpiter)->scale_;
                  prev_tail_curve->set_best_match_next(best_match_for_tail);


                  double tail_euc_dist=prev_tail_curve->compute_euclidean_distance_next(R1,T1,s);
                  // confirm it using transformation
                  R1=best_match_for_tail->R_;
                  T1=best_match_for_tail->T_;
                  s=best_match_for_tail->scale_;
                  // to compute the euc dist
                  (*min_iter_new)->set_best_match_prev((*minpiter));

                  double parent_euc_dist=(*min_iter_new)->compute_euclidean_distance(R1,T1,s);
                  (*min_iter_new)->set_best_match_prev(0);
                  // join the two curves and add it to the new curves;
                  if (vcl_fabs(2*(tail_euc_dist-parent_euc_dist)/(tail_euc_dist+parent_euc_dist))<1.0
                     && (tail_euc_dist+parent_euc_dist)/2<10)
                  {
                    //merged two curves to form a virtual curve
                    bdgl_tracker_curve_sptr c=new bdgl_tracker_curve;
                    merge_curves((*min_iter_new),best_match_for_tail->match_curve_set[0],c);
                    c->set_id((*new_curves).size());


                    // fill in the values of best_match_prev
                    match_data_sptr temp= new match_data;
                    temp->match_curve_set.push_back((*minpiter)->match_curve_set[0]);

                    double cst=match_DP(c->desc,temp->match_curve_set[0]->desc,
                                        temp->mapping_, temp->euc_,temp->R_,temp->T_,
                                        temp->Tbar,temp->scale_,temp->tail1_,temp->tail2_);

                    temp->energy_=cst;
                    temp->cost_=cst;
                    best_match_for_tail->match_curve_set[0]->set_best_match_prev(temp);
                    (*min_iter_new)->set_best_match_prev(temp);

                    // fill the values for best match next
                    match_data_sptr temp1= new match_data;
                    temp1->match_curve_set.push_back( (*min_iter_new));
                    temp1->match_curve_set.push_back( best_match_for_tail->match_curve_set[0]);

                    temp1->curve_set=c;
                    temp1->mapping_=temp->mapping_;
                    temp1->R_=temp->R_;
                    temp1->T_=temp->T_;
                    temp1->Tbar=temp->Tbar;
                    temp1->cost_=temp->cost_;
                    temp1->energy_=temp->energy_;
                    temp1->tail1_=temp->tail1_;
                    temp1->tail2_=temp->tail2_;
                    temp1->euc_=temp->euc_;
                    temp1->scale_=temp->scale_;
                    (*minpiter)->match_curve_set[0]->set_best_match_next(temp);

                     // set the match_ids
                    best_match_for_tail->match_curve_set[0]->match_id_=min_iter_old->match_id_;
                    (*min_iter_new)->match_id_=min_iter_old->match_id_;

                    best_match_for_tail->match_curve_set[0]->ismatchedprev_=true;
                    (*min_iter_new)->ismatchedprev_=true;
                    c->ismatchedprev_=true;
                    c->ismatchednext_=true;
                    c->isreal_=false;
                    min_iter_old->ismatchednext_=true;

                    selection=true;
                    new_curves->push_back(c);
                  }
                }
          }

          // tail in the current frame
          else if ((*minpiter)->tail2_.size()>0
                    && (*minpiter)->tail2_.size()>(*minpiter)->tail1_.size())
          {
            vcl_vector<vgl_point_2d<double> > points;
            for (unsigned int k=0;k<(*minpiter)->tail2_.size();++k)
            {
              vgl_point_2d<double> point;
              point.set((*min_iter_new)->desc->points_[(*minpiter)->tail2_[k]].x(),
                        (*min_iter_new)->desc->points_[(*minpiter)->tail2_[k]].y());
              points.push_back(point);
            }

            bdgl_tracker_curve_sptr  next_tail_curve = new bdgl_tracker_curve;
            next_tail_curve->init_set(points,0);

            vnl_matrix<double> R2,T2,Tbar2;
            // matching the tail
            match_next_tail_curve((*min_iter_new),next_tail_curve,old_curves);
            match_data_sptr best_match_for_tail=0;
            if (next_tail_curve->prev_[0]->match_curve_set[0]==min_iter_old)
            {
              if (next_tail_curve->prev_.size()>1)
                best_match_for_tail=next_tail_curve->prev_[1];
              else
                best_match_for_tail=NULL;
            }
            else
              best_match_for_tail=next_tail_curve->prev_[0];

            if (best_match_for_tail)
              if (best_match_for_tail->match_curve_set[0]->next_.size()>0 )
                if (best_match_for_tail->cost_   <
                    best_match_for_tail->match_curve_set[0]->next_[0]->cost_)
                {
                  R2=(*minpiter)->R_;
                  T2=(*minpiter)->T_;
                  Tbar2=(*minpiter)->Tbar;
                  s=(*minpiter)->scale_;
                  next_tail_curve->set_best_match_prev(best_match_for_tail);
                  double tail_euc_dist=next_tail_curve->compute_euclidean_distance(R2,T2,s);

                  //computing parent eulidean distance
                  R2=best_match_for_tail->R_;
                  T2=best_match_for_tail->T_;
                  s=best_match_for_tail->scale_;
                  (*min_iter_new)->set_best_match_prev((*minpiter));

                  double parent_euc_dist=(*min_iter_new)->compute_euclidean_distance(R2,T2,s);
                  (*min_iter_new)->set_best_match_prev(0);

                  // join the two curves and add it to the new curves;
                  if (vcl_fabs(2*(tail_euc_dist-parent_euc_dist)/(tail_euc_dist+parent_euc_dist))<1.0
                     && (tail_euc_dist+parent_euc_dist)/2<10)
                  {
                    //merged two curves to form a virtual curve
                    bdgl_tracker_curve_sptr c=new bdgl_tracker_curve;
                    merge_curves((*minpiter)->match_curve_set[0],
                                 best_match_for_tail->match_curve_set[0],c);
                    c->set_id((*old_curves).size());
                    c->isreal_=false;

                    match_data_sptr temp= new match_data;
                    temp->match_curve_set.push_back(*min_iter_new);
                    double cst=match_DP(c->desc,temp->match_curve_set[0]->desc,
                                        temp->mapping_,temp->euc_,temp->R_,temp->T_,temp->Tbar,
                                        temp->scale_,temp->tail1_,temp->tail2_);
                    temp->cost_=cst;
                    temp->energy_=cst;
                    // fill in the values of best_match_next
                    (*minpiter)->match_curve_set[0]->set_best_match_next(temp);
                    best_match_for_tail->match_curve_set[0]->set_best_match_next(temp);

                    // fill in the values of the best_match_prev
                    match_data_sptr temp1= new match_data;
                    temp1->match_curve_set.push_back((*minpiter)->match_curve_set[0]);
                    temp1->match_curve_set.push_back(best_match_for_tail->match_curve_set[0]);
                    temp1->curve_set=c;
                    temp1->mapping_=temp->mapping_;
                    temp1->energy_=temp->energy_;
                    temp1->cost_=temp->cost_;
                    temp1->R_=temp->R_;
                    temp1->T_=temp->T_;
                    temp1->Tbar=temp->Tbar;
                    temp1->tail1_=temp->tail1_;
                    temp1->tail2_=temp->tail2_;
                    temp1->euc_=temp->euc_;
                    temp1->scale_=temp->scale_;
                    (*min_iter_new)->set_best_match_prev(temp1);

                    //set_the_match_ids
                    c->ismatchedprev_=true;
                    c->ismatchednext_=true;
                    (*min_iter_new)->ismatchedprev_=true;
                    (*minpiter)->match_curve_set[0]->ismatchednext_=true;
                    best_match_for_tail->match_curve_set[0]->ismatchednext_=true;
                    (*min_iter_new)->match_id_=temp1->match_curve_set[1]->match_id_;

                    selection=true;
                  }
                }
          }
          // add the curve
          if (!selection)
          {
            (*min_iter_new)->ismatchedprev_=true;
            min_iter_old->ismatchednext_=true;
            //previous_id[min_cost_previous_id]=1;
            selection=true;
            if (min_cost >= 1e6)
            {
              (*min_iter_new)->match_id_ = -1;
            }
            else
            {
              if (min_iter_old->match_id_>-1)
                (*min_iter_new)->match_id_ = min_iter_old->match_id_;
              else
              {
                min_iter_old->match_id_=min_iter_old->get_id();
                (*min_iter_new)->match_id_=min_iter_old->match_id_;
              }

              (*min_iter_new)->set_best_match_prev((*minpiter));

              match_data_sptr temp=new match_data;

              temp->match_curve_set.push_back((*min_iter_new));
              temp->cost_=(*minpiter)->cost_;
              temp->mapping_=(*minpiter)->mapping_;
              temp->euc_=(*minpiter)->euc_;
              temp->scale_=(*minpiter)->scale_;
              temp->R_=(*minpiter)->R_;
              temp->T_=(*minpiter)->T_;
              temp->Tbar=(*minpiter)->Tbar;

              min_iter_old->set_best_match_next(temp);
            }
          }
        }
  }

  for (unsigned int i=0;i<(*new_curves).size();++i)
  {
    if (!(*new_curves)[i]->ismatchedprev_)
    {
      (*new_curves)[i]->match_id_ =-1;
      (*new_curves)[i]->group_id_ =-1;
    }
  }
}


double bdgl_curve_matching::compute_euc_dist(bdgl_tracker_curve_sptr a,
                                             bdgl_tracker_curve_sptr b)
{
  vnl_matrix<double> Ri,Ti;
  double x2,y2;
  double cost=0;
  vcl_map<int,int>::iterator iter;
  for (iter=a->get_best_match_prev()->mapping_.begin();
       iter!=a->get_best_match_prev()->mapping_.end();
       iter++)
  {
    double point[2];
    point[0]=a->get_best_match_prev()->match_curve_set[0]->desc->points_[(*iter).first].x();
    point[1]=a->get_best_match_prev()->match_curve_set[0]->desc->points_[(*iter).first].y();
    Ri=b->get_best_match_prev()->R_;
    Ti=b->get_best_match_prev()->T_;
    vnl_matrix<double> P (point, 2, 1);
    vnl_matrix<double> Q = Ri*P + Ti;

    x2=a->desc->points_[(*iter).second].x();
    y2=a->desc->points_[(*iter).second].y();

    cost+=vcl_sqrt((Q(0,0)-x2)*(Q(0,0)-x2)+(Q(1,0)-y2)*(Q(1,0)-y2));
  }
  cost/=a->get_best_match_prev()->mapping_.size();
  return cost;
}

void bdgl_curve_matching::merge_curves(bdgl_tracker_curve_sptr cs1,
                                       bdgl_tracker_curve_sptr cs2,
                                       bdgl_tracker_curve_sptr &outcs)
{
  outcs=new bdgl_tracker_curve;
  outcs->set_curve(cs1->get_curve_set());
  outcs->set_curve(cs2->get_curve_set());
  int id = cs1->get_id();

  unsigned int s1=cs1->desc->points_.size();
  unsigned int s2=cs2->desc->points_.size();

  vcl_vector<vgl_point_2d<double> > f;
  vcl_vector<vgl_point_2d<double> > s;

  for (unsigned int i=0;i<s1;++i)
    f.push_back(cs1->desc->points_[i]);

  for (unsigned int i=0;i<s2;++i)
    s.push_back(cs2->desc->points_[i]);

  double x11=cs1->desc->points_[0].x();
  double y11=cs1->desc->points_[0].y();
  double x12=cs1->desc->points_[s1-1].x();
  double y12=cs1->desc->points_[s1-1].y();

  double x21=cs2->desc->points_[0].x();
  double y21=cs2->desc->points_[0].y();
  double x22=cs2->desc->points_[s2-1].x();
  double y22=cs2->desc->points_[s2-1].y();

  vcl_pair<int,int> p;

  vcl_map<double,vcl_pair<int,int> > dist;

  p.first=0;p.second=0;
  double e11=vcl_sqrt((x11-x21)*(x11-x21)+(y11-y21)*(y11-y21));
  dist[e11]=p;

  p.first=0;p.second=s2-1;
  double e12=vcl_sqrt((x11-x22)*(x11-x22)+(y11-y22)*(y11-y22));
  dist[e12]=p;

  p.first=s1-1;p.second=s2-1;
  double e22=vcl_sqrt((x12-x22)*(x12-x22)+(y12-y22)*(y12-y22));
  dist[e22]=p;

  p.first=s1-1;p.second=0;
  double e21=vcl_sqrt((x12-x21)*(x12-x21)+(y12-y21)*(y12-y21));
  dist[e21]=p;


  p=((*dist.begin()).second);
  if (p.first>0 )
  {
    if (p.second>0)
      {
        vcl_reverse(s.begin(),s.end());
        f.insert(f.end(),s.begin(),s.end());
      }
    else
      {
        f.insert(f.end(),s.begin(),s.end());
      }
  }
  else
  {
    if (p.second>0)
    {
      f.insert(f.begin(),s.begin(),s.end());
    }
    else
    {
      vcl_reverse(s.begin(),s.end());
      f.insert(f.begin(),s.begin(),s.end());
    }
  }
  outcs->init_set(f,id);
}


double bdgl_curve_matching::compute_mean(vcl_vector<double> t)
{
  double sum=0;
  for (unsigned int i=0;i<t.size();++i)
    sum+=t[i];
  return sum/=t.size();
}

double bdgl_curve_matching::compute_std(vcl_vector<double> t)
{
  double sum=compute_mean(t);
  double std=0;
  for (unsigned int i=0;i<t.size();++i)
    std+=(t[i]-sum)*(t[i]-sum);
  return vcl_sqrt(std/t.size());
}
