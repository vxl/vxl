// This is oxl/mvl/FileNameGenerator.h
#ifndef FileNameGenerator_h_
#define FileNameGenerator_h_
//:
// \file
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 25 Feb 98
//
// \verbatim
//  Modifications
//   Dec.2001 - Ported to vxl by Peter Vanroose
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <mvl/FileNameGeneratorBase.h>

class FileNameGenerator
{
  FileNameGeneratorBase* fng_;
  vcl_string subdir_;
  vcl_string suffix_;

  // default copy ctor OK
 public:
  FileNameGenerator(FileNameGeneratorBase* fng,
                    vcl_string const& suffix)
  : fng_(fng), subdir_(), suffix_(suffix)
  {
  }

  FileNameGenerator(FileNameGeneratorBase* fng,
                    const char* suffix)
  : fng_(fng), subdir_(), suffix_(suffix)
  {
  }

  FileNameGenerator(FileNameGeneratorBase* fng,
                    vcl_string const& subdir,
                    vcl_string const& suffix)
  : fng_(fng), subdir_(), suffix_(suffix)
  {
    if (fng->use_subdirs) subdir_ = subdir;
  }

  FileNameGenerator(FileNameGeneratorBase* fng,
                    const char* subdir,
                    const char* suffix)
  : fng_(fng), subdir_(), suffix_(suffix)
  {
    if (fng->use_subdirs) subdir_ = subdir;
  }

  FileNameGeneratorBase* get_base() const { return fng_; }
  vcl_string get_subdir() const { return subdir_; }
  vcl_string get_suffix() const { return suffix_; }
  bool uses_subdir() const { return subdir_.c_str()[0]!='\0'; }

  vcl_string subdir_name() const { return fng_->dirname() + "/" + subdir_; }

  vcl_string name() {
    return (subdir_.c_str()[0]=='\0') ?
      fng_->basename() + suffix_    :
      fng_->basename(subdir_.c_str()) + suffix_;
  }

  vcl_string frame_name(int i1) {
    return (subdir_.c_str()[0]=='\0') ?
      fng_->frame_basename(i1) + suffix_ :
      fng_->frame_basename(subdir_.c_str(), i1) + suffix_;
  }

  vcl_string pair_name(int i1, int i2) {
    return (subdir_.c_str()[0]=='\0') ?
      fng_->pair_basename(i1, i2) + suffix_ :
      fng_->pair_basename(subdir_.c_str(), i1, i2) + suffix_;
  }

  vcl_string triplet_name(int i1, int i2, int i3) {
    return (subdir_.c_str()[0]=='\0') ?
      fng_->triplet_basename(i1, i2, i3) + suffix_ :
      fng_->triplet_basename(subdir_.c_str(), i1, i2, i3) + suffix_;
  }
};

#endif // FileNameGenerator_h_
