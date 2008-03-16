// This is brl/bbas/bil/bil_bounded_image_view.txx
#ifndef bil_bounded_image_view_txx_
#define bil_bounded_image_view_txx_

#include "bil_bounded_image_view.h"
//:
// \file
#include <vcl_cassert.h>
#include <vil/io/vil_io_image_view.h>


//:The main constructor
template<class T>
bil_bounded_image_view<T>::bil_bounded_image_view(const vil_image_view<T>& bounded_data,
                                                  unsigned ib0, unsigned jb0,
                                                  unsigned nib, unsigned njb )
  : vil_image_view<T>(bounded_data)
{
  //Require that bounds are consistent
  assert(ib0+bounded_data.ni()<=nib);
  assert(jb0+bounded_data.nj()<=njb);
  zero_ = (T)0;
  ib0_ = ib0;
  jb0_ = jb0;
  nib_ = nib;
  njb_ = njb;
}

//: Copy constructor
template<class T>
bil_bounded_image_view<T>::bil_bounded_image_view(const bil_bounded_image_view<T>& rhs)
:  vil_image_view<T>((vil_image_view<T>)rhs)
{
  zero_ = (T)0;
  ib0_ = rhs.ib0();
  jb0_ = rhs.jb0();
  nib_ = rhs.nib();
  njb_ = rhs.njb();
}

template<class T> bil_bounded_image_view<T>::
bil_bounded_image_view(const vil_image_view_base& rhs)
  : vil_image_view<T>(rhs)
{
  const bil_bounded_image_view<T>& bv =
    static_cast<const bil_bounded_image_view<T>& >(rhs);
  zero_ = (T)0;
  ib0_ = bv.ib0();
  jb0_ = bv.jb0();
  nib_ = bv.nib();
  njb_ = bv.njb();
}

template<class T>
const T&  bil_bounded_image_view<T>::gpix(unsigned i, unsigned j) const
{
  assert(i<nib_ );
  assert(j<njb_ );
  int ti = (int)i-(int)ib0_, tj =(int)j-(int)jb0_;
  if (ti>=0&&ti<(int)this->ni_&&(tj>=0&&tj<(int)this->nj_))
    return this->top_left_[ti*this->istep_+tj*this->jstep_];
  else
    return zero_;
}

template<class T>
T& bil_bounded_image_view<T>::gpix(unsigned i, unsigned j)
{
  //  assert(i<nib_); assert(j<njb_); fill with zeros outside real data
  int ti = (int)i-(int)ib0_, tj =(int)j-(int)jb0_;
  if (ti>=0&&ti<(int)this->ni_&&(tj>=0&&tj<(int)this->nj_))
    return this->top_left_[ti*this->istep_+tj*this->jstep_];
  else
    return zero_;
}

template<class T>
const T& bil_bounded_image_view<T>::
gpix(unsigned i, unsigned j, unsigned p) const
{
  //assert(i<nib_); assert(j<njb_); //fill with zeros outside real data
  assert(p<this->nplanes_);
  int ti = (int)i-(int)ib0_, tj =(int)j-(int)jb0_;
  if (ti>=0&&ti<(int)this->ni_&&(tj>=0&&tj<(int)this->nj_))
    return this->top_left_[p*this->planestep_ + ti*this->istep_+tj*this->jstep_];
  else
    return zero_;
}

template<class T>
T& bil_bounded_image_view<T>::
gpix(unsigned i, unsigned j, unsigned p)
{
  //assert(i<nib_); assert(j<njb_);
  assert(p<this->nplanes_);
  int ti = (int)i-(int)ib0_, tj =(int)j-(int)jb0_;
  if (ti>=0&&ti<(int)this->ni_&&(tj>=0&&tj<(int)this->nj_))
    return this->top_left_[p*this->planestep_ + ti*this->istep_+tj*this->jstep_];
  else
    return zero_;
}

template<class T>
void bil_bounded_image_view<T>::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, ib0_);
  vsl_b_write(os, jb0_);
  vsl_b_write(os, nib_);
  vsl_b_write(os, njb_);

  vsl_b_write(os, (vil_image_view<T>)*this);
}

template<class T>
void bil_bounded_image_view<T>::b_read(vsl_b_istream &is)
{
  short ver;
  unsigned ib0, jb0, nib, njb;
  vil_image_view<T> img;

  if (!is)
    return;
  vsl_b_read(is, ver);
  switch (ver)
  {
  case 1:
    vsl_b_read(is, ib0);
    vsl_b_read(is, jb0);
    vsl_b_read(is, nib);
    vsl_b_read(is, njb);
    vsl_b_read(is, img);
    *this = *(new bil_bounded_image_view<T>(img, ib0, jb0, nib, njb));
    break;

  default:
    vcl_cerr << "I/O ERROR: bil_bounded_image_view<T>::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream*/
    return;
  }
}

#define BIL_BOUNDED_IMAGE_VIEW_INSTANTIATE(T) \
template class bil_bounded_image_view<T >

#endif // bil_bounded_image_view_txx_
