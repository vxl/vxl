#ifndef vil3d_quad_distance_function_h_
#define vil3d_quad_distance_function_h_
//:
//  \file
//  \brief Functions to compute quadratic distance functions
//  \author Tim Cootes

#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_plane.h>
#include <vil/algo/vil_quad_distance_function.h>
#include <vil3d/vil3d_switch_axes.h>

//: Apply quadratic distance transform along each row of src
//
//  dest(x,y,z)=min_i (src(x+i,y,z)+ai(i^2))
template<class srcT, class destT>
inline void vil3d_quad_distance_function_i(const vil3d_image_view<srcT>& src,
                                           double ai,
                                           vil3d_image_view<destT>& dest)
{
  assert(src.nplanes()==1);
  unsigned int ni=src.ni(),nj=src.nj(),nk=src.nk();
  dest.set_size(ni,nj,nk);
  vcl_ptrdiff_t s_istep = src.istep(),   s_jstep = src.jstep(), s_kstep = src.kstep();
  vcl_ptrdiff_t d_istep = dest.istep(),  d_jstep = dest.jstep(), d_kstep = dest.kstep();

  const srcT* s_plane = src.origin_ptr();
  destT*      d_plane = dest.origin_ptr();

  vcl_vector<double> x,y,z;

  // Apply transform along i direction
  for (unsigned k=0;k<nk;++k, s_plane+=s_kstep, d_plane+=d_kstep)
  {
    const srcT* s_row = s_plane;
    destT* d_row = d_plane;
    for (unsigned int j=0;j<nj;++j, s_row+=s_jstep, d_row+=d_jstep)
    {
      vil_quad_envelope(s_row,s_istep,ni,x,y,z,ai);
      vil_sample_quad_envelope(x,y,z,ai,d_row,d_istep,ni);
    }
  }
}


//: Apply quadratic distance transform
//  dest(x,y,z)=min_i,j,k (src(x+i,y+j,z+k)+ai(i^2)+aj(j^2)+ak(k^2))
template<class srcT, class destT>
inline void vil3d_quad_distance_function(const vil3d_image_view<srcT>& src,
                                       double ai, double aj, double ak,
                                       vil3d_image_view<destT>& dest)
{
  assert(src.nplanes()==1);

  unsigned int ni=src.ni(),nj=src.nj(),nk=src.nk();
  vil3d_image_view<destT> tmp(ni,nj,nk);  // Intermediate result

  // Apply along i and store in dest temporarily
  vil3d_quad_distance_function_i(src,ai,dest);

  // Apply along j by switching axes, storing result in tmp
  vil3d_image_view<destT> tmp_j1 = vil3d_switch_axes_jik(dest);
  vil3d_image_view<destT> tmp_j2 = vil3d_switch_axes_jik(tmp);
  vil3d_quad_distance_function_i(tmp_j1,aj,tmp_j2);

  // Apply along k by switching axes, storing result in dest
  vil3d_image_view<destT> tmp_k1 = vil3d_switch_axes_kij(tmp);
  vil3d_image_view<destT> tmp_k2 = vil3d_switch_axes_kij(dest);
  vil3d_quad_distance_function_i(tmp_k1,ak,tmp_k2);
}

//: Apply quadratic distance transform along i, storing relative position of minima
//  dest(x,y,z)=min_i (src(x+i,y,z)+ai(i^2))
//  (pos(x,y,0)) gives the position (x+i,y,z) leading to minima
template<class srcT, class destT, class posT>
inline void vil3d_quad_distance_function_i(const vil3d_image_view<srcT>& src,
                                           double ai,
                                           vil3d_image_view<destT>& dest,
                                           vil3d_image_view<posT>& pos)
{
  assert(src.nplanes()==1);
  unsigned int ni=src.ni(), nj=src.nj(), nk=src.nk();
  dest.set_size(ni,nj,nk);
  pos.set_size(ni,nj,nk,1);
  vcl_ptrdiff_t s_istep = src.istep(),  s_jstep = src.jstep(),  s_kstep = src.kstep();
  vcl_ptrdiff_t d_istep = dest.istep(), d_jstep = dest.jstep(), d_kstep = dest.kstep();
  vcl_ptrdiff_t  p_istep = pos.istep(), p_jstep = pos.jstep(),  p_kstep = pos.kstep();

  const srcT* s_plane = src.origin_ptr();
  destT*      d_plane = dest.origin_ptr();
  posT*       p_plane  = pos.origin_ptr();

  vcl_vector<double> x,y,z;

  // Apply transform along i direction to get tmp
  for (unsigned k=0;k<nk;++k,s_plane+=s_kstep,d_plane+=d_kstep,p_plane+=p_kstep)
  {
    const srcT* s_row = s_plane;
    destT*      d_row = d_plane;
    posT*       p_row = p_plane;
    for (unsigned j=0;j<nj;++j,s_row+=s_jstep,d_row+=d_jstep,p_row+=p_jstep)
    {
     vil_quad_envelope(s_row,s_istep,ni,x,y,z,ai);
     vil_sample_quad_envelope_with_pos(x,y,z,ai,d_row,d_istep,ni,p_row,p_istep);
    }
  }
}

//: Apply quadratic distance transform, storing relative position of minima
//  dest(x,y,z)=min_i,j,k (src(x+i,y+j,z+k)+ai(i^2)+aj(j^2)+ak(k^2))
//  (pos(x,y,0),pos(x,y,1),pos(x,y,2))
//  gives the position (x+i,y+j,z+k) leading to minima
template<class srcT, class destT, class posT>
inline void vil3d_quad_distance_function(const vil3d_image_view<srcT>& src,
                                       double ai, double aj, double ak,
                                       vil3d_image_view<destT>& dest,
                                       vil3d_image_view<posT>& pos)
{
  assert(src.nplanes()==1);
  unsigned int ni=src.ni(), nj=src.nj(), nk=src.nk();
  pos.set_size(ni,nj,nk,3);

  // Apply transformation along i
  vil3d_image_view<destT> tmp_i(ni,nj,nk);
  vil3d_image_view<posT>  pos_i(ni,nj,nk);
  vil3d_quad_distance_function_i(src,ai,tmp_i,pos_i);

  // Apply transformation along j by switching axes
  vil3d_image_view<destT> tmp_j(ni,nj,nk);
  vil3d_image_view<posT>  pos_j(ni,nj,nk);
  vil3d_image_view<destT> tmp_i1 = vil3d_switch_axes_jik(tmp_i);
  vil3d_image_view<destT> tmp_j1 = vil3d_switch_axes_jik(tmp_j);
  vil3d_image_view<posT>  pos_j1 = vil3d_switch_axes_jik(pos_j);
  vil3d_quad_distance_function_i(tmp_i1,aj,tmp_j1,pos_j1);

  // Apply transformation along k by switching axes
  dest.set_size(ni,nj,nk);
  vil3d_image_view<posT>  pos_k = vil3d_plane(pos,2);
  vil3d_image_view<destT> tmp_k = vil3d_switch_axes_kij(tmp_j);
  vil3d_image_view<destT> dest_k = vil3d_switch_axes_kij(dest);
  vil3d_image_view<posT>  pos_k1 = vil3d_switch_axes_kij(pos_k);
  vil3d_quad_distance_function_i(tmp_k,ak,dest_k,pos_k1);

  // Now deduce the relative positions
  for (unsigned k=0;k<nk;++k)
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i)
      {
        pos(i,j,k,1)=pos_j(i,j,pos_k(i,j,k));
        pos(i,j,k,0)=pos_i(i,pos(i,j,k,1),pos_k(i,j,k));
      }
}

#endif
