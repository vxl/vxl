#include "vmal_track_lines.h"

#include <vnl/vnl_math.h> // for pi
#include <vnl/vnl_double_2.h>

#include <vtol/vtol_edge_2d.h>

#include <vcl_cmath.h>
#include <vcl_algorithm.h> // for vcl_min

#include <vmal/vmal_lines_correlation.h>
#include <vmal/vmal_refine_lines.h>
#include <vmal/vmal_operators.h>
#include <vmal/vmal_convert.h>

#define PI vnl_math::pi

vmal_track_lines::vmal_track_lines()
{
}

vmal_track_lines::~vmal_track_lines()
{
}

void vmal_track_lines::track_lines(const vcl_vector<vcl_vector<vtol_edge_2d_sptr>*>* fit_lines,
                                   const vcl_vector<vcl_vector<vtol_edge_2d_sptr>*>* transformed_lines,
                                   const vcl_vector<vil1_image> &images,
                                   const vcl_vector<vnl_double_3x3> &homo,
                                   vmal_multi_view_data_edge_sptr matches)
{
  theta_=0.0873;//0.0873;
  radius_=5.0;
  vmal_multi_view_data_edge_sptr tmp_matches;
  tmp_matches=new vmal_multi_view_data<vtol_edge_2d_sptr>(matches->get_nb_views());

  if (fit_lines->size()==(transformed_lines->size()+1))
  {
    //int match_num=0;
    unsigned int min_line=0;
    double min_dist=-1;
    bool match=false;
    bool replace=false;
    vtol_edge_2d_sptr cur_fl;
    vtol_edge_2d_sptr cur_tl;

    for (unsigned int i=0;i<(*transformed_lines)[0]->size();i++)
    {
      bool found=true;
      unsigned int match_line=i;
      unsigned int view_num=0;
      tmp_matches->new_track();
      while ((view_num < transformed_lines->size()) && found)
      {
        found=false;
        cur_tl=(*(*transformed_lines)[view_num])[match_line];
        //double tl1x=cur_tl->v1()->cast_to_vertex_2d()->x();
        //double tl2x=cur_tl->v2()->cast_to_vertex_2d()->x();
        //double tl1y=cur_tl->v1()->cast_to_vertex_2d()->y();
        //double tl2y=cur_tl->v2()->cast_to_vertex_2d()->y();
        vtol_edge_2d_sptr other_match;
        for (unsigned int j=0; j<(*fit_lines)[view_num+1]->size(); ++j)
        {
          cur_fl=(*(*fit_lines)[view_num+1])[j];
          //double fl1x=cur_fl->v1()->cast_to_vertex_2d()->x();
          //double fl2x=cur_fl->v2()->cast_to_vertex_2d()->x();
          //double fl1y=cur_fl->v1()->cast_to_vertex_2d()->y();
          //double fl2y=cur_fl->v2()->cast_to_vertex_2d()->y();

          double angle=seg_angle(cur_tl,cur_fl);
          if (angle<theta_)
          {
            if (belong(cur_tl,cur_fl))
            {
              double cur_dist;
              //int size=(*fit_lines)[view_num]->size();
              vtol_edge_2d_sptr pred_fl=(*(*fit_lines)[view_num])[match_line];
              cost_function(pred_fl,
                            cur_tl,
                            cur_fl,
                            images[view_num],
                            images[view_num+1],
                            homo[view_num], cur_dist);
              if (min_dist==-1)//Initial value
                min_dist=cur_dist;
              if ((cur_dist<=min_dist) && (cur_dist!=-1))
              {
                //look if this line have already been matched.
                //If so, perform a test between those two lines
                //to see which one is the best. It can also decide
                //that they both suit.
                if (tmp_matches->get_pred_match(view_num,cur_fl,other_match))
                {
                  vtol_edge_2d_sptr t_other_match=find_transfo(other_match,*(*fit_lines)[view_num],*(*transformed_lines)[view_num]);
                  double other_dist;
                  cost_function(other_match,
                                t_other_match,
                                cur_fl,
                                images[view_num],
                                images[view_num+1],
                                homo[view_num], other_dist);
                  //double ol1x=t_other_match->v1()->cast_to_vertex_2d()->x();
                  //double ol2x=t_other_match->v2()->cast_to_vertex_2d()->x();
                  //double ol1y=t_other_match->v1()->cast_to_vertex_2d()->y();
                  //double ol2y=t_other_match->v2()->cast_to_vertex_2d()->y();

                  //int choice=is_cur_best(cur_tl,cur_fl,t_other_match);
                  int choice;
                  if (other_dist<cur_dist)
                      choice=-1;
                    else
                      choice=1;
                  if (choice==1) //the new is better
                  {
                    found=true;
                    min_dist=cur_dist;
                    min_line=j;
                    replace=true;
                  }
                  else if (choice==0) //they both suit
                  {
                    found=true;
                    min_dist=cur_dist;
                    min_line=j;
                    replace=false;
                  }
                  else if (choice==-1) //the old match is the best
                  {
                    found=false;
                    min_dist=cur_dist;
                    min_line=j;
                    replace=false;
                  }
                }
                else
                {
                  found=true;
                  min_dist=cur_dist;
                  min_line=j;
                  replace=false;
                }
              }
            }
          }
        }
        if (found)
        {
          if (replace)
          {
            tmp_matches->remove(view_num+1, (*(*fit_lines)[view_num+1])[min_line]);
            replace=false;
          }
          tmp_matches->set(view_num,(*(*fit_lines)[view_num])[match_line]);
          match=true;
          view_num++;
        }
        match_line=min_line;
        min_dist=-1 ;
      }
      if (match)
      {
        vtol_edge_2d_sptr p=(*(*fit_lines)[view_num])[match_line];
        tmp_matches->set(view_num,p);
        match=false;
        tmp_matches->close_track();
      }
    }
  }
  sort_lines(tmp_matches,matches);
  matches->print(vcl_cerr);
}

double vmal_track_lines::seg_angle(vtol_edge_2d_sptr trans_line,vtol_edge_2d_sptr fit_line)
{
  double vect_tlx=(trans_line->v2()->cast_to_vertex_2d()->x())-(trans_line->v1()->cast_to_vertex_2d()->x());
  double vect_tly=(trans_line->v2()->cast_to_vertex_2d()->y())-(trans_line->v1()->cast_to_vertex_2d()->y());

  double vect_flx=(fit_line->v2()->cast_to_vertex_2d()->x())-(fit_line->v1()->cast_to_vertex_2d()->x());
  double vect_fly=(fit_line->v2()->cast_to_vertex_2d()->y())-(fit_line->v1()->cast_to_vertex_2d()->y());

  vnl_double_2 vect_tl(vect_tlx,vect_tly);

  vnl_double_2 vect_fl(vect_flx,vect_fly);

  double alpha=angle(vect_tl,vect_fl);

  return alpha;
}

bool vmal_track_lines::belong(vtol_edge_2d_sptr trans_line,vtol_edge_2d_sptr fit_line)
{
  double tl1x=trans_line->v1()->cast_to_vertex_2d()->x();
  double tl2x=trans_line->v2()->cast_to_vertex_2d()->x();
  double vect_tlx=tl2x-tl1x;

  double tl1y=trans_line->v1()->cast_to_vertex_2d()->y();
  double tl2y=trans_line->v2()->cast_to_vertex_2d()->y();
  double vect_tly=tl2y-tl1y;

  vnl_double_2 vect_tl(vect_tlx,vect_tly);
  vnl_double_2 norma=vect_tl.normalize();

  double fl1x=fit_line->v1()->cast_to_vertex_2d()->x();
  double fl2x=fit_line->v2()->cast_to_vertex_2d()->x();

  double fl1y=fit_line->v1()->cast_to_vertex_2d()->y();
  double fl2y=fit_line->v2()->cast_to_vertex_2d()->y();

  //defined the first bounding segment
  double bound1_tl1x=tl1x+(-norma[1]*radius_);
  double bound1_tl1y=tl1y+(norma[0]*radius_);
  double bound1_tl2x=tl2x+(-norma[1]*radius_);
  double bound1_tl2y=tl2y+(norma[0]*radius_);

  //defined the second bounding segment
  double bound2_tl1x=tl1x+(norma[1]*radius_);
  double bound2_tl1y=tl1y+(-norma[0]*radius_);
  double bound2_tl2x=tl2x+(norma[1]*radius_);
  double bound2_tl2y=tl2y+(-norma[0]*radius_);


  if (vmal_operators::cross_seg(bound1_tl1x, bound1_tl1y, bound2_tl1x, bound2_tl1y,
                                fl1x, fl1y, fl2x, fl2y))

    return true;
  else if (vmal_operators::cross_seg(bound1_tl2x, bound1_tl2y, bound2_tl2x,bound2_tl2y,
                                     fl1x, fl1y,fl2x, fl2y))
    return true;
  else //test if at least one of the extremities of the fit line
     //is inside the bounding rectanlge.
  {
    double x1,y1,x2,y2;
    vmal_operators::project_point(fl1x,fl1y,bound1_tl1x,bound1_tl1y,bound1_tl2x,bound1_tl2y,&x1,&y1);
    vmal_operators::project_point(fl1x,fl1y,bound2_tl1x,bound2_tl1y,bound2_tl2x,bound2_tl2y,&x2,&y2);
    if ((x1!=-1)&&(x2!=-1))
    {
      if (((x1-fl1x)*(x2-fl1x)+(y1-fl1y)*(y2-fl1y))<0)
        return true;
    }
    vmal_operators::project_point(fl2x,fl2y,bound1_tl1x,bound1_tl1y,bound1_tl2x,bound1_tl2y,&x1,&y1);
    vmal_operators::project_point(fl2x,fl2y,bound2_tl1x,bound2_tl1y,bound2_tl2x,bound2_tl2y,&x2,&y2);
    if ((x1!=-1)&&(x2!=-1))
    {
      if (((x1-fl2x)*(x2-fl2x)+(y1-fl2y)*(y2-fl2y))<0)
        return true;
    }
  }
  return false;
}

double vmal_track_lines::dist(vtol_edge_2d_sptr trans_line,vtol_edge_2d_sptr fit_line)
{
  double tl1x=trans_line->v1()->cast_to_vertex_2d()->x();
  double tl2x=trans_line->v2()->cast_to_vertex_2d()->x();
  double tl1y=trans_line->v1()->cast_to_vertex_2d()->y();
  double tl2y=trans_line->v2()->cast_to_vertex_2d()->y();
  double vect_tlx=tl2x-tl1x;
  double vect_tly=tl2y-tl1y;
  vnl_double_2 vect_tl(vect_tlx,vect_tly);
  vect_tl=vect_tl.normalize();

  double fl1x=fit_line->v1()->cast_to_vertex_2d()->x();
  double fl2x=fit_line->v2()->cast_to_vertex_2d()->x();
  double fl1y=fit_line->v1()->cast_to_vertex_2d()->y();
  double fl2y=fit_line->v2()->cast_to_vertex_2d()->y();
  double vect_flx=fl2x-fl1x;
  double vect_fly=fl2y-fl1y;
  vnl_double_2 vect_fl(vect_flx,vect_fly);
  vect_fl=vect_fl.normalize();

  double dist=0;
  double distover;

  double x1,y1;
  double x2,y2;
  double x3,y3;
  double x4,y4;

  double dist1=vmal_operators::project_point(fl1x,fl1y,tl1x,tl1y,tl2x,tl2y,&x1,&y1);
  double dist2=vmal_operators::project_point(fl2x,fl2y,tl1x,tl1y,tl2x,tl2y,&x2,&y2);
  double dist3=vmal_operators::project_point(tl1x,tl1y,fl1x,fl1y,fl2x,fl2y,&x3,&y3);
  double dist4=vmal_operators::project_point(tl2x,tl2y,fl1x,fl1y,fl2x,fl2y,&x4,&y4);

  if (dist1==-1)
    return -1;

  if (x1!=-1 && x2!=-1) // the first segment totally project on the second
  {
    if (x3!=-1)
    {
      dist=dist3+vcl_min(dist1,dist2);
      if (dist1<dist2)
      distover=vcl_sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))+
             vcl_sqrt((x3-fl1x)*(x3-fl1x)+(y3-fl1y)*(y3-fl1y));
      else
      distover=vcl_sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))+
             vcl_sqrt((x3-fl2x)*(x3-fl2x)+(y3-fl2y)*(y3-fl2y));
    }
    else if (x4!=-1)
    {
      dist=dist4+vcl_min(dist1,dist2);
      if (dist1<dist2)
      distover=vcl_sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))+
             vcl_sqrt((x4-fl1x)*(x4-fl1x)+(y4-fl1y)*(y4-fl1y));
      else
      distover=vcl_sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))+
             vcl_sqrt((x4-fl2x)*(x4-fl2x)+(y4-fl2y)*(y4-fl2y));
    }
    else
    {
      dist=dist1+dist2;
      distover=vcl_sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))+
             vcl_sqrt((fl1x-fl2x)*(fl1x-fl2x)+(fl1y-fl2y)*(fl1y-fl2y));
    }
  }
  else if (x1!=-1) //case 2
  {
    dist=dist1;

    if ((x3!=-1)&&(x4!=-1))
    {
      dist+=vcl_min(dist3,dist4);
      if (dist3<dist4)
      distover=vcl_sqrt((x3-fl1x)*(x3-fl1x)+(y3-fl1y)*(y3-fl1y))+
             vcl_sqrt((x1-tl1x)*(x1-tl1x)+(y1-tl1y)*(y1-tl1y));
      else
      distover=vcl_sqrt((x4-fl1x)*(x4-fl1x)+(y4-fl1y)*(y4-fl1y))+
             vcl_sqrt((x1-tl2x)*(x1-tl2x)+(y1-tl2y)*(y1-tl2y));
    }
    else if (x3!=-1)
    {
      dist+=dist3;
      distover=vcl_sqrt((x3-fl1x)*(x3-fl1x)+(y3-fl1y)*(y3-fl1y))+
             vcl_sqrt((x1-tl1x)*(x1-tl1x)+(y1-tl1y)*(y1-tl1y));
    }
    else if (x4!=-1)
    {
      dist+=dist4;
      distover=vcl_sqrt((x4-fl1x)*(x4-fl1x)+(y4-fl1y)*(y4-fl1y))+
             vcl_sqrt((x1-tl2x)*(x1-tl2x)+(y1-tl2y)*(y1-tl2y));
    }
    else
    {
    // dist=2*dist;
    // distover=vcl_min(vcl_sqrt((x1-tl2x)*(x1-tl2x)+(y1-tl2y)*(y1-tl2y)),
          // vcl_sqrt((x1-tl1x)*(x1-tl1x)+(y1-tl1y)*(y1-tl1y)));
      dist=-1;
      distover=1;
    }
  }
  else if (x2!=-1)
  {
    dist=dist2;

    if ((x3!=-1)&&(x4!=-1))
    {
      dist+=vcl_min(dist3,dist4);
      if (dist3<dist4)
      distover=vcl_sqrt((x3-fl2x)*(x3-fl2x)+(y3-fl2y)*(y3-fl2y))+
             vcl_sqrt((x2-tl1x)*(x2-tl1x)+(y2-tl1y)*(y2-tl1y));
      else
      distover=vcl_sqrt((x4-fl2x)*(x4-fl2x)+(y4-fl2y)*(y4-fl2y))+
             vcl_sqrt((x2-tl2x)*(x2-tl2x)+(y2-tl2y)*(y2-tl2y));
    }
    else if (x3!=-1)
    {
      dist+=dist3;
      distover=vcl_sqrt((x3-fl2x)*(x3-fl2x)+(y3-fl2y)*(y3-fl2y))+
             vcl_sqrt((x2-tl1x)*(x2-tl1x)+(y2-tl1y)*(y2-tl1y));
    }
    else if (x4!=-1)
    {
      dist+=dist4;
      distover=vcl_sqrt((x4-fl2x)*(x4-fl2x)+(y4-fl2y)*(y4-fl2y))+
             vcl_sqrt((x2-tl2x)*(x2-tl2x)+(y2-tl2y)*(y2-tl2y));
    }
    else
    {
      dist=-1;
      distover=1;
    }
  }
  else
  {
    dist=dist3+dist4;
    distover=vcl_sqrt((x3-x4)*(x3-x4)+(y3-y4)*(y3-y4))+
         vcl_sqrt((tl1x-tl2x)*(tl1x-tl2x)+(tl1y-tl2y)*(tl1y-tl2y));
  }

#if 0
  double length=vcl_sqrt((tl1x-tl2x)*(tl1x-tl2x)+(tl1y-tl2y)*(tl1y-tl2y))
              + vcl_sqrt((fl1x-fl2x)*(fl1x-fl2x)+(fl1y-fl2y)*(fl1y-fl2y));
#endif

  return dist/distover;
}

int vmal_track_lines::is_cur_best(vtol_edge_2d_sptr trans_line,vtol_edge_2d_sptr fit_line,vtol_edge_2d_sptr other_line)
{
  double tl1x=trans_line->v1()->cast_to_vertex_2d()->x();
  double tl2x=trans_line->v2()->cast_to_vertex_2d()->x();
  double tl1y=trans_line->v1()->cast_to_vertex_2d()->y();
  double tl2y=trans_line->v2()->cast_to_vertex_2d()->y();
#if 0
  double fl1x=fit_line->v1()->cast_to_vertex_2d()->x();
  double fl2x=fit_line->v2()->cast_to_vertex_2d()->x();
  double fl1y=fit_line->v1()->cast_to_vertex_2d()->y();
  double fl2y=fit_line->v2()->cast_to_vertex_2d()->y();
#endif
  double ol1x=other_line->v1()->cast_to_vertex_2d()->x();
  double ol2x=other_line->v2()->cast_to_vertex_2d()->x();
  double ol1y=other_line->v1()->cast_to_vertex_2d()->y();
  double ol2y=other_line->v2()->cast_to_vertex_2d()->y();

  double x1,y1;
  double x2,y2;
  double x3,y3;
  double x4,y4;

  vmal_operators::project_point(tl1x,tl1y,ol1x,ol1y,ol2x,ol2y,&x1,&y1);
  vmal_operators::project_point(tl2x,tl2y,ol1x,ol1y,ol2x,ol2y,&x2,&y2);
  vmal_operators::project_point(ol1x,ol1y,tl1x,tl1y,tl2x,tl2y,&x3,&y3);
  vmal_operators::project_point(ol2x,ol2y,tl1x,tl1y,tl2x,tl2y,&x4,&y4);

#if 0
  if ((x1==-1)&&(x2==-1)&&(x3==-1)&&(x4==-1))
    return 0;
  else
#endif
  {
    double dist1=dist(trans_line,fit_line);
    double dist2=dist(other_line,fit_line);
    if (dist1<dist2)
      return 1;
    else
      return -1;
  }
}

vtol_edge_2d_sptr vmal_track_lines::find_transfo(vtol_edge_2d_sptr line,
                                                 vcl_vector<vtol_edge_2d_sptr>& fit_lines,
                                                 const vcl_vector<vtol_edge_2d_sptr>& transformed_lines
                                                )
{
  vcl_vector<vtol_edge_2d_sptr>::iterator iter;
  int i=0;
  for (iter=fit_lines.begin(); iter!=fit_lines.end(); ++iter)
  {
    if (*(*iter)==*line)
      return transformed_lines[i];
    i++;
  }
  return NULL;
}

void vmal_track_lines::sort_lines(vmal_multi_view_data_edge_sptr matches,
                                  vmal_multi_view_data_edge_sptr sorted_matches)
{
  bool still_track;
  vcl_map<int,vtol_edge_2d_sptr,vcl_less<int> > track;
  still_track=matches->get_first_track(track);
  while (still_track)
  {
    sorted_matches->new_track();
    vcl_map<int,vtol_edge_2d_sptr,vcl_less<int> >::iterator iter1;
    vcl_map<int,vtol_edge_2d_sptr,vcl_less<int> >::iterator iter2=track.begin();
    iter2++;
    for (iter1=track.begin(); iter2!=track.end(); ++iter1)
    {
      int key1=(*iter1).first;
      vtol_edge_2d_sptr value0=(*iter1).second;
      int key2=(*iter2).first;
      vtol_edge_2d_sptr value1=(*iter2).second;

      vtol_edge_2d_sptr out0;
      vtol_edge_2d_sptr out1;

      sort_a_pair_of_line(value0, value1, out0, out1);

      sorted_matches->set(key1,out0);
      sorted_matches->set(key2,out1);
      iter2++;
    }
    sorted_matches->close_track();
    still_track=matches->get_next_track(track);
  }
}

void vmal_track_lines::sort_a_pair_of_line(vtol_edge_2d_sptr line0,
                                           vtol_edge_2d_sptr line1,
                                           vtol_edge_2d_sptr &new_line0,
                                           vtol_edge_2d_sptr &new_line1)
{
  double cur_1x=line0->v1()->cast_to_vertex_2d()->x();
  double cur_2x=line0->v2()->cast_to_vertex_2d()->x();
  double cur_1y=line0->v1()->cast_to_vertex_2d()->y();
  double cur_2y=line0->v2()->cast_to_vertex_2d()->y();
  vnl_double_2 cur(cur_2x-cur_1x,cur_2y-cur_1y);

  double next_1x=line1->v1()->cast_to_vertex_2d()->x();
  double next_2x=line1->v2()->cast_to_vertex_2d()->x();
  double next_1y=line1->v1()->cast_to_vertex_2d()->y();
  double next_2y=line1->v2()->cast_to_vertex_2d()->y();
  vnl_double_2 next(next_2x-next_1x,next_2y-next_1y);

  if (dot_product(cur,next)<0)
  {
    new_line0=new vtol_edge_2d(cur_1x,cur_1y,cur_2x,cur_2y);
    new_line1=new vtol_edge_2d(next_2x,next_2y,next_1x,next_1y);
  }
  else
  {
    new_line0=new vtol_edge_2d(cur_1x,cur_1y,cur_2x,cur_2y);
    new_line1=new vtol_edge_2d(next_1x,next_1y,next_2x,next_2y);
  }
}


double vmal_track_lines::lines_correlation(vtol_edge_2d_sptr line0,
                                           vtol_edge_2d_sptr line1,
                                           const vnl_double_3x3 & H,
                                           vil1_memory_image_of<vxl_byte> &image0,
                                           vil1_memory_image_of<vxl_byte> &image1)
{
  vtol_edge_2d_sptr s_line0;
  vtol_edge_2d_sptr s_line1;
  // sort the lines so that their end-points match
  sort_a_pair_of_line(line0, line1, s_line0, s_line1);

  vnl_double_3 s_line0_p, s_line0_q;
  vnl_double_3 s_line1_p, s_line1_q;

  convert_line_double_3(s_line0, s_line0_p, s_line0_q);
  convert_line_double_3(s_line1, s_line1_p, s_line1_q);

  vnl_double_3 r_line0_p, r_line0_q;
  vnl_double_3 r_line1_p, r_line1_q;

  // refine the lines so that their lenght are the same
  vmal_refine_lines ref;
  ref.refine_lines_min_h(s_line0_p, s_line0_q,
                         s_line1_p, s_line1_q,
                         H,
                         r_line0_p, r_line0_q,
                         r_line1_p, r_line1_q);

  vmal_lines_correlation correl;
  vnl_double_3 trans;
  double res;
  res=correl.find_min_corr(r_line0_p, r_line0_q,
                           r_line1_p, r_line1_q,
                           image0, image1,
                           trans);
  return res;
}


void vmal_track_lines::cost_function(vtol_edge_2d_sptr line0,
                                     vtol_edge_2d_sptr t_line0,
                                     vtol_edge_2d_sptr line1,
                                     const vil1_image &image0,
                                     const vil1_image &image1,
                                     const vnl_double_3x3 homo,
                                     double &result)
{
  vil1_memory_image_of<vxl_byte> i0;
  vil1_memory_image_of<vxl_byte> i1;
  convert_grey_memory_image(image0,i0);
  convert_grey_memory_image(image1,i1);
  result=lines_correlation(line0, line1, homo, i0, i1);
#if 0 // TODO ?
  result=dist(t_line0, line1);
  double alpha=0.5;
  result=result*alpha+(1-alpha);
#endif
}
