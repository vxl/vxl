#!/bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6

# Author: Dave Cooper
# Modification: fsm, awf. various.

$vcl = 0; # convert Config-IUE and standard C/C++ code to vcl.
$vbl = 0; # convert Basics code to vbl.
$vnl = 0; # convert Numeric code to vnl.
$vil = 0; # convert EasyImage to vil
$vgui = 0; # convert old vgui to new vgui

foreach my $arg (@ARGV) {
  if ($arg eq "-piglet") { print "# pragma piglet\n"; }
  elsif ($arg eq "-vcl") { $vcl = 1; }
  elsif ($arg eq "-vbl") { $vbl = 1; }
  elsif ($arg eq "-vnl") { $vnl = 1; }
  elsif ($arg eq "-vil") { $vil = 1; }
  elsif ($arg eq "-vgui") { $vgui = 1; }
  else { die "dunno about '$arg'\n"; }
}

#------------------------------ filter ------------------------------

die unless open(FD, "-");

while(<FD>) {
  if ($vcl) {
    # IUE_compiler.h -> vcl_compiler.h
    s/IUE_GCC/VCL_GCC/g;
    s/IUE_SGI_CC/VCL_SGI_CC/g;
    s/IUE_SUNPRO_CC/VCL_SUNPRO_CC/g;
    s/IUE_WIN32/VCL_WIN32/g;
    s/IUE_VC50/VCL_VC50/g;
    s/IUE_VC60/VCL_VC60/g;
    
    # defines from IUE_compiler :
    s/IUE_FOR_SCOPE_HACK/VCL_FOR_SCOPE_HACK/g;
    s/IUE_HAS_MEMBER_TEMPLATES/VCL_HAS_MEMBER_TEMPLATES/g;
    s/IUE_CAN_DO_PARTIAL_SPECIALIZATION/VCL_CAN_DO_PARTIAL_SPECIALIZATION/g;
    s/IUE_HAS_DYNAMIC_CAST/VCL_HAS_DYNAMIC_CAST/g;
    s/IUE_STATIC_CONST_INIT_FLOAT/VCL_STATIC_CONST_INIT_FLOAT/g;
    s/IUE_STATIC_CONST_INIT_INT/VCL_STATIC_CONST_INIT_INT/g;
    s/IUE_IMPLEMENT_STATIC_CONSTS/VCL_IMPLEMENT_STATIC_CONSTS/g;
    s/IUE_INSTANTIATE_INLINE/VCL_INSTANTIATE_INLINE/g;
    s/IUE_DO_NOT_INSTANTIATE/VCL_DO_NOT_INSTANTIATE/g;
    s/IUE_UNINSTANTIATE_SPECIALIZATION/VCL_UNINSTANTIATE_SPECIALIZATION/g;
    s/IUE_UNINSTANTIATE_UNSEEN_SPECIALIZATION/VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION/g;
    s/IUE_INSTANTIATE_STATIC_TEMPLATE_MEMBER/VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER/g;
    s/IUE_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER/VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER/g;
    s/IUE_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD/VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD/g;
    s/IUE_DECLARE_SPECIALIZATION/VCL_DECLARE_SPECIALIZATION/g;
    s/IUE_STL_NULL_TMPL_ARGS/VCL_STL_NULL_TMPL_ARGS/g;
    s/IUE_DEFAULT_VALUE/VCL_DEFAULT_VALUE/g;
    s/IUE_DFL_TYPE_PARAM_STLDECL/VCL_DFL_TYPE_PARAM_STLDECL/g;
    s/IUE_DFL_TMPL_PARAM_STLDECL/VCL_DFL_TMPL_PARAM_STLDECL/g;
    s/IUE_DFL_TMPL_ARG/VCL_DFL_TMPL_ARG/g;
    s/IUE_SUNPRO_ALLOCATOR_HACK/VCL_SUNPRO_ALLOCATOR_HACK/g;
    
    s/IUE_USE_NATIVE_STL/VCL_USE_NATIVE_STL/g;
    s/IUE_USE_NATIVE_COMPLEX/VCL_USE_NATIVE_COMPLEX/g;
    
    # the <IUE_*.h> bunch
    s/<IUE_compiler\.h>/<vcl\/vcl_compiler.h>/;
    s/<IUE_string\.h>/<vcl\/vcl_string.h>/;
    s/<IUE_iosfwd\.h>/<vcl\/vcl_iosfwd.h>/;
    
    # standard C/C++
    s/<assert\.h>/<vcl\/vcl_cassert.h>/;
    s/<ctype\.h>/<vcl\/vcl_cctype.h>/;
    s/<math\.h>/<vcl\/vcl_cmath.h>/;
    s/<string\.h>/<vcl\/vcl_cstring.h>/;
    s/<stdio\.h>/<vcl\/vcl_cstdio.h>/;
    s/<stdlib\.h>/<vcl\/vcl_cstdlib.h>/;
    s/<stdarg\.h>/<vcl\/vcl_cstdarg.h>/;
    s/<iostream\.h>/<vcl\/vcl_iostream.h>/;
    s/<complex\.h>/<vcl\/vcl_complex.h>/;
    s/include <string>/include <vcl\/vcl_string.h>/; # don't break Arg<string> ....
    
    # STL headers
    s/<algo>/<vcl\/vcl_algorithm.h>/;
    s/<algo.h>/<vcl\/vcl_algorithm.h>/;
    s/<algobase>/<vcl\/vcl_algorithm.h>/;
    s/<algobase.h>/<vcl\/vcl_algorithm.h>/;
    s/<function>/<vcl\/vcl_function.h>/;
    s/<function\.h>/<vcl\/vcl_function.h>/;
    s/<functional>/<vcl\/vcl_function.h>/;
    s/<vector>/<vcl\/vcl_vector.h>/;
    s/<vector\.h>/<vcl\/vcl_vector.h>/;
    s/<list>/<vcl\/vcl_list.h>/;
    s/<list\.h>/<vcl\/vcl_list.h>/;
    
    # classes and functions
    s/\bvector</vcl_vector</g;
    s/\blist</vcl_list</g;
  }
  
  if ($vbl) {
    # headers
    s/<Basics\/RGB\.h>/<vbl\/vbl_rgb.h>/;
    s/<Basics\/RGBA\.h>/<vbl\/vbl_rgba.h>/;
    s/<Basics\/ArgParse\.h>/<vbl\/vbl_arg.h>/;
    
    # classes and functions
    s/\bRGB</vbl_rgb</g;
    s/\bRGBcell\b/vbl_rgb<byte> /g;
    s/\bRGBA/vbl_rgba/g;
    s/\bRGBAcell\b/vbl_rgba<byte> /g;
    s/\bArg</vbl_arg</g;
    s/\bArgBase::Parse/vbl_arg_base::parse/g;
  }
  
  if ($vnl) {
    # 
    s/IUE_USED_COMPLEX/VNL_USED_COMPLEX/g;
    s/IUE_COMPLEX_AVAILABLE/VNL_COMPLEX_AVAILABLE/g;
    
    # includes
    s/<math\/decls\.h>/<vnl\/vnl_fwd.h>/;
    s/<math\/test\.h>/<vnl\/vnl_test.h>/;
    s/<math\/fortran_copy\.h>/<vnl\/vnl_fortran_copy.h>/;
    s/<math\/complex\.h>/<vnl\/vnl_complex.h>/;
    s/<math\/vector\.h>/<vnl\/vnl_vector.h>/;
    s/<math\/matrix\.h>/<vnl\/vnl_matrix.h>/;
    s/<math\/DiagMatrix\.h>/<vnl\/vnl_diag_matrix.h>/;
    s/<math\/math\.h>/<vnl\/vnl_math.h>/;
    s/<math\/c_vector\.h>/<vnl\/vnl_c_vector.h>/;
    s/<Numerics\/Cholesky.h>/<vnl\/algo\/vnl_cholesky.h>/;
    s/<Numerics\/QR.h>/<vnl\/algo\/vnl_qr.h>/;
    s/<Numerics\/SVD.h>/<vnl\/algo\/vnl_svd.h>/;
    s/<Numerics\/ComplexEigensystem\.h>/<vnl\/algo\/vnl_complex_eigensystem.h>/;
    s/<Numerics\/GeneralizedEigensystem\.h>/<vnl\/algo\/vnl_generalized_eigensystem.h>/;
    s/<Numerics\/RealPolynomial\.h>/<vnl\/vnl_real_polynomial.h>/g;
    s/<Numerics\/FileMatrix\.h>/<vnl\/vnl_file_matrix.h>/;
    s/<Numerics\/FileVector\.h>/<vnl\/vnl_file_vector.h>/;
    s/<Numerics\/MatrixInverse\.h>/<vnl\/vnl_matrix_inverse.h>/;
    s/<Numerics\/Eigensystem\.h>/<vnl\/algo\/vnl_eigensystem.h>/;
    s/<Numerics\/SparseMatrix\.h>/<vnl\/vnl_sparse_matrix.h>/;
    s/<math\/BaseSVD\.h>/<vnl\/algo\/vnl_svd.h>/;
    s/<math\/numeric_limits\.h>/<vnl\/vnl_numeric_limits.h>/;
    s/<math\/numeric_traits\.h>/<vnl\/vnl_numeric_traits.h>/;
    s/<math\/complex_traits\.h>/<vnl\/vnl_complex_traits.h>/;
    s/<math\/MatOps\.h>/<vnl\/vnl_matops.h>/;
    s/<math\/UnaryFunction\.h>/<vnl\/vnl_unary_function.h>/;
    s/<math\/VectorRef\.h>/<vnl\/vnl_vector_ref.h>/;
    s/<math\/MatrixFixedRef\.h>/<vnl\/vnl_matrix_fixed_ref.h>/;
    s/<math\/VectorFixed\.h>/<vnl\/vnl_vector_fixed.h>/;
    s/<math\/MatrixFixed\.h>/<vnl\/vnl_matrix_fixed.h>/;
    s/<math\/MatrixRef\.h>/<vnl\/vnl_matrix_ref.h>/;
    s/UnaryFunction\.h/vnl_unary_function.h>/;
    s/Identity\.h/vnl_identity.h>/;
    
    s/<Numerics\/ComplexVectorT/<vnl\/vnl_complex_vector_t/g;
    s/<Numerics\/ComplexVector/<vnl\/vnl_complex_vector/g;
    s/<Numerics\/ComplexMatrixT/<vnl\/vnl_complex_matrix_t/g;
    s/<Numerics\/ComplexMatrix/<vnl\/vnl_complex_matrix/g;
    s/<Numerics\/LeastSquaresFunction/<vnl\/vnl_least_squares_function/g;
    s/<Numerics\/LeastSquaresCostFunction\.h>/<vnl\/vnl_least_squares_cost_function.h>/g;
    s/<Numerics\/NonLinearMinimizer\.h>/<vnl\/vnl_nonlinear_minimizer.h>/g;
    s/<Numerics\/Double3x3\.h>/<vnl\/vnl_double_3x3.h>/g;
    s/<Numerics\/Double3\.h>/<vnl\/vnl_double_3.h>/g;
    s/<Numerics\/LinearOperators3\.h>/<vnl\/vnl_linear_operators_3.h>/g;
    s/<Numerics\/ChiSquared\.h>/<vnl\/algo\/vnl_chi_squared.h>/g;
    
    # classes and functions
    s/CPolyRoots/vnl_cpoly_roots/g;
    s/IUE_c_vector/vnl_c_vector/g;
    s/IUE_vector/vnl_vector/g;
    s/IUE_matrix/vnl_matrix/g;
    s/DiagMatrix/vnl_diag_matrix/g;
    s/IUE_math/vnl_math/g;
    
    s/ChiSquared::ChiSquaredCumulative/vnl_chi_squared_cumulative/g;
  
    s/SparseSymmetricEigensystem/vnl_sparse_symmetric_eigensystem/g;
    s/ComplexEigensystem/vnl_complex_eigensystem/g;
    s/SymmetricEigensystem/vnl_symmetric_eigensystem/g;
    s/Eigensystem/vnl_eigensystem/g;
    s/GeneralizedEigensystem/vnl_generalized_eigensystem/g;
    s/RealPolynomial/vnl_real_polynomial/g;
    s/DoubleVector/vnl_vector<double>/g;
    s/DoubleMatrix/vnl_matrix<double>/g;
    s/SparseMatrix/vnl_sparse_matrix/g;
    s/DoubleMatrix/vnl_matrix<double>/g;
    s/\bfortran_copy\b/vnl_fortran_copy/g;
    s/MatOps/vnl_matops/g;
    s/FileMatrix/vnl_file_matrix<double>/g;
    s/FileVector/vnl_file_matrix<double>/g;
    s/MatrixInverse/vnl_matrix_inverse<double>/g;
    s/RPolyRoots/vnl_rpoly_roots/g;
    s/RealPolynomial/vnl_real_polynomial/g;
    s/RealNPolynomial/vnl_real_npolynomial/g;
    s/RNPolySolve/vnl_rnpoly_solve/g;
    s/Cholesky/vnl_cholesky/g;
    s/\bQR\b/vnl_qr/g;
    s/BaseSVD/vnl_svd/g;
    s/\bSVD\b/vnl_svd<double>/g;
    s/IUE_numeric_limits/vnl_numeric_limits/g;
    s/IUE_numeric_traits/vnl_numeric_traits/g;
    s/IUE_complex_traits/vnl_complex_traits/g;
    s/ComplexVectorT/vnl_complex_vector_t/g;
    s/ComplexVector/vnl_complex_vector/g;
    s/ComplexMatrixT/vnl_complex_matrix_t/g;
    s/ComplexMatrix/vnl_complex_matrix/g;
    s/ConjugateGradient/vnl_conjugate_gradient/g;
    s/LeastSquaresFunction/vnl_least_squares_function/g;
    s/LeastSquaresCostFunction/vnl_least_squares_cost_function/g;
    s/CostFunction/vnl_cost_function/g;
    s/IUE_UnaryFunction/vnl_unary_function/g;
    s/IUE_Identity/vnl_identity/g;
    s/LevenbergMarquardt/vnl_levenberg_marquardt/g;
    s/NonLinearMinimizer/vnl_nonlinear_minimizer/g;
    s/MatrixFixedRef/vnl_matrix_fixed_ref/g;
    s/MatrixFixed/vnl_matrix_fixed/g;
    s/VectorFixed/vnl_vector_fixed/g;
    s/VectorRef/vnl_vector_ref/g;
    s/MatrixRef/vnl_matrix_ref/g;
    s/RotationMatrix/vnl_rotation_matrix/g;
    s/Identity3x3/vnl_identity_3x3/g;
    s/CrossProductMatrix/vnl_cross_product_matrix/g;
    s/\bLBFGS\b/vnl_lbfgs/g;
    s/\bTranspose\b/vnl_transpose/g;
    s/FastOps/vnl_fastops/g;
    s/DiscreteDiff/vnl_discrete_diff/g;
    s/\bFFT1D\b/vnl_fft1d/g;
    s/\bFFT2D\b/vnl_fft2d/g;
    s/FFTxDPrimeFactors/vnl_fftxd_prime_factors/g;
    s/GaussianKernel1D/vnl_gaussian_kernel_1d/g;
    s/\bDouble2x2\b/vnl_double_2x2/g;
    s/\bDouble2x3\b/vnl_double_2x3/g;
    s/\bDouble3x3\b/vnl_double_3x3/g;
    s/\bDouble3x4\b/vnl_double_3x4/g;
    s/\bDouble4x4\b/vnl_double_4x4/g;
    s/\bDouble4x3\b/vnl_double_4x3/g;
    s/\bDouble4\b/vnl_double_4/g;
    s/\bDouble3\b/vnl_double_3/g;
    s/\bDouble2\b/vnl_double_2/g;
    s/LinearOperators3/vnl_linear_operators_3/g;
    s/IntMatrix/vnl_int_matrix/g;
    s/AffineApprox/vnl_affine_approx/g;
    s/Float2/vnl_float_2/g;
    s/Float3/vnl_float_3/g;
    s/Scatter3x3/vnl_scatter_3x3<double>/g;
    s/Amoeba/vnl_amoeba/g;
    s/IUE_VectorDereference/vnl_vector_dereference/g;
    s/VectorDereference/vnl_vector_dereference/g;
    s/Int2/vnl_int_2/g;
    s/Int3/vnl_int_3/g;
    s/Int4/vnl_int_4/g;
    s/ScalarJoinIterator/vnl_scalar_join_iterator/g;
    #awf  s/\bresize\b/vnl_resize/g;
    s/(\.|\->)maxVal\b/$1max_value/g;
    
    # test names
    s/Numerics_Test_AssertNear/vnl_test_assert_near/g;
    s/Numerics_Test_Assert/vnl_test_assert/g;
  }
  
  if ($vil) {
    s!<EasyImage/ImageBuffer.h>!<vil/vil_memory_image_of.h>!g;
  }
  
  if ($vgui) {
    s/\bVGUI/vgui/g;
    s!tableaux/!vgui/!g;
    s!tableaux_DLLDATA!vgui_DLLDATA!g;
    s!vgui_displaybase.C!vgui_displaybase.txx!g;
    s!obl/RGBA!vbl/vbl_rgba!g;

    s!obl/bool_ostream!vbl/vbl_bool_ostream!g;
    s!obl_on_off!vbl_bool_ostream::on_off!g;
    s!obl_true_false!vbl_bool_ostream::true_false!g;
    s!obl_high_low!vbl_bool_ostream::high_low!g;
    s!IUE_glu.h!vgui/vgui_glu.h!g;
    s!IUE_glut.h!vgui/vgui_glut.h!g;
    s!GL/glu.h!vgui/vgui_glu.h!g;
    s!GL/glut.h!vgui/vgui_glut.h!g;
    s!<IUE_gl!<vgui/vgui_gl!g;
    s!<vgui_gtk/!<vgui/impl/gtk/!g;
  }
  
  print;
}

close(FD);

