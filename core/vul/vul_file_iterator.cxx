// This is vxl/vul/vul_file_iterator.cxx
#ifdef __GNUC__
#pragma implementation
#endif

#include "vul_file_iterator.h"
#include <vcl_string.h>
#include <vcl_cassert.h>

#include <vul/vul_file.h>

// Declare pimpl, reset, and iteration routines
// for each OS
#ifdef VCL_WIN32
#include <io.h>
struct vul_file_iterator_data {
  vcl_string original;
  vcl_string original_dirname;
  struct _finddata_t data;
  long handle;
  vcl_string found;
  char const* name;

  vul_file_iterator_data(char const* glob) {
    original = glob;
    handle = _findfirst(glob, &data);
    original_dirname = vul_file::dirname(glob);

    if (handle != -1)
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
      handle = -1;
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
// Er, I can't do this one.  I think it would mean hauling
// in libglob.  Can that do "*/*"?
// Done just enough that "dir/*" will work....
#include <dirent.h>
struct vul_file_iterator_data {
  vcl_string original_dirname;
  vcl_string baseglob;
  DIR* dir_handle;
  dirent* de;
  vcl_string found;
  char const* name;

  vul_file_iterator_data(char const* glob) {
    original_dirname = vul_file::dirname(glob);
    baseglob = vul_file::basename(glob);
    dir_handle = opendir(original_dirname.c_str());
    next();
  }

  void mkname() {
    // Remember full path
    found = original_dirname + "/" + de->d_name;
    name = found.c_str();
    // no need to remember filename, it's in data.name
  }

  void next() {
    assert(dir_handle != 0);
    de = readdir(dir_handle);
    if (de)
      mkname();
    else {
      closedir(dir_handle);
      dir_handle = 0;
    }
  }

  // should be constish, and ret 0 when nuffink
  char const* value() {
    if (!dir_handle) return 0;
    return name;
  }

  // Return non-dir part of fn
  char const* value_filename() {
    if (!dir_handle) return 0;
    return de->d_name;
  }

  ~vul_file_iterator_data() {
    if (dir_handle)
      closedir(dir_handle);
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
