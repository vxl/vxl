// This is brl/bbas/bil/algo/bil_perform_tensor_decomposition.h
#ifndef bil_perform_tensor_decomposition_h_
#define bil_perform_tensor_decomposition_h_
//:
// \file
// \brief  Eigendecomposition
// \author  jdfunaro based off of matlab code by Gabriel Peyre
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vil/vil_image_view.h>

template <class T>
void bil_perform_tensor_decomposition( const vil_image_view< T >& k11,
                                       const vil_image_view< T >& k12,
                                       const vil_image_view< T >& k21,
                                       const vil_image_view< T >& k22,
                                       vil_image_view< T >& e1,
                                       vil_image_view< T >& e2,
                                       vil_image_view< T >& l1,
                                       vil_image_view< T >& l2);

#endif // bil_perform_tensor_decomposition_h_
