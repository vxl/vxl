#include<bdgl/bdgl_curve_tracking.h>
#include<bdgl/bdgl_curve_clustering.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>

bdgl_curve_tracking ::bdgl_curve_tracking(bdgl_curve_tracking_params &tp)
: tp_(tp)
{
  clustering_=tp.clustering_;
  min_len_of_curves_=tp.min_length_of_curves;
  tp_=tp;
}

void bdgl_curve_tracking ::track()
{
  vcl_cout<<"-- testing batch tracking --\n";
  // for all images:
  for (unsigned int t=0; t<input_curves_.size(); t++)
    track_frame(t);
  return;
}

//-----------------------------------------------------------------------------
bdgl_tracker_curve_sptr bdgl_curve_tracking ::get_output_curve(unsigned int frame_no, int id)
{
  if (frame_no >= output_curves_.size())
  {
    vcl_cerr<<"\n frame "<<frame_no<<" doesn't exist\n";
    return 0;
  }
  else
    return output_curves_[frame_no][id];
}

void bdgl_curve_tracking ::get_reliable_curves(unsigned int frame_no,unsigned int window)
{
  if (frame_no >= output_curves_.size())
    vcl_cerr<<"\n frame "<<frame_no<<" doesn't exist\n";
  else if (frame_no<window)
    vcl_cerr<<"\n cannot compute reliable curves for frame "<<frame_no<<", window size="<<window<<'\n';
  else
  {
    bdgl_curve_matching_params mp_tc=tp_.mp;
    mp_tc.motion_in_pixels=(window+1)*tp_.mp.motion_in_pixels;
    bdgl_curve_matching matcher_alternative(mp_tc);
    matcher_alternative.match(&output_curves_[frame_no],&output_curves_tc_[frame_no-window]);
    unsigned int prev_frame=frame_no-window;

    bool found=false;

   for (int i=0;i<output_curves_[prev_frame].size();i++)
      {
        // avoid the virtual curves added
        if (output_curves_[prev_frame][i]->isreal_)
		{
         
         bdgl_tracker_curve_sptr way2;
         for (int j=0;j<output_curves_tc_[prev_frame].size();j++)
          {
            if (output_curves_tc_[prev_frame][j]->get_id()==output_curves_[prev_frame][i]->get_id())
            {
             way2=output_curves_tc_[prev_frame][j];
            }
        }
      bdgl_tracker_curve_sptr way1=output_curves_[prev_frame][i];


        vcl_map<int,vcl_vector<bdgl_tracker_curve_sptr> > way1_map,way2_map;
        way1_map[0].push_back(way1);

        found=true;
        for (unsigned int k=0;k<window;k++)
        {
          for (unsigned int l=0; l<way1_map[k].size(); ++l)
          if (way1_map[k][l]->get_best_match_next())
            for (unsigned int n=0; n<way1_map[k][l]->get_best_match_next()->match_curve_set.size(); ++n)
              way1_map[k+1].push_back(way1_map[k][l]->get_best_match_next()->match_curve_set[n]);
        }

        if (way1_map[window].size()==0)
          found=false;

        if (way2->get_best_match_next())
          for (unsigned int k=0; k<way2->get_best_match_next()->match_curve_set.size(); ++k)
            way2_map[window].push_back(way2->get_best_match_next()->match_curve_set[k]);

        if (way2_map[window].size()==0)
          found=false;

        if (found)
          for (unsigned int k=0; k<way1_map[window].size(); ++k)
            for (unsigned int l=0; l<way2_map[window].size(); ++l)
              if (way2_map[window][l]->get_id()==way1_map[window][k]->get_id())
                way1_map[window][k]->isreliable_=true;
      }
    }
  }
}

void bdgl_curve_tracking ::write_results(vcl_string filename)
{
  vcl_ofstream ofile(filename.c_str());
  ofile<<"<\?xml version=\"1.0\" encoding=\"UTF-8\" \?>\n<vxl>\n"
       <<"<results video=\""<<filename<<"\" nframes=\""<<output_curves_.size()<<"\">";
  for (unsigned int i=0; i<output_curves_.size(); ++i)
  {
    double cnt=0;
    for (unsigned int j=0; j<output_curves_[i].size(); ++j)
      if (output_curves_[i][j]->isreliable_ && output_curves_[i][j]->match_id_ >0)
        ++cnt;
    vcl_cout<<"\n frame no : "<<i <<" score : "<<cnt/output_curves_[i].size();
    ofile<<"<frame no=\""<<i<<"\" score=\""<<cnt/output_curves_[i].size()<<"\"/>";
  }
  ofile<<"</results>\n</vxl>\n";
}

void bdgl_curve_tracking ::track_frame(unsigned int frame)
{
  bdgl_tracker_curve_sptr                primitive;
  vcl_vector< bdgl_tracker_curve_sptr >  primitive_list;

  bdgl_tracker_curve_sptr                primitive1;
  vcl_vector< bdgl_tracker_curve_sptr >  primitive_list1;
  bdgl_curve_matching                    matcher(tp_.mp);

  if (input_curves_.size()<=frame) return;

  // init : copy the first curves
  if (frame==0)
  {
    primitive_list.clear();
    primitive_list1.clear();
    for (unsigned int i=0;i<input_curves_[0].size();i++)
    {
      primitive = new bdgl_tracker_curve;
      primitive->init_set(input_curves_[0][i],i);
      primitive->frame_number=0;
	  primitive->ismatchedprev_=true;
      primitive_list.insert(primitive_list.end(), primitive);

      // for alternate frame tracking
      primitive1= new bdgl_tracker_curve;
      primitive1->frame_number=0;
	  primitive1->ismatchedprev_=true;
      primitive1->init_set(input_curves_[0][i],i);
      primitive_list1.insert(primitive_list1.end(), primitive1);
    }
    output_curves_.insert(output_curves_.end(), primitive_list);
    output_curves_tc_.insert(output_curves_tc_.end(), primitive_list1);
	vcl_cout<<"\n"<<primitive1->frame_number;
	//moving_curves_.clear();
  }
  else
  {
    // init : duplicate empty primitive lists
    primitive_list.clear();
    primitive_list1.clear();
    
    for (unsigned int i=0;i<input_curves_[frame].size();i++)
    {
      primitive = new bdgl_tracker_curve;
      primitive->init_set(input_curves_[frame][i],i);
      primitive_list.insert(primitive_list.end(), primitive);

      // for alternate frame tracking
      primitive1= new bdgl_tracker_curve;
      primitive1->init_set(input_curves_[frame][i],i);
      primitive_list1.insert(primitive_list1.end(), primitive1);
    }
    output_curves_.insert(output_curves_.end(), primitive_list);
    output_curves_tc_.insert(output_curves_tc_.end(), primitive_list1);

    // give the new and old curves to matcher to do the matching
	if(tp_.mp.matching_)
	{
	    vcl_cout<<"\n the no of curves entered "<<output_curves_[frame].size();
		matcher.match(&output_curves_[frame],&output_curves_[frame-1]);
		if(tp_.transitive_closure )
		{
		 if( tp_.window_size-1<=frame)
		 {
			tp_.mp.motion_in_pixels*=tp_.window_size;
			bdgl_curve_matching matcher_tc(tp_.mp);
			matcher_tc.match(&output_curves_tc_[frame],&output_curves_tc_[frame-tp_.window_size+1]);
			matcher.best_matches_tc(&output_curves_[frame-1],&output_curves_[frame-2],&output_curves_tc_[frame]);
		 }
		}
		else
			matcher.best_matches(&output_curves_[frame],&output_curves_[frame-1]);

		if (tp_.clustering_)
		{
		 bdgl_curve_clustering cl(tp_.cp);
		 cl.cluster_curves(&output_curves_[frame]);
		 vcl_ostringstream s;
		 s<<"c:\\clusters\\cl_dist_table"<<frame<<".txt";
		 cl.write_distance_table(s.str(),&output_curves_[frame]);
		 cl.get_moving_objects(frame,moving_curves_);
		 int t1=moving_curves_.size();
		 //frame_moving_curves_.push_back(moving_curves_);
		 //vcl_ostringstream s;
		 //s<<"c:\\clusters\\clusters"<<frame<<".cem";
		 //write_clusters(s.str(),t1-1);
		}
	}
	//  transitive closure : input window size=3,5 
	
  }
}

void bdgl_curve_tracking :: write_clusters(vcl_string filename,int i)
{
 

 vcl_ofstream f;
 f.open(filename.c_str());
 if(!f)
	{
	 vcl_cerr<<"\n could not open the file to write ";
	 return;
 }
 //header of the file
 f<<"#.cem files\n";
 f<<"#Each contour block will consist of the following\n";
 f<<"#[Begin Contour ]\n";
 

 if(moving_curves_.size()>0)
 {
  //for(int i=0;i<moving_curves_.size();i++)
  {
	for(int j=0;j<moving_curves_[i].size();j++)
	{
		f<<"[BEGIN CONTOUR]\n";
		f<<"EDGE_COUNT="<<moving_curves_[i][j]->desc->points_.size()<<"\n";
		for(int k=0;k<moving_curves_[i][j]->desc->points_.size();k++)
		{
			f<<"[0,0]\t0\t0\t"<<"["<<moving_curves_[i][j]->desc->points_[k].x()<<","
			 <<moving_curves_[i][j]->desc->points_[k].y()<<"]"
			 <<"\t"<<moving_curves_[i][j]->desc->angles_[k]<<"\t"
			 <<moving_curves_[i][j]->desc->grad_[k]<<"\n";
		}
		f<<"[BEGIN TRANSFORMATION]\n";
		f<<"["<<moving_curves_[i][j]->get_best_match_prev()->T_(0,0)<<"\t"<<moving_curves_[i][j]->get_best_match_prev()->T_(1,0)<<"\t"<<
			moving_curves_[i][j]->get_best_match_prev()->R_(0,0)<<"\t"<<moving_curves_[i][j]->get_best_match_prev()->R_(0,1)<<"\t"
			<<moving_curves_[i][j]->get_best_match_prev()->R_(1,0)<<"\t"<<moving_curves_[i][j]->get_best_match_prev()->R_(1,1)<<"\t"
			<<moving_curves_[i][j]->get_best_match_prev()->scale_<<"]\n";
		if(moving_curves_[i][j]->get_best_match_next())
			f<<"["<<moving_curves_[i][j]->get_best_match_next()->T_(0,0)<<"\t"<<moving_curves_[i][j]->get_best_match_next()->T_(1,0)<<"\t"<<
			moving_curves_[i][j]->get_best_match_next()->R_(0,0)<<"\t"<<moving_curves_[i][j]->get_best_match_next()->R_(0,1)<<"\t"
			<<moving_curves_[i][j]->get_best_match_next()->R_(1,0)<<"\t"<<moving_curves_[i][j]->get_best_match_next()->R_(1,1)<<
			"\t"<<moving_curves_[i][j]->get_best_match_next()->scale_<<"]\n";
		f<<"[END TRANSFORMATION]\n";
		f<<"[END CONTOUR]\n";
	}

  }
 }
 f.close();

}

vcl_vector< bdgl_tracker_curve_sptr> *bdgl_curve_tracking ::get_output_curves(unsigned int frame_no)

{
  if (frame_no >= output_curves_.size())
    return NULL;
  else
    return &output_curves_[frame_no];
}
void bdgl_curve_tracking ::obtain_tracks()
{
 if(output_curves_.size()<1)
	 return ;

 int trackid=0;
 for(int i=0;i<output_curves_.size();i++)
 {
	for(int j=0;j<get_output_size_at(i);j++)
	{
	 bdgl_tracker_curve_sptr curve=get_output_curve(i,j);
	 if(curve->track_id_==-1)
	 {
		 vcl_list<bdgl_tracker_curve_sptr> tr;
		 vcl_list<bdgl_tracker_curve_sptr>::iterator iter;

		level_order_traversal(curve,tr);
		if(tr.size()>0)
		{
			for (iter=tr.begin();iter!=tr.end();iter++)
				(*iter)->track_id_=trackid;
			len_of_tracks_[trackid]=tr.size();
			trackid++;
		}
	 }
	}
 }
}
void bdgl_curve_tracking ::write_tracks(bdgl_tracker_curve_sptr curve,
                                        vcl_string fname,
                                        int min_length_of_track)
{
  vcl_map<int, vcl_list<bdgl_tracker_curve_sptr> > track_;
  vcl_map<int, vcl_list<bdgl_tracker_curve_sptr> >::iterator  itrack;
  vcl_list<bdgl_tracker_curve_sptr> tr;
  vcl_list<bdgl_tracker_curve_sptr>::iterator iter;

  level_order_traversal(curve,tr);
  for (iter=tr.begin();iter!=tr.end();iter++)
    track_[(*iter)->frame_number].push_back((*iter));

  if (int(track_.size())>min_length_of_track)
  {
    vcl_ofstream f(fname.c_str());
    f<<"# CONTOUR_EDGE_MAP : canny+van-ducks\n"
     <<"# .cem files\n"
     <<"# Format :\n"
     <<"# Each contour block will consist of the following\n"
     <<"# [BEGIN CONTOUR]\n"
     <<"# EDGE_COUNT=num_of_edges\n"
     <<"# [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Sub_Pixel_Conf\n"
     <<"CURVE\n";
    for (itrack=track_.begin();itrack!=track_.end();itrack++)
    {
      f<<"[BEGIN FRAME NO = "<<(*itrack).first<<"]\n";
      for (iter=(*itrack).second.begin();iter!=(*itrack).second.end();iter++)
      {
        bdgl_tracker_curve_sptr obj=(*iter);
        f<<"[BEGIN CONTOUR]\n"
         <<"EDGE_COUNT="<<obj->desc->points_.size()<<"\n";

        for (unsigned int i=0; i<obj->desc->points_.size(); ++i)
        {
          f<<" ["<<obj->desc->points_[i].x()<<", "<<obj->desc->points_[i].y()
           <<"]   0 0\n";
        }
        f<<"[END CONTOUR]\n";
      }
    }
    f<<"END CURVE\n";
    f.close();
  }
}

void bdgl_curve_tracking ::level_order_traversal(bdgl_tracker_curve_sptr curve,vcl_list<bdgl_tracker_curve_sptr> & tr)
{
  tr.clear();
  vcl_list<bdgl_tracker_curve_sptr> q;
  if (!curve)
    return;

  q.push_back(curve);
  while (q.size()>0)
  {
    bdgl_tracker_curve_sptr c=q.front();
    q.pop_front();
    if (c)
    {
      tr.push_back(c);
      if (c->get_best_match_next())
        for (unsigned int i=0; i<c->get_best_match_next()->match_curve_set.size(); ++i)
          q.push_back(c->get_best_match_next()->match_curve_set[i]);
    }
    else
      break;
  }
}
