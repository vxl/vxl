#!/bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6

# Author: Dave Cooper
# Modification: fsm, awf. various.

$vcl = 0;  # convert Config-IUE and standard C/C++ code to vcl
$vbl = 0;  # convert Basics code to vbl
$vnl = 0;  # convert Numerics code to vnl
$vil = 0;  # convert EasyImage to vil
$vgui = 0; # convert old vgui to new vgui
$fsm = 0;  # make Fascist Source Modifications
$lint = 0; # 
$tmpl = 0; # fancy mode for template files

$mvl = 0;

%oked = (); # used with -fsm

foreach my $arg (@ARGV) {
  if ($arg eq "-piglet") { print "# pragma piglet\n"; }
  elsif ($arg eq "-vcl" ) { $vcl = 1; }
  elsif ($arg eq "-vbl" ) { $vbl = 1; }
  elsif ($arg eq "-vnl" ) { $vnl = 1; }
  elsif ($arg eq "-vil" ) { $vil = 1; }
  elsif ($arg eq "-vgl" ) { $vgl = 1; }
  elsif ($arg eq "-vxl" ) { $vgl = $vnl = $vil = $vbl = $vcl = 1; }
  elsif ($arg eq "-vgui") { $vgui = 1;}
  elsif ($arg eq "-mvl") { $mvl = 1;}
  elsif ($arg eq "-fsm" ) { $fsm = 1; }
  elsif ($arg eq "-lint") { $lint = 1;}
  elsif ($arg eq "-tmpl") { $tmpl = 1;}
  else { 
    print STDERR "dunno about '$arg'\n"; 
  }
}

# -tmpl things
if ($tmpl) {
  @lines = ();              # accumulate lines processed by filter here.
  @txx_needed = ();         # class names (e.g. 'map', 'vector') needing .txx file.
  $saw_stl_instantiate = 0; # if <stl-instantiate.h> was included.
  $saw_stl_functional = 0;  # if <functional> was included.
  $saw_less = 0;            # if the less<T> template was seen.
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
    s!^\#include.*<IUE_stlfwd\.h>!//$&!;
    
    # standard C/C++
    s/include <assert\.h>/include <vcl\/vcl_cassert.h>/;
    s/include <ctype\.h>/include <vcl\/vcl_cctype.h>/;
    s/include <math\.h>/include <vcl\/vcl_cmath.h>/;
    s/include <string\.h>/include <vcl\/vcl_cstring.h>/;
    s/include <stdio\.h>/include <vcl\/vcl_cstdio.h>/;
    s/include <stdlib\.h>/include <vcl\/vcl_cstdlib.h>/;
    s/include <stdarg\.h>/include <vcl\/vcl_cstdarg.h>/;
    s/include <iostream\.h>/include <vcl\/vcl_iostream.h>/;
    s/include <iomanip\.h>/include <vcl\/vcl_iomanip.h>/;
    s/include <fstream\.h>/include <vcl\/vcl_fstream.h>/;
    s/include <strstream\.h>/include <vcl\/vcl_strstream.h>/;
    s/include <complex\.h>/include <vcl\/vcl_complex.h>/;
    s/include <string>/include <vcl\/vcl_string.h>/; # don't break Arg<string> ....
    s/include <new>/include <vcl\/vcl_new.h>/;
    s/include <new\.h>/include <vcl\/vcl_new.h>/;
    
    # STL headers
    s/include <algo>/include <vcl\/vcl_algorithm.h>/;
    s/include <algo.h>/include <vcl\/vcl_algorithm.h>/;
    s/include <algobase>/include <vcl\/vcl_algorithm.h>/;
    s/include <algobase.h>/include <vcl\/vcl_algorithm.h>/;
    $saw_stl_functional = 1 if s/include <function>/include <vcl\/vcl_functional.h>/;
    $saw_stl_functional = 1 if s/include <function\.h>/include <vcl\/vcl_functional.h>/;
    $saw_stl_functional = 1 if s/include <functional>/include <vcl\/vcl_functional.h>/;
    $saw_stl_functional = 1 if s/include <vcl\/vcl_function\.h>/include <vcl\/vcl_functional.h>/;
    s/include <vector>/include <vcl\/vcl_vector.h>/;
    s/include <vector\.h>/include <vcl\/vcl_vector.h>/;
    s/include <list>/include <vcl\/vcl_list.h>/;
    s/include <list\.h>/include <vcl\/vcl_list.h>/;
    s/include <map>/include <vcl\/vcl_map.h>/;
    s/include <map\.h>/include <vcl\/vcl_map.h>/;
    s/include <multimap>/include <vcl\/vcl_multimap.h>/;
    s/include <multimap\.h>/include <vcl\/vcl_multimap.h>/;
    s/include <set\.h>/include <vcl\/vcl_set.h>/;
    s/include <multiset\.h>/include <vcl\/vcl_multiset.h>/;
    s/include <tree\.h>/include <vcl\/vcl_tree.h>/;
    s/include <stack\.h>/include <vcl\/vcl_stack.h>/;

    # COOL
    s!<cool/String.h>!<vcl/vcl_string.h>!g;
    
    # classes and functions
    s/\bvector</vcl_vector</g;
    s/\blist</vcl_list</g;
    s/\bmap</vcl_map</g;
    s/\bstring\b/vcl_string/g;
    s/\bCoolString\b/vcl_string/g;
    s/\bmultimap</vcl_multimap</g;
    $saw_less = 1 if s/\bless</vcl_less</g;
    s/\bpair</vcl_pair</g;
    s/\bhash</vcl_hash</g;
    s/\bset</vcl_set</g;



    # instantiation macros
    if ( s/^(\#include <stl\-instantiate\.h>)/\/\/ stl-instantiate.h/ ) {
      $saw_stl_instantiate = 1; 
      print STDERR "WARNING: stl-instantiate.h included, but not using -tmpl\n" unless $tmpl;
    }
    #fsm s/\bINSTANTIATE_UNARY\b/VCL_SWAP_INSTANTIATE /g;
    while (s/\bINSTANTIATE_(VECTOR|SET|HASHKEY|HASHMAP|HASHTABLE|SORT|MAP|CONTAINABLE|MULTIMAP|LIST|PAIR|COPY|COPY_BACKWARD|SWAP|LESS|FIND)\s*\(\b/VCL_$1_INSTANTIATE\(/) {
      if ($tmpl) {
	my ($tmp) = $1;
	$tmp =~ tr/A-Z/a-z/; # convert to lower case
	push @txx_needed, $tmp;
      } else {
	print STDERR "WARNING: saw instantiation macro for $1, but not using -tmpl\n";
      }
    }
  }
  
  if ($vbl) {
    # headers
    s!Basics/ansi.h!vcl/vcl_compiler.h!;
    s!^\#include.*<Basics/point2d.h>!//$&!; # delete Basics/point2d

    s/<Basics\/RGB\.h>/<vbl\/vbl_rgb.h>/;
    s/<Basics\/RGBcell\.h>/<vbl\/vbl_rgb.h>/;
    s/<Basics\/RGBA\.h>/<vbl\/vbl_rgba.h>/;
    s/<Basics\/ArgParse\.h>/<vbl\/vbl_arg.h>/;
    s/<Basics\/ArgInfoList\.h>/<vbl\/vbl_arg.h>/;
    s/<Basics\/IUE_sprintf\.h>/<vbl\/vbl_sprintf.h>/;
    s/<Basics\/Array2D\.h>/<vbl\/vbl_array_2d.h>/;
    s/<Basics\/Array3D\.h>/<vbl\/vbl_array_3d.h>/;
    s/<Basics\/Array2D\.C>/<vbl\/vbl_array_2d.txx>/;
    s/<Basics\/Array3D\.C>/<vbl\/vbl_array_3d.txx>/;
    s/<Basics\/file\.h>/<vbl\/vbl_file.h>/;
    s/<Basics\/stat\.h>/<vbl\/vbl_file.h> \/\/ use vbl_file instead of IUE_stat/;
    s/<Basics\/IUE_stat\.h>/<vbl\/vbl_file.h> \/\/ use vbl_file instead of IUE_stat/;
    s/<Basics\/directory\.h>/<vbl\/vbl_file.h> \/\/ use vbl_file instead of IUE_IUE_directory/;
    s/<Basics\/IUE_directory\.h>/<vbl\/vbl_file.h> \/\/ use vbl_file instead of IUE_directory/;
    s/<Basics\/printf\.h>/<vbl\/vbl_printf.h>/;
    s/<Basics\/AWK\.h>/<vbl\/vbl_awk.h>/;
    s/<Basics\/clamp\.h>/<vbl\/vbl_clamp.h>/;
    s/<Basics\/QSort\.h>/<vbl\/vbl_qsort.h>/;
    s!Basics/BoundingBox.h!vbl/vbl_bounding_box.h!g;
    s!Basics/BoundingBox.C!vbl/vbl_bounding_box.txx!g;
    s!\bBoundingBox<!vbl_bounding_box<!g;
    s!INSTANTIATE_BOUNDING_BOX!VBL_BOUNDING_BOX_INSTANTIATE!g;

    s/<cool\/Timer\.h>/<vbl\/vbl_timer.h>/;
    s/<cool\/RegExp\.h>/<vbl\/vbl_reg_exp.h>/;
    s!(\#include *\<cool\/decls.h\>)!//$1!;

    # Do these symbols, and then do their modified headers.
    s!\bSparseArray\b!vbl_sparse_array!g;
    s!\bSparseArray([123])D\b!vbl_sparse_array_$1d!g;
    s!<Basics/vbl_sparse_array!<vbl/vbl_sparse_array!g;
    s!<vbl/vbl_sparse_array.C!<vbl/vbl_sparse_array.txx!g;
    s!<vbl/vbl_sparse_array_([123]d).C!<vbl/vbl_sparse_array_$1.txx!g;
    s!\bINSTANTIATE_SPARSEARRAY([123])D!VBL_SPARSE_ARRAY_$1D_INSTANTIATE!;
    s!\bINSTANTIATE_SPARSEARRAY\b!VBL_SPARSE_ARRAY_INSTANTIATE!;
  
    s!\bARRAY2D_INSTANTIATE\b!VBL_ARRAY_2D_INSTANTIATE!g;
  
    # classes and functions
    s/\bRGB</vbl_rgb</g;
    s/\bRGBcell\b/vbl_rgb<byte> /g;
    s/\bRGBA/vbl_rgba/g;
    s/\bRGBAcell\b/vbl_rgba<byte> /g;
    s/\bArg</vbl_arg</g;
    s/\bArgInfoList\b/vbl_arg_info_list/g;
    s/\bArgBase::Parse/vbl_arg_parse/g;
    s/\bArgBase::DisplayUsageAndExit/vbl_arg_display_usage_and_exit/g;
    s/\bIUE_sprintf\b/vbl_sprintf/g;
    s/\bIUE_file\b/vbl_file/g;
    s/\bArray2D</vbl_array_2d</g;
    s/\bArray3D</vbl_array_3d</g;
    s/\bAWK\b/vbl_awk/g;
    s/\bprintf\(([^\"])/vbl_printf\($1/g;
    s/\bCoolTimer/vbl_timer/g;
    s/\bCoolRegExp/vbl_reg_exp/g;
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

    s/<Numerics\/Math\.h>/<vnl\/vnl_math.h>/g;
    s/<Numerics\/RPolyRoots\.h>/<vnl\/algo\/vnl_rpoly_roots.h>/;
    s/<Numerics\/CPolyRoots\.h>/<vnl\/algo\/vnl_cpoly_roots.h>/;
    s/<Numerics\/Cholesky.h>/<vnl\/algo\/vnl_cholesky.h>/;
    s/<Numerics\/QR.h>/<vnl\/algo\/vnl_qr.h>/;
    s/<Numerics\/SVD.h>/<vnl\/algo\/vnl_svd.h>/;
    s/<Numerics\/ComplexEigensystem\.h>/<vnl\/algo\/vnl_complex_eigensystem.h>/;
    s/<Numerics\/GeneralizedEigensystem\.h>/<vnl\/algo\/vnl_generalized_eigensystem.h>/;
    s/<Numerics\/RealPolynomial\.h>/<vnl\/vnl_real_polynomial.h>/g;
    s/<Numerics\/FileMatrix\.h>/<vnl\/vnl_file_matrix.h>/;
    s/<Numerics\/FileVector\.h>/<vnl\/vnl_file_vector.h>/;
    s/<Numerics\/MatrixInverse\.h>/<vnl\/algo\/vnl_matrix_inverse.h>/;
    s/<Numerics\/Eigensystem\.h>/<vnl\/algo\/vnl_real_eigensystem.h>/;
    s/<Numerics\/SparseMatrix\.h>/<vnl\/vnl_sparse_matrix.h>/;
    s/<math\/resize\.h>/<vnl\/vnl_resize.h>/;
    s/^(\#include <math\/matlab\.h>)/\#define matlab(M) M \/\/$1/;
    s/<math\/BaseSVD\.h>/<vnl\/algo\/vnl_svd.h>/;
    s/<math\/numeric_limits\.h>/<vnl\/vnl_numeric_limits.h>/;
    s/<math\/numeric_traits\.h>/<vnl\/vnl_numeric_traits.h>/;
    s/<math\/complex_traits\.h>/<vnl\/vnl_complex_traits.h>/;
    s/<math\/MatOps\.h>/<vnl\/vnl_matops.h> \/\/ use vnl_matlab_print.h for pretty printing/;
    s/<math\/UnaryFunction\.h>/<vnl\/vnl_unary_function.h>/;
    s/<math\/VectorRef\.h>/<vnl\/vnl_vector_ref.h>/;
    s/<(math|Numerics)\/MatrixFixedRef\.h>/<vnl\/vnl_matrix_fixed_ref.h>/;
    s/<(math|Numerics)\/VectorFixed\.h>/<vnl\/vnl_vector_fixed.h>/;
    s/<(math|Numerics)\/MatrixFixed\.h>/<vnl\/vnl_matrix_fixed.h>/;
    s/<Numerics\/MatrixRef\.h>/<vnl\/vnl_matrix_ref.h>/;
    s/UnaryFunction\.h/vnl_unary_function.h>/;
    s/Identity\.h/vnl_identity.h>/;
    
    s/<Numerics\/LevenbergMarquardt\.h>/<vnl\/algo\/vnl_levenberg_marquardt.h>/g;
    s/<Numerics\/SymmetricEigensystem\.h>/<vnl\/algo\/vnl_symmetric_eigensystem.h>/g;
    s/<Numerics\/DiscreteDiff\.h>/<vnl\/algo\/vnl_discrete_diff.h>/g;

    s/<Numerics\/ComplexVectorT/<vnl\/vnl_complex_vector_t/g;
    s/<Numerics\/ComplexVector/<vnl\/vnl_complex_vector/g;
    s/<Numerics\/ComplexMatrixT/<vnl\/vnl_complex_matrix_t/g;
    s/<Numerics\/ComplexMatrix/<vnl\/vnl_complex_matrix/g;
    s/<Numerics\/LeastSquaresFunction/<vnl\/vnl_least_squares_function/g;
    s/<Numerics\/LeastSquaresCostFunction\.h>/<vnl\/vnl_least_squares_cost_function.h>/g;
    s/<Numerics\/NonLinearMinimizer\.h>/<vnl\/vnl_nonlinear_minimizer.h>/g;
    s/<Numerics\/Double([0-9]x[0-9])\.h>/<vnl\/vnl_double_$1.h>/g;
    s/<Numerics\/Double([0-9])\.h>/<vnl\/vnl_double_$1.h>/g;
    s/<Numerics\/LinearOperators3\.h>/<vnl\/vnl_linear_operators_3.h>/g;
    s/<Numerics\/ChiSquared\.h>/<vnl\/algo\/vnl_chi_squared.h>/g;

    s!<cool/Quaternion.h>!<vnl/vnl_quaternion.h>!g;
    
    # classes and functions
    s/\bCPolyRoots\b/vnl_cpoly_roots/g;
    s/\bIUE_c_vector\b/vnl_c_vector/g;
    s/\bIUE_vector\b/vnl_vector/g;
    s/\bIUE_matrix\b/vnl_matrix/g;
    s/\bDiagMatrix\b/vnl_diag_matrix/g;
    s/IUE_math/vnl_math/g;
    s/\bMath\b/vnl_math/g;
    s/vnl_math::isnan/vnl_math_isnan/g;
    s/vnl_math::isinf/vnl_math_isinf/g;
    s/vnl_math::isfinite/vnl_math_isfinite/g;
    s/vnl_math::rnd\b/vnl_math_rnd/g;
    s/vnl_math::abs\b/vnl_math_abs/g;
    s/vnl_math::max\b/vnl_math_max/g;
    s/vnl_math::min\b/vnl_math_min/g;
    s/vnl_math::sqr\b/vnl_math_sqr/g;
    s/vnl_math::sgn/vnl_math_sgn/g;
    s/vnl_math::squared_magnitude/vnl_math_squared_magnitude/g;
    s/vnl_math::cuberoot/vnl_math_cuberoot/g;
    s/vnl_math::cbrt/vnl_math_cuberoot/g;
    s/vnl_math::hypot/vnl_math_hypot/g;
    s/\bfloat_complex\b/vnl_float_complex/g;
    s/\bdouble_complex\b/vnl_double_complex/g;
    s/ChiSquared::ChiSquaredCumulative/vnl_chi_squared_cumulative/g;
    s/\bSparseSymmetricEigensystem\b/vnl_sparse_symmetric_eigensystem/g;
    s/\bComplexEigensystem\b/vnl_complex_eigensystem/g;
    s/\bSymmetricEigensystem\b/vnl_symmetric_eigensystem<double> /g;
    s/\bEigensystem\b/vnl_real_eigensystem/g;
    s/GeneralizedEigensystem/vnl_generalized_eigensystem/g;
    s/RealPolynomial/vnl_real_polynomial/g;
    s/DoubleVector/vnl_vector<double>/g;
    s/DoubleMatrix/vnl_matrix<double>/g;
    s/SparseMatrix/vnl_sparse_matrix/g;
    s/DoubleMatrix/vnl_matrix<double>/g;
    s/\bfortran_copy\b/vnl_fortran_copy/g;
    s/\bMatOps\b/vnl_matops/g;
    s/\bvnl_matops::matlab_print\b/vnl_matlab_print/g;
    s/\bMATLABMatFileWrite\b/vnl_matlab_filewrite/g;
    s/FileMatrix/vnl_file_matrix<double>/g;
    s/FileVector/vnl_file_matrix<double>/g;
    s/MatrixInverse/vnl_matrix_inverse<double>/g;
    s/RPolyRoots/vnl_rpoly_roots/g;
    s/RealPolynomial/vnl_real_polynomial/g;
    s/RealNPolynomial/vnl_real_npolynomial/g;
    s/RNPolySolve/vnl_rnpoly_solve/g;
    s/\bCholesky\b/vnl_cholesky/g;
    s/\bQR\b/vnl_qr/g;
    s/\bBaseSVD\b/vnl_svd/g;
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
    s/\bIUE_Identity\b/vnl_identity/g;
    s/LevenbergMarquardt/vnl_levenberg_marquardt/g;
    s/NonLinearMinimizer/vnl_nonlinear_minimizer/g;
    s/\bMatrixFixedRef\b/vnl_matrix_fixed_ref/g;
    s/\bMatrixFixed\b/vnl_matrix_fixed/g;
    s/\bVectorFixed\b/vnl_vector_fixed/g;
    s/\bVectorRef\b/vnl_vector_ref/g;
    s/\bMatrixRef\b/vnl_matrix_ref/g;
    s/RotationMatrix/vnl_rotation_matrix/g;
    s/Identity3x3/vnl_identity_3x3/g;
    s/CrossProductMatrix/vnl_cross_product_matrix/g;
    s/\bLBFGS\b/vnl_lbfgs/g;
    s/\bTranspose\b/vnl_transpose/g;
    s/\bFastOps\b/vnl_fastops/g;
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
    s/\bAmoeba\b/vnl_amoeba/g;
    s/IUE_VectorDereference/vnl_vector_dereference/g;
    s/VectorDereference/vnl_vector_dereference/g;
    s/\bInt2\b/vnl_int_2/g;
    s/\bInt3\b/vnl_int_3/g;
    s/\bInt4\b/vnl_int_4/g;
    s/ScalarJoinIterator/vnl_scalar_join_iterator/g;
    #awf s/\bresize\b/vnl_resize/g;
    s/(\.|\->)maxVal\b/$1max_value/g;
    s!\bCoolQuaternion<!vnl_quaternion<!g;
    
    # test names
    s/Numerics_Test_AssertNear/vnl_test_assert_near/g;
    s/Numerics_Test_Assert/vnl_test_assert/g;
  }

  if ($vgl) {
    # includes
    #s!MViewBasics/HomgPoint2D!vgl/vgl_homg_point_2d!g;
    #s!MViewBasics/HomgLine2D!vgl/vgl_homg_line_2d!g;
  }
  
  if ($vil) {
    # includes
    s!<EasyImage/ImageBuffer.h>!<vil/vil_memory_image_of.h>!g;
    s!<EasyImage/ImageWindowOps.h>!<vil/vil_memory_image_window.h>!g;
    s!<EasyImage/FileImage.h>!<vil/vil_file_image.h>!g;
    s!<ImageClasses/Image.h>!<vil/vil_image.h>!g;
    
    # classes
    s!\bFileImage\b!vil_file_image!g;
    s!\bImageBuffer\b!vil_memory_image_of!g;
    s!\bImageWindowOps\b!vil_memory_image_window!g;
    s!\bImage *\* *\b!vil_image !g;

    # possibly risky: methods
    #  .GetSizeX()     .width()
    # ->GetSizeX()    ->width()
    
    s!(\.|\-\>)GetSizeX\(\)!$1width()!g;
    s!(\.|\-\>)GetSizeY\(\)!$1height()!g;
    s!(\.|\-\>)GetBuffer\(\)!$1get_buffer()!g;
  }
  
  if ($mvl) {

    # includes
    s!MViewBasics/!mvl/!g;
    s!MViewCompute/!mvl/!g;
    s!MViewComputeOX/!mvox/!g;

    s!<cool/Array.h>!<vcl/vcl_vector.h>!;
    s!<cool/ArrayP.h>!<vcl/vcl_vector.h>!;
    s!<cool/Array.C>!<vcl/vcl_vector.txx>!;
    s!<cool/List(|P).h>!<vcl/vcl_list.h>!;
    s!<cool/List.C>!<vcl/vcl_list.txx>!;
    s!^(\#include +\<(Geometry|Topology)/)!//$1!g;

    # symbols
    s!\bCoolArray\b!vcl_vector!g;
    s!\bCoolArrayP\b!vcl_vector!g;
    s!\bCoolList\b!vcl_list!g;
    s!\bCoolListP\b!vcl_list!g;

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

  if ($fsm) {
    # identifiers may not start with underscore :
    if ( m/\b_(\w+)\b/ ) {
      my ($id) = $1;
      if ($id =~ m/^_/) {
	# system ones, like __GNUC__, may so just ignore them.
      } else {
	if (!defined($oked{$id})) {
	  $oked{$id} = 1;
	  print STDERR "replacing '_${id}' with '${id}_'\n";
	}
	eval "s/\\b_${id}\\b/${id}_/g" if $oked{$id};
      }
    }
    # it's "T const", not "const T" :
    s/\bconst (void|char|unsigned char|int|float|double)\b/$1 const/g;
    s/\bconst (vnl_vector<float>|vnl_vector<double>|vnl_vector<T>|vnl_vector<T >)/$1 const/g;
    s/\bconst (vnl_vector<vcl_double_complex>|vnl_vector<vcl_float_complex>)/$1 const/g;
    s/\bconst (vnl_vector<vnl_double_complex>|vnl_vector<vnl_float_complex>)/$1 const/g;
    s/\bconst (vnl_matrix<float>|vnl_matrix<double>|vnl_matrix<T>|vnl_matrix<T >)/$1 const/g;
    s/\bconst (vnl_matrix<vcl_double_complex>|vnl_matrix<vcl_float_complex>)/$1 const/g;
    s/\bconst (vnl_matrix<vnl_double_complex>|vnl_matrix<vnl_float_complex>)/$1 const/g;
  }

  #
  if ($lint) {
    print STDERR "use of 'NULL'\n" if m/\bNULL\b/;
    print STDERR "use of 'nil'\n" if m/\bnil\b/;
    print STDERR "use of VCL_DECLARE_SPECIALIZATION\n" if m/\bVCL_DECLARE_SPECIALIZATION\b/;
  }

  if ($tmpl) {
    push @lines, $_;
  } else {
    print;
  }
}

close(FD);

#--------------------------------------------------------------------------------

# 
if ($tmpl) {
  print "// vxl_converted with -tmpl\n";
  # first emit the lines with headers on them :
  #print "// original headers\n";
  foreach my $line (@lines) {
    print $line if ($line =~ m/^\s*\#include/);
  }
  # if used less<T> but not function.h, include the latter :
  print "#include <vcl/vcl_functional.h>\n" if ($saw_less && !$saw_stl_functional);
  # emit any .txx files needed :
  #print "// .txx files\n";
  foreach my $class (@txx_needed) {
    print "#include <vcl/vcl_$class.txx>\n";
  }
  # then emit non-header lines :
  #print "// non-header :\n";
  foreach my $line (@lines) {
    print $line unless ($line =~ m/^\s*\#include/);
  }
}
