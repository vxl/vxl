// This is oxl/mvl/RawPMatrixStore.cxx
//:
// \file
// \author
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 22 Aug 97
// Modifications:
//   970822 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>
#include "RawPMatrixStore.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mvl/PMatrix.h>

#include <mvl/FileNameGenerator.h>

// Operations----------------------------------------------------------------

bool RawPMatrixStore::Load(int image_index)
{
  if (!check_index(image_index))
    return false;

  std::string filename = fng_.frame_name(image_index);

  std::ifstream fin(filename.c_str());

  if (!fin.good())
    {
      std::cerr << "Read PMatrix [" << filename << "] failed\n";
      return false;
    }

  pmatrix_[image_index]= new PMatrix;
  pmatrix_[image_index]->read_ascii(fin);

  std::cerr << "Read PMatrix [" << filename << "]\n";

  return true;
}

// - Save not implemented
bool RawPMatrixStore::Save(int)
{
  std::cerr << "RawPMatrixStore::Save not implemented\n";
  return false;
}

//: Return Image for frame $i$, loading if necessary.
PMatrix_sptr RawPMatrixStore::Get(int i)
{
  if (i< 0)
    return nullptr;

  if (!check_index(i))
    return nullptr;

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
