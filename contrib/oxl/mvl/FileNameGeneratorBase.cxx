// This is oxl/mvl/FileNameGeneratorBase.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "FileNameGeneratorBase.h"

#include <vnl/vnl_identity.h>

//: Construct by copying the supplied indexer 
FileNameGeneratorBase::FileNameGeneratorBase()
{
  indexer_ = vnl_identity<int>().Copy();
}

//: Construct by copying the supplied indexer 
FileNameGeneratorBase::FileNameGeneratorBase(const FileNameGeneratorBase::Indexer& indexer)
{
  indexer_ = indexer.Copy();
}

FileNameGeneratorBase::FileNameGeneratorBase(const FileNameGeneratorBase& that)
{
  indexer_ = that.indexer_->Copy();
}

FileNameGeneratorBase& FileNameGeneratorBase::operator=(const FileNameGeneratorBase& that)
{
  delete indexer_;
  indexer_ = that.indexer_->Copy();
  return *this;
}

int FileNameGeneratorBase::get_real_index(int index) const
{
  return indexer_->f(index);
}
