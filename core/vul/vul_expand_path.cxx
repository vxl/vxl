// This is core/vul/vul_expand_path.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vul_expand_path.h"
#include <vcl_vector.h>

#if defined(VCL_WIN32)

//:
// \note This Windows version only performs some of the operations done by the Unix version.
vcl_string vul_expand_path_internal(vcl_string path)
{
  if (path == "/")
    return path; // FIXME: without this something breaks; not sure why.

  { // main processing and reduction goes here.
    vcl_vector<vcl_string> bits;

    // split the path into bits. a "bit" is either a single slash or a
    // sequence of non-slash characters.
    for (unsigned int i=0; i<path.size(); ) {
      if (path[i] == '/') {
        bits.push_back("/");
        ++i;
      }
      else {
        unsigned int j=i;
        while (j<path.size() && path[j]!='/')
          ++j;
        bits.push_back(vcl_string(path.c_str()+i, path.c_str()+j));
        i = j;
      }
    }

    // process the bits
    while (true)
    {
      bool again = false;
      for (unsigned int i=0; i<bits.size(); ++i)
      {
        // remove repeated / unless it is initial '//' as used in windows UNC names
        if (i>0 && i+1<bits.size() && bits[i] == "/" && bits[i+1] == "/") {
          bits.erase(bits.begin() + i);
          again = true;
        }

        // remove trailing /
        if (i+1 == bits.size() && bits[i] == "/") {
          bits.pop_back();
          again = true;
        }

        // collapse foo/.. into /
        if (i+2<bits.size() && !(bits[i]=="/") && bits[i+1]=="/" && bits[i+2]=="..") {
          bits.erase(bits.begin() + i+2); // ..
          bits.erase(bits.begin() + i);   // foo
          again = true;
        }

        // remove /. altogether
        if (i+1<bits.size() && bits[i]=="/" && bits[i+1]==".") {
          bits.erase(bits.begin() + i+1); // /
          bits.erase(bits.begin() + i);   // .
          again = true;
        }
      }
      if (!again)
        break;
    }

    // recompose the path from its bits
    path = "";
    for (unsigned int i=0; i<bits.size(); ++i)
      path += bits[i];
#ifdef DEBUG
    vcl_cerr << "recomposed : " << path << '\n';
#endif
  }

  // no more ideas
  return path;
}

//:
// Note: this Windows version in similar to the uncached Unix version
vcl_string vul_expand_path(vcl_string path)
{
  return vul_expand_path_internal(path);
}


#if VXL_USE_WIN_WCHAR_T
//:
// \note This Windows version only performs some of the operations done by the Unix version.
std::wstring vul_expand_path_internal(std::wstring path)
{
  if (path == L"/")
    return path; // FIXME: without this something breaks; not sure why.

  { // main processing and reduction goes here.
    vcl_vector<std::wstring> bits;

    // split the path into bits. a "bit" is either a single slash or a
    // sequence of non-slash characters.
    for (unsigned int i=0; i<path.size(); ) {
      if (path[i] == L'/') {
        bits.push_back(L"/");
        ++i;
      }
      else {
        unsigned int j=i;
        while (j<path.size() && path[j]!=L'/')
          ++j;
        bits.push_back(std::wstring(path.c_str()+i, path.c_str()+j));
        i = j;
      }
    }

    // process the bits
    while (true)
    {
      bool again = false;
      for (unsigned int i=0; i<bits.size(); ++i)
      {
        // remove repeated / unless it is initial '//' as used in windows UNC names
        if (i>0 && i+1<bits.size() && bits[i] == L"/" && bits[i+1] == L"/") {
          bits.erase(bits.begin() + i);
          again = true;
        }

        // remove trailing /
        if (i+1 == bits.size() && bits[i] == L"/") {
          bits.pop_back();
          again = true;
        }

        // collapse foo/.. into /
        if (i+2<bits.size() && !(bits[i]==L"/") && bits[i+1]==L"/" && bits[i+2]==L"..") {
          bits.erase(bits.begin() + i+2); // ..
          bits.erase(bits.begin() + i);   // foo
          again = true;
        }

        // remove /. altogether
        if (i+1<bits.size() && bits[i]==L"/" && bits[i+1]==L".") {
          bits.erase(bits.begin() + i+1); // /
          bits.erase(bits.begin() + i);   // .
          again = true;
        }
      }
      if (!again)
        break;
    }

    // recompose the path from its bits
    path = L"";
    for (unsigned int i=0; i<bits.size(); ++i)
      path += bits[i];
#ifdef DEBUG
    vcl_cerr << "recomposed : " << path << '\n';
#endif
  }

  // no more ideas
  return path;
}

//:
// Note: this Windows version in similar to the uncached Unix version
std::wstring vul_expand_path(std::wstring path)
{
  return vul_expand_path_internal(path);
}

#endif  //VXL_USE_WIN_WCHAR_T

#else // #if defined(VCL_WIN32)

#include <vcl_functional.h>
#include <vcl_map.h>
#include <vcl_cstdlib.h> // for getenv()
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

static
vcl_string vul_expand_path_internal(vcl_string path)
{
  if (path == "/")
    return path; // FIXME: without this something breaks; not sure why.

  // expand ~/ or just ~
  if ((path.size()>=2 && path[0] == '~' && path[1] == '/') || path == "~") {
    char const *HOME = vcl_getenv("HOME");
    if (! HOME) {
      // urgh!
      HOME = "/HOME";
    }
    path = vcl_string(HOME) + vcl_string(path.c_str() + 1);
  }

  // if the path doesn't begin with a / then it must be relative to the
  // current directory.
  if (path.size()>=1 && path[0] != '/')
    path = vcl_string("./") + path;

  // expand ./ or just .
  if ((path.size()>=2 && path[0] == '.' && path[1] == '/') || path == ".") {
    char cwd[4096];
    if( getcwd(cwd, sizeof cwd) == NULL ) {
      path = "<error: current working directory path > 4096 characters>";
    } else {
      path = vcl_string(cwd) + path.substr(1);
    }
  }

  { // main processing and reduction goes here.
    vcl_vector<vcl_string> bits;

    // split the path into bits. a "bit" is either a single slash or a
    // sequence of non-slash characters.
    for (unsigned int i=0; i<path.size(); ) {
      if (path[i] == '/') {
        bits.push_back("/");
        ++i;
      }
      else {
        unsigned int j=i;
        while (j<path.size() && path[j]!='/')
          ++j;
        bits.push_back(vcl_string(path.c_str()+i, path.c_str()+j));
        i = j;
      }
    }

    // process the bits
    while (true)
    {
      bool again = false;
      for (unsigned int i=0; i<bits.size(); ++i)
      {
        // remove repeated /
        if (i+1<bits.size() && bits[i] == "/" && bits[i+1] == "/") {
          bits.erase(bits.begin() + i);
          again = true;
        }

        // remove trailing /
        if (i+1 == bits.size() && bits[i] == "/") {
          bits.pop_back();
          again = true;
        }

        // collapse foo/.. into /
        if (i+2<bits.size() && !(bits[i]=="/") && bits[i+1]=="/" && bits[i+2]=="..") {
          bits.erase(bits.begin() + i+2); // ..
          bits.erase(bits.begin() + i);   // foo
          again = true;
        }

        // remove /. altogether
        if (i+1<bits.size() && bits[i]=="/" && bits[i+1]==".") {
          bits.erase(bits.begin() + i+1); // /
          bits.erase(bits.begin() + i);   // .
          again = true;
        }
      }
      if (!again)
        break;
    }

    // recompose the path from its bits
    path = "";
    for (unsigned int i=0; i<bits.size(); ++i)
      path += bits[i];
#ifdef DEBUG
    vcl_cerr << "recomposed : " << path << '\n';
#endif
  }

  // look for symbolic links to expand
  for (unsigned int i=1; i<=path.size(); ++i)
  {
    if (i==path.size() || path[i] == '/')
    {
      vcl_string sub(path.c_str(), path.c_str() + i);
      char buf[4096];
      int len = readlink(sub.c_str(), buf, sizeof buf);
      if (len != -1)
      {
        // it's a symlink. we should expand it and recurse.
#ifdef DEBUG
        vcl_cerr << "before expansion : " << path << '\n';
#endif
        if (buf[0] == '/') {
          // the target of the link starts with '/' so must be an
          // absolute path : ...foo/bar/etc... => buf/etc...
          path = vcl_string(buf, buf+len) + vcl_string(path.c_str() + i);
        }
        else
        {
          // the target is relative to the symlink's directory.
          int j=i-1;
          while (j>=0 && path[j] != '/')
            --j;
          if (j>=0) {
            // found another slash :   ...foo/bar/etc... where bar is the symlink.
            vcl_string a = vcl_string(path.c_str(), path.c_str()+j+1);
            vcl_string b = vcl_string(buf, buf+len);
            vcl_string c = vcl_string(path.c_str() + i, path.c_str() + path.size());
#ifdef DEBUG
            vcl_cerr << "a = " << a << "\nb = " << b << "\nc = " << c << '\n';
#endif
            path = a + b + c;
          }
          else {
            // gurgle. only one slash. must be : /bar/etc where bar is the symlink.
            path = vcl_string(buf, buf+len) + vcl_string(path.c_str() + i);
          }
        }

#ifdef DEBUG
        vcl_cerr << "after expansion : " << path << '\n';
#endif
        return vul_expand_path_internal(path);
      }
    }
  }

  // no more ideas
  return path;
}

typedef vcl_map<vcl_string, vcl_string, vcl_less<vcl_string> > map_t;

vcl_string vul_expand_path(vcl_string path)
{
  // create the cache.
  static map_t the_map;

  // look for the given path in the map.
  map_t::iterator i = the_map.find(path);

  if (i == the_map.end()) {
    // not in the map, so compute it :
    vcl_string mapped = vul_expand_path_internal(path);
    // cache it :
    i = the_map.insert(map_t::value_type(path, mapped)).first;
  }

  //
  return (*i).second;
}

vcl_string vul_expand_path_uncached(vcl_string path)
{
  return vul_expand_path_internal(path);
}

#endif // VCL_WIN32
