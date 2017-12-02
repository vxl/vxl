// This is core/vgl/algo/vgl_fit_conics_2d.hxx
#ifndef vgl_fit_conics_2d_hxx_
#define vgl_fit_conics_2d_hxx_
//:
// \file

#include <iostream>
#include "vgl_fit_conics_2d.h"
#include <vgl/vgl_vector_2d.h>
#include <vgl/algo/vgl_conic_2d_regression.h>
#include <vcl_compiler.h>
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
  std::cout << p << '\n';
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
  if (center.ideal(static_cast<T>(1e-06)))
  {
    std::cout << "Can't output a conic at infinity in vgl_fit_conics<T>\n";
    return;
  }

  //Need to establish the sense of the fitted curve
  //The curve should proceed in a counter-clockwise direction from p1 to p2.
  // 1) choose a point in the middle of the curve
  int middle_index = (end_index-1-start_index)/2 + start_index;
  if (middle_index == static_cast<int>(start_index))
    middle_index = end_index-1;
  vgl_point_2d<T> pm = curve_[middle_index];
  // 2) construct a vector from the middle to ps
  vgl_point_2d<T> ps = curve_[start_index], pe = curve_[end_index-1];
  vgl_vector_2d<T> vms(ps.x()-pm.x(), ps.y()-pm.y());
  // 3) construct a vector from the middle to pe
  vgl_vector_2d<T> vme(pe.x()-pm.x(), pe.y()-pm.y());
  //The cross product should be negative and significant
  T cp = cross_product(vms, vme);

  //If not, exchange p1 and p2
  unsigned i1=start_index, i2 = end_index-1;
  if (cp > 1e-04)
  {
    i1 = end_index-1;
    i2 = start_index;
  }


  //   unsigned i1=start_index, i2 = end_index-1;
  vgl_conic_segment_2d<T> e_seg(curve_[i1], curve_[i2], conic);
#ifdef DEBUG
  std::cout << "output " << e_seg << '\n';
#endif
  segs_.push_back(e_seg);
}

template <class T>
bool vgl_fit_conics_2d<T>::fit()
{
  if (curve_.size()<min_length_)
  {
    std::cout << "In vgl_fit_conics_2d<T>::fit() - number of points < min_length "
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
#ifdef DEBUG
      std::cout << "Initial fit error " << reg.get_rms_sampson_error()
               << " for\n" << reg.conic() << '\n';
#endif
      bool below_error_tol = true;
      bool data_added = false;
      while (nf<cur_len&&below_error_tol)
      {
        vgl_point_2d<T>& p = curve_[nf];
        //if the point can be added without exeeding the threshold, do so
        T error = reg.get_rms_error_est(p);
        below_error_tol = error<tol_;
        if (below_error_tol)
        {
          reg.add_point(p);
          data_added = true;
          nf++;
#ifdef DEBUG
          std::cout << "Adding point " << p << "with estimated error "
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
          nf--;
          reg.remove_point(curve_[nf]);
        }
      else if (nf<cur_len)
      {
        reg.add_point(curve_[nf]);
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

#endif // vgl_fit_conics_2d_hxx_
