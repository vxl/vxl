// This is oxl/mvl/RawPMatrixStore.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 22 Aug 97
// Modifications:
//   970822 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "RawPMatrixStore.h"
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <mvl/PMatrix.h>

#include <mvl/FileNameGenerator.h>

// Operations----------------------------------------------------------------

bool RawPMatrixStore::Load(int image_index)
{
  if (!check_index(image_index))
    return false;

  vcl_string filename = fng_.frame_name(image_index);

  vcl_ifstream fin(filename.c_str());

  if (!fin.good())
    {
      vcl_cerr << "Read PMatrix [" << filename << "] failed\n";
      return false;
    }

  pmatrix_[image_index]= new PMatrix;
  pmatrix_[image_index]->read_ascii(fin);

  vcl_cerr << "Read PMatrix [" << filename << "]\n";

  return true;
}

// - Save not implemented
bool RawPMatrixStore::Save(int)
{
  vcl_cerr << "RawPMatrixStore::Save not implemented\n";
  return false;
}

//: Return Image for frame $i$, loading if necessary.
PMatrix_sptr RawPMatrixStore::Get(int i)
{
  if (i< 0)
    return 0;

  if (!check_index(i))
    return 0;

  if (!pmatrix_[i])
    Load(i);

  return pmatrix_[i];
}


bool RawPMatrixStore::check_index(int i)
{
  if (i < 0)
    return false;

  if ((unsigned int)i >= pmatrix_.size())
    pmatrix_.resize(i + 10);

  return true;
}
