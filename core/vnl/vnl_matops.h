#ifndef vnl_matops_h_
#define vnl_matops_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME        vnl_matops - A collection of Matrix operations.
// .FILE        vnl/vnl_matops.cxx
//
// .SECTION Description
//    matops is a collection of Matrix operations, mostly declared
//    as static methods.  Highlights include the in-place transpose,
//    type conversions, and MATLABPRINT.
//
// .SECTION Author
//    Andrew W. Fitzgibbon, Oxford RRG, 05 Aug 96
//
// .SECTION Modifications:
//    23 may 97, Peter Vanroose - "NO_COMPLEX" option added
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

class vnl_matops {
public:
  static double homg_diff(const vnl_matrix<double>& A, const vnl_matrix<double>& B);

  // Laminating
  static vnl_matrix<double> cat(const vnl_matrix<double>& A, const vnl_matrix<double>& B);
  static vnl_matrix<double> cat(const vnl_matrix<double>& A, const vnl_vector<double>& B);
  static vnl_matrix<double> cat(const vnl_vector<double>& A, const vnl_matrix<double>& B);

  static vnl_matrix<double> vcat(const vnl_matrix<double>& A, const vnl_matrix<double>& B);

  // Conversions
  static vnl_matrix<double> f2d(const vnl_matrix<float>&);
  static vnl_matrix<float>  d2f(const vnl_matrix<double>&);
  static vnl_vector<double> f2d(const vnl_vector<float>&);
  static vnl_vector<float>  d2f(const vnl_vector<double>&);

  // Pretty-printing
  static ostream& matlab_print(ostream&, const vnl_matrix<double>& M, const char* variable_name = 0);
  static ostream& matlab_print(ostream&, const vnl_matrix<float>& M, const char* variable_name = 0);
  static ostream& matlab_print(ostream&, const vnl_vector<double>& v, const char* variable_name = 0);
  enum Format {
    fmt_short,
    fmt_long,
    fmt_short_e,
    fmt_long_e
  };
  static Format format(Format);
  static void print_double(double v, char *buf);
  static void print_float(float v, char *buf);

#ifdef VNL_COMPLEX_AVAILABLE
  static ostream& matlab_print(ostream&, const vnl_matrix<vnl_double_complex>& v, const char* variable_name = 0);
  static void print_double_complex(vnl_double_complex v, char *buf);

  static ostream& matlab_print(ostream&, const vnl_matrix<vnl_float_complex>& v, const char* variable_name = 0);
  static void print_float_complex(vnl_float_complex v, char *buf);
#endif

  /// Deprecated interfaces
#ifndef VCL_GCC_295
  static double inf_norm(const vnl_matrix<double>& M) { return M.inf_norm(); }  
#endif

private:
  static Format matlab_format;
};


#define MATLABPRINT(X) (vnl_matops::matlab_print(cerr, X, #X))

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_matops.
