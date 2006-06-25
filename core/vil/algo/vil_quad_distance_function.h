#ifndef vil_quad_distance_function_h_
#define vil_quad_distance_function_h_
//:
//  \file
//  \brief Functions to compute quadratic distance functions
//  \author Tim Cootes

#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>

//: Add parabola y=y0+(x-x0)^2 to lower envelope defined by (x,y,z)
//  Parabolas are  y' = y[i]+a(x'-x[i])^2
//  Parabola i defines the envelope in the range (z[i],z[i+1]).
inline void vil_update_parabola_set(vcl_vector<double>& x,
                                    vcl_vector<double>& y,
                                    vcl_vector<double>& z, double a,
                                    double x0, double y0, double n)
{
  unsigned int k=x.size()-1;
  while (true)
  {
    // Compute intercept of parabola centred at x0 with that at v[k]
    // New parabola is below old for all x'>s
    double new_z = 0.5*((y0-y[k])/a + (x0*x0-x[k]*x[k]))/(x0-x[k]);
    if (new_z>n) return;  // Only useful outsize (0,n) so discard
    if (new_z>z[k])
    {
      x.push_back(x0);
      y.push_back(y0);
      z.push_back(new_z);
      return;
    }
    if (k==0)   // new_z<=0.0 since z[0]=0.0
    {
      // This parabola dominates in valid region so far
      x[0]=x0; y[0]=y0; z[0]=0.0;
      return;
    }

    // Remove parabola k and repeat
    x.erase(x.begin()+k);
    y.erase(y.begin()+k);
    z.erase(z.begin()+k);
    --k;
  }
}

//: Compute parabolas forming lower envelope from set over range [0,n)
//  Set of parabolas  y=src[i*s_step]+a(x-i)^2    i=0..n-1
//  Select those defining lower envelope in range [0,n)
//  On exit, selected parabolas are  y' = y[i]+(x'-x[i])^2
//  Parabola i defines the envelope in the range (z[i],z[i+1]).
//  Thus z.size()==x.size()+1
template<class srcT>
inline void vil_quad_envelope(const srcT* src,vcl_ptrdiff_t s_step,
                              unsigned int n,
                              vcl_vector<double>& x,
                              vcl_vector<double>& y,
                              vcl_vector<double>& z, double a)
{
  x.resize(1);  x[0]=0.0;
  y.resize(1);  y[0]=double(*src);
  z.resize(1);  z[0]=0.0;
  src+=s_step;
  for (unsigned int x0=1;x0<n;++x0,src+=s_step)
  {
    vil_update_parabola_set(x,y,z,a,x0,double(*src),double(n));
  }
  z.push_back(n);
}

//: Sample from lower envelope of a set of parabolas
//  Parabolas are  y' = y[i]+a(x'-x[i])^2
//  Parabola i defines the envelope in the range (z[i],z[i+1]).
//  Thus z.size()==x.size()+1
template<class destT>
inline void vil_sample_quad_envelope(const vcl_vector<double>& x,
                                     const vcl_vector<double>& y,
                                     const vcl_vector<double>& z, double a,
                                     destT* dest, vcl_ptrdiff_t d_step,
                                     unsigned int n)
{
  unsigned int k=0;
  for (unsigned int i=0;i<n;++i,dest+=d_step)
  {
    while (z[k+1]<i) ++k;  // Select relevant parabola
    *dest = destT(y[k] + a*(i-x[k])*(i-x[k]));
  }
}

//: Sample from lower envelope of a set of parabolas
//  Parabolas are  y' = y[i]+a(x'-x[i])^2
//  Parabola i defines the envelope in the range (z[i],z[i+1]).
//  Thus z.size()==x.size()+1
//
//  iT assumed to be an integer type (vxl_byte,short, int etc)
//  On exit, pos[i*p_step] gives the x position of the parabola
//  used to compute the envelope at position i.
template<class destT, class iT>
inline void vil_sample_quad_envelope_with_pos(const vcl_vector<double>& x,
                                              const vcl_vector<double>& y,
                                              const vcl_vector<double>& z,
                                              double a,
                                              destT* dest, vcl_ptrdiff_t d_step,
                                              unsigned int n,
                                              iT* pos, vcl_ptrdiff_t p_step)
{
  unsigned int k=0;
  for (unsigned int i=0;i<n;++i,dest+=d_step,pos+=p_step)
  {
    while (z[k+1]<i) ++k;  // Select relevant parabola
    *dest = destT(y[k] + a*(i-x[k])*(i-x[k]));
    *pos  = iT(x[k]+0.5);  // x[k] assumed positive, round to nearest integer
  }
}

//: Compute quadratic distance function for a 1D function
//  On exit dest(x) = min_i src(x+i)+a(i^2)
//  Implementation of Felzenszwalb and Huttenlocher's algorithm,
//  as described in "Distance Transforms of Sampled Functions".
//
//  dest(x) = dest[x*d_step], src(x)=src[x*s_step]
template<class srcT, class destT>
inline void vil_quad_distance_function_1D(const srcT* src,vcl_ptrdiff_t s_step,
                                          unsigned int n,
                                          double a,
                                          destT* dest, vcl_ptrdiff_t d_step)
{
  vcl_vector<double> x,y,z;
  vil_quad_envelope(src,s_step,n,x,y,z,a);
  vil_sample_quad_envelope(x,y,z,a,dest,d_step,n);
}

//: Compute quadratic distance function for a 1D function
//  On exit dest(x) = min_i src(x+i)+a(i^2),
//  pos(x) gives position (x+i) which leads to the minima
//  Implementation of Felzenszwalb and Huttenlocher's algorithm,
//  as described in "Distance Transforms of Sampled Functions".
//
//  dest(x) = dest[x*d_step], src(x)=src[x*s_step], pos(x)=pos[x*p_step]
template<class srcT, class destT, class posT>
inline void vil_quad_distance_function_1D(const srcT* src,vcl_ptrdiff_t s_step,
                                          unsigned int n,
                                          double a,
                                          destT* dest, vcl_ptrdiff_t d_step,
                                          posT* pos, vcl_ptrdiff_t p_step)
{
  vcl_vector<double> x,y,z;
  vil_quad_envelope(src,s_step,n,x,y,z,a);
  vil_sample_quad_envelope_with_pos(x,y,z,a,dest,d_step,n, pos,p_step);
}

//: Apply quadratic distance transform along each row of src
//
//  dest(x,y)=min_i,j (src(x+i,y+j)+ai(i^2)+aj(j^2))
template<class srcT, class destT>
inline void vil_quad_distance_function(const vil_image_view<srcT>& src,
                                       double ai, double aj,
                                       vil_image_view<destT>& dest)
{
  assert(src.nplanes()==1);
  unsigned int ni=src.ni(),nj=src.nj();
  dest.set_size(ni,nj);
  vil_image_view<destT> tmp(ni,nj);  // Intermediate result
  vcl_ptrdiff_t s_istep = src.istep(),   s_jstep = src.jstep();
  vcl_ptrdiff_t t_istep = tmp.istep(),   t_jstep = tmp.jstep();
  vcl_ptrdiff_t d_istep = dest.istep(),  d_jstep = dest.jstep();

  const srcT* s_row = src.top_left_ptr();
  destT* t_row = tmp.top_left_ptr();

  vcl_vector<double> x,y,z;

  // Apply transform along i direction to get tmp
  for (unsigned int j=0;j<nj;++j, s_row+=s_jstep, t_row+=t_jstep)
  {
    vil_quad_envelope(s_row,s_istep,ni,x,y,z,ai);
    vil_sample_quad_envelope(x,y,z,ai,t_row,t_istep,ni);
  }
  // Apply transform along j direction to get dest
  destT* t_col = tmp.top_left_ptr();
  destT* d_col = dest.top_left_ptr();
  for (unsigned int i=0;i<ni;++i, d_col+=d_istep, t_col+=t_istep)
  {
    vil_quad_envelope(t_col,t_jstep,nj,x,y,z,aj);
    vil_sample_quad_envelope(x,y,z,aj,d_col,d_jstep,nj);
  }
}

//: Apply quadratic distance transform along each row of src
//
//  dest(x,y)=min_i,j (src(x+i,y+j)+ai(i^2)+aj(j^2))
//  (pos(x,y,0),pos(x,y,1)) gives the position (x+i,y+j) leading to minima
template<class srcT, class destT, class posT>
inline void vil_quad_distance_function(const vil_image_view<srcT>& src,
                                       double ai, double aj,
                                       vil_image_view<destT>& dest,
                                       vil_image_view<posT>& pos)
{
  assert(src.nplanes()==1);
  unsigned int ni=src.ni(), nj=src.nj();
  dest.set_size(ni,nj);
  pos.set_size(ni,nj,2);
  vil_image_view<destT> tmp(ni,nj);  // Intermediate result
  vil_image_view<posT> tmp_pos(ni,nj); // Intermediate result
  vcl_ptrdiff_t  s_istep = src.istep(),      s_jstep = src.jstep();
  vcl_ptrdiff_t  t_istep = tmp.istep(),      t_jstep = tmp.jstep();
  vcl_ptrdiff_t  d_istep = dest.istep(),     d_jstep = dest.jstep();
  vcl_ptrdiff_t tp_istep = tmp_pos.istep(), tp_jstep = tmp_pos.jstep();
  vcl_ptrdiff_t  p_istep = pos.istep(),      p_jstep = pos.jstep();

  const srcT* s_row = src.top_left_ptr();
  destT* t_row = tmp.top_left_ptr();
  posT* tp_row  = tmp_pos.top_left_ptr();

  vcl_vector<double> x,y,z;

  // Apply transform along i direction to get tmp
  for (unsigned int j=0;j<nj;++j,s_row+=s_jstep,t_row+=t_jstep,tp_row+=tp_jstep)
  {
   vil_quad_envelope(s_row,s_istep,ni,x,y,z,ai);
   vil_sample_quad_envelope_with_pos(x,y,z,ai,t_row,t_istep,ni,tp_row,tp_istep);
  }
  // Apply transform along j direction to get dest
  destT* t_col = tmp.top_left_ptr();
  destT* d_col = dest.top_left_ptr();
  posT* pi_col = pos.top_left_ptr();
  posT* pj_col = pi_col + pos.planestep();
  posT* tp_col = tmp_pos.top_left_ptr();

  for (unsigned int i=0;i<ni;++i, d_col+=d_istep, t_col+=t_istep,
                                  pi_col+=p_istep, pj_col+=p_istep,
                                  tp_col+=tp_istep)
  {
    vil_quad_envelope(t_col,t_jstep,nj,x,y,z,aj);
    vil_sample_quad_envelope_with_pos(x,y,z,aj,d_col,d_jstep,nj,pj_col,p_jstep);
    // Now deduce the i position using the current offset
    for (unsigned int j=0;j<nj;++j)
    {
      pi_col[j*p_jstep] = tp_col[pj_col[j*p_jstep]*tp_jstep];
    }
  }
}

#endif
