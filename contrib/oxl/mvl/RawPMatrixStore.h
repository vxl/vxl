// This is oxl/mvl/RawPMatrixStore.h
#ifndef RawPMatrixStore_h_
#define RawPMatrixStore_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Demand-load image sequence from disk
//    The RawPMatrixStore provides demand-based access to an image sequence.
//    Individual images are accessed via integer indices, and the class
//    contains a printf-style format specifier to convert indices to
//    filenames.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 22 Aug 97
//
//-----------------------------------------------------------------------------

#include <vcl_vector.h>

#include <mvl/FileNameGenerator.h>

#include <mvl/PMatrix_sptr.h>

class RawPMatrixStore
{
 public:
  // Constructors/Destructors--------------------------------------------------

  //: Construct a PMatrix store given a FileNameGenerator
  RawPMatrixStore(FileNameGenerator const& fng) : fng_(fng) {}

  // Operations----------------------------------------------------------------

  bool Load(int image_index);
  bool Save(int i);
  PMatrix_sptr Get(int i);

  void AddExtension(char const*);

 protected:

  // Data Members--------------------------------------------------------------
  vcl_vector<PMatrix_sptr> pmatrix_;
  bool check_index(int i);

  FileNameGenerator fng_;
};

#endif // RawPMatrixStore_h_
