// This is gel/vmal/vmal_multi_view_data.cxx
#include "vmal_multi_view_data.h"

#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>

template <class T>
vmal_multi_view_data<T>::vmal_multi_view_data():
nbviews_(0)
{
}

template <class T>
vmal_multi_view_data<T>::vmal_multi_view_data(int nbviews):
nbviews_(nbviews),size_vect_ft_(0),matchnum_(-1),closed_track_(true),MVM(0)
{
  //all_pts=new vcl_vector<T>();
}

template <class T>
vmal_multi_view_data<T>::~vmal_multi_view_data()
{
}

template <class T>
void vmal_multi_view_data<T>::set_params(int nbviews)
{
  nbviews_=nbviews;
}

template <class T>
void vmal_multi_view_data<T>::set(int view_num,int matchnum,T feature)
{
  all_pts.push_back(feature);
  if (matchnum>matchnum_)
  {
    matchnum_=matchnum;
    NViewMatch temp_match(nbviews_);
    temp_match[view_num]=size_vect_ft_;
    MVM.push_back(temp_match);
  }else {
    MVM[matchnum][view_num]=size_vect_ft_;
  }
  size_vect_ft_++;
}

template <class T>
void vmal_multi_view_data<T>::set(int view_num,T feature)
{
  if ((matchnum_>=0)&& (view_num>=0) && (view_num<nbviews_))
  {
    all_pts.push_back(feature);
    MVM[matchnum_][view_num]=size_vect_ft_;
    size_vect_ft_++;
  }
}

template <class T>
void vmal_multi_view_data<T>::new_track()
{
  if (closed_track_)
  {
    matchnum_++;
    NViewMatch temp_match(nbviews_);
    MVM.push_back(temp_match);
    closed_track_=false;
  }
}

template <class T>
void vmal_multi_view_data<T>::close_track()
{
    closed_track_=true;
}


template <class T>
bool vmal_multi_view_data<T>::get_first_track(vcl_map<int,T,vcl_less<int> > & track)
{
  matchnum_=0;
  return get_next_track(track);
}

template <class T>
bool vmal_multi_view_data<T>::get_next_track(vcl_map<int,T,vcl_less<int> > & track)
{
  if (int(MVM.size())>matchnum_)
  {
    track.clear();
    for (unsigned int i=0;i <MVM[matchnum_].size();i++)
    {
      int value=MVM[matchnum_][i];
      if (value !=NViewMatch::nomatch)
      {
        T tmp_edge=all_pts[value];
        track.insert(typename vcl_map<int,T,vcl_less<int> >::value_type(i,tmp_edge));
      }
    }
    matchnum_++;
    return true;
  }
  else
    return false;
}

//put in point_vector all the points that have been detected and
//matched in at least two views in the view view_num
template <class T>
void vmal_multi_view_data<T>::get(int view_num,vcl_vector<T> &ft_vector)
{
  if ((view_num>=0) && (view_num<nbviews_))
  {
  ft_vector.clear();
  NViewMatches::iterator iter;
  vcl_cerr << "MVM->size():"<<MVM.size()<<vcl_endl;
  for (iter=MVM.begin();iter!=MVM.end();iter++)
  {
    int ft_num=(*iter)[view_num];
    if (ft_num!=NViewMatch::nomatch)
    {
      T temp_ft=all_pts[ft_num];
      ft_vector.push_back(temp_ft);
    }
  }
  }
}

//Set point_vector1 and point_vector2 to the coordinates of points that
//have been matched between thoses 2 views.
template <class T>
void vmal_multi_view_data<T>::get(int view_num1,int view_num2,
                  vcl_vector<T> &ft_vector1,
                  vcl_vector<T> &ft_vector2)
{
  if ((view_num1>=0) && (view_num1<nbviews_) &&
    (view_num2>=0) && (view_num2<nbviews_) && (view_num2!=view_num1))
  {
  ft_vector1.clear();
  ft_vector2.clear();
  NViewMatches::iterator iter;
  for (iter=MVM.begin();iter!=MVM.end();iter++)
  {
    int ft_num1=(*iter)[view_num1];
    int ft_num2=(*iter)[view_num2];
    if ((ft_num1 != NViewMatch::nomatch) && (ft_num2!=NViewMatch::nomatch))
    {
      T temp_ft1=all_pts[ft_num1];
      T temp_ft2=all_pts[ft_num2];
      ft_vector1.push_back(temp_ft1);
      ft_vector2.push_back(temp_ft2);
    }
  }
  }
}

template <class T>
void vmal_multi_view_data<T>::get(int view_num1,int view_num2,int view_num3,
                  vcl_vector<T> &ft_vector1,
                  vcl_vector<T> &ft_vector2,
                  vcl_vector<T> &ft_vector3)
{
  if ((view_num1>=0) && (view_num1<nbviews_) &&
    (view_num2>=0) && (view_num2<nbviews_) &&
    (view_num3>=0) && (view_num3<nbviews_) &&
    (view_num2!=view_num1) && (view_num2!=view_num3) && (view_num3!=view_num1))
  {
  ft_vector1.clear();
  ft_vector2.clear();
  ft_vector3.clear();

  NViewMatches::iterator iter;
  for (iter=MVM.begin();iter!=MVM.end();iter++)
  {
    int ft_num1=(*iter)[view_num1];
    int ft_num2=(*iter)[view_num2];
    int ft_num3=(*iter)[view_num3];
    if (ft_num1 != NViewMatch::nomatch &&
        ft_num2 != NViewMatch::nomatch &&
        ft_num3 != NViewMatch::nomatch)
    {
      T temp_ft1=all_pts[ft_num1];
      T temp_ft2=all_pts[ft_num2];
      T temp_ft3=all_pts[ft_num3];
      ft_vector1.push_back(temp_ft1);
      ft_vector2.push_back(temp_ft2);
      ft_vector3.push_back(temp_ft3);
    }
  }
  }
}

template <class T>
bool vmal_multi_view_data<T>::get_pred_match(int view_num,T obj,T & res)
{
  if ((view_num>=0) && (view_num<nbviews_-1))
  {
    int i,j;
    NViewMatches::iterator iter;
    for (iter=MVM.begin();iter!=MVM.end();iter++)
    {
      i=(*iter)[view_num];
      j=(*iter)[view_num+1];
      if ((i!=NViewMatch::nomatch) && (j!=NViewMatch::nomatch))
        if (*all_pts[j]==*obj)
        {
          res=all_pts[i];
          return true;
        }
    }
  }
  return false;
}

template <class T>
void vmal_multi_view_data<T>::remove(int view_num, T match)
{
  if ((view_num>0) && (view_num<nbviews_))
  {
    int i;
    NViewMatches::iterator iter;
    for (iter=MVM.begin();iter!=MVM.end();iter++)
    {
      i=(*iter)[view_num];
      if (i!=NViewMatch::nomatch)
        if (*all_pts[i]==*match)
        {
          (*iter)[view_num]= NViewMatch::nomatch;
          if (view_num==1)
          {
            MVM.erase(iter);
            matchnum_--;
          }
        }
    }
  }
}

template <class T>
vcl_ostream& vmal_multi_view_data<T>::print(vcl_ostream& str)
{
  for (unsigned int j=0;j<MVM.size();j++)
  {
    for (int i=0;i<nbviews_;i++)
      if (MVM[j][i]<0)
      str<<"  "<<MVM[j][i];
      else if (MVM[j][i]<10)
      str<<"   "<<MVM[j][i];
      else if (MVM[j][i]<100)
      str<<"  "<<MVM[j][i];
      else
      str<<" "<<MVM[j][i];
    str<<vcl_endl;
  }
  return str;
}

template class vmal_multi_view_data<vtol_vertex_2d_sptr>;
template class vmal_multi_view_data<vtol_edge_2d_sptr>;
