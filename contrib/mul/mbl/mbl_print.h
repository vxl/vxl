#ifndef mbl_print_h_
#define mbl_print_h_
#ifndef NDEBUG // skip all this if not debugging

//:
// \file
// \brief Commands that MS debuggers can run easily on demand.
// \author Ian Scott

//=======================================================================
// inclusions
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vcl_vector.h>


//: Print the first entries of an array of doubles to vcl_cout.
// Print at most n entries
void mbl_print_vcl_vector_double(const vcl_vector<double>& v, int n)
{
  if (n>v.size()) n=v.size();
  vcl_cout << "( ";
  for (int i=0; i<n;++i) vcl_cout << v[i] << " ";
  if (v.size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of an array of doubles to vcl_cout.
// Print at most 5 entries
void mbl_print_vcl_vector_double(const vcl_vector<double>& v)
{
  int n = 5;
  if (n>v.size()) n=v.size();
  vcl_cout << "( ";
  for (int i=0; i<n ;++i) vcl_cout << v[i] << " ";
  if (v.size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of an array of unsigned to vcl_cout.
// Print at most n entries
void mbl_print_vcl_vector_double(const vcl_vector<double>* v, int n)
{
  if (n>v->size()) n=v->size();
  vcl_cout << "( ";
  for (int i=0; i<n ;++i) vcl_cout << (*v)[i] << " ";
  if (v->size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of an array of unsigned to vcl_cout.
// Print at most 5 entries
void mbl_print_vcl_vector_double(const vcl_vector<double>* v)
{
  int n = 5;
  if (n>v->size()) n=v->size();
  vcl_cout << "( ";
  for (int i=0; i<n ;++i) vcl_cout << (*v)[i] << " ";
  if (v->size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of an array of unsigned to vcl_cout.
// Print at most n entries
void mbl_print_vcl_vector_unsigned(const vcl_vector<unsigned>& v, int n)
{
  if (n>v.size()) n=v.size();
  vcl_cout << "( ";
  for (int i=0; i<n ;++i) vcl_cout << (v)[i] << " ";
  if (v.size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of an array of unsigned to vcl_cout.
// Print at most 5 entries
void mbl_print_vcl_vector_unsigned(const vcl_vector<unsigned>& v)
{
  int n = 5;
  if (n>v.size()) n=v.size();
  vcl_cout << "( ";
  for (int i=0; i<n ;++i) vcl_cout << (v)[i] << " ";
  if (v.size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of an array of unsigned to vcl_cout.
// Print at most n entries
void mbl_print_vcl_vector_unsigned(const vcl_vector<unsigned>* v, int n)
{
  if (n>v->size()) n=v->size();
  vcl_cout << "( ";
  for (int i=0; i<n ;++i) vcl_cout << (*v)[i] << " ";
  if (v->size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of an array of unsigned to vcl_cout.
// Print at most 5 entries
void mbl_print_vcl_vector_unsigned(const vcl_vector<unsigned>* v)
{
  int n = 5;
  if (n>v->size()) n=v->size();
  vcl_cout << "( ";
  for (int i=0; i<n ;++i) vcl_cout << (*v)[i] << " ";
  if (v->size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of an array of unsigned to vcl_cout
// print at most n entries
void mbl_print_vcl_vector_int(const vcl_vector<int>& v, int n)
{
  if (n>v.size()) n=v.size();
  vcl_cout << "( ";
  for (int i=0; i<n ;++i) vcl_cout << (v)[i] << " ";
  if (v.size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of an array of unsigned to vcl_cout
// print at most 5 entries
void mbl_print_vcl_vector_int(const vcl_vector<int>& v)
{
  int n = 5;
  if (n>v.size()) n=v.size();
  vcl_cout << "( ";
  for (int i=0; i<n ;++i) vcl_cout << (v)[i] << " ";
  if (v.size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of an array of unsigned to vcl_cout
// print at most n entries
void mbl_print_vcl_vector_int(const vcl_vector<int>* v, int n)
{
  if (n>v->size()) n=v->size();
  vcl_cout << "( ";
  for (int i=0; i<n ;++i) vcl_cout << (*v)[i] << " ";
  if (v->size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of an array of unsigned to vcl_cout
// print at most 5 entries
void mbl_print_vcl_vector_int(const vcl_vector<int>* v)
{
  int n = 5;
  if (n>v->size()) n=v->size();
  vcl_cout << "( ";
  for (int i=0; i<n ;++i) vcl_cout << (*v)[i] << " ";
  if (v->size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of the a vector to vcl_cout.
// Print at most n entries
void mbl_print_vnl_vecd(const vnl_vector<double>* v, int n)
{
  if (n>v->size()) n=v->size();
  vcl_cout << "( ";
  for (int i=0;i<n;++i) vcl_cout << (*v)(i) << " ";
  if (v->size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of the a vector to vcl_cout.
// Print at most 5 entries
void mbl_print_vnl_vecd(const vnl_vector<double>* v)
{
  int n = 5;
  if (n>v->size()) n=v->size();
  vcl_cout << "( ";
  for (int i=0;i<n;++i) vcl_cout << (*v)(i) << " ";
  if (v->size()>n) vcl_cout << "... ";
  vcl_cout << ")\n";
}

//: Print the first entries of the a matrix to vcl_cout.
// Print at most m x n entries
void mbl_print_vnl_matd(const vnl_matrix<double>* A, int m, int n)
{
  vcl_cout << A->rows() << " by " << A->columns() << " Matrix\n";

  if (m>A->rows()) m=A->rows();
  if (n>A->columns()) n=A->columns();

  for (int i=0;i<m;++i)
  {
    vcl_cout << "( ";

    for ( int j=0; j<n; ++j)
      vcl_cout << (*A)(i,j) << " ";
    if (A->columns()>n) vcl_cout << "... ";
    vcl_cout << ")\n";
  }
  if (A->rows()>m) vcl_cout << "( ... )\n";
}

//: Print the first entries of a matrix to vcl_cout.
// Print at most 5 x 5 entries
void mbl_print_vnl_matd(const vnl_matrix<double>* A)
{
  int m = 5; int n = 5;
  vcl_cout << A->rows() << " by " << A->columns() << " Matrix\n";

  if (m>A->rows()) m=A->rows();
  if (n>A->columns()) n=A->columns();

  for (int i=0;i<m;++i)
  {
    vcl_cout << "( ";

    for ( int j=0; j<n; ++j)
      vcl_cout << (*A)(i,j) << " ";
    if (A->columns()>n) vcl_cout << "... ";
    vcl_cout << ")\n";
  }
  if (A->rows()>m) vcl_cout << "( ... )\n";
}

#endif // NDEBUG // skip all this if not debugging
#endif // mbl_print_h_
