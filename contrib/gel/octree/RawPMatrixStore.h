//-*- c++ -*-------------------------------------------------------------------
#ifndef RawPMatrixStore_h_
#define RawPMatrixStore_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : RawPMatrixStore
//
// .SECTION Description
//    The RawPMatrixStore provides demand-based access to an image sequence.
//    Individual images are accessed via integer indices, and the class
//    contains a printf-style format specifier to convert indices to
//    filenames.
//
// .NAME        RawPMatrixStore - Demand-load image sequence from disk.
// .LIBRARY     MViewDatabase
// .HEADER	MultiViewOX Package
// .INCLUDE     MViewDatabase/RawPMatrixStore.h
// .FILE        RawPMatrixStore.h
// .FILE        RawPMatrixStore.C
// .SECTION Author
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
  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS RawPMatrixStore.

