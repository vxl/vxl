// This is core/vgl/algo/vgl_fit_conics_2d.txx
#ifndef vgl_fit_conics_2d_txx_
#define vgl_fit_conics_2d_txx_
//:
// \file

#include "vgl_fit_conics_2d.h"
#include <vgl/vgl_vector_2d.h>
#include <vgl/algo/vgl_conic_2d_regression.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>

//--------------------------------------------------------------
//: Constructor
template <class T>
vgl_fit_conics_2d<T>::vgl_fit_conics_2d(const unsigned min_length,
                                        const T tol)
  :  min_length_(min_length), tol_(tol)
{
}

// == OPERATIONS ==

//: add point
template <class T>
void vgl_fit_conics_2d<T>::add_point(vgl_point_2d<T> const& p)
{
  curve_.push_back(p);
#ifdef DEBUG
  vcl_cout << p << '\n';
#endif
}

//: add point
template <class T>
void vgl_fit_conics_2d<T>::add_point(T x, T y)
{
  curve_.push_back(vgl_point_2d<T>(x, y));
}

//: clear
template <class T>
void vgl_fit_conics_2d<T>::clear()
{
  curve_.clear();
  segs_.clear();
}

template <class T>
void vgl_fit_conics_2d<T>::output(const unsigned start_index,
                                  const unsigned end_index,
                                  vgl_conic<T> const& conic)
{
  assert(start_index < curve_.size() && end_index <= curve_.size());

  vgl_homg_point_2d<T> center = conic.centre();
  if(center.ideal(static_cast<T>(1e-06)))
    {
      vcl_cout << "Can't output a conic at infinity in vgl_fit_conics<T>\n";
      return;
    }
  T cx = center.x()/center.w(), cy = center.y()/center.w();
  //Need to establish the sense of the fitted curve
  //The curve should proceed in a counter-clockwise direction from p1 to p2.
  // construct a vector at p1
  vgl_vector_2d<T> v1(curve_[start_index].x()-cx, curve_[start_index].y()-cy);
  //choose a point in the middle of the curve
  int middle_index = (end_index-1-start_index)/2 + start_index;
  if(middle_index == static_cast<int>(start_index))
    middle_index = end_index-1;
  //construct a vector at that point
  vgl_vector_2d<T> v(curve_[middle_index].x()-cx, curve_[middle_index].y()-cy);
  //The cross product should be positive
  T cp = cross_product(v1, v);

  //If not, exchange p1 and p2
  unsigned i1=start_index, i2 = end_index-1;
  if(cp< static_cast<T>(0))
    {
      i1 = end_index-1;
      i2 = start_index;
    }

  //   unsigned i1=start_index, i2 = end_index-1;
  vgl_conic_segment_2d<T> e_seg(curve_[i1], curve_[i2],
                                conic);
#ifdef DEBUG
  vcl_cout << "output " << e_seg << '\n';
#endif
  segs_.push_back(e_seg);
}

template <class T>
bool vgl_fit_conics_2d<T>::fit()
{
  if (curve_.size()<min_length_)
  {
    vcl_cout << "In vgl_fit_conics_2d<T>::fit() - number of points < min_length "
             << min_length_ << '\n';
    return false;
  }
  //A helper to hold points and do the linear regression
  vgl_conic_2d_regression<T> reg;

  // Start at the beginning of the curve with
  // a segment with minimum number of points
  unsigned int ns = 0, nf = min_length_, cur_len = curve_.size();
  for (unsigned int i = ns; i<nf; ++i)
    reg.add_point(curve_[i]);
  //The main loop
  while (nf<=cur_len)
  {
    if (reg.fit()&&reg.get_rms_sampson_error()<tol_)
    {
      if (nf==cur_len)
      {
        output(ns, nf, reg.conic());
        return true;
      }
#if DEBUG
      vcl_cout << "Initial fit error " << reg.get_rms_sampson_error() 
               << " for \n" << reg.conic() << '\n';
#endif
      bool below_error_tol = true;
      bool data_added = false;
      while (nf<cur_len&&below_error_tol)
      {
        vgl_point_2d<T>& p = curve_[nf];
        //if the point can be added without exeeding the threshold, do so
        double error = reg.get_rms_error_est(p);
        below_error_tol = error<tol_;
        if (below_error_tol)
        {
          reg.add_point(p);
          data_added = true;
          nf++;
#if DEBUG
          vcl_cout << "Adding point " << p << "with estimated error " 
                   << error << '\n';
#endif
        }
      }
       //if no points were added output the conic
       //and initialize a new fit
      if (!data_added)
      {
        output(ns, nf, reg.conic());
        ns = nf-1; nf=ns+min_length_;
        if (nf<=cur_len)
        {
          reg.clear_points();
          for (unsigned int i = ns; i<nf; i++)
            reg.add_point(curve_[i]);
        }
      }
    }
        // Else the fit is not good enough. We therefore remove the first
    // point and add or delete points from the end of the current line
    // segment until the resulting segment length is _min_fit_length
    else
    {
      reg.remove_point(curve_[ns]);
      ns++;
      if (reg.get_n_pts()>min_length_)
        while (reg.get_n_pts()>min_length_+1)
        {
          reg.remove_point(curve_[nf]);
          nf--;
        }
      else if (nf<cur_len)
      {
        reg.remove_point(curve_[nf]);
        nf++;
      }
      else
        nf++;
    }
  }
  return true;
}
//--------------------------------------------------------------------------
#undef VGL_FIT_CONICS_2D_INSTANTIATE
#define VGL_FIT_CONICS_2D_INSTANTIATE(T) \
template class vgl_fit_conics_2d<T >

#endif // vgl_fit_conics_2d_txx_
