// This is oxl/mvl/FileNameGeneratorBase.h
#ifndef FileNameGeneratorBase_h_
#define FileNameGeneratorBase_h_
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 28 Aug 98
//
// \verbatim
// Modifications
//  Dec.2001 - Ported to vxl by Peter Vanroose
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <vnl/vnl_unary_function.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class FileNameGeneratorBase
{
 public:
  typedef vnl_unary_function<int,int> Indexer;

  // Constructors/Destructors--------------------------------------------------
  FileNameGeneratorBase();
  FileNameGeneratorBase(const Indexer& indexer /* = vnl_identity<int>() */ );
  FileNameGeneratorBase(const FileNameGeneratorBase&);
  FileNameGeneratorBase& operator=(const FileNameGeneratorBase&);

  // Destructor
  virtual ~FileNameGeneratorBase() { delete indexer_; }

  // Operations----------------------------------------------------------------
  virtual std::string dirname()=0;
  virtual std::string basename()=0;
  virtual std::string image_extension()=0;

  virtual std::string frame_basename(int i1)=0;
  virtual std::string pair_basename(int i1, int i2)=0;
  virtual std::string triplet_basename(int i1, int i2, int i3)=0;

  virtual std::string basename(const char* subdir)=0;
  virtual std::string frame_basename(const char* subdir, int i1)=0;
  virtual std::string pair_basename(const char* subdir, int i1, int i2)=0;
  virtual std::string triplet_basename(const char* subdir, int i1, int i2, int i3)=0;

  virtual int get_real_index(int index) const;

  virtual FileNameGeneratorBase* copy()=0;
  virtual FileNameGeneratorBase* copy(char const* new_basename)=0;

  bool use_subdirs;

 protected:
  // Data Members--------------------------------------------------------------
  Indexer* indexer_;
};

#endif // FileNameGeneratorBase_h_
