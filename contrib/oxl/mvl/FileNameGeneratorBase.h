// This is oxl/mvl/FileNameGeneratorBase.h
#ifndef FileNameGeneratorBase_h_
#define FileNameGeneratorBase_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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

#include <vnl/vnl_unary_function.h>
#include <vcl_string.h>

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
  virtual vcl_string dirname()=0;
  virtual vcl_string basename()=0;
  virtual vcl_string image_extension()=0;

  virtual vcl_string frame_basename(int i1)=0;
  virtual vcl_string pair_basename(int i1, int i2)=0;
  virtual vcl_string triplet_basename(int i1, int i2, int i3)=0;

  virtual vcl_string basename(const char* subdir)=0;
  virtual vcl_string frame_basename(const char* subdir, int i1)=0;
  virtual vcl_string pair_basename(const char* subdir, int i1, int i2)=0;
  virtual vcl_string triplet_basename(const char* subdir, int i1, int i2, int i3)=0;

  virtual int get_real_index(int index) const;

  virtual FileNameGeneratorBase* copy()=0;
  virtual FileNameGeneratorBase* copy(char const* new_basename)=0;

  bool use_subdirs;

 protected:
  // Data Members--------------------------------------------------------------
  Indexer* indexer_;
};

#endif // FileNameGeneratorBase_h_
