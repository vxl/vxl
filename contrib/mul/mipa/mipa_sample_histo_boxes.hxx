#ifndef mipa_sample_histo_boxes_hxx_
#define mipa_sample_histo_boxes_hxx_
//:
// \file
// \brief Functions to sample blocks of histograms
// \author Tim Cootes

#include "mipa_sample_histo_boxes.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Extract data from an image of histograms over nested boxes
//  Assume h_im(i,j,A) gives frequency stored in angle bin A
//  for (i,j)th histogram, organised so that h_im.planestep()=1
//  This copies the individual histograms from a rectangular
//  region of the image with corner (i0,j0).
//
//  It samples a grid of 2ni x 2nj histos,
//  ie h_im([i0,i0+2ni),[j0,j0+2nj)).
//
//  The first 4*nA*ni*nj elements of v are copies of the
//  histograms.  The next nA*ni*nj elements are a set of
//  pooled histograms, adding the 4 histos in each 2 x 2 block.
//  The final nA elements give a total histogram, adding
//  all the histograms in the region together.
template<class srcT, class vT>
void mipa_sample_histo_boxes_3L(const vil_image_view<srcT>& h_im,
                                unsigned i0, unsigned j0,
                                vnl_vector<vT>& vec,
                                unsigned ni, unsigned nj)
{
  unsigned nA=h_im.nplanes();
  vec.set_size(nA*(5*ni*nj+1));

  assert(h_im.planestep()==1);
  assert(h_im.istep()==int(nA));

  // Set up some pointers and offsets so that we
  // can do everything in one pass.
  vT *v = vec.data_block();
  unsigned dv1=nA,dv2=2*ni*nA,dv3=dv1+dv2;

  vT *w = v + 4*nA*ni*nj;  // Start of data for 2x2 blocks
  vT *sum = w + nA*ni*nj;  // Start of data for total summation

  // Zero the total summation
  for (unsigned a=0;a<nA;++a) sum[a]=0;

  const srcT *h_row = &h_im(i0,j0);
  std::ptrdiff_t hj_step = 2*h_im.jstep();
  unsigned dh1=nA,dh2=nA*h_im.ni(),dh3=dh1+dh2;

  for (unsigned j=0;j<nj;++j,h_row+=hj_step,v+=dv2)
  {
    // Set pointers to start of each pair of rows
    const srcT *h = h_row;

    // Process i-th 2x2 block in the row
    // (The +=nA at end of each pass steps to next pair)
    for (unsigned i=0;i<ni;++i,v+=nA,h+=nA,w+=nA)
      for (unsigned a=0;a<nA;++a,++v,++h)
      {
        // Copy elements from 2 x 2 block into vector
        v[0]=h[0];     v[dv1]=h[dh1];
        v[dv2]=h[dh2]; v[dv3]=h[dh3];

        // Sum elements over 2 x 2 block
        w[a] = v[0]+v[dv1]+v[dv2]+v[dv3];

        // Add this to the total sum
        sum[a]+=w[a];
      }
  }
}


//---------------------- Perform inverse transform -------------------
// Note this is mainly used in testing
// Note only the first 4*nA*ni*nj elements of v are accessed

template<class srcT, class vT>
void mipa_sample_histo_boxes_3L_inv(vil_image_view<srcT>& h_im,
                                    unsigned i0, unsigned j0,
                                    const vnl_vector<vT>& vec,
                                    unsigned ni, unsigned nj)
{
  unsigned nA=h_im.nplanes();
  //vec.set_size(nA*(5*ni*nj+1));

  assert(h_im.planestep()==1);
  assert(h_im.istep()==int(nA));

  // Set up some pointers and offsets so that we
  // can do everything in one pass.
  const vT *v = vec.data_block();
  unsigned dv1=nA,dv2=2*ni*nA,dv3=dv1+dv2;

  srcT *h_row = &h_im(i0,j0);
  std::ptrdiff_t hj_step = 2*h_im.jstep();
  unsigned dh1=nA,dh2=nA*h_im.ni(),dh3=dh1+dh2;

  for (unsigned j=0;j<nj;++j,h_row+=hj_step,v+=dv2)
  {
    // Set pointers to start of each pair of rows
    srcT *h = h_row;

    // Process i-th 2x2 block in the row
    // (The +=nA at end of each pass steps to next pair)
    for (unsigned i=0;i<ni;++i,v+=nA,h+=nA)
    {
      for (unsigned a=0;a<nA;++a,++v,++h)
      {
        // Copy elements from vector into 2 x 2 block
        h[0  ]=srcT(v[0]);   h[dh1]=srcT(v[dv1]);
        h[dh2]=srcT(v[dv2]); h[dh3]=srcT(v[dv3]);
      }
    }
  }
}


#undef MIPA_SAMPLE_HISTO_BOXES_INSTANTIATE
#define MIPA_SAMPLE_HISTO_BOXES_INSTANTIATE(srcT, vT) \
template void mipa_sample_histo_boxes_3L(const vil_image_view<srcT >& h_im, \
                                         unsigned i0, unsigned j0, \
                                         vnl_vector<vT >& vec, \
                                         unsigned ni, unsigned nj)

#undef MIPA_SAMPLE_HISTO_BOXES_INV_INSTANTIATE
#define MIPA_SAMPLE_HISTO_BOXES_INV_INSTANTIATE(srcT, vT) \
template void mipa_sample_histo_boxes_3L_inv(vil_image_view<srcT >& h_im, \
                                             unsigned i0, unsigned j0, \
                                             const vnl_vector<vT >& vec, \
                                             unsigned ni, unsigned nj)

#endif // mipa_sample_histo_boxes_hxx_
