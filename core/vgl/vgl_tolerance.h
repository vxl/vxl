#ifndef vgl_tolerance_h_
#define vgl_tolerance_h_

//! \file
//  \author Kieran O'Mahony
//  \date 21 August 2007
//  \brief Tolerances used throughout vgl when performing comparisons
#include <vgl/vgl_export.h>
#include <vcl_compiler_detection.h>

template <typename T>
class vgl_tolerance
{
  public:
    //! Tolerance for judging 4 points to be coplanar
    static VGL_EXPORT const T point_3d_coplanarity;

    //! Tolerance for judging positions to be equal
    static VGL_EXPORT const T position;
};

extern template class vgl_tolerance<double>;
extern template class vgl_tolerance<float>;
extern template class vgl_tolerance<int>;
extern template class vgl_tolerance<unsigned int>;

#endif
