// This is core/vul/vul_file_iterator.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vul_file_iterator.h"
#include <vcl_string.h>
#include <vcl_cassert.h>

#include <vul/vul_file.h>



// Modifications:
// Ian Scott 09/06/2003 Add filename general globbing support


// Declare pimpl, reset, and iteration routines
// for each OS
#if defined(VCL_WIN32) && !defined(__CYGWIN__)
#include <io.h>
struct vul_file_iterator_data
{
  vcl_string original;
  vcl_string original_dirname;
  struct _finddata_t data;
  // intptr_t handle;  // not found by msvc6
  __int64 handle;      // works with msvc6
  vcl_string found;
  char const* name;

  vul_file_iterator_data(char const* glob) {
    original = glob;
    handle = _findfirst(glob, &data);
    original_dirname = vul_file::dirname(glob);

    if (handle != -1L)
      mkname();
  }

  void mkname() {
    // Remember full path
    found = original_dirname + "/" + data.name;
    name = found.c_str();
    // no need to remember filename, it's in data.name
  }

  void next() {
    assert(handle != -1L);
    if (_findnext(handle, &data) == 0) {
      // found one
      mkname();
    } else {
      // failed to find one, done
      _findclose(handle);
      handle = -1L;
    }
  }

  // should be constish, and ret 0 when nuffink
  char const* value() {
    if (handle == -1L) return 0;
    return name;
  }

  // Return non-dir part of fn
  char const* value_filename() {
    if (handle == -1L) return 0;
    return data.name;
  }

  ~vul_file_iterator_data() {
    if (handle != -1L)
      _findclose(handle);
  }
};
#else
#include <dirent.h>
#include <vul/vul_reg_exp.h>
struct vul_file_iterator_data
{
  vcl_string original_dirname_;
  vcl_string baseglob_;
  DIR* dir_handle_;
  dirent* de_;
  vcl_string found_;
  char const* name_;
  vul_reg_exp reg_exp_;
  
  vul_file_iterator_data(char const* glob) {
    original_dirname_ = vul_file::dirname(glob) + "/";

    baseglob_ = vul_file::basename(glob);
    vcl_string::iterator i = baseglob_.begin();
    bool prev_slash=false, in_sqr_brackets=false;
    vcl_string re; //assemble the Regexp string
    while (i != baseglob_.end())
    {
      if (*i=='\\' && !prev_slash)
        prev_slash = true;
      else if (prev_slash)
      {
        prev_slash == false;
        re.push_back('\\');
        re.push_back(*i);
      }
      else if (*i=='[' && !in_sqr_brackets)
      {
        in_sqr_brackets = true;
        re.push_back('[');
      }
      else if (*i==']' && in_sqr_brackets)
      {
        in_sqr_brackets = false;
        re.push_back(']');
      }
      else if (*i=='?' && !in_sqr_brackets)
        re.push_back('.');
      else if (*i=='*' && !in_sqr_brackets)
        re.append(".*");
      else
        re.append(vul_reg_exp::protect(*i));

      ++i;
    }

    reg_exp_.compile(re.c_str());

    dir_handle_ = opendir(original_dirname_.c_str());
    
    // Strip unnecessary ./
    if (original_dirname_ == "./") original_dirname_.clear();


    next();
  }

  void mkname() {
    // Remember full path
    found_ = original_dirname_ + de_->d_name;
    name_ = found_.c_str();
    // no need to remember filename, it's in data.name
  }


  void next() {
    assert(dir_handle_ != 0);
    do
    {
      de_ = readdir(dir_handle_);
      if (de_==0) {
        closedir(dir_handle_);
        dir_handle_ = 0;
        return;
      }
    } while ( ! reg_exp_.find(de_->d_name) );
    mkname();

  }

  // should be constish, and ret 0 when nuffink
  char const* value() {
    if (!dir_handle_) return 0;
    return name_;
  }

  // Return non-dir part of fn
  char const* value_filename() {
    if (!dir_handle_) return 0;
    return de_->d_name;
  }

  ~vul_file_iterator_data() {
    if (dir_handle_)
      closedir(dir_handle_);
  }
};

#endif

// -----------------------------------------------------------------------------
vul_file_iterator::vul_file_iterator()
{
  p = 0;
}

vul_file_iterator::vul_file_iterator(char const* glob)
{
  p = 0;
  reset(glob);
}

vul_file_iterator::vul_file_iterator(vcl_string const& glob)
{
  p = 0;
  reset(glob.c_str());
}

vul_file_iterator::~vul_file_iterator()
{
  delete p;
}

void vul_file_iterator::reset(char const* glob)
{
  delete p;
  p = new vul_file_iterator_data(glob);
}

char const* vul_file_iterator::operator()()
{
  return p->value();
}

char const* vul_file_iterator::filename()
{
  return p->value_filename();
}

vul_file_iterator::operator bool()
{
  return p->value() != 0;
}

vul_file_iterator& vul_file_iterator::operator++()
{
  p->next();
  return *this;
}
