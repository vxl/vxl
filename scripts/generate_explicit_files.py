#!/usr/bin/env python
# \author Hans J. Johnson
# A simple utility script for reducing the
# tedium of generating explicit instantiation
# files for the fixed types.

def write_file_from_template1D(prefix, datatype, dim1, template):
    filename = "{prefix}+{datatype}.{dim1}-.cxx".format(
               prefix=prefix, datatype=datatype, dim1=dim1,
               template=template)
    with open(filename, 'w') as fid:
        fid.write( template.format(
          datatype=datatype,dim1=dim1) )

def write_file_from_template2D(prefix, datatype, dim1, dim2, template):
    filename = "{prefix}+{datatype}.{dim1}.{dim2}-.cxx".format(
               prefix=prefix, datatype=datatype, dim1=dim1, dim2=dim2,
               template=template)
    with open(filename, 'w') as fid:
        fid.write( template.format(
          datatype=datatype,dim1=dim1, dim2=dim2) )


# -- vnl/Template
prefix = '../core/vnl/Templates/vnl_matrix_fixed'
for datatype in ['float', 'double']:
    for matchdims in [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]:
      write_file_from_template2D(prefix, datatype, matchdims, matchdims,
"""#include <vnl/vnl_matrix_fixed.hxx>
VNL_MATRIX_FIXED_INSTANTIATE({datatype},{dim1},{dim2});
"""
)

prefix = '../core/vnl/Templates/vnl_matrix_fixed_ref'
for datatype in ['float', 'double']:
    for matchdims in [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]:
      write_file_from_template2D(prefix, datatype, matchdims, matchdims,
"""#include <vnl/vnl_matrix_fixed_ref.hxx>
VNL_MATRIX_FIXED_REF_INSTANTIATE({datatype},{dim1},{dim2});
"""
)

prefix = '../core/vnl/Templates/vnl_fortran_copy_fixed'
for datatype in ['float', 'double']:
    for matchdims in [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]:
      write_file_from_template2D(prefix, datatype, matchdims, matchdims,
"""#include <vnl/vnl_fortran_copy_fixed.hxx>
VNL_FORTRAN_COPY_FIXED_INSTANTIATE({datatype},{dim1},{dim2});
"""
)

prefix = '../core/vnl/Templates/vnl_diag_matrix_fixed'
for datatype in ['float', 'double']:
    for matchdims in [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]:
      write_file_from_template1D(prefix, datatype, matchdims,
"""#include <vnl/vnl_diag_matrix_fixed.hxx>
VNL_DIAG_MATRIX_FIXED_INSTANTIATE({datatype}, {dim1});
"""
)

# -- vnl/algo/Template
prefix = '../core/vnl/algo/Templates/vnl_svd_fixed'
for datatype in ['float', 'double']:
    for matchdims in [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]:
      write_file_from_template2D(prefix, datatype, matchdims, matchdims,
"""#include <vnl/algo/vnl_svd_fixed.hxx>
VNL_SVD_FIXED_INSTANTIATE({datatype}, {dim1}, {dim2});
"""
)

# Note only generating to 4 dims for these large object fft base
# class.  This is to support a reference fft implementations
# of small test data.
prefix = '../core/vnl/algo/Templates/vnl_fft_base'
for datatype in ['float', 'double']:
    for matchdims in [ 1, 2, 3, 4 ]:
      write_file_from_template1D(prefix, datatype, matchdims,
"""#include <vnl/algo/vnl_fft_base.hxx>
VNL_FFT_BASE_INSTANTIATE({dim1}, {datatype});
"""
)
