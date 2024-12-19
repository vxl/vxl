// This is core/vil/algo/vil_2nd_ deriv_3x3.hxx
#ifndef vil_2nd_deriv_3x3_hxx_
#define vil_2nd_deriv_3x3_hxx_
//:
// \file
// \brief Apply 2nd derivative filter to an image
// \author J.L. Mundy

#include "vil_2nd_deriv_3x3.h"

//: Compute 2nd derivatives of an image using 3x3 filters
//  Computes 2nd derivatives with respect to i,j and ij of an ni x nj plane of data
//  1 pixel border around 2nd derivative images is set to zero
// \relatesalso vil_image_view
template <class srcT, class destT>
void
vil_2nd_deriv_3x3(const vil_image_view<srcT> & src, vil_image_view<destT> & d2I_dij)
{
  int np = src.nplanes();
  int ni = src.ni();
  int nj = src.nj();
  d2I_dij.set_size(ni, nj, 3 * np);
  for (int p = 0; p < np; ++p)
  {
    vil_2nd_deriv_3x3_1plane(src.top_left_ptr() + p * src.planestep(),
                             src.istep(),
                             src.jstep(),
                             d2I_dij.top_left_ptr() + 2 * p * d2I_dij.planestep(),
                             d2I_dij.istep(),
                             d2I_dij.jstep(),
                             d2I_dij.top_left_ptr() + (2 * p + 1) * d2I_dij.planestep(),
                             d2I_dij.istep(),
                             d2I_dij.jstep(),
                             d2I_dij.top_left_ptr() + (2 * p + 2) * d2I_dij.planestep(),
                             d2I_dij.istep(),
                             d2I_dij.jstep(),
                             ni,
                             nj);
  }
}

//: Apply 2nd_deriv 3x3 filter to 2D image
template <class srcT, class destT>
void
vil_2nd_deriv_3x3(const vil_image_view<srcT> & src,
                  vil_image_view<destT> & d2I_di2,
                  vil_image_view<destT> & d2I_dj2,
                  vil_image_view<destT> & d2I_didj)
{

  int np = src.nplanes();
  int ni = src.ni();
  int nj = src.nj();
  d2I_di2.set_size(ni, nj, np);
  d2I_dj2.set_size(ni, nj, np);
  d2I_didj.set_size(ni, nj, np);
  for (int p = 0; p < np; ++p)
  {
    vil_2nd_deriv_3x3_1plane(src.top_left_ptr() + p * src.planestep(),
                             src.istep(),
                             src.jstep(),
                             d2I_di2.top_left_ptr() + p * d2I_di2.planestep(),
                             d2I_di2.istep(),
                             d2I_di2.jstep(),
                             d2I_dj2.top_left_ptr() + p * d2I_dj2.planestep(),
                             d2I_dj2.istep(),
                             d2I_dj2.jstep(),
                             d2I_didj.top_left_ptr() + p * d2I_didj.planestep(),
                             d2I_didj.istep(),
                             d2I_didj.jstep(),
                             ni,
                             nj);
  }
}
//: run 2nd_deriv 3x3  filter on a single plane of an image
//  Computes  i j  and ij dirivatives of an ni x nj plane of data
template <class srcT, class destT>
void
vil_2nd_deriv_3x3_1plane(srcT * const src,
                         std::ptrdiff_t s_istep,
                         std::ptrdiff_t s_jstep,
                         destT * d2Idi,
                         std::ptrdiff_t d2Idi_istep,
                         std::ptrdiff_t d2Idi_jstep,
                         destT * d2Idj,
                         std::ptrdiff_t d2Idj_istep,
                         std::ptrdiff_t d2Idj_jstep,
                         destT * d2I_didj,
                         std::ptrdiff_t d2I_didj_istep,
                         std::ptrdiff_t d2I_didj_jstep,
                         unsigned ni,
                         unsigned nj)
{
  const destT k125 = static_cast<destT>(0.125);
  const destT k25 = static_cast<destT>(0.25);
  const destT k5 = static_cast<destT>(0.5);
  const destT zero = static_cast<destT>(0.0);

  const srcT * s_data = src;
  destT * d2i_data = d2Idi;
  destT * d2j_data = d2Idj;
  destT * d2ij_data = d2I_didj;

  if (ni == 0 || nj == 0)
    return;
  if (ni == 1)
  {
    // Zero the elements in the column
    for (unsigned j = 0; j < nj; ++j)
    {
      *d2i_data = zero;
      *d2j_data = zero;
      *d2ij_data = zero;
      d2i_data += d2Idi_jstep;
      d2j_data += d2Idj_jstep;
      d2ij_data += d2I_didj_jstep;
    }
    return;
  }
  if (nj == 1)
  {
    // Zero the elements in the column
    for (unsigned i = 0; i < ni; ++i)
    {
      *d2i_data = zero;
      *d2j_data = zero;
      *d2ij_data = zero;
      d2i_data += d2Idi_istep;
      d2j_data += d2Idj_istep;
      d2ij_data += d2I_didj_istep;
    }
    return;
  }

  // Compute relative grid positions
  //  o1 o2 o3
  //  o4 o0 o5
  //  o6 o7 o8
  const std::ptrdiff_t o0 = 0;
  const std::ptrdiff_t o1 = s_jstep - s_istep;
  const std::ptrdiff_t o2 = s_jstep;
  const std::ptrdiff_t o3 = s_istep + s_jstep;
  const std::ptrdiff_t o4 = -s_istep;
  const std::ptrdiff_t o5 = s_istep;
  const std::ptrdiff_t o6 = -s_istep - s_jstep;
  const std::ptrdiff_t o7 = -s_jstep;
  const std::ptrdiff_t o8 = s_istep - s_jstep;

  const unsigned ni1 = ni - 1;
  const unsigned nj1 = nj - 1;

  s_data += s_istep + s_jstep;
  d2i_data += d2Idi_jstep;
  d2j_data += d2Idj_jstep;
  d2ij_data += d2I_didj_jstep;
  destT * d2i;
  destT * d2j;
  destT * d2ij;
  for (unsigned j = 1; j < nj1; ++j)
  {
    const srcT * s = s_data;
    d2i = d2i_data;
    d2j = d2j_data;
    d2ij = d2ij_data;

    // Zero the first elements in the rows
    *d2i = 0;
    d2i += d2Idi_istep;
    *d2j = 0;
    d2j += d2Idj_istep;
    *d2ij = 0;
    d2ij += d2I_didj_istep;
    for (unsigned i = 1; i < ni1; ++i)
    {
      // Compute 2nd derivative rt. i
      //           | k125  -k25  k125|
      // d2I_di2 = | k25   -k5   k25 |
      //           | k125  -k25  k125|
      // Note: Multiply each element individually
      //      to ensure conversion to destT before addition
      *d2i = k125 * static_cast<destT>(s[o1]) - k25 * static_cast<destT>(s[o2]) + k125 * static_cast<destT>(s[o3]) +
             k25 * static_cast<destT>(s[o4]) - k5 * static_cast<destT>(s[o0]) + k25 * static_cast<destT>(s[o5]) +
             k125 * static_cast<destT>(s[o6]) - k25 * static_cast<destT>(s[o7]) + k125 * static_cast<destT>(s[o8]);

      // Compute 2nd derivative rt. j
      //           | k125  k25  k125|
      // d2I_dj2 = | -k25  -k5 -k25 |
      //           | k125  k25  k125|
      //
      *d2j = k125 * static_cast<destT>(s[o1]) + k25 * static_cast<destT>(s[o2]) + k125 * static_cast<destT>(s[o3]) -
             k25 * static_cast<destT>(s[o4]) - k5 * static_cast<destT>(s[o0]) - k25 * static_cast<destT>(s[o5]) +
             k125 * static_cast<destT>(s[o6]) + k25 * static_cast<destT>(s[o7]) + k125 * static_cast<destT>(s[o8]);

      // Compute 2nd derivative rt. i and j
      //           |  k25  0 -k25|
      // d2I_dj2 = |   0   0   0 |
      //           | -k25  0  k25|
      //
      *d2ij = k25 * static_cast<destT>(s[o1]) - k25 * static_cast<destT>(s[o3]) - k25 * static_cast<destT>(s[o6]) +
              k25 * static_cast<destT>(s[o8]);

      s += s_istep;
      d2i += d2Idi_istep;
      d2j += d2Idj_istep;
      d2ij += d2I_didj_istep;
    }

    // Zero the last elements in the rows
    *d2i = zero;
    *d2j = zero;
    *d2ij = zero;
    // Move to next row
    s_data += s_jstep;
    d2i_data += d2Idi_jstep;
    d2j_data += d2Idj_jstep;
    d2ij_data += d2I_didj_jstep;
  }

  // Zero the first and last rows
  for (unsigned i = 0; i < ni; ++i)
  {
    *d2i = zero;
    d2i += d2Idi_istep;
    *d2j = zero;
    d2j += d2Idj_istep;
    *d2ij = zero;
    d2ij += d2I_didj_istep;
    *d2i_data = zero;
    d2i_data += d2Idi_istep;
    *d2j_data = zero;
    d2j_data += d2Idj_istep;
    *d2ij_data = zero;
    d2ij_data += d2I_didj_istep;
  }
}


#undef VIL_2ND_DERIV_3X3_INSTANTIATE
#define VIL_2ND_DERIV_3X3_INSTANTIATE(srcT, destT)                                                    \
  void vil_2nd_deriv_3x3_1plane(srcT * const src,                                                     \
                                std::ptrdiff_t s_istep,                                               \
                                std::ptrdiff_t s_jstep,                                               \
                                destT * d2Idi,                                                        \
                                std::ptrdiff_t d2Idi_istep,                                           \
                                std::ptrdiff_t d2Idi_jstep,                                           \
                                destT * d2Idj,                                                        \
                                std::ptrdiff_t d2Idj_istep,                                           \
                                std::ptrdiff_t d2Idj_jstep,                                           \
                                destT * d2I_didj,                                                     \
                                std::ptrdiff_t d2I_didj_istep,                                        \
                                std::ptrdiff_t d2I_didj_jstep,                                        \
                                unsigned ni,                                                          \
                                unsigned nj);                                                         \
  template void vil_2nd_deriv_3x3(const vil_image_view<srcT> & src, vil_image_view<destT> & d2I_dij); \
  template void vil_2nd_deriv_3x3(const vil_image_view<srcT> & src,                                   \
                                  vil_image_view<destT> & d2I_di2,                                    \
                                  vil_image_view<destT> & d2I_dj2,                                    \
                                  vil_image_view<destT> & d2I_didj)


#endif // vil_2nd_deriv_3x3_hxx_
