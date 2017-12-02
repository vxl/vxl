#ifndef mipa_sample_histo_boxes_h_
#define mipa_sample_histo_boxes_h_

//:
// \file
// \brief Functions to sample blocks of histograms
// \author Tim Cootes

#include <vil/vil_image_view.h>
#include <vnl/vnl_vector.h>

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
                             unsigned ni, unsigned nj);

//: Performs inverse transform mapping from the sample vector back to a hist image
// Note only the first 4*nA*ni*nj elements of v are accessed
template<class srcT, class vT>
void mipa_sample_histo_boxes_3L_inv(vil_image_view<srcT>& h_im,
                                    unsigned i0, unsigned j0,
                                    const vnl_vector<vT>& vec,
                                    unsigned ni, unsigned nj);

#endif // mipa_sample_histo_boxes_h_
