//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
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
#include <cool/String.h>
#include <cool/List.h>
#include <vcl/vcl_fstream.h>
#include <vcl/vcl_compiler.h>
#include <MViewBasics/PMatrix.h>

#include <MViewDatabase/FileNameGenerator.h>


static bool debug = false;

#if 0
RawPMatrixStore::RawPMatrixStore( const char *format, int startframe, int):
  _fmt(format),
  _startframe(startframe)
{
}
#endif

//: Construct an PMatrix Store given a FileNameGenerator
RawPMatrixStore::RawPMatrixStore(const FileNameGenerator& fng) :
  fng_(fng)
{
}


RawPMatrixStore::~RawPMatrixStore()
{
  for(int i = 0; i < _pmatrix.length(); ++i)
    if (_pmatrix[i]) delete _pmatrix[i];
}

// Operations----------------------------------------------------------------

bool RawPMatrixStore::Load(int image_index)
{
  if( !check_index( image_index))
    return false;

  CoolString filename = fng_.frame_name( image_index);

  vcl_ifstream fin( filename);

  if( !fin.good())
    {
      vcl_cerr << "Read PMatrix [" << filename << "] failed" << vcl_endl;
      return false;
    }

  PMatrix P;
  P.read_ascii( fin);

  vcl_cout << "Read PMatrix [" << filename << "]" << vcl_endl;

  _pmatrix[image_index]= new PMatrix(P);

  return true;
}

// - Save not implemented
bool RawPMatrixStore::Save(int)
{
  vcl_cerr << "RawPMatrixStore::Save not implemented\n";
  return false;
}

//: Return Image for frame $i$, loading if necessary.
PMatrix* RawPMatrixStore::Get(int i)
{
  if( i< 0)
    return 0;

  if( !check_index(i))
    return 0;

  if (!_pmatrix[i])
    Load(i);

  return _pmatrix[i];
}


bool RawPMatrixStore::check_index(int i)
{
  if (i < 0) {
    return false;
  }

  int l = _pmatrix.length();

  if (i >= l)
    _pmatrix.push((PMatrix*)0, i - l + 10);

  return true;
}
