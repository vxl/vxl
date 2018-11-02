// This is mul/mbl/mbl_linear_interpolator.cxx
#include <iostream>
#include <limits>
#include "mbl_linear_interpolator.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_index_sort.h>


mbl_linear_interpolator::mbl_linear_interpolator()
{
  clear();
}

void mbl_linear_interpolator::clear()
{
  x_.resize(0);
  y_.resize(0);
}

bool mbl_linear_interpolator::set(const std::vector<double> &x, const std::vector<double> &y)
{
  bool ret=false;
  clear();

  if (x.size()==y.size() && x.size()>0)
  {
    x_=x;
    y_=y;
    sort();
    ret=true;
  }

  return ret;
}

void mbl_linear_interpolator::sort()
{
   std::vector<int> index;
   mbl_index_sort(x_,index);
   std::vector<double> tmp_x=x_;
   std::vector<double> tmp_y=y_;

   for (unsigned i=0;i<index.size();++i)
   {
     x_[i]=tmp_x[index[i]];
     y_[i]=tmp_y[index[i]];
   }

}


double mbl_linear_interpolator::y(double x) const
{
  double yval=std::numeric_limits<double>::quiet_NaN();

  if (x_.size()>0)
  {
    if (x<=x_.front())
      yval=y_.front();
    else if (x>=x_.back())
      yval=y_.back();
    else
    {
      for (unsigned i=1;i<x_.size();++i)
      {
        if (x<x_[i])
        {
          double x1=x_[i-1];
          double x2=x_[i];
          double y1=y_[i-1];
          double y2=y_[i];
          double f= (x-x1)/(x2-x1);
          yval=y1+f*(y2-y1);
          break;
        }
      }
    }
  }

  return yval;
}







