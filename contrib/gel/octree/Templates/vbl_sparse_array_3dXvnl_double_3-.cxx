// This file uses an 'X' in place of the usual '+' character in its
// filename to get around a bug in the Borland 5.5.1 comiler, which
// cannot handle having a '+' character in an object file when linking
// an executable (libraries are OK).  CMake has the potential to fix
// this problem and a bug report has been submitted.  Cmake version
// 1.8.2 does not, but later versions may fix the problem, allowing us
// to revert to using the '+' character.

#include <vnl/vnl_double_3.h>
#include <vbl/vbl_sparse_array_3d.txx>
VBL_SPARSE_ARRAY_3D_INSTANTIATE(vnl_double_3);
