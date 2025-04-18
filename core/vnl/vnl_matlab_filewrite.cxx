// This is core/vnl/vnl_matlab_filewrite.cxx
//:
// \file

#include <sstream>
#include <iostream>
#include <complex>
#include "vnl_matlab_filewrite.h"

#include "vnl/vnl_matlab_write.h"

vnl_matlab_filewrite::vnl_matlab_filewrite(const char * file_name, const char * basename)
  : basename_(basename ? basename : "targetvar")

{
  out_.open(file_name, std::ios::out | std::ios::binary);
  if (out_.bad())
    std::cerr << __FILE__ << ':' << __LINE__ << ", WARNING : output stream is bad\n";
}

std::string
vnl_matlab_filewrite::make_var_name(const char * variable_name)
{
  if (variable_name)
    return { variable_name };
  else
  {
    std::stringstream ss;
    ss << variable_int_++;
    return basename_ + ss.str();
  }
}

//--------------------------------------------------------------------------------

//: scalar
void
vnl_matlab_filewrite::write(double v, const char * variable_name)
{
  vnl_matlab_write(out_, v, make_var_name(variable_name).c_str());
}

//: vector
void
vnl_matlab_filewrite::write(const vnl_vector<double> & v, const char * variable_name)
{
  vnl_matlab_write(out_, v.data_block(), v.size(), make_var_name(variable_name).c_str());
}

void
vnl_matlab_filewrite::write(const vnl_vector<std::complex<double>> & v, const char * variable_name)
{
  vnl_matlab_write(out_, v.data_block(), v.size(), make_var_name(variable_name).c_str());
}

//: matrix
void
vnl_matlab_filewrite::write(const vnl_matrix<float> & M, const char * variable_name)
{
  vnl_matlab_write(out_, M.data_array(), M.rows(), M.cols(), make_var_name(variable_name).c_str());
}

void
vnl_matlab_filewrite::write(const vnl_matrix<double> & M, const char * variable_name)
{
  vnl_matlab_write(out_, M.data_array(), M.rows(), M.cols(), make_var_name(variable_name).c_str());
}

void
vnl_matlab_filewrite::write(const vnl_matrix<std::complex<float>> & M, const char * variable_name)
{
  vnl_matlab_write(out_, M.data_array(), M.rows(), M.cols(), make_var_name(variable_name).c_str());
}

void
vnl_matlab_filewrite::write(const vnl_matrix<std::complex<double>> & M, const char * variable_name)
{
  vnl_matlab_write(out_, M.data_array(), M.rows(), M.cols(), make_var_name(variable_name).c_str());
}

void
vnl_matlab_filewrite::write(const double * const * M, int rows, int cols, const char * variable_name)
{
  vnl_matlab_write(out_, M, rows, cols, make_var_name(variable_name).c_str());
}
