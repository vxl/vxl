// This is core/vil/algo/vil_find_peaks.h
#ifndef vil_find_peaks_h_
#define vil_find_peaks_h_
//:
// \file
// \brief Find peaks in image
// \author Tim Cootes

#include <vector>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: True if pixel at *im is strictly above 8 neighbours.
// \sa vil_is_plateau_3x3()
template <class T>
inline bool vil_is_peak_3x3(const T* im, std::ptrdiff_t i_step, std::ptrdiff_t j_step)
{
  T v = *im;
  return v > im[i_step]
      && v > im[-i_step]
      && v > im[j_step]
      && v > im[-j_step]
      && v > im[i_step+j_step]
      && v > im[i_step-j_step]
      && v > im[j_step-i_step]
      && v > im[-i_step-j_step];
}

//: Return (pi,pj) for all points in image strictly above their 8 neighbours
//  Compute position of all local peaks (pi[k],pj[k]) above given threshold value.
// \param clear_list  If true (the default) then empty lists before adding new examples
// \sa vil_find_plateaus_3x3()
// \relatesalso vil_image_view
template <class T>
inline void vil_find_peaks_3x3(std::vector<unsigned>& pi,
                               std::vector<unsigned>& pj,
                               const vil_image_view<T>& image,
                               const T& min_thresh,
                               bool clear_list=true)
{
  if (clear_list) {
    pi.resize(0);
    pj.resize(0);
  }
  const unsigned ni1=image.ni()-1,nj1=image.nj()-1;
  const std::ptrdiff_t istep = image.istep(),jstep=image.jstep();
  const T* row = image.top_left_ptr()+istep+jstep;
  for (unsigned j=1;j<nj1;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni1;++i,pixel+=istep)
      if (*pixel>=min_thresh && vil_is_peak_3x3(pixel,istep,jstep))
      { pi.push_back(i); pj.push_back(j); }
  }
}


//: Fit a paraboloid to a pixel and its 8 neighbors to interpolate the peak.
// \return true if the neighborhood produces a proper peak (not a saddle)
// return by reference the sub-pixel offsets from the pixel center
// \a dx and \a dy as well as the interpolated peak value \a val.
template<class T>
bool vil_interpolate_peak(const T* pixel,
                          std::ptrdiff_t istep, std::ptrdiff_t jstep,
                          double& dx, double& dy, double& val)
{
  dx=dy=0;
  //extract the neighborhood
  // +-----+-----+-----+
  // | p00 | p10 | p20 |
  // +-----+-----+-----+
  // | p01 | p11 | p21 |
  // +-----+-----+-----+
  // | p02 | p12 | p22 |
  // +-----+-----+-----+
  const T& p11 = *pixel;
  const T& p01 = *(pixel-istep);
  const T& p21 = *(pixel+istep);
  const T& p10 = *(pixel-jstep);
  const T& p12 = *(pixel+jstep);
  const T& p00 = *(&p10-istep);
  const T& p20 = *(&p10+istep);
  const T& p02 = *(&p12-istep);
  const T& p22 = *(&p12+istep);

  //Compute the 2nd order quadratic coefficients
  //      1/6 * [ -1  0 +1 ]
  // Ix =       [ -1  0 +1 ]
  //            [ -1  0 +1 ]
  double Ix =(-p00-p01-p02 +p20+p21+p22)/6.0;
  //      1/6 * [ -1 -1 -1 ]
  // Iy =       [  0  0  0 ]
  //            [ +1 +1 +1 ]
  double Iy =(-p00-p10-p20 +p02+p12+p22)/6.0;
  //      1/3 * [ +1 -2 +1 ]
  // Ixx =      [ +1 -2 +1 ]
  //            [ +1 -2 +1 ]
  double Ixx = ((p00+p01+p02 +p20+p21+p22)-2.0*(p10+p11+p12))/3.0;
  //      1/4 * [ +1  0 -1 ]
  // Ixy =      [  0  0  0 ]
  //            [ -1  0 +1 ]
  double Ixy = (p00+p22 -p02-p20)/4.0;
  //      1/3 * [ +1 +1 +1 ]
  // Iyy =      [ -2 -2 -2 ]
  //            [ +1 +1 +1 ]
  double Iyy = ((p00+p10+p20 +p02+p12+p22)-2.0*(p01+p11+p21))/3.0;

  //
  // The next bit is to find the extremum of the fitted surface by setting its
  // partial derivatives to zero. We need to solve the following linear system :
  // Given the fitted surface is
  // I(x,y) = Io + Ix x + Iy y + 1/2 Ixx x^2 + Ixy x y + 1/2 Iyy y^2
  // we solve for the maximum (x,y),
  //
  //  [ Ixx Ixy ] [ dx ] + [ Ix ] = [ 0 ]      (dI/dx = 0)
  //  [ Ixy Iyy ] [ dy ]   [ Iy ]   [ 0 ]      (dI/dy = 0)
  //
  double det = Ixx*Iyy - Ixy*Ixy;
  // det>0 corresponds to a true local extremum otherwise a saddle point
  if (det<=0)
    return false;

  dx = (Iy*Ixy - Ix*Iyy) / det;
  dy = (Ix*Ixy - Iy*Ixx) / det;
  // more than one pixel away
  if (dx > 1.0 || dx < -1.0 || dy > 1.0 || dy < -1.0)
    return false;

  double Io =(p00+p01+p02 +p10+p11+p12 +p20+p21+p22)/9.0;

  val = Io + (Ix + 0.5*Ixx*dx + Ixy*dy)*dx + (Iy + 0.5*Iyy*dy)*dy;

  return true;
}


//: Return sub-pixel (px,py,val) for all points in image strictly above their 8 neighbours
//  Interpolation sub-pixel position of all local peaks (px[k],py[k])
//  above given threshold value by fitting a paraboloid.
//  Interpolated peak values are returned in \a val.
// \param clear_list  If true (the default) then empty lists before adding new examples.
// \relatesalso vil_image_view
template <class T>
inline void vil_find_peaks_3x3_subpixel(std::vector<double>& px,
                                        std::vector<double>& py,
                                        std::vector<double>& val,
                                        const vil_image_view<T>& image,
                                        const T& min_thresh,
                                        bool clear_list=true)
{
  if (clear_list) {
    px.resize(0);
    py.resize(0);
    val.resize(0);
  }
  const unsigned ni1=image.ni()-1,nj1=image.nj()-1;
  const std::ptrdiff_t istep = image.istep(),jstep=image.jstep();
  const T* row = image.top_left_ptr()+istep+jstep;
  double dx,dy,v;
  for (unsigned j=1;j<nj1;++j,row+=jstep)
  {
    const T* pixel = row;
    for (unsigned i=1;i<ni1;++i,pixel+=istep)
      if (*pixel>=min_thresh && vil_is_peak_3x3(pixel,istep,jstep) &&
          vil_interpolate_peak(pixel,istep,jstep,dx,dy,v))
      {
        px.push_back(i+dx);
        py.push_back(j+dy);
        val.push_back(v);
      }
  }
}

#endif // vil_find_peaks_h_
