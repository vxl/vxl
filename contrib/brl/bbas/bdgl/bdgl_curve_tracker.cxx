//-----------------------------------------------------------------------------
//:
// \file
// \author P.L. Bazin
//-----------------------------------------------------------------------------


#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <bdgl/bdgl_curve_tracker.h>

#include <bdgl/bdgl_curve_matcher.h>
#include <bdgl/bdgl_curve_tracker_primitive.h>
#include <bdgl/bdgl_curve_region.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vcl_iostream.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>

#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_edgel_chain.h>


#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>

/*
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
<<<<<<< bdgl_curve_tracker.cxx

=======
>>>>>>> 1.19
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

    // take every primitive and find the best related curve
    for (unsigned int i=0;i<output_curve_[frame-1].size();i++)
    {
<<<<<<< bdgl_curve_tracker.cxx
      best_id = -1;
      best_val = 1e6;
     
	  cost_map.clear();
	  first_id[i]=0;
	  //no_of_candidates=0;
      // look for curves in the neighborhood
      for (unsigned int j=0;j<input_curve_[frame].size();j++){
        // test for neighborhood
        dist = vcl_sqrt( (regions[j].x()- output_curve_[frame-1][i].region_.x())
                        *(regions[j].x()- output_curve_[frame-1][i].region_.x())
                       + (regions[j].y()- output_curve_[frame-1][i].region_.y())
                        *(regions[j].y()- output_curve_[frame-1][i].region_.y()) );
		
        if (dist < regions[j].r() + output_curve_[frame-1][i].region_.r() ){
	  
          matcher.init(output_curve_[frame-1][i], input_curve_[frame][j]);
		  matcher.match();    
		  cost_map[j]=matcher.score();
          // no handling of multiple curve matching : to do
          if (matcher.score() < best_val){
            best_id = j;
            best_val = matcher.score();
          }
        }
      }
	 cost_table.push_back(cost_map);
	}
  filtered_cost_table.clear();
  filtered_cost_table=filter_top_ranks(cost_table);
  
  cost_table.clear();

 //running dp match on filtered results

 map_of_T.clear();
 store_matches.clear();
 

 for (unsigned int i=0;i<filtered_cost_table.size();i++)
    {
	  first_id[i]=0;
	  cost_map.clear();
      for (iter=filtered_cost_table[i].begin();iter!=filtered_cost_table[i].end();iter++){
 	
          matcher.init(output_curve_[frame-1][i], input_curve_[frame][(*iter).first]);
	      matcher.match_DP();
		  temp.first=i;
		  temp.second=(*iter).first;
//		  map_of_T[temp].push_back(matcher.Tx_);
//		  map_of_T[temp].push_back(matcher.Ty_);
//		  map_of_T[temp].push_back(matcher.Rtheta_);
          cost_map[(*iter).first]=matcher.score();
	  }
	  
	 cost_table.push_back(cost_map);
	}

 // to output top 5 ranks
 filtered_cost_table.clear();
 filtered_cost_table=filter_top_ranks(cost_table);

 for (unsigned int i=3;i<4;i++)
    {
	  first_id[i]=0;
	  cost_map.clear();
      for (iter=filtered_cost_table[i].begin();iter!=filtered_cost_table[i].end();iter++){
 	
		  primitive.init(output_curve_[frame-1][i].get_id(), input_curve_[frame][(*iter).first]);
		  primitive.prev_num_ = i;
          primitive.next_num_ = -1;
	      temp_output_curve_.insert(temp_output_curve_.end(), primitive); 
	  }
	}
=======
//    int best_id = -1;
//    double best_val = 1e6;
#ifdef DEBUG
      vcl_cout<<'.';
#endif
    }
  }
}
>>>>>>> 1.19


<<<<<<< bdgl_curve_tracker.cxx
 
  
=======
vcl_vector<vcl_map<int,double> > bdgl_curve_tracker::filter_top_ranks(vcl_vector<vcl_map<int,double> > &cost_table)
{
  vcl_vector<vcl_map<int,double> > top_ordered_curves;
  vcl_map<int,double> temp_list;
  vcl_map<int,double>::iterator iter;
>>>>>>> 1.19

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

<<<<<<< bdgl_curve_tracker.cxx
    output_curve_.insert(output_curve_.end(), primitive_list);

	delete(first_id);
	delete(second_id);

	segment_curves(frame);
		
 }
  
  return;
}

void bdgl_curve_tracker::segment_curves(int frame)
{
	vcl_map<int,int>::iterator iter1;
	vcl_map<int,int>::iterator iter2;
	vcl_vector<vcl_vector<int> >::iterator it_aspects1;
    vcl_vector<vcl_vector<int> >::iterator it_aspects2;
	int no_of_iterations=0;
	
	double tx1;
	double tx2;
	double ty1;
	double ty2;
	double Rtheta1;
	double Rtheta2;

	double dist;
	double lambda=10;

	vcl_map<vcl_pair<int,int>,double > segment;// pair of curve numbers in frame n-1;
	vcl_pair<int,int > temp;
	vcl_map<vcl_pair<int,int>,double >::iterator it;
	segment.clear();
	vcl_ostringstream o;
	o<<frame;
	
	vcl_string temp_str=o.str();
	temp_str="file"+temp_str+".txt";
    //vcl_ofstream of(temp_str.c_str());
		for(iter1=store_matches.begin();iter1!=store_matches.end();iter1++)
		{
		 vcl_pair<int,int> temp1((*iter1).first,(*iter1).second);
		 tx1=map_of_T[temp1][0];
		 ty1=map_of_T[temp1][1];
		 Rtheta1=map_of_T[temp1][2];
		
		 for (iter2=store_matches.begin();iter2!=store_matches.end();iter2++)
			{
			vcl_pair<int,int> temp2((*iter2).first,(*iter2).second);
			tx2=map_of_T[temp2][0];
			ty2=map_of_T[temp2][1];
			Rtheta2=map_of_T[temp2][2];
	
			dist=vcl_sqrt((tx1-tx2)*(tx1-tx2)+(ty1-ty2)*(ty1-ty2));//+lambda*(Rtheta1-Rtheta2)*(Rtheta1-Rtheta2));
			temp.first=temp1.first;
			temp.second=temp2.first;
			segment[temp]=dist;
			
			vcl_cout<<dist<<"\t";
			}
		}


		vcl_map<vcl_pair<int,int>,double > top5ranks;
	
		for(iter1=store_matches.begin();iter1!=store_matches.end();iter1++)
		{
		 vcl_pair<int,int> temp1((*iter1).first,(*iter1).second);
		 
			for (iter2=store_matches.begin();iter2!=store_matches.end();iter2++)
			{
			 vcl_pair<int,int> temp2((*iter2).first,(*iter2).second);
			  
			 temp.first=temp1.first;
			 temp.second=temp2.first;
			 
			 segment[temp]=dist;
			 vcl_cout<<dist<<"\t";
			}

		}

	
		vcl_pair<int,int> min_pair_pair;
		int first,second;

		vcl_cout<<"\n the size of aspects is "<<aspects.size();

		// initialize the clusters
		vcl_vector<vcl_pair<vcl_vector<int>,vcl_pair<double,double > > > clusters;
		vcl_vector<vcl_pair<vcl_vector<int>,vcl_pair<double,double > > >::iterator ite;
		for(iter1=store_matches.begin();iter1!=store_matches.end();iter1++)
		{
			vcl_pair<int,int> match_pair((*iter1).first,(*iter1).second);
			vcl_pair<double,double> tr_pair(map_of_T[match_pair][0],map_of_T[match_pair][1]);
			vcl_pair<vcl_vector<int>,vcl_pair<double,double> >temp;

			temp.first.push_back((*iter1).second);
			temp.second=tr_pair;

			clusters.push_back(temp);
		}
		vcl_cout<<"\n the initial size is "<<clusters.size();
		while (no_of_iterations<6)
		{
		double min=1e6;
		for(int i=0;i<clusters.size();i++)
		{
			for(int j=i+1;j<clusters.size();j++)
			{
				//compute the cost
				tx1=clusters[i].second.first;
				ty1=clusters[i].second.second;

				tx2=clusters[j].second.first;
				ty2=clusters[j].second.second;

				//choose the minimum
				dist=vcl_sqrt((tx1-tx2)*(tx1-tx2)+(ty1-ty2)*(ty1-ty2));
				
				if(min>dist)
				{
					min=dist;
					temp.first=i;
					temp.second=j;
				}
			}
		}

		int i=temp.first;
		int j=temp.second;
		//merge_clusters

		if(clusters[i].first.size()>clusters[j].first.size())
		{
			
			
			clusters[i].second.first= (clusters[i].second.first*clusters[i].first.size()+ clusters[j].second.first*clusters[j].first.size())/clusters[i].first.size();
			clusters[i].second.second=(clusters[i].second.second*clusters[i].first.size()+ clusters[j].second.second*clusters[j].first.size())/clusters[i].first.size();
			for(int k=0;k<clusters[j].first.size();k++)
			{
				clusters[i].first.push_back(clusters[j].first[k]);
			}
			clusters[i].second.first=clusters[i].second.first/clusters[i].first.size();
			clusters[i].second.second=clusters[i].second.second/clusters[i].first.size();
			ite=clusters.begin();
			ite+=j;
			//(*ite)=clusters[j];
			clusters.erase(ite);
		}
		else
		{
			
			//vcl_cout<<"\n the tx is "<<clusters[j].second.first<<"\t";
			clusters[j].second.first= (clusters[j].second.first*clusters[j].first.size()+ clusters[i].second.first*clusters[i].first.size());
			clusters[j].second.second=(clusters[j].second.second*clusters[j].first.size()+ clusters[i].second.second*clusters[i].first.size());
			for(int k=0;k<clusters[i].first.size();k++)
			{
				clusters[j].first.push_back(clusters[i].first[k]);
			}
			clusters[j].second.first=clusters[j].second.first/clusters[j].first.size();
			clusters[j].second.second=clusters[j].second.second/clusters[j].first.size();
			//vcl_cout<<"\t"<<clusters[j].second.first;
			ite=clusters.begin();
			ite+=i;
		    clusters.erase(ite);
		}
		//update the cluster list

		no_of_iterations++;
		}

		for(int i=0;i<clusters.size();i++)
		{
			for(int j=0;j<clusters[i].first.size();j++)
			{
				vcl_cout<<clusters[i].first[j]<<"\t";
			}
			vcl_cout<<"\n";
		}
		vcl_cout<<"\n the size is  "<<clusters.size();


	for (unsigned int i=0;i<get_output_size_at(frame);i++){
    //vcl_cout<<".";
    
//	if(output_curve_[frame][i].tx_prev_>2 && output_curve_[frame][i].tx_prev_ <13)
	{
	vdgl_interpolator_sptr  intp = new vdgl_interpolator_linear( get_output_curve_at(frame,i) );
    vdgl_digital_curve_sptr dc = new vdgl_digital_curve(intp);
    group_spat_objs_.push_back( dc->cast_to_spatial_object_2d() );
    group_spat_objs_[group_spat_objs_.size()-1]->set_tag_id(1);
	}
=======
    top_ordered_curves.push_back(temp_list);
>>>>>>> 1.19
  }
<<<<<<< bdgl_curve_tracker.cxx

	//	group_curves
}


=======
>>>>>>> 1.19

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
  //for (int i=0;i<child_.size
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
          //tracked_curve_[frame][j].add_child(vdgl_digital_curve_sptr candidate_curve,coarse_cost)
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

      for (int i=0;i<cost_table.size();i++)
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
*/