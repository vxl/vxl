// This is core/vgl/algo/vgl_fit_lines_2d.txx
#ifndef vgl_fit_lines_2d_txx_
#define vgl_fit_lines_2d_txx_
//:
// \file

#include "vgl_fit_lines_2d.h"
#include <vgl/algo/vgl_line_2d_regression.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>

//--------------------------------------------------------------
//: Constructor
template <class T>
vgl_fit_lines_2d<T>::vgl_fit_lines_2d(unsigned int min_length, T tol)
{
  min_length_ = min_length;
  tol_ = tol;
}

// == OPERATIONS ==

//: add point
template <class T>
void vgl_fit_lines_2d<T>::add_point(vgl_point_2d<T> const& p)
{
  curve_.push_back(p);
#ifdef DEBUG
  vcl_cout << p << '\n';
#endif
}

//: add point
template <class T>
void vgl_fit_lines_2d<T>::add_point(T x, T y)
{
  curve_.push_back(vgl_point_2d<T>(x, y));
}

//: clear
template <class T>
void vgl_fit_lines_2d<T>::clear()
{
  curve_.clear();
  segs_.clear();
}

template <class T>
void vgl_fit_lines_2d<T>::output(unsigned int start_index, unsigned int end_index)
{
  assert(start_index < curve_.size() && end_index <= curve_.size());
  vgl_line_segment_2d<T> line(curve_[start_index], curve_[end_index-1]);
#ifdef DEBUG
  vcl_cout << "output " << line << '\n';
#endif
  segs_.push_back(line);
}

template <class T>
bool vgl_fit_lines_2d<T>::fit()
{
  if (curve_.size()<min_length_)
  {
    vcl_cout << "In vgl_fit_lines_2d<T>::fit() - number of points < min_length "
             << min_length_ << '\n';
    return false;
  }
  //A helper to hold points and do the linear regression
  vgl_line_2d_regression<T> reg;
  // Start at the beginning of the curve with
  // a segment with minimum number of points
  unsigned int ns = 0, nf = min_length_, cur_len = curve_.size();
  for (unsigned int i = ns; i<nf; ++i)
    reg.increment_partial_sums(curve_[i].x(), curve_[i].y());
  //The main loop
  while (nf<=cur_len)
  {
    reg.fit();
    reg.init_rms_error_est();
    if (reg.get_rms_error()<tol_)
    {
      if (nf==cur_len)
      {
        output(ns, nf);
        return true;
      }
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
          reg.increment_partial_sums(p.x(), p.y());
          data_added = true;
          nf++;
        }
      }
       //if no points were added output the line
       //and initialize a new fit
      if (!data_added)
      {
        output(ns, nf);
        ns = nf-1; nf=ns+min_length_;
        if (nf<=cur_len)
        {
          reg.clear();
          for (unsigned int i = ns; i<nf; i++)
            reg.increment_partial_sums(curve_[i].x(), curve_[i].y());
        }
      }
    }
    // Else the fit is not good enough. We therefore remove the first
    // point and add or delete points from the end of the current line
    // segment until the resulting segment length is _min_fit_length
    else
    {
      reg.decrement_partial_sums(curve_[ns].x(), curve_[ns].y());
      ns++;
      if (reg.get_n_pts()>min_length_)
        while (reg.get_n_pts()>min_length_+1)
        {
          reg.decrement_partial_sums(curve_[nf].x(), curve_[nf].y());
          nf--;
        }
      else if (nf<cur_len)
      {
        reg.increment_partial_sums(curve_[nf].x(), curve_[nf].y());
        nf++;
      }
      else
        nf++;
    }
  }
  return true;
}

//--------------------------------------------------------------------------
#undef VGL_FIT_LINES_2D_INSTANTIATE
#define VGL_FIT_LINES_2D_INSTANTIATE(T) \
template class vgl_fit_lines_2d<T >

#endif // vgl_fit_lines_2d_txx_
