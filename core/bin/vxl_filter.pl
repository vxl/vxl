#! /bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6

# Author: Dave Cooper
#
# How it works:
# Lines are read from stdin, processed and output on stdout.
# The processing stages are :
#     - each line is separated into code + comment.
#     - header file conversion.
#     - look for hints about various things, like whether
#       'string' is a type or an identifier.
#     - conversion of code (using hints found).
#     - emit code + comment for each processed line.
#
# The script keeps track of which headers have been included and
# which stl classes have been used.
#
# Bugs and improvements.
#   1. the script uses some pretty good heuristics to decide
#      whether 'string' is an identifier or a type, but at
#      the moment the decision is made globally, for an entire
#      source file. it sometimes get it wrong.
#
#   2. Sometimes .txx files are included several times.
#
#   3. Most substitutions in this script could be done with the
#      o flag (s/.../.../o;) so that the regexp is compiled
#      only once. That may or may not speed up the script.

$vcl  = 0; # convert Config-IUE and standard C/C++ code to vcl
$vbl  = 0; # convert Basics code to vbl
$vnl  = 0; # convert Numerics code to vnl
$vil  = 0; # convert EasyImage to vil
$osl  = 0; #
$vgui = 0; # convert old vgui to new vgui
$lint = 0; #
$mvl  = 0;
$cool = 0;

sub parse_options {
  foreach my $arg (@ARGV) {
    if ($arg eq "-piglet") { print "# pragma piglet\n"; }
    elsif ($arg eq "-vcl" ) { $vcl  = 1; }
    elsif ($arg eq "-vbl" ) { $vbl  = 1; }
    elsif ($arg eq "-vnl" ) { $vnl  = 1; }
    elsif ($arg eq "-vil" ) { $vil  = 1; }
    elsif ($arg eq "-osl" ) { $osl  = 1; }
    elsif ($arg eq "-vgl" ) { $vgl  = 1; }
    elsif ($arg eq "-vxl" ) { $vgl  = $vnl = $vil = $vbl = $vcl = 1; }
    elsif ($arg eq "-vgui") { $vgui = 1; }
    elsif ($arg eq "-mvl")  { $mvl  = 1; }
    elsif ($arg eq "-cool") { $cool = 1; }
    elsif ($arg eq "-lint") { $lint = 1; }
    else {
      print STDERR "dunno about '$arg'\n";
    }
  }
}

# flags to indicate that certain headers have been used.
$saw_stl_instantiate = 0; # <stl-instantiate.h>
$saw_stlfwd          = 0; # <IUE_stlfwd.h>
$saw_functional_h    = 0; # <functional>
$saw_utility_h       = 0; # <utility>
$saw_vector_h        = 0; # <vector>
$saw_list_h          = 0; # <list>
$saw_deque_h         = 0; # <deque>
$saw_queue_h         = 0; # <queue>
$saw_map_h           = 0; # <map>
$saw_set_h           = 0; # <set>
$saw_string_h        = 0; # <string>
$saw_memory_h        = 0; # <memory>

# flags to indicate that certain types have been used.
$saw_less      = 0; # less<>
$saw_pair      = 0; # pair<>
$saw_vector    = 0; # vector<>
$saw_list      = 0; # list<>
$saw_deque     = 0; # deque<>
$saw_queue     = 0; # queue<>
$saw_map       = 0; # saw map<> or multimap<>
$saw_set       = 0; # set<> or multiset<>
$saw_string    = 0; # string
$saw_allocator = 0; # allocator<>

# list of .txx files needed (e.g. 'map', 'vector', 'utility')
@txx_needed = ();

#--------------------------------------------------------------------

@lines = ();           # accumulator for lines.
@comments = ();        # accumulator for comments.
$num_lines = 0;

# read all lines :
sub read_lines {
  @lines = ();
  @comments = ();

  die unless open(FD, "-");
  while (<FD>) {
    # extract comment-part, if any. this is to make sure we only
    # perform substitutions on the code, not the comments.
    chop;
    if (m/^([^\"]*)(\/\/.*)/) {
      push @lines, $1;
      push @comments, $2;
    }
    else {
      push @lines, $_;
      push @comments, "";
    }
  }
  close(FD);
  die unless ($#lines == $#comments);
  $num_lines = 1 + $#lines;
}

#--------------------------------------------------------------------------------

# process include files :
sub process_headers {
  for (my $ii=0; $ii<$num_lines; ++$ii) {
    # copy line from array to $_:
    $_ = $lines[$ii];

    # skip lines that contain the word "dont_vxl_filter"
    next if m/\bdont_vxl_filter\b/;

    #---------------------------------------- old <vcl/vcl_blah> -> new <vcl_blah>
    s/include\s+<vcl\/([^>]*vcl_[^>]*)>/include <$1>/;

    #---------------------------------------- Config-IUE -> vcl
    if ($vcl) {
      #-------------------- begin

      # standard C headers <blah.h>
      s/include\s*<assert\.h>/include <vcl_cassert.h>/;
      s/include\s*<iso646\.h>/include <vcl_ciso646.h>/;
      s/include\s*<setjmp\.h>/include <vcl_csetjmp.h>/;
      s/include\s*<stdio\.h>/include <vcl_cstdio.h>/;
      s/include\s*<time\.h>/include <vcl_ctime.h>/;
      s/include\s*<ctype\.h>/include <vcl_cctype.h>/;
      s/include\s*<limits\.h>/include <vcl_climits.h>/;
      s/include\s*<signal\.h>/include <vcl_csignal.h>/;
      s/include\s*<stdlib\.h>/include <vcl_cstdlib.h>/;
      s/include\s*<wchar\.h>/include <vcl_cwchar.h>/;
      s/include\s*<errno\.h>/include <vcl_cerrno.h>/;
      s/include\s*<locale\.h>/include <vcl_clocale.h>/;
      s/include\s*<stdarg\.h>/include <vcl_cstdarg.h>/;
      s/include\s*<string\.h>/include <vcl_cstring.h>/;
      s/include\s*<wctype\.h>/include <vcl_cwctype.h>/;
      s/include\s*<float\.h>/include <vcl_cfloat.h>/;
      s/include\s*<math\.h>/include <vcl_cmath.h>/;
      s/include\s*<stddef\.h>/include <vcl_cstddef.h>/;

      # standard C headers <cblah>
      s/include\s*<cassert>/include <vcl_cassert.h>/;
      s/include\s*<ciso646>/include <vcl_ciso646.h>/;
      s/include\s*<csetjmp>/include <vcl_csetjmp.h>/;
      s/include\s*<cstdio>/include <vcl_cstdio.h>/;
      s/include\s*<ctime>/include <vcl_ctime.h>/;
      s/include\s*<cctype>/include <vcl_cctype.h>/;
      s/include\s*<climits>/include <vcl_climits.h>/;
      s/include\s*<csignal>/include <vcl_csignal.h>/;
      s/include\s*<cstdlib>/include <vcl_cstdlib.h>/;
      s/include\s*<cwchar>/include <vcl_cwchar.h>/;
      s/include\s*<cerrno>/include <vcl_cerrno.h>/;
      s/include\s*<clocale>/include <vcl_clocale.h>/;
      s/include\s*<cstdarg>/include <vcl_cstdarg.h>/;
      s/include\s*<cstring>/include <vcl_cstring.h>/;
      s/include\s*<cwctype>/include <vcl_cwctype.h>/;
      s/include\s*<cfloat>/include <vcl_cfloat.h>/;
      s/include\s*<cmath>/include <vcl_cmath.h>/;
      s/include\s*<cstddef>/include <vcl_cstddef.h>/;

      # standard C++ headers <blah.h>
      s/include\s*<algorithm\.h>/include <vcl_algorithm.h>/;
      s/include\s*<iomanip\.h>/include <vcl_iomanip.h>/;
      s/include\s*<list\.h>/include <vcl_list.h>/;
      s/include\s*<iostream\.h>/include <vcl_iostream.h>/;
      s/include\s*<streambuf\.h>/include <vcl_streambuf.h>/;
      s/include\s*<bitset\.h>/include <vcl_bitset.h>/;
      s/include\s*<ios\.h>/include <vcl_ios.h>/;
      s/include\s*<locale\.h>/include <vcl_locale.h>/;
      s/include\s*<queue\.h>/include <vcl_queue.h>/;
      #s/include\s*<string\.h>/include <vcl_string.h>/;
      s/include\s*<complex\.h>/include <vcl_complex.h>/;
      s/include\s*<iosfwd\.h>/include <vcl_iosfwd.h>/;
      s/include\s*<map\.h>/include <vcl_map.h>/;
      s/include\s*<set\.h>/include <vcl_set.h>/;
      s/include\s*<typeinfo\.h>/include <vcl_typeinfo.h>/;
      s/include\s*<deque\.h>/include <vcl_deque.h>/;
      s/include\s*<iostream\.h>/include <vcl_iostream.h>/;
      s/include\s*<memory\.h>/include <vcl_memory.h>/;
      s/include\s*<sstream\.h>/include <vcl_sstream.h>/;
      s/include\s*<utility\.h>/include <vcl_utility.h>/;
      s/include\s*<exception\.h>/include <vcl_exception.h>/;
      s/include\s*<istream\.h>/include <vcl_istream.h>/;
      s/include\s*<new\.h>/include <vcl_new.h>/;
      s/include\s*<stack\.h>/include <vcl_stack.h>/;
      s/include\s*<valarray\.h>/include <vcl_valarray.h>/;
      s/include\s*<fstream\.h>/include <vcl_fstream.h>/;
      s/include\s*<iterator\.h>/include <vcl_iterator.h>/;
      s/include\s*<numeric\.h>/include <vcl_numeric.h>/;
      s/include\s*<stdexcept\.h>/include <vcl_stdexcept.h>/;
      s/include\s*<vector\.h>/include <vcl_vector.h>/;
      s/include\s*<functional\.h>/include <vcl_functional.h>/;
      #s/include\s*<limits\.h>/include <vcl_limits.h>/;
      s/include\s*<strstream\.h>/include <vcl_strstream.h>/;
      s/include\s*<strstrea\.h>/include <vcl_strstream.h>/;  # win32

      # standard C++ headers <blah>
      s/include\s*<algorithm>/include <vcl_algorithm.h>/;
      s/include\s*<iomanip>/include <vcl_iomanip.h>/;
      s/include\s*<list>/include <vcl_list.h>/;
      s/include\s*<iostream>/include <vcl_iostream.h>/;
      s/include\s*<streambuf>/include <vcl_streambuf.h>/;
      s/include\s*<bitset>/include <vcl_bitset.h>/;
      s/include\s*<ios>/include <vcl_ios.h>/;
      s/include\s*<locale>/include <vcl_locale.h>/;
      s/include\s*<queue>/include <vcl_queue.h>/;
      s/include\s*<string>/include <vcl_string.h>/;
      s/include\s*<complex>/include <vcl_complex.h>/;
      s/include\s*<iosfwd>/include <vcl_iosfwd.h>/;
      s/include\s*<map>/include <vcl_map.h>/;
      s/include\s*<set>/include <vcl_set.h>/;
      s/include\s*<typeinfo>/include <vcl_typeinfo.h>/;
      s/include\s*<deque>/include <vcl_deque.h>/;
      s/include\s*<iostream>/include <vcl_iostream.h>/;
      s/include\s*<memory>/include <vcl_memory.h>/;
      s/include\s*<sstream>/include <vcl_sstream.h>/;
      s/include\s*<utility>/include <vcl_utility.h>/;
      s/include\s*<exception>/include <vcl_exception.h>/;
      s/include\s*<istream>/include <vcl_istream.h>/;
      s/include\s*<new>/include <vcl_new.h>/;
      s/include\s*<stack>/include <vcl_stack.h>/;
      s/include\s*<valarray>/include <vcl_valarray.h>/;
      s/include\s*<fstream>/include <vcl_fstream.h>/;
      s/include\s*<iterator>/include <vcl_iterator.h>/;
      s/include\s*<numeric>/include <vcl_numeric.h>/;
      s/include\s*<stdexcept>/include <vcl_stdexcept.h>/;
      s/include\s*<vector>/include <vcl_vector.h>/;
      s/include\s*<functional>/include <vcl_functional.h>/;
      s/include\s*<limits>/include <vcl_limits.h>/;
      s/include\s*<strstream>/include <vcl_strstream.h>/;

      #-------------------- end

      # the <IUE_*.h> bunch
      s/<IUE_compiler\.h>/<vcl_compiler.h>/;
      s/<IUE_specific_compiler\.h>/<vcl_compiler.h>/;
      s/<IUE_string\.h>/<vcl_string.h>/;
      s/<IUE_iosfwd\.h>/<vcl_iosfwd.h>/;
      s/<IUE_complex\.h>/<vcl_complex.h>/;
      s/<IUE_cmath\.h>/<vcl_cmath.h>/;
      s/\bIUE_long_double\b/long double/g;
      $saw_stlfwd = 1 if s!^\#include.*<IUE_stlfwd\.h>!//$&!;

      # non-standard STL headers which can be fixed.
      s/include\s*<algo(\.h)?>/include <vcl_algorithm.h>/;
      s/include\s*<algobase(\.h)?>/include <vcl_algorithm.h>/;
      s/include\s*<function(\.h)?>/include <vcl_functional.h>/;
      s/include\s*<multimap(\.h)?>/include <vcl_map.h>/;
      #s/include\s*<hash_map\.h>/include <vcl_map.h>/;
      s/include\s*<multiset(\.h)?>/include <vcl_set.h>/;
      #s/include\s*<hash_set\.h>/include <vcl_set.h>/;
      s/include\s*<pair(\.h)?>/include <vcl_utility.h>/;
      s/include\s*<alloc(\.h)?>/include <vcl_memory.h>/;
      # non-standard STL headers which are to be commented out.
      s/^(\s*\#\s*include <defalloc.h>)/\/\/$1/;
      s/^(\s*\#\s*include <tree(\.h)?>)/\/\/$1/;
      # `global' stl algorithms to be replaced with their vcl_ conterpart.
      s/(\s)::((sort|find|swap_ranges|copy|fill|find_if)\s*\()/$1vcl_$2/;

      # std::cout, std::cerr, std::endl etc
      s/\b(std::)\s*(cout|cerr|endl|ends|(i|o|f|if|of|str|ostr)stream)\b/vcl_$2/g;
      s/\b(cout|cerr|endl|ends|ios|(i|o|f|if|of|str|ostr)stream)\b/vcl_$1/g;

      # misc functions names.
      s/\b(memcpy|memset|memmove|strcat|strchr|strcmp|strncmp|strcpy|strncpy|strlen)\b/vcl_$1/g;
      s/\b(sprintf|printf|fprintf|fwrite|fread|fopen|fclose)\b/vcl_$1/g;
      s/\b(abort|exit|abs|atoi|atof)\b/vcl_$1/g;
      s/\b(sqrt|exp|log|atan2|cos|sin|tan|pow|acos|asin|atan|ceil|floor|fabs)\b/vcl_$1/g;
      s/\b(ptrdiff_t|size_t|time_t)\b/vcl_$1/g;

      # remember what we saw
      $saw_functional_h = 1 if m/include <vcl_functional\.h>/;
      $saw_map_h        = 1 if m/include <vcl_map\.h>/;
      $saw_set_h        = 1 if m/include <vcl_set\.h>/;
      $saw_utility_h    = 1 if m/include <vcl_utility\.h>/;
      $saw_vector_h     = 1 if m/include <vcl_vector\.h>/;
      $saw_list_h       = 1 if m/include <vcl_list\.h>/;
      $saw_deque_h      = 1 if m/include <vcl_deque\.h>/;
      $saw_queue_h      = 1 if m/include <vcl_queue\.h>/;
      $saw_string_h     = 1 if m/include <vcl_string\.h>/;
      $saw_memory_h     = 1 if m/include <vcl_memory\.h>/;

      # warn about hash_map and hash_set
      if ( m/include\s+\<hash_(map|set)\.h\>/ ) {
        print STDERR "[WARNING: saw hash_map/hash_set]\n";
      }

      # instantiation macros
      $saw_stl_instantiate = 1 if ( s/^(\#include <stl\-instantiate\.h>)/\/\/ stl-instantiate.h/ );
    }

    #---------------------------------------- COOL -> vcl
    if ($cool) {
      s!<cool/String.h>!<vcl_string.h>!;
    }

    #---------------------------------------- Basics -> vbl
    if ($vbl) {
      # headers
      s!Basics/ansi.h!vcl_compiler.h!;
      s!^\#include.*<Basics/point2d.h>!//$&!;  # delete Basics/point2d

      s/<Basics\/RGB\.h>/<vil\/vil_rgb.h>/;
      s/<Basics\/RGBcell\.h>/<vil\/vil_rgb.h>/;
      s/<Basics\/RGBA\.h>/<vil\/vil_rgba.h>/;
      s/<vbl\/vbl_rgb\.h>/<vil\/vil_rgb.h>/;
      s/<vbl\/vbl_rgba\.h>/<vil\/vil_rgba.h>/;
      s/<Basics\/ArgParse\.h>/<vbl\/vbl_arg.h>/;
      s/<Basics\/ArgInfoList\.h>/<vbl\/vbl_arg.h>/;
      s/<Basics\/IUE_sprintf\.h>/<vbl\/vbl_sprintf.h>/;
      s/<Basics\/Array2D\.h>/<vbl\/vbl_array_2d.h>/;
      s/<Basics\/Array3D\.h>/<vbl\/vbl_array_3d.h>/;
      s/<Basics\/Array2D\.C>/<vbl\/vbl_array_2d.txx>/;
      s/<Basics\/Array3D\.C>/<vbl\/vbl_array_3d.txx>/;
      s/<Basics\/BaseArray3D\.h>/<vbl\/vbl_base_array_3d.h>/;
      s/<Basics\/file\.h>/<vbl\/vbl_file.h>/;
      s/<Basics\/stat\.h>/<vbl\/vbl_file.h> \/\/ use vbl_file instead of IUE_stat/;
      s/<Basics\/IUE_stat\.h>/<vbl\/vbl_file.h> \/\/ use vbl_file instead of IUE_stat/;
      s/<Basics\/directory\.h>/<vbl\/vbl_file.h> \/\/ use vbl_file instead of IUE_IUE_directory/;
      s/<Basics\/IUE_directory\.h>/<vbl\/vbl_file.h> \/\/ use vbl_file instead of IUE_directory/;
      s/<Basics\/printf\.h>/<vbl\/vbl_printf.h>/;
      s/<Basics\/AWK\.h>/<vbl\/vbl_awk.h>/;
      s/<Basics\/clamp\.h>/<vil\/vil_clamp.h>/;
      s/<vbl\/vbl_clamp\.h>/<vil\/vil_clamp.h>/;
      s/<Basics\/QSort\.h>/<vbl\/vbl_qsort.h>/;
      s!Basics/BoundingBox.h!vbl/vbl_bounding_box.h!;
      s!Basics/BoundingBox.C!vbl/vbl_bounding_box.txx!;

      # sparse array things.
      s!<Basics/SparseArray([123])D.h>!<vbl/vbl_sparse_array_$1d.h>!;
      s!<Basics/SparseArray([123])D.C>!<vbl/vbl_sparse_array_$1d.txx>!;
      s!<Basics/SparseArray.h>!<vbl/vbl_sparse_array.h>!;
      s!<Basics/SparseArray.C>!<vbl/vbl_sparse_array.txx>!;

      s/<cool\/Timer\.h>/<vbl\/vbl_timer.h>/;
      s/<cool\/RegExp\.h>/<vbl\/vbl_reg_exp.h>/;
      s!(\#include *\<cool\/decls.h\>)!//$1!;
    }

    #---------------------------------------- Numerics -> vnl
    if ($vnl) {
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
      s/<cool\/Vector\.h>/<vnl\/vnl_vector.h>/;
      s/<cool\/Matrix\.h>/<vnl\/vnl_matrix.h>/;

      s/<Numerics\/Math\.h>/<vnl\/vnl_math.h>/;
      s/<Numerics\/RPolyRoots\.h>/<vnl\/algo\/vnl_rpoly_roots.h>/;
      s/<Numerics\/CPolyRoots\.h>/<vnl\/algo\/vnl_cpoly_roots.h>/;
      s/<Numerics\/Cholesky.h>/<vnl\/algo\/vnl_cholesky.h>/;
      s/<Numerics\/QR.h>/<vnl\/algo\/vnl_qr.h>/;
      s/<Numerics\/SVD.h>/<vnl\/algo\/vnl_svd.h>/;
      s/<Numerics\/ComplexEigensystem\.h>/<vnl\/algo\/vnl_complex_eigensystem.h>/;
      s/<Numerics\/GeneralizedEigensystem\.h>/<vnl\/algo\/vnl_generalized_eigensystem.h>/;
      s/<Numerics\/RealPolynomial\.h>/<vnl\/vnl_real_polynomial.h>/;
      s/<Numerics\/FileMatrix\.h>/<vnl\/vnl_file_matrix.h>/;
      s/<Numerics\/FileVector\.h>/<vnl\/vnl_file_vector.h>/;
      s/<Numerics\/MatrixInverse\.h>/<vnl\/algo\/vnl_matrix_inverse.h>/;
      s/<Numerics\/Eigensystem\.h>/<vnl\/algo\/vnl_real_eigensystem.h>/;
      s/<Numerics\/SparseMatrix\.h>/<vnl\/vnl_sparse_matrix.h>/;
      s/<Numerics\/Transpose\.h>/<vnl\/vnl_transpose.h>/;
      s/<Numerics\/Scatter3x3\.h>/<vnl\/algo\/vnl_scatter_3x3.h>/g;
      s/<Numerics\/Amoeba\.h>/<vnl\/algo\/vnl_amoeba.h>/g;
      s/<Numerics\/CrossProductMatrix\.h>/<vnl\/vnl_cross_product_matrix.h>/g;
      s/<Numerics\/Identity3x3\.h>/<vnl\/vnl_identity_3x3.h>/g;
      s/<Numerics\/Int([0-9x]+)\.h>/<vnl\/vnl_int_$1.h>/;
      s/<Numerics\/Float([0-9x]+)\.h>/<vnl\/vnl_float_$1.h>/;
      s/<Numerics\/Double([0-9x]+)\.h>/<vnl\/vnl_double_$1.h>/;
      s/<Numerics\/OuterProduct(|3x3)\.h>/<vnl\/vnl_vector.h>/g;
      s/<math\/resize\.h>/<vnl\/vnl_resize.h>/;
      s/^(\#include <math\/matlab\.h>)/\#define matlab(M) M \/\/$1/;
      s/<math\/BaseSVD\.h>/<vnl\/algo\/vnl_svd.h>/;
      s/<math\/numeric_limits(|_double|_float|_int)\.h>/<vcl_limits.h>/;
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
      s/math\/Identity\.h/vnl\/vnl_identity.h>/;

      s/<Numerics\/LevenbergMarquardt\.h>/<vnl\/algo\/vnl_levenberg_marquardt.h>/;
      s/<Numerics\/SymmetricEigensystem\.h>/<vnl\/algo\/vnl_symmetric_eigensystem.h>/;
      s/<Numerics\/DiscreteDiff\.h>/<vnl\/algo\/vnl_discrete_diff.h>/;

      s/<Numerics\/ComplexVectorT/<vnl\/vnl_complex_vector_t/;
      s/<Numerics\/ComplexVector/<vnl\/vnl_complex_vector/;
      s/<Numerics\/ComplexMatrixT/<vnl\/vnl_complex_matrix_t/;
      s/<Numerics\/ComplexMatrix/<vnl\/vnl_complex_matrix/;
      s/<Numerics\/LeastSquaresFunction/<vnl\/vnl_least_squares_function/;
      s/<Numerics\/LeastSquaresCostFunction\.h>/<vnl\/vnl_least_squares_cost_function.h>/;
      s/<Numerics\/NonLinearMinimizer\.h>/<vnl\/vnl_nonlinear_minimizer.h>/;
      s/<Numerics\/Double([0-9]x[0-9])\.h>/<vnl\/vnl_double_$1.h>/;
      s/<Numerics\/Double([0-9])\.h>/<vnl\/vnl_double_$1.h>/;
      s/<Numerics\/LinearOperators3\.h>/<vnl\/vnl_linear_operators_3.h>/;
      s/<Numerics\/ChiSquared\.h>/<vnl\/algo\/vnl_chi_squared.h>/;

      s!<cool/Quaternion.h>!<vnl/vnl_quaternion.h>!;
    }

    #---------------------------------------- vgl
    if ($vgl) {
      # includes
      #s!MViewBasics/HomgPoint2D!vgl/vgl_homg_point_2d!;
      #s!MViewBasics/HomgLine2D!vgl/vgl_homg_line_2d!;
    }

    #---------------------------------------- vil
    if ($vil) {
      # includes
      s!<EasyImage/ImageBuffer.h>!<vil/vil_memory_image_of.h>!;
      s!<EasyImage/ImageWindowOps.h>!<vil/vil_memory_image_window.h>!;
      s!<EasyImage/FileImage.h>!<vil/vil_file_image.h>!;
      s!<ImageClasses/Image.h>!<vil/vil_image.h>!;
    }

    #---------------------------------------- MultiView -> mvl
    if ($mvl) {

      # includes
      s!MViewBasics/!mvl/!;
      s!MViewCompute/!mvl/!;
      s!MViewComputeOX/!mvox/!;

      s!<cool/Array.h>!<vcl_vector.h>!;
      s!<cool/ArrayP.h>!<vcl_vector.h>!;
      s!<cool/Array.C>!<vcl_vector.txx>!;
      s!<cool/List(|P).h>!<vcl_list.h>!;
      s!<cool/List.C>!<vcl_list.txx>!;
      s!^(\#include +\<(Geometry|Topology)/)!//$1!;
    }

    #---------------------------------------- osl
    if ($osl) {
      s!<osl/fsm_ortho_regress.h>!<osl/osl_ortho_regress.h>!;
    }

    #---------------------------------------- VGUI -> vgui
    if ($vgui) {
      s/\bVGUI/vgui/g;
      s/<vgui\/vgui_image\.h>/<vil\/vil_image.h>/;
      s!tableaux/!vgui/!;
      s!tableaux_DLLDATA!vgui_DLLDATA!g;
      s!vgui_displaybase.C!vgui_displaybase.txx!;
      s!obl/RGBA!vbl/vbl_rgba!g;

      s!obl/bool_ostream!vbl/vbl_bool_ostream!g;
      s!obl_on_off!vbl_bool_ostream::on_off!g;
      s!obl_true_false!vbl_bool_ostream::true_false!g;
      s!obl_high_low!vbl_bool_ostream::high_low!g;
      s!IUE_glu.h!vgui/vgui_glu.h!;
      s!IUE_glut.h!vgui/vgui_glut.h!;
      s!GL/glx.h!vgui/vgui_glx.h!;
      s!GL/glu.h!vgui/vgui_glu.h!;
      s!GL/glut.h!vgui/vgui_glut.h!;
      s!<IUE_gl!<vgui/vgui_gl!;
      s!<vgui_gtk/!<vgui/impl/gtk/!;
    }

    #---------------------------------------- misc
    if ($lint) {
    }

    # put $_ back into array:
    $lines[$ii] = $_;
  }
}

#--------------------------------------------------------------------------------

my $string_is_typename = 0;
my $string_is_identifier = 0;

# scan @lines for hints :
sub scan_for_hints {
  for (my $ii=0; $ii<$num_lines; ++$ii) {
    # copy line from array to $_:
    $_ = $lines[$ii];

    if ($vcl) {
      # sometimes "string" is the name of a type.
      if (!$string_is_typename && !$string_is_identifier) {
        if ( m/^\s*string\b/ ||                     #^string          (return value or decl-statement)
             m/\bstring\s*::/ ||                    # string::        (e.g. string::npos)
             m/\bstring\s*&/ ||                     # string &
             m/\bstring\s*\*/ ||                    # string *
             m/\bstring\s+const\s*&/ ||             # string const &
             m/\bconst\s+string\s*&/ ||             # const string &
             m/\<\s*string\s*\>/ ||                 # <string>        (template parameter)
             m/\<\s*string\s*,.*\>/ ||              # <string,???>    (template parameter)
             m/\<[a-zA-Z_0-9\s\*,]+,\s*string\b/ || # <???,string     (template parameter)
             m/\(\s*const\s+string\b/ ||            # (const string   (function parameter)
             m/\(\s*string\s+const\b/ ||            # (string const   (function parameter)
             m/_INSTANTIATE\s*\(\s*string\b/ ||     #                 (instantiation macro)
             0) {
          $saw_string = 1;
          $string_is_typename = 1;
          $string_is_identifier = 0;
          #print STDERR "['string' being used as typename]\n";
        }
      }

      # sometimes "string" is used as an identifier.
      if (!$string_is_typename && !$string_is_identifier && !$saw_string && !$saw_string_h) {
       if (m/char\s*\*\s*string\b/ ||         # "char * string"
           m/char\s*const\s*\*\s*string\b/ || # "char const * string"
           m/\bstring\s*\=/ ||                # "string ="
           m/\bstring\s*\;/ ||             # "string;"
           m/\bstring\s*\./ ||             # "string."
           m/\bstring\s*,\s*this\b/   ||   # "string, this"
           m/\bstring\s*,\s*strlen\b/ ||   # "string, strlen"
           m/\bstring\s*,\s*\&/       ||   # "string, &"
           #m/\bstring\s*\&/ ||             # "string &"
           m/\<\s*string\b/ ||             # "< string"   hmm..
           m/\>\s*string\b/ ||             # "> string"
           m/\bstring\s*\</ ||             # "string <"
           m/\bstring\s*\>/ ||             # "string >"   hmm..
           m/\bstring\s*\!/ ||             # "string !"
           m/\bstring\s*\+/ ||             # "string +"
           m/\bstring\s*\-/ ||             # "string -"
           #m/\bstring\s*\\/ ||             # "string \\"
           #m/\bstring\s*\"/ ||             # "string \""
           m/\bstring\s*\[/ ||             # "string ["
           m/\bstrcpy\s*\(\s*string\b/ ||  # "strcpy\(string"
           m/\bstrcat\s*\(\s*string\b/ ||  # "strcat\(string"
           m/\bstrcmp\s*\(\s*string\b/ ||  # "strcmp\(string"
           m/\bstrlen\s*\(\s*string\b/ ||  # "strlen\(string"
           m/QV_NODE_ADD_FIELD/ ||         # libQv does not use <string>
           0) {
         $string_is_typename = 0;
         $string_is_identifier = 1;
         print STDERR "[WARNING: 'string' being used as identifier]"; # no \n, please
       }
      }
    }
  }
}

#--------------------------------------------------------------------------------

# process @lines :
sub process_lines {
  for (my $ii=0; $ii<$num_lines; ++$ii) {
    # copy line from array to $_:
    $_ = $lines[$ii];

    #---------------------------------------- Config-IUE -> vcl
    if ($vcl) {
      # IUE_compiler.h -> vcl_compiler.h
      s/\bIUE_GCC/VCL_GCC/g;
      s/\bIUE_EGCS\b/VCL_EGCS/g;
      s/\bIUE_SGI_CC/VCL_SGI_CC/g;
      s/\bIUE_SUNPRO/VCL_SUNPRO/g;
      s/\bIUE_WIN32\b/VCL_WIN32/g;
      s/\bIUE_VC50\b/VCL_VC50/g;
      s/\bIUE_VC60\b/VCL_VC60/g;

      # defines from IUE_compiler :
      s/\bIUE_COMMA\b/VCL_COMMA/g;
      s/\bIUE_FOR_SCOPE_HACK\b/VCL_FOR_SCOPE_HACK/g;
      s/\bIUE_HAS_MEMBER_TEMPLATES\b/VCL_HAS_MEMBER_TEMPLATES/g;
      s/\bIUE_NO_TEMPLATE_SYMBOLS\b/!VCL_HAS_TEMPLATE_SYMBOLS/g;
      s/\bIUE_CAN_DO_PARTIAL_SPECIALIZATION\b/VCL_CAN_DO_PARTIAL_SPECIALIZATION/g;
      s/\bIUE_HAS_DYNAMIC_CAST\b/VCL_HAS_DYNAMIC_CAST/g;
      s/\bIUE_STATIC_CONST_INIT_FLOAT\b/VCL_STATIC_CONST_INIT_FLOAT_DECL/g;
      s/\bIUE_STATIC_CONST_INIT_INT\b/VCL_STATIC_CONST_INIT_INT_DECL/g;
      s/\bIUE_IMPLEMENT_STATIC_CONSTS\b/\!VCL_CAN_STATIC_CONST_INIT_INT/g;
      s/\bdefined\(IUE_NO_STATIC_DATA_MEMBERS\)/VCL_NO_STATIC_DATA_MEMBERS/g;
      s/(\#\s*if)def\s+IUE_NO_STATIC_DATA_MEMBERS\b/$1 VCL_NO_STATIC_DATA_MEMBERS/g;
      s/(\#\s*if)ndef\s+IUE_NO_STATIC_DATA_MEMBERS\b/$1 !VCL_NO_STATIC_DATA_MEMBERS/g;
      s/\bIUE_INSTANTIATE_INLINE\b/VCL_INSTANTIATE_INLINE/g;
      s/\bIUE_INSTANTIATE_NONINLINE\s*\((.*)\)/template $1/;
      s/\bIUE_DO_NOT_INSTANTIATE\b/VCL_DO_NOT_INSTANTIATE/g;
      s/\bIUE_UNINSTANTIATE_SPECIALIZATION\b/VCL_UNINSTANTIATE_SPECIALIZATION/g;
      s/\bIUE_UNINSTANTIATE_UNSEEN_SPECIALIZATION\b/VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION/g;
      s/\bIUE_INSTANTIATE_STATIC_TEMPLATE_MEMBER\b/VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER/g;
      s/\bIUE_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER\b/VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER/g;
      s/\bIUE_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD\b/VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD/g;
      s/\bIUE_DECLARE_SPECIALIZATION\s*\(.*\)\s*;\b/VCL_DEFINE_SPECIALIZATION/g;
      s/\bIUE_DECLARE_SPECIALIZATION\b/VCL_DECLARE_SPECIALIZATION/g;
      s/\bIUE_STL_NULL_TMPL_ARGS\b/VCL_NULL_TMPL_ARGS/g;
      s/\bIUE_DEFAULT_VALUE\b/VCL_DEFAULT_VALUE/g;
      s/\bIUE_DFL_TYPE_PARAM_STLDECL\b/VCL_DFL_TYPE_PARAM_STLDECL/g;
      s/\bIUE_DFL_TMPL_PARAM_STLDECL\b/VCL_DFL_TMPL_PARAM_STLDECL/g;
      s/\bIUE_DFL_TMPL_ARG\b/VCL_DFL_TMPL_ARG/g;
      s/\bIUE_SUNPRO_ALLOCATOR_HACK\b/VCL_SUNPRO_ALLOCATOR_HACK/g;

      s/\bIUE_STRING_IS_TYPEDEF\b/VCL_STRING_IS_TYPEDEF/g;
      s/\bdefined\(IUE_USE_NATIVE_STL\)/VCL_USE_NATIVE_STL/g;
      s/(\#\s*if)def\s+IUE_USE_NATIVE_STL\b/$1 VCL_USE_NATIVE_STL/g;
      s/(\#\s*if)ndef\s+IUE_USE_NATIVE_STL\b/$1 !VCL_USE_NATIVE_STL/g;
      s/\bdefined\(IUE_USE_NATIVE_COMPLEX\)/VCL_USE_NATIVE_COMPLEX/g;
      s/(\#\s*if)def\s+IUE_USE_NATIVE_COMPLEX\b/$1 VCL_USE_NATIVE_COMPLEX/g;
      s/(\#\s*if)ndef\s+IUE_USE_NATIVE_COMPLEX\b/$1 !VCL_USE_NATIVE_COMPLEX/g;
      s/\bIUE_STL_USE_ABBREVS\b/__STL_USE_ABBREVS/g;

      # classes and functions
      #this is often a typedef member.  s/\breverse_iterator\b/vcl_reverse_iterator/g;
      s/\bistream_iterator\b/vcl_istream_iterator/g;
      s/\bostream_iterator\b/vcl_ostream_iterator/g;
      s/\bforward_iterator\b/vcl_iterator/g; # no ISO forward_iterator<>

      # stl
      s/\bvector\s*</vcl_vector</g;
      s/\blist\s*</vcl_list</g;
      s/\bstack\s*</vcl_stack</g;
      s/\bdeque\s*</vcl_deque</g;
      s/\bqueue\s*</vcl_queue</g;
      s/\bpriority_queue\s*</vcl_priority_queue</g;
      s/\bmap\s*</vcl_map</g;
      s/\bmultimap\s*</vcl_multimap</g;
      s/\bless\s*</vcl_less</g;
      s/\bpair\s*</vcl_pair</g;
      s/\bset\s*</vcl_set</g;
      #no ISO hash<>: s/\bhash\s*</vcl_hash</g;
      s/\bcomplex\s*</vcl_complex</g;
      s/\bbinary_function\b/vcl_binary_function/g;
      s/\ballocator\s*</vcl_allocator</g;

      # protect certain occurrences of 'string':
      s/\.string\b/.string_dont_filter/g;                      # .string
      s/\bstring\./string_dont_filter./g;                      # string.
      s/\"string\"/\"string_dont_filter\"/g;                   # "string"
      s/(Cool[a-zA-Z0-9_]*\s+)string\b/$1string_dont_filter/g; # Cool??? string
      # if "string" has been used as a typename, we perform the substitution.
      # otherwise, it is probably safe to substitute if <string> has not been
      # included, and "string" has not been used as an identifier - it is not
      # a big problem if parameter names are renamed, say.
      if ($string_is_typename || (($saw_string || $saw_string_h) && !$string_is_identifier)) {
        # substitute "string" -> "vcl_string"
        $saw_string = 1 if s/\bstring\b/vcl_string/g;
      }
      # unprotect:
      s/\bstring_dont_filter\b/string/g;
      # long form -> short form
      s/\bbasic_string\s*<\s*char\s*,\s*string_char_traits\s*<\s*char\s*>\s*>/vcl_string/g;

      # hex, dec, etc
      s/\<\<\s*setprecision\s*\(/\<\< vcl_setprecision\(/g;
      s/\<\<\s*setw\b/\<\< vcl_setw/g;
      s/\<\<\s*hex\s*\<\</\<\< vcl_hex \<\</g;
      s/\<\<\s*dec\s*\<\</\<\< vcl_dec \<\</g;
      s/\>\>\s*ws\b/\>\> vcl_ws/g;

      s/\bcin\b/vcl_cin/g;
      s/\bcout\b/vcl_cout/g;
      s/\bcerr\b/vcl_cerr/g;
      s/\<\<\s*endl\b/\<\< vcl_endl/g;
      s/<\<\s*flush\b/\<\< vcl_flush/g;

      $saw_vector    = 1 if m/\bvcl_vector\s*</;
      $saw_list      = 1 if m/\bvcl_list\s*</;
      $saw_deque     = 1 if m/\bvcl_deque\s*</;
      $saw_queue     = 1 if m/\bvcl_queue\s*</;
      $saw_queue     = 1 if m/\bvcl_priority_queue\s*</;
      $saw_map       = 1 if m/\bvcl_map\s*</;
      $saw_map       = 1 if m/\bvcl_multimap\s*</;
      $saw_less      = 1 if m/\bvcl_less\s*</;
      $saw_pair      = 1 if m/\bvcl_pair\s*</;
      $saw_set       = 1 if m/\bvcl_set\s*</;
      $saw_allocator = 1 if m/\bvcl_allocator\s*</;

      # Instantiation macros.
      #deprecated:
      #  s/\bINSTANTIATE_UNARY\b/VCL_SWAP_INSTANTIATE /g;
      #  HASHKEY, HASHMAP, HASHTABLE
      s/\bINSTANTIATE_COMPARISONS/\/\/INSTANTIATE_COMPARISONS/;
      s/\bINSTANTIATE_UNARY/\/\/INSTANTIATE_UNARY/;
      while (s/\bINSTANTIATE_(VECTOR|SET|SORT|MAP|MULTIMAP|LIST|PAIR|COPY|COPY_BACKWARD|CONTAINABLE|SWAP|LESS|FIND)\s*\(/VCL_$1_INSTANTIATE\(/) {
       my ($tmp) = $1;
       $tmp =~ tr/A-Z/a-z/;  # convert to lower case
       $tmp = "map" if ($tmp eq "multimap");
       $tmp = "set" if ($tmp eq "multiset");
       $tmp = "utility" if ($tmp eq "pair");
       $tmp = "algorithm" if ($tmp eq "swap" ||
                              $tmp eq "containable" ||
                              $tmp eq "find" ||
                              $tmp eq "copy" ||
                              $tmp eq "copy_backward" ||
                              $tmp eq "sort");
       $tmp = "functional" if ($tmp eq "less");
       push @txx_needed, $tmp;
      }
    }

    #---------------------------------------- COOL -> vcl
    if ($cool) {
      s/\bCoolString\b/vcl_string/g;
    }

    #---------------------------------------- Basics -> vbl
    if ($vbl) {
      #
      s/\bvbl_clamp\b/vil_clamp/g;
      s!\bBoundingBox<!vbl_bounding_box<!g;
      s!INSTANTIATE_BOUNDING_BOX!VBL_BOUNDING_BOX_INSTANTIATE!g;

      #
      s!\bSparseArray\b!vbl_sparse_array!g;
      s!\bSparseArray([123])D\b!vbl_sparse_array_$1d!g;
      s!\bINSTANTIATE_SPARSEARRAY([123])D\b!VBL_SPARSE_ARRAY_$1D_INSTANTIATE!;
      s!\bINSTANTIATE_SPARSEARRAY\b!VBL_SPARSE_ARRAY_INSTANTIATE!;

      s!\bARRAY2D_INSTANTIATE\b!VBL_ARRAY_2D_INSTANTIATE!g;

      # classes and functions
      s/\bRGB</vil_rgb</g;
      s/\bRGBcell\b/vil_rgb<byte> /g;
      s/\bRGBA/vil_rgba/g;
      s/\bRGBAcell\b/vil_rgba<byte> /g;
      s/\bvbl_rgb\b/vil_rgb/g;
      s/\bvbl_rgba\b/vil_rgba/g;
      s/\bArg</vbl_arg</g;
      s/\bArgInfoList\b/vbl_arg_info_list/g;
      s/\bArgBase::Parse/vbl_arg_parse/g;
      s/\bArgBase::DisplayUsageAndExit/vbl_arg_display_usage_and_exit/g;
      s/\bIUE_sprintf\b/vbl_sprintf/g;
      s/\bIUE_file\b/vbl_file/g;
      s/\bArray2D</vbl_array_2d</g;
      s/\bArray3D</vbl_array_3d</g;
      s/\bBaseArray3D</vbl_base_array_3d</g;
      s/\bAWK\b/vbl_awk/g;
      s/\bprintf\(([^\"])/vbl_printf\($1/g;
      s/\bCoolTimer/vbl_timer/g;
      s/\bCoolRegExp/vbl_reg_exp/g;
    }

    #---------------------------------------- Numerics -> vnl
    if ($vnl) {
      # macros
      s/IUE_USED_COMPLEX/VNL_USED_COMPLEX/g;
      s/IUE_COMPLEX_AVAILABLE/VNL_COMPLEX_AVAILABLE/g;

      # enums
      s/\bvnl_matlab_Format/vnl_matlab_print_format/g;
      s/\bvnl_matlab_fmt_/vnl_matlab_print_format_/g;

      # CoolVector constructor with 3 arguments
      s/\bCoolVector(\s*\<[^>]+\>\s*[a-zA-Z_][a-zA-Z0-9_]*\s*\()([^,]+,[^,]+,[^,;]+\)\s*\;)/vnl_vector$1 3, $2/g;

      # classes and functions
      s/\bCPolyRoots\b/vnl_cpoly_roots/g;
      s/\bIUE_c_vector\b/vnl_c_vector/g;
      s/\bIUE_vector\b/vnl_vector/g;
      s/\bCoolVector\b/vnl_vector/g;
      s/\bDouble([234])\b/vnl_double_$1/g;
      s/\bIUE_matrix\b/vnl_matrix/g;
      s/\bCoolMatrix\b/vnl_matrix/g;
      s/\bDouble([234]x[234])\b/vnl_double_$1/g;
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
      s/\bfloat_complex\b/vnl_complex<float>/g;
      s/\bdouble_complex\b/vnl_complex<double>/g;
      s/\bChiSquared::ChiSquaredCumulative\b/vnl_chi_squared_cumulative/g;
      s/\bSparseSymmetricEigensystem\b/vnl_sparse_symmetric_eigensystem/g;
      s/\bComplexEigensystem\b/vnl_complex_eigensystem/g;
      s/\bSymmetricEigensystem\b/vnl_symmetric_eigensystem<double> /g;
      s/\bEigensystem\b/vnl_real_eigensystem/g;
      s/\bGeneralizedEigensystem\b/vnl_generalized_eigensystem/g;
      s/\bRealPolynomial\b/vnl_real_polynomial/g;
      s/\bDoubleVector\b/vnl_vector<double>/g;
      s/\bDoubleMatrix\b/vnl_matrix<double>/g;
      s/\bSparseMatrix\b/vnl_sparse_matrix/g;
      s/\bDoubleMatrix\b/vnl_matrix<double>/g;
      s/\bfortran_copy\b/vnl_fortran_copy/g;
      s/\bMatOps\b/vnl_matops/g;
      s/\bvnl_matops::matlab_print\b/vnl_matlab_print/g;
      s/\bMATLABMatFileWrite\b/vnl_matlab_filewrite/g;
      s/\bFileMatrix\b/vnl_file_matrix<double>/g;
      s/\bFileVector\b/vnl_file_matrix<double>/g;
      s/\bMatrixInverse\b/vnl_matrix_inverse<double>/g;
      s/\bRPolyRoots\b/vnl_rpoly_roots/g;
      s/\bRealPolynomial\b/vnl_real_polynomial/g;
      s/\bRealNPolynomial\b/vnl_real_npolynomial/g;
      s/\bRNPolySolve\b/vnl_rnpoly_solve/g;
      s/\bCholesky\b/vnl_cholesky/g;
      s/\bQR\b/vnl_qr<double>/g;
      s/\bBaseSVD\b/vnl_svd/g;
      s/\bSVD\b/vnl_svd<double>/g;
      s/\bIUE_numeric_limits\b/vcl_limits/g;
      s/\bIUE_numeric_traits\b/vnl_numeric_traits/g;
      s/\bIUE_complex_traits\b/vnl_complex_traits/g;
      s/\bComplexVectorT\b/vnl_complex_vector_t/g;
      s/\bComplexVector\b/vnl_complex_vector/g;
      s/\bComplexMatrixT\b/vnl_complex_matrix_t/g;
      s/\bComplexMatrix\b/vnl_complex_matrix/g;
      s/\bConjugateGradient\b/vnl_conjugate_gradient/g;
      s/\bLeastSquaresFunction\b/vnl_least_squares_function/g;
      s/\bLeastSquaresCostFunction\b/vnl_least_squares_cost_function/g;
      s/\bCostFunction\b/vnl_cost_function/g;
      s/\bIUE_UnaryFunction\b/vnl_unary_function/g;
      s/\bIUE_Identity\b/vnl_identity/g;
      s/\bLevenbergMarquardt\b/vnl_levenberg_marquardt/g;
      s/\bNonLinearMinimizer\b/vnl_nonlinear_minimizer/g;
      s/\bMatrixFixedRef\b/vnl_matrix_fixed_ref/g;
      s/\bMatrixFixed\b/vnl_matrix_fixed/g;
      s/\bVectorFixed\b/vnl_vector_fixed/g;
      s/\bVectorRef\b/vnl_vector_ref/g;
      s/\bMatrixRef\b/vnl_matrix_ref/g;
      s/\bRotationMatrix\b/vnl_rotation_matrix/g;
      s/\bIdentity3x3\b/vnl_identity_3x3/g;
      s/\bCrossProductMatrix\b/vnl_cross_product_matrix/g;
      s/\bLBFGS\b/vnl_lbfgs/g;
      s/\bTranspose\b/vnl_transpose/g;
      s/\bFastOps\b/vnl_fastops/g;
      s/\bDiscreteDiff\b/vnl_discrete_diff/g;
      s/\bFFT1D\b/vnl_fft1d/g;
      s/\bFFT2D\b/vnl_fft2d/g;
      s/\bFFTxDPrimeFactors\b/vnl_fftxd_prime_factors/g;
      s/\bGaussianKernel1D\b/vnl_gaussian_kernel_1d/g;
      s/\bDouble2x2\b/vnl_double_2x2/g;
      s/\bDouble2x3\b/vnl_double_2x3/g;
      s/\bDouble3x3\b/vnl_double_3x3/g;
      s/\bDouble3x4\b/vnl_double_3x4/g;
      s/\bDouble4x4\b/vnl_double_4x4/g;
      s/\bDouble4x3\b/vnl_double_4x3/g;
      s/\bDouble4\b/vnl_double_4/g;
      s/\bDouble3\b/vnl_double_3/g;
      s/\bDouble2\b/vnl_double_2/g;
      s/\bLinearOperators3\b/vnl_linear_operators_3/g;
      s/\bIntMatrix\b/vnl_int_matrix/g;
      s/\bAffineApprox\b/vnl_affine_approx/g;
      s/\bFloat2\b/vnl_float_2/g;
      s/\bFloat3\b/vnl_float_3/g;
      s/\bScatter3x3\b/vnl_scatter_3x3<double>/g;
      s/\bOuterProduct(|3x3)\s*<\s*(float|double|int)\s*>\s*(\S+)\s*\(/vnl_matrix<$2> $2 = outer_product(/g;
      s/\bOuterProduct(|3x3)\b/outer_product/g;
      s/\bAmoeba\b/vnl_amoeba/g;
      s/\bIUE_VectorDereference\b/vnl_vector_dereference/g;
      s/\bVectorDereference\b/vnl_vector_dereference/g;
      s/\bInt2\b/vnl_int_2/g;
      s/\bInt3\b/vnl_int_3/g;
      s/\bInt4\b/vnl_int_4/g;
      s/\bScalarJoinIterator\b/vnl_scalar_join_iterator/g;
      #awf s/\bresize\b/vnl_resize/g;
      s/(\.|\->)maxVal\b/$1max_value/g;
      s!\bCoolQuaternion<!vnl_quaternion<!g;
      s!\bsquared_distance_[23]d\b!vnl_vector_ssd!g;

      # test names
      s/Numerics_Test_AssertNear/testlib_test_assert_near/g;
      s/Numerics_Test_Assert/testlib_test_assert/g;
    }

    #---------------------------------------- vgl
    if ($vgl) {
    }

    #---------------------------------------- vil
    if ($vil) {
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

    #---------------------------------------- MultiView -> mvl
    if ($mvl) {
      # symbols
      s!\bCoolArray\b!vcl_vector!g;
      s!\bCoolArrayP\b!vcl_vector!g;
      s!\bCoolList\b!vcl_list!g;
      s!\bCoolListP\b!vcl_list!g;
    }

    #---------------------------------------- osl
    if ($osl) {
      s/\bosl_EdgelChain\b/osl_edgel_chain/g;
      s/\bosl_Edge\b/osl_edge/g;
      s/\bosl_Vertex\b/osl_vertex/g;
    }

    #---------------------------------------- VGUI -> vgui
    if ($vgui) {
      s/\bVGUI/vgui/g;
      s/\bvgui_image\b/vil_image/g; # class
      s!tableaux_DLLDATA!vgui_DLLDATA!g;
      s!vgui_displaybase.C!vgui_displaybase.txx!g;

      s!obl_on_off!vbl_bool_ostream::on_off!g;
      s!obl_true_false!vbl_bool_ostream::true_false!g;
      s!obl_high_low!vbl_bool_ostream::high_low!g;
    }

    #---------------------------------------- misc
    if ($lint) {
      #print STDERR "use of 'NULL'\n" if m/\bNULL\b/;
      #print STDERR "use of 'nil'\n" if m/\bnil\b/;
      print STDERR "use of VCL_DECLARE_SPECIALIZATION\n" if m/\bVCL_DECLARE_SPECIALIZATION\b/;
    }

    # put $_ back into array:
    $lines[$ii] = $_;
  }
}

#--------------------------------------------------------------------------------


# output lines, adding extra headers just before the first header in the file.
sub output_lines {
  #print "// vxl_converted\n";
  my ($seen_include) = 0;
  for (my $ii=0; $ii<$num_lines; ++$ii) {
    if (!$seen_include && $lines[$ii] =~ m/^\s*\#\s*include/) {
      # if IUE_stlfwd.h was used, take appropriate action
      if ($saw_stlfwd) {
        print "#include <vcl_utility.h>\n" if ($saw_pair   && !$saw_utility_h);
        print "#include <vcl_vector.h>\n"  if ($saw_vector && !$saw_vector_h);
        print "#include <vcl_list.h>\n"    if ($saw_list   && !$saw_list_h);
        print "#include <vcl_deque.h>\n"   if ($saw_deque  && !$saw_deque_h);
        print "#include <vcl_queue.h>\n"   if ($saw_queue  && !$saw_queue_h);
        print "#include <vcl_map.h>\n"     if ($saw_map    && !$saw_map_h);
        print "#include <vcl_set.h>\n"     if ($saw_set    && !$saw_set_h);
      }

      # we need <memory> with allocator<> :
      print "#include <vcl_memory.h>\n"    if ($saw_allocator && !$saw_memory_h);

      # we need <functional> with less<> :
      print "#include <vcl_functional.h>\n" if ($saw_less && !$saw_functional_h);

      # we need <utility> with map<> or multimap<> :
      print "#include <vcl_utility.h>\n"   if ($saw_map && !$saw_utility_h);

      # emit any .txx files needed :
      foreach my $class (@txx_needed) {
        print "#include <vcl_$class.txx>\n";
      }

      #
      $seen_include = 1;
    }

    # output processed line and comment :
    print "$lines[$ii]$comments[$ii]\n";
  }
}

#--------------------------------------------------------------------------------

sub main {
  &parse_options();
  &read_lines();
  &process_headers();
  &scan_for_hints();
  &process_lines();
  &output_lines();
}

exit &main();
