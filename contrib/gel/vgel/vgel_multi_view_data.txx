#ifndef vgel_multi_view_data_txx_
#define vgel_multi_view_data_txx_

#include "vgel_multi_view_data.h"
#include <vcl_iostream.h>

template <class T>
vgel_multi_view_data<T>::vgel_multi_view_data():
nbviews_(0)
{
}

template <class T>
vgel_multi_view_data<T>::vgel_multi_view_data(int nbviews):
nbviews_(nbviews),size_vect_ft_(0),matchnum_(-1),closed_track_(true),MVM(0)
{
  //all_pts=new vcl_vector<T>();
}

template <class T>
vgel_multi_view_data<T>::~vgel_multi_view_data()
{
}

template <class T>
void vgel_multi_view_data<T>::set_params(int nbviews)
{
  nbviews_=nbviews;
}

template <class T>
void vgel_multi_view_data<T>::set(int view_num,int matchnum,T feature)
{
  all_pts.push_back(feature);
  //vcl_cerr << "Matchnum:" <<matchnum<<vcl_endl;
  //vcl_cerr << "Point number:" <<size_vect_pt_<<vcl_endl;
  if (matchnum>matchnum_)
  {
    matchnum_=matchnum;
    NViewMatch temp_match(nbviews_);
    temp_match[view_num]=size_vect_ft_;
    MVM.push_back(temp_match);
  }
  else
    MVM[matchnum][view_num]=size_vect_ft_;

  size_vect_ft_++;
}

template <class T>
void vgel_multi_view_data<T>::set(int view_num,T feature)
{
  if ((matchnum_>=0)&& (view_num>=0) && (view_num<nbviews_))
  {
    all_pts.push_back(feature);
    MVM[matchnum_][view_num]=size_vect_ft_;
    size_vect_ft_++;
  }
}

template <class T>
void vgel_multi_view_data<T>::new_track()
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
void vgel_multi_view_data<T>::close_track()
{
  closed_track_=true;
}

//put in point_vector all the points that have been detected and
//matched in at least two views in the view view_num
template <class T>
void vgel_multi_view_data<T>::get(int view_num,vcl_vector<T> &ft_vector)
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
void vgel_multi_view_data<T>::get(int view_num1,int view_num2,
                                  vcl_vector<T> &ft_vector1,
                                  vcl_vector<T> &ft_vector2)
{
  if ((view_num1>=0) && (view_num1<nbviews_) &&
      (view_num2>=0) && (view_num2<nbviews_) &&
      (view_num2!=view_num1))
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
void vgel_multi_view_data<T>::get(int view_num1,int view_num2,int view_num3,
                                  vcl_vector<T> &ft_vector1,
                                  vcl_vector<T> &ft_vector2,
                                  vcl_vector<T> &ft_vector3)
{
  if ((view_num1>=0) && (view_num1<nbviews_) &&
      (view_num2>=0) && (view_num2<nbviews_) &&
      (view_num3>=0) && (view_num3<nbviews_) &&
      (view_num2!=view_num1) &&
      (view_num2!=view_num3) &&
      (view_num3!=view_num1))
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
bool vgel_multi_view_data<T>::get_pred_match(int view_num,T obj,T & res)
{
  if ((view_num>=0) && (view_num<nbviews_-1))
  {
    NViewMatches::iterator iter;
    for (iter=MVM.begin();iter!=MVM.end();iter++)
    {
      int i=(*iter)[view_num];
      int j=(*iter)[view_num+1];
      if ((i!=NViewMatch::nomatch) && (j!=NViewMatch::nomatch))
        if (all_pts[j]==obj)
        {
          res=all_pts[i];
          return true;
        }
    }
  }
  return false;
}

template <class T>
void vgel_multi_view_data<T>::remove(int view_num, T match)
{
  if ((view_num>=0) && (view_num<nbviews_-1))
  {
    NViewMatches::iterator iter;
    for (iter=MVM.begin();iter!=MVM.end();iter++)
    {
      int i=(*iter)[view_num];
      if (i!=NViewMatch::nomatch)
        if (all_pts[i]==match)
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
vcl_ostream& vgel_multi_view_data<T>::print(vcl_ostream& str)
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

#endif // vgel_multi_view_data_txx_
