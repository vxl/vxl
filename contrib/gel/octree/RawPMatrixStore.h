//-*- c++ -*-------------------------------------------------------------------
#ifndef RawPMatrixStore_h_
#define RawPMatrixStore_h_
#ifdef __GNUC__
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

#include <cool/ArrayP.h>
#include <cool/String.h>

#include <MViewDatabase/FileNameGenerator.h>

class PMatrix;

class RawPMatrixStore {
public:
  // Constructors/Destructors--------------------------------------------------
  RawPMatrixStore(const FileNameGenerator& fng);

  // RawPMatrixStore(const RawPMatrixStore& that); - use default
  ~RawPMatrixStore();
  // RawPMatrixStore& operator=(const RawPMatrixStore& that); - use default

  // Operations----------------------------------------------------------------

  bool Load(int image_index);
  bool Save(int i);
  PMatrix *Get(int i);

  void AddExtension(char const*);

protected:

  // Data Members--------------------------------------------------------------
  CoolArrayP<PMatrix *> _pmatrix;
  bool check_index(int i);

  FileNameGenerator fng_;
};

#endif // RawPMatrixStore_h_
