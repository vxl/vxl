#ifndef mbl_print_h_
#define mbl_print_h_
//:
// \file
// \brief Commands that MS debuggers can run easily on demand.
// \author Ian Scott
//
//=======================================================================

#ifndef NDEBUG // skip all this if not debugging

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>


//: Print the first entries of an array of doubles to std::cout.
// Print at most n entries
void mbl_print_vcl_vector_double(const std::vector<double>& v, int n)
{
  if (n>int(v.size())) n=v.size();
  std::cout << "( ";
  for (int i=0; i<n;++i) std::cout << v[i] << ' ';
  if (n<int(v.size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of an array of doubles to std::cout.
// Print at most 5 entries
void mbl_print_vcl_vector_double(const std::vector<double>& v)
{
  int n = 5;
  if (n>int(v.size())) n=v.size();
  std::cout << "( ";
  for (int i=0; i<n ;++i) std::cout << v[i] << ' ';
  if (n<int(v.size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of an array of unsigned to std::cout.
// Print at most n entries
void mbl_print_vcl_vector_double(const std::vector<double>* v, int n)
{
  if (n>int(v->size())) n=v->size();
  std::cout << "( ";
  for (int i=0; i<n ;++i) std::cout << (*v)[i] << ' ';
  if (n<int(v->size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of an array of unsigned to std::cout.
// Print at most 5 entries
void mbl_print_vcl_vector_double(const std::vector<double>* v)
{
  int n = 5;
  if (n>int(v->size())) n=v->size();
  std::cout << "( ";
  for (int i=0; i<n ;++i) std::cout << (*v)[i] << ' ';
  if (n<int(v->size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of an array of unsigned to std::cout.
// Print at most n entries
void mbl_print_vcl_vector_unsigned(const std::vector<unsigned>& v, int n)
{
  if (n>int(v.size())) n=v.size();
  std::cout << "( ";
  for (int i=0; i<n ;++i) std::cout << (v)[i] << ' ';
  if (n<int(v.size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of an array of unsigned to std::cout.
// Print at most 5 entries
void mbl_print_vcl_vector_unsigned(const std::vector<unsigned>& v)
{
  int n = 5;
  if (n>int(v.size())) n=v.size();
  std::cout << "( ";
  for (int i=0; i<n ;++i) std::cout << (v)[i] << ' ';
  if (n<int(v.size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of an array of unsigned to std::cout.
// Print at most n entries
void mbl_print_vcl_vector_unsigned(const std::vector<unsigned>* v, int n)
{
  if (n>int(v->size())) n=v->size();
  std::cout << "( ";
  for (int i=0; i<n ;++i) std::cout << (*v)[i] << ' ';
  if (n<int(v->size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of an array of unsigned to std::cout.
// Print at most 5 entries
void mbl_print_vcl_vector_unsigned(const std::vector<unsigned>* v)
{
  int n = 5;
  if (n>int(v->size())) n=v->size();
  std::cout << "( ";
  for (int i=0; i<n ;++i) std::cout << (*v)[i] << ' ';
  if (n<int(v->size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of an array of unsigned to std::cout.
// Print at most n entries
void mbl_print_vcl_vector_int(const std::vector<int>& v, int n)
{
  if (n>int(v.size())) n=v.size();
  std::cout << "( ";
  for (int i=0; i<n ;++i) std::cout << (v)[i] << ' ';
  if (n<int(v.size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of an array of unsigned to std::cout.
// Print at most 5 entries
void mbl_print_vcl_vector_int(const std::vector<int>& v)
{
  int n = 5;
  if (n>int(v.size())) n=v.size();
  std::cout << "( ";
  for (int i=0; i<n ;++i) std::cout << (v)[i] << ' ';
  if (n<int(v.size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of an array of unsigned to std::cout.
// Print at most n entries
void mbl_print_vcl_vector_int(const std::vector<int>* v, int n)
{
  if (n>int(v->size())) n=v->size();
  std::cout << "( ";
  for (int i=0; i<n ;++i) std::cout << (*v)[i] << ' ';
  if (n<int(v->size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of an array of unsigned to std::cout.
// Print at most 5 entries
void mbl_print_vcl_vector_int(const std::vector<int>* v)
{
  int n = 5;
  if (n>int(v->size())) n=v->size();
  std::cout << "( ";
  for (int i=0; i<n ;++i) std::cout << (*v)[i] << ' ';
  if (n<int(v->size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of the a vector to std::cout.
// Print at most n entries
void mbl_print_vnl_vecd(const vnl_vector<double>* v, int n)
{
  if (n>int(v->size())) n=v->size();
  std::cout << "( ";
  for (int i=0;i<n;++i) std::cout << (*v)(i) << ' ';
  if (n<int(v->size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of the a vector to std::cout.
// Print at most 5 entries
void mbl_print_vnl_vecd(const vnl_vector<double>* v)
{
  int n = 5;
  if (n>int(v->size())) n=v->size();
  std::cout << "( ";
  for (int i=0;i<n;++i) std::cout << (*v)(i) << ' ';
  if (n<int(v->size())) std::cout << "... ";
  std::cout << ")\n";
}

//: Print the first entries of the a matrix to std::cout.
// Print at most m x n entries
void mbl_print_vnl_matd(const vnl_matrix<double>* A, int m, int n)
{
  std::cout << A->rows() << " by " << A->columns() << " Matrix\n";

  if (m>int(A->rows())) m=A->rows();
  if (n>int(A->columns())) n=A->columns();

  for (int i=0;i<m;++i)
  {
    std::cout << "( ";

    for (int j=0; j<n; ++j)
      std::cout << (*A)(i,j) << ' ';
    if (n<int(A->columns())) std::cout << "... ";
    std::cout << ")\n";
  }
  if (m<int(A->rows())) std::cout << "( ... )\n";
}

//: Print the first entries of a matrix to std::cout.
// Print at most 5 x 5 entries
void mbl_print_vnl_matd(const vnl_matrix<double>* A)
{
  int m = 5; int n = 5;
  std::cout << A->rows() << " by " << A->columns() << " Matrix\n";

  if (m>int(A->rows())) m=A->rows();
  if (n>int(A->columns())) n=A->columns();

  for (int i=0;i<m;++i)
  {
    std::cout << "( ";

    for ( int j=0; j<n; ++j)
      std::cout << (*A)(i,j) << ' ';
    if (n<int(A->columns())) std::cout << "... ";
    std::cout << ")\n";
  }
  if (m<int(A->rows())) std::cout << "( ... )\n";
}

void mbl_print_carray_double(const double* v, int n)
{
  std::cout << "( ";
  for (int i=0; i<n ;++i) std::cout << v[i] << ' ';
  std::cout << ")\n";
}

#endif // NDEBUG // skip all this if not debugging

#endif // mbl_print_h_
