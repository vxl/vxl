//-----------------------------------------------------------------------------
//:
// \file
// \author P.L. Bazin
//-----------------------------------------------------------------------------


#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <bdgl/bdgl_curve_tracker.h>
#include <bdgl/bdgl_curve_matcher.h>
#include <bdgl/bdgl_curve_tracker_primitive.h>
#include <bdgl/bdgl_curve_region.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>

#if 0 // This whole file is commented out !!

//-----------------------------------------------------------------------------
void bdgl_curve_tracker::track()
{
  vcl_cout<<"-- testing batch tracking --\n";
  // for all images:
  for (unsigned int t=0;t<input_curve_.size();t++)
    track_frame(t);
  return;
}

//-----------------------------------------------------------------------------
void bdgl_curve_tracker::track_frame(unsigned int frame)
{
  bdgl_curve_region                           reg;
  vcl_vector< bdgl_curve_region >             regions;
  bdgl_curve_tracker_primitive                primitive;
  vcl_vector< bdgl_curve_tracker_primitive >  primitive_list;

  aspects.clear();

  //declaring two arrays
//int * first_id, * second_id;

#ifdef DEBUG
  vcl_cout<<"-- tracking frame "<<frame<<" --\n";
#endif

  if (input_curve_.size()<frame) return;
#ifdef DEBUG
  vcl_cout<<input_curve_[frame].size()<<" curves on image "<<frame<<'\n';
#endif

  // init : copy the first curves
  if (frame==0)
  {
    primitive_list.clear();
    for (unsigned int i=0;i<input_curve_[0].size();i++)
    {
      primitive.init(i, input_curve_[0][i]);
      primitive_list.insert(primitive_list.end(), primitive);
    }
    output_curve_.insert(output_curve_.end(), primitive_list);
  }
  else
  {
    // init : duplicate empty primitive lists
    primitive_list.clear();

#if 0 // is_used is not being used ;-)
    // init the used flag
    vcl_vector<int> is_used;
    is_used.clear();
    for (unsigned int j=0;j<input_curve_[frame].size();j++)
      is_used.insert(is_used.end(), 0);

    first_id= new int[output_curve_[frame-1].size()];
    second_id = new int[input_curve_[frame].size()];
#endif // 0

    // compute regions
#ifdef DEBUG
    vcl_cout<<"-> compute regions\n";
#endif

    regions.clear();
    for (unsigned int j=0;j<input_curve_[frame].size();j++)
    {
      reg.init(input_curve_[frame][j]);
//    input_curve_[frame][j]->compute_boundary_box();
      regions.insert(regions.end(), reg);
//    second_id[j]=0;
    }

#ifdef DEBUG
    // take every primitive and find the best related curve
    for (unsigned int i=0;i<output_curve_[frame-1].size();i++)
    {
//    int best_id = -1;
//    double best_val = 1e6;
      vcl_cout<<'.';
    }
#endif
  }
}


vcl_vector<vcl_map<int,double> > bdgl_curve_tracker::filter_top_ranks(vcl_vector<vcl_map<int,double> > &cost_table)
{
  vcl_vector<vcl_map<int,double> > top_ordered_curves;
  vcl_map<int,double> temp_list;
  vcl_map<int,double>::iterator iter;

  int min_cost_second_id=0;
  for (unsigned int i=0; i<cost_table.size(); ++i)
  {
    temp_list.clear();
    for (unsigned int j=0; j<5 && j<cost_table[i].size(); ++j)
    {
      double min_cost=1e6;
      for (iter=cost_table[i].begin();iter!=cost_table[i].end();iter++)
        if (min_cost>(*iter).second)
        {
          min_cost=(*iter).second;
          min_cost_second_id=(*iter).first;
        }
      //vcl_cout<<"\n the cost is "<<min_cost;
      temp_list[min_cost_second_id]=min_cost;
      cost_table[i].erase(min_cost_second_id);
    }

    top_ordered_curves.push_back(temp_list);
  }

  return top_ordered_curves;
}


// vj : date 28 april 2003  in haste of completing the assignment
//the idea is that each curve will track its parent and chil and
//it supports occlusion over 1 frame only.
// bdgl tracker feature

bdgl_tracking_feature::bdgl_tracking_feature(vdgl_digital_curve_sptr & edge)
{
  member_edge_=edge;
  //desc.init(member_edge_->get_interpolator()->get_edgel_chain());
}

int bdgl_tracking_feature::add_child(vdgl_digital_curve_sptr  c)
{
  child_.push_back(c);
  return 1;
}

int bdgl_tracking_feature::add_parent(vdgl_digital_curve_sptr p)
{
  parent_.push_back(p);
  return 1;
}

vcl_vector<vdgl_digital_curve_sptr> bdgl_tracking_feature::get_childern()
{
  return child_;
}

vcl_vector<vdgl_digital_curve_sptr> bdgl_tracking_feature::get_parents()
{
  return parent_;
}


#if 0 // rest of file commented out

//another class to implement things differently : bdgl_curve_match_tracker

void bdgl_curve_match_tracker::track_match()
{
  vcl_cout<<"-- testing batch tracking --\n";
  // for all images:
  for (unsigned int t=0;t<untracked_curve_.size();t++)
    track_match_frame(t);
  return;
}

void bdgl_curve_match_tracker::track_match_frame(unsigned int frame)
{
  vcl_vector< int >                           is_used;
  vcl_map<int,double>                         cost_map;
  vcl_vector< vcl_map< int,double> >          cost_table;
  vcl_vector< vcl_map< int,double> >          filtered_cost_table;
  bdgl_curve_matcher                          matcher(params_.match_params_);
  vcl_map<int,double>:: iterator              iter;
  int best_id;
  double best_val, dist;
  //declaring two arrays
  int * first_id, * second_id;
  vcl_vector<vdgl_digital_curve_sptr> * prev_curves;
  vcl_vector<bdgl_tracking_feature> init_features;

  //vcl_cout<<"-- tracking frame "<<frame<<" --\n";

  if (untracked_curve_.size()<frame) return;

  // init : copy the first curves
  if (frame==0)
  {
    init_features.clear();
    for (unsigned int i=0;i<untracked_curve_[0].size();i++)
    {
      untracked_curve_[0][i]->compute_bounding_box();
      bdgl_tracking_feature temp(untracked_curve_[0][i]);
      init_features.push_back(temp);
    }
    tracked_curve_.push_back(init_features);
  }
  else
  {
    // compute the bounding boxes of the current frame

    // assuming motion to be maximum of 5 pixels
    int motion=5;
    //find the neighbouring curves using bounding box intersection
    vcl_cout<<"\n the total number of curves are "<<untracked_curve_[frame].size();
    for (unsigned int i=0;i<tracked_curve_[frame-1].size();i++)
    {
      vsol_box_2d_sptr box1=untracked_curve_[frame-1][i]->get_bounding_box();
      int count=0;
      for (unsigned int j=0;j<untracked_curve_[frame].size();j++)
      {
        untracked_curve_[frame][j]->compute_bounding_box();
        vsol_box_2d_sptr box2=untracked_curve_[frame][j]->get_bounding_box();
        if ((box2->get_min_x()<=box1->get_max_x()+motion &&
             box2->get_max_x()>=box1->get_min_x()-motion) ||
            (box2->get_min_y()<=box1->get_max_y()+motion &&
             box2->get_max_y()>=box1->get_min_y()-motion))
        {
          count++;
          bdgl_curve_description desc1(untracked_curve_[frame][j]->get_interpolator()->get_edgel_chain());
          bdgl_curve_description desc2(tracked_curve_[frame-1][j].get_member_edge()->get_interpolator()->get_edgel_chain());

          double coarse_cost=coarse_match(desc1,desc2);
          //tracked_curve_[frame][j].add_child(vdgl_digital_curve_sptr candidate_curve,coarse_cost);
          vcl_cout<<"\n the lengths are "<< desc1.curvature_<<'\t'<<desc2.curvature_<<'\t'<<dist;
        }
      }
    }
  }
  for (unsigned int i=0;i<untracked_curve_[frame-1].size();i++)
  {
    best_id = -1;
    best_val = 1e6;

    cost_map.clear();
    first_id[i]=0;
//  no_of_candidates=0;
    // look for curves in the neighborhood
    for (unsigned int j=0;j<input_curve_[frame].size();j++)
    {
      // test for neighborhood
      dist = vcl_sqrt( (regions[j].x()- output_curve_[frame-1][i].region_.x())
                      *(regions[j].x()- output_curve_[frame-1][i].region_.x())
                     + (regions[j].y()- output_curve_[frame-1][i].region_.y())
                      *(regions[j].y()- output_curve_[frame-1][i].region_.y()) );

      if (dist < regions[j].r() + output_curve_[frame-1][i].region_.r() )
      {
#ifdef DEBUG
        vcl_cout<<"try ("<<i<<"->"<<j<<')';
#endif
        matcher.init(output_curve_[frame-1][i], input_curve_[frame][j]);
        matcher.match();
#ifdef DEBUG
        vcl_cout<<"->"<<matcher.score()<<'\n';
#endif
        // no handling of multiple curve matching : to do
        if (matcher.score() < best_val)
        {
          best_id = j;
          best_val = matcher.score();
        }
      }
    }
#ifdef DEBUG
    vcl_cout<<'('<<i<<"->"<<best_id<<")->"<<best_val<<'\n';
#endif

    //running dp match on filtered results
    for (unsigned int i=0;i<filtered_cost_table.size();i++)
    {
      first_id[i]=0;
      cost_map.clear();
      for (iter=filtered_cost_table[i].begin();iter!=filtered_cost_table[i].end();iter++)
      {
        //no_of_candidates++;
        matcher.init(output_curve_[frame-1][i], input_curve_[frame][(*iter).first]);
        matcher.match_DP();
        //matcher.match();
        cost_map[(*iter).first]=matcher.score();
      }
      cost_table.push_back(cost_map);
    }

    double min_cost=1e6;
    int min_cost_first_id=-1;
    int min_cost_second_id=-1;
    vcl_vector< vcl_pair < int,double> > mincost;
    vcl_vector<double> cost_vec;
    vcl_pair<int,double> temp;

    int cnt=1;
    while (cnt<cost_table.size())
    {
      min_cost=1e9;
      min_cost_first_id=-1;
      min_cost_second_id=-1;

      for (unsigned int i=0; i<cost_table.size(); ++i)
        if (first_id[i]==0)
          for (iter=cost_table[i].begin();iter!=cost_table[i].end();iter++)
            if (second_id[(*iter).first]==0)
              if (min_cost>(*iter).second)
              {
                min_cost=(*iter).second;
                min_cost_second_id=(*iter).first;
                min_cost_first_id=i;
              }
    }
    if (min_cost_first_id!=-1 || min_cost_second_id!=-1)
    {
      first_id[min_cost_first_id]=1;
      second_id[min_cost_second_id]=1;

      // fillin data to compute histogram;
      cost_vec.push_back(min_cost);

      if ( min_cost_second_id == -1)
      {
        // do nothing: the primitive is not continued
        output_curve_[frame-1][min_cost_first_id].next_num_ = -1;
      }
      else
      {
        // check if above a thresholded value
        if (min_cost > 50) // params_.match_thres_)
        {
          // do nothing: the primitive is not continued
          output_curve_[frame-1][min_cost_first_id].next_num_ = -1;
        }
        else
        {
          // if good, refine the primitive from it
          primitive.init(output_curve_[frame-1][min_cost_first_id].get_id(), input_curve_[frame][min_cost_second_id]);
          // update the ids
          output_curve_[frame-1][min_cost_first_id].next_num_ = primitive_list.size();
          primitive.prev_num_ = min_cost_first_id;
          primitive.next_num_ = -1;

          primitive_list.insert(primitive_list.end(), primitive);
          is_used[best_id]=1;
#ifdef DEBUG
          vcl_cout<<'.';
#endif
        }
      }
    }
#ifdef DEBUG
    vcl_cout<<"-unmatched primitives-\n";
#endif
    // take every curve left, and build a primitive for it
    for (unsigned int j=0;j<input_curve_[frame].size();j++)
    {
      if (!is_used[j])
      {
        primitive.init(primitive_list[primitive_list.size()-1].get_id()+1,
                       input_curve_[frame][j]);
        primitive_list.insert(primitive_list.end(), primitive);
      }
    }
    output_curve_.insert(output_curve_.end(), primitive_list);
    delete first_id;
    delete second_id;
  }

  return;
}

double bdgl_curve_match_tracker::coarse_match(bdgl_curve_description desc1,bdgl_curve_description desc2)
{
  double dist;
  int image_scale=1;
  double grad_scale=10;
  double angle_scale=0.31412;
  dist = 1.0*vcl_sqrt( vnl_math_sqr( (desc1.center_.x()-desc2.center_.x())/image_scale )
                      +vnl_math_sqr( (desc1.center_.y()-desc2.center_.y())/image_scale ));
  dist+= 0.5*vnl_math_abs( (desc1.length_-desc2.length_)/image_scale );
  dist+= 0.5*vnl_math_abs( (desc1.curvature_-desc2.curvature_)/image_scale );
  dist+= 1.0*vnl_math_abs( (desc1.gradient_mean_val_-desc2.gradient_mean_val_)/grad_scale );
  dist+= 1.0*vnl_math_abs( (desc1.gradient_mean_dir_-desc2.gradient_mean_dir_)/angle_scale );
  dist+= 1.0*vnl_math_abs( (desc1.gradient_std_val_-desc2.gradient_std_val_)/grad_scale );
  dist+= 1.0*vnl_math_abs( (desc1.gradient_std_dir_-desc2.gradient_std_dir_)/angle_scale );

  return dist;
}

#endif // 0
#endif // 0
