// This is core/vul/vul_file_iterator.cxx

#include <string>
#include "vul_file_iterator.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vul/vul_file.h>
#include <vul/vul_reg_exp.h>
//:
// \file
// \verbatim
//  Modifications:
//   Ian Scott 09/06/2003 Add filename general globbing support
// \endverbatim

//: Declare pimpl, reset, and iteration routines for each OS
#if defined(_WIN32) && !defined(__CYGWIN__)

#include <io.h>

struct vul_file_iterator_data
{
  struct _finddata_t data_;
# if defined __MINGW32__
  typedef long handle_type;      // works with msvc6
# else
  typedef intptr_t handle_type;  // not found by msvc6
#endif
  handle_type handle_;

  std::string found_;
  char const* name_;
  vul_reg_exp reg_exp_;
  std::string original_dirname_;

  handle_type find_first(const char* dirname, struct _finddata_t* data)
  {
    return _findfirst(const_cast<char*>(dirname), data);
  }

  vul_file_iterator_data(char const* glob);

  void mkname() {
    // Remember full path
    found_ = original_dirname_ + "\\" + data_.name;
    name_ = found_.c_str();
    // no need to remember filename, it's in data_.name
  }


  void next() {
    assert(handle_ != 0);
    do
    {
      if (_findnext(handle_, &data_) != 0) {
        _findclose(handle_);
        handle_ = -1L;
        return;
      }
    } while ( ! reg_exp_.find(data_.name) );
    mkname();
  }


  // should be constish, and ret 0 when nuffink
  char const* value() {
    if (handle_ == -1L) return 0;
    return name_;
  }

  // Return non-dir part of fn
  char const* value_filename() {
    if (handle_ == -1L) return 0;
    return data_.name;
  }

  ~vul_file_iterator_data() {
    if (handle_ != -1L)
      _findclose(handle_);
  }
};

vul_file_iterator_data::vul_file_iterator_data(char const* glob)
{
  original_dirname_ = vul_file::dirname(glob);
  handle_ = find_first((original_dirname_ + "\\*").c_str(), &data_);

  std::string baseglob = vul_file::basename(glob);
  std::string::iterator i = baseglob.begin();
  bool prev_slash=false, in_sqr_brackets=false;
  //assemble the Regexp string
  std::string re = "^"; // match the start of the string
  while (i != baseglob.end())
  {
    if (*i=='\\' && !prev_slash)
      prev_slash = true;
    else if (prev_slash)
    {
      prev_slash = false;
      re.append(1,('\\'));
      re.append(1,*i);
    }
    else if (*i=='[' && !in_sqr_brackets)
    {
      in_sqr_brackets = true;
      re.append(1,'[');
    }
    else if (*i==']' && in_sqr_brackets)
    {
      in_sqr_brackets = false;
      re.append(1,']');
    }
    else if (*i=='?' && !in_sqr_brackets)
      re.append(1,'.');
    else if (*i=='*' && !in_sqr_brackets)
      re.append(".*");
    else
      re.append(vul_reg_exp::protect(*i));

    ++i;
  }
  // match the end of the string
  re += '$';

  reg_exp_.compile(re.c_str());


  if (handle_ != -1L)
  {
    while ( ! reg_exp_.find(data_.name) )
    {
      if (_findnext(handle_, &data_) != 0) {
        _findclose(handle_);
        handle_ = -1L;
        return;
      }
    }
    mkname();
  }
}

#else // !defined(_WIN32) || defined(__CYGWIN__)

#include <dirent.h>

struct vul_file_iterator_data
{
  std::string original_dirname_;
  DIR* dir_handle_;
  dirent* de_;
  std::string found_;
  char const* name_;
  vul_reg_exp reg_exp_;

  vul_file_iterator_data(char const* glob);

  void mkname() {
    // Remember full path
    found_ = original_dirname_ + de_->d_name;
    name_ = found_.c_str();
    // no need to remember filename, it's in data_.name
  }

  void next() {
    // if dir_handle_ is NULL, then the directory probably doesn't
    // exist.
    if(dir_handle_ == nullptr) {
      return;
    }
    do
    {
      de_ = readdir(dir_handle_);
      if (de_==nullptr) {
        closedir(dir_handle_);
        dir_handle_ = nullptr;
        return;
      }
    } while ( ! reg_exp_.find(de_->d_name) );
    mkname();
  }

  // should be constish, and ret 0 when nuffink
  char const* value() {
    if (!dir_handle_) return nullptr;
    return name_;
  }

  // Return non-dir part of fn
  char const* value_filename() {
    if (!dir_handle_) return nullptr;
    return de_->d_name;
  }

  ~vul_file_iterator_data() {
    if (dir_handle_)
      closedir(dir_handle_);
  }
};

vul_file_iterator_data::vul_file_iterator_data(char const* glob)
{
  original_dirname_ = vul_file::dirname(glob) + "/";

  std::string baseglob = vul_file::basename(glob);
  std::string::iterator i = baseglob.begin();
  bool prev_slash=false, in_sqr_brackets=false;
  //assemble the Regexp string
  std::string re = "^"; // match the start of the string
  while (i != baseglob.end())
  {
    if (*i=='\\' && !prev_slash)
      prev_slash = true;
    else if (prev_slash)
    {
      prev_slash = false;
      re += '\\';
      re += *i;
    }
    else if (*i=='[' && !in_sqr_brackets)
    {
      in_sqr_brackets = true;
      re += '[';
    }
    else if (*i==']' && in_sqr_brackets)
    {
      in_sqr_brackets = false;
      re += ']';
    }
    else if (*i=='?' && !in_sqr_brackets)
      re += '.';
    else if (*i=='*' && !in_sqr_brackets)
      re += ".*";
    else
      re += vul_reg_exp::protect(*i);

    ++i;
  }
  // match the end of the string
  re += '$';

  reg_exp_.compile(re.c_str());

  dir_handle_ = opendir(original_dirname_.c_str());

  next();
}

#endif // !defined(_WIN32) || defined(__CYGWIN__)

// -----------------------------------------------------------------------------

vul_file_iterator::vul_file_iterator(char const* glob)
{
  p = nullptr;
  reset(glob);
}

vul_file_iterator::vul_file_iterator(std::string const& glob)
{
  p = nullptr;
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

vul_file_iterator::operator bool() const
{
  return (p->value() != nullptr)? true : false;
}

bool vul_file_iterator::operator!() const
{
  return (p->value() != nullptr)? false : true;
}

vul_file_iterator& vul_file_iterator::operator++()
{
  p->next();
  return *this;
}
