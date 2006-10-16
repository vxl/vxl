// This is mul/vil3d/algo/vil3d_grad_3x3x3.h
#ifndef vil3d_grad_3x3x3_h_
#define vil3d_grad_3x3x3_h_
//:
// \file
// \brief Compute gradient using 3D version of sobel operator.
// \author Tim Cootes

#include <vil3d/algo/vil3d_grad_1x3.h>
#include <vil3d/algo/vil3d_smooth_121.h>

//: Compute i-gradient using 3D version of sobel operator.
//  Resulting image has same size. Border pixels set to zero.
//  Smooths in j and k with 1-2-1 filters, then applies (-0.5 0 0.5)
//  filter along i.  Intermediate images of type destT
template<class srcT, class destT>
void vil3d_grad_3x3x3_i(const vil3d_image_view<srcT>& src_im,
                        vil3d_image_view<destT>& grad_im)
{
  vil3d_image_view<destT> tmp_im;
  vil3d_smooth_121_j(src_im,grad_im);  // Use grad_im as temporary storage
  vil3d_smooth_121_k(grad_im,tmp_im);
  vil3d_grad_1x3_i(tmp_im,grad_im);
}

//: Compute j-gradient using 3D version of sobel operator.
//  Resulting image has same size. Border pixels set to zero.
//  Smooths in i and k with 1-2-1 filters, then applies (-0.5 0 0.5)
//  filter along j.  Intermediate images of type destT
template<class srcT, class destT>
void vil3d_grad_3x3x3_j(const vil3d_image_view<srcT>& src_im,
                        vil3d_image_view<destT>& grad_im)
{
  vil3d_image_view<destT> tmp_im;
  vil3d_smooth_121_i(src_im,grad_im);  // Use grad_im as temporary storage
  vil3d_smooth_121_k(grad_im,tmp_im);
  vil3d_grad_1x3_j(tmp_im,grad_im);
}


//: Compute k-gradient using 3D version of sobel operator.
//  Resulting image has same size. Border pixels set to zero.
//  Smooths in i and j with 1-2-1 filters, then applies (-0.5 0 0.5)
//  filter along k.  Intermediate images of type destT
template<class srcT, class destT>
void vil3d_grad_3x3x3_j(const vil3d_image_view<srcT>& src_im,
                        vil3d_image_view<destT>& grad_im)
{
  vil3d_image_view<destT> tmp_im;
  vil3d_smooth_121_i(src_im,grad_im);  // Use grad_im as temporary storage
  vil3d_smooth_121_j(grad_im,tmp_im);
  vil3d_grad_1x3_k(tmp_im,grad_im);
}

//: Compute gradients using 3D version of sobel operator.
//  Resulting images have same size as src_im. Border pixels set to zero.
//  Smooths in two directions with 1-2-1 filters, then applies (-0.5 0 0.5)
//  filter along the third.  Intermediate images of type destT
template<class srcT, class destT>
void vil3d_grad_3x3x3(const vil3d_image_view<srcT>& src_im,
                        vil3d_image_view<destT>& grad_i,
                        vil3d_image_view<destT>& grad_j,
                        vil3d_image_view<destT>& grad_k)
{
  vil3d_image_view<destT> smth_i,smth_ij,smth_ik;
  vil3d_smooth_121_i(src_im,smth_i);
  vil3d_smooth_121_j(smth_i,smth_ij);
  vil3d_smooth_121_k(smth_i,smth_ik);
  vil3d_grad_1x3_j(smth_ik,grad_j);
  vil3d_grad_1x3_k(smth_ij,grad_k);

  vil3d_image_view<destT> smth_j,smth_jk;
  vil3d_smooth_121_j(src_im,smth_j);
  vil3d_smooth_121_k(smth_j,smth_jk);
  vil3d_grad_1x3_i(smth_jk,grad_i);
}
