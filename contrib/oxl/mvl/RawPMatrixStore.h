// This is oxl/mvl/RawPMatrixStore.h
#ifndef RawPMatrixStore_h_
#define RawPMatrixStore_h_
//:
// \file
// \brief Demand-load image sequence from disk
//
//  The RawPMatrixStore provides demand-based access to an image sequence.
//  Individual images are accessed via integer indices, and the class
//  contains a printf-style format specifier to convert indices to
//  filenames.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 22 Aug 97
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <utility>
#include <vector>
#include <mvl/FileNameGenerator.h>
#include <mvl/PMatrix_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class RawPMatrixStore
{
 public:
  // Constructors/Destructors--------------------------------------------------

  //: Construct a PMatrix store given a FileNameGenerator
  RawPMatrixStore(FileNameGenerator  fng) : fng_(std::move(fng)) {}

  // Operations----------------------------------------------------------------

  bool Load(int image_index);
  bool Save(int i);
  PMatrix_sptr Get(int i);

  void AddExtension(char const*);

 protected:

  // Data Members--------------------------------------------------------------
  std::vector<PMatrix_sptr> pmatrix_;
  bool check_index(int i);

  FileNameGenerator fng_;
};

#endif // RawPMatrixStore_h_
