/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include <vcl/vcl_utility.h>
#include "vbl_canonical_path.h"

#ifdef VCL_WIN32
vcl_string vbl_canonical_path(char const *path_) { return vcl_string(path_); }
#else

#include <vcl/vcl_functional.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_map.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

static
vcl_string vbl_canonical_path_internal(vcl_string path)
{
  // expand ~/ or just ~
  if ((path.size()>=2 && path[0] == '~' && path[1] == '/') || path == "~") {
    char const *HOME = getenv("HOME");
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
    getcwd(cwd, sizeof cwd);
    path = vcl_string(cwd) + vcl_string(path.c_str() + 1);
  }
  
  { // main processing and reduction goes here.
    vcl_vector<vcl_string> bits;

    // split the path into bits. a "bit" is either a single slash or a
    // sequence of non-slash characters.
    for (int i=0; i<path.size(); ) {
      if (path[i] == '/') {
	bits.push_back("/");
	++i;
      }
      else {
	int j=i;
	while (j<path.size() && path[j]!='/')
	  ++j;
	bits.push_back(vcl_string(path.c_str()+i, path.c_str()+j));
	i = j;
      }
    }

    // process the bits
    while (true) {
      bool again = false;
      for (int i=0; i<bits.size(); ++i) {
	// remove repeated /
	if (i+1<bits.size() && bits[i] == "/" && bits[i+1] == "/") {
	  bits.erase(bits.begin() + i);
	  again = true;
	}
	
	// remove trailing /
	if (i == bits.size()-1 && bits[i] == "/") {
	  bits.pop_back();
	  again = true;
	}
	
	// collapse foo/.. into /
	if (i+2<bits.size() && bits[i]!="/" && bits[i+1]=="/" && bits[i+2]=="..") {
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
    for (int i=0; i<bits.size(); ++i)
      path += bits[i];
    //cerr << "recomposed : " << path << endl;
  }
  
  // look for symbolic links to expand
  for (int i=1; i<=path.size(); ++i) {
    if (i==path.size() || path[i] == '/') {
      vcl_string sub(path.c_str(), path.c_str() + i);
      char buf[4096];
      int len = readlink(sub.c_str(), buf, sizeof buf);
      if (len != -1) {
	// it's a symlink. we should expand it and recurse.
	//cerr << "before expansion : " << path << endl;
	
	if (buf[0] == '/') {
	  // the target of the link starts with '/' so must be an
	  // absolute path : ...foo/bar/etc... => buf/etc...
	  path = vcl_string(buf, buf+len) + vcl_string(path.c_str() + i);
	}
	else {
	  // the target is relative to the symlink's directory.
	  int j=i-1;
	  while (j>=0 && path[j] != '/')
	    --j;
	  if (j>=0) {
	    // found another slash :   ...foo/bar/etc... where bar is the symlink.
	    vcl_string a = vcl_string(path.c_str(), path.c_str()+j+1);
	    vcl_string b = vcl_string(buf, buf+len);
	    vcl_string c = vcl_string(path.c_str() + i, path.c_str() + path.size());
	    //cerr << "a = " << a << endl;
	    //cerr << "b = " << b << endl;
	    //cerr << "c = " << c << endl;
	    path = a + b + c;
	  }
	  else {
	    // gurgle. only one slash. must be : /bar/etc where bar is the symlink.
	    path = vcl_string(buf, buf+len) + vcl_string(path.c_str() + i);
	  }
	}

	//cerr << "after expansion : " << path << endl;
	
	return vbl_canonical_path_internal(path);
      }
    }
  }
  
  // no more ideas
  return path;
}

typedef vcl_map<vcl_string, vcl_string, vcl_less<vcl_string> > map_t;

vcl_string vbl_canonical_path(vcl_string path)
{
  // create the map if not already created.
  static map_t *the_map = 0;
  if (! the_map)
    the_map = new map_t;

  // look for the given path in the map.
  map_t::iterator i = the_map->find(path);
  
  if (i == the_map->end()) {
    // not in the map, so compute it :
    vcl_string mapped = vbl_canonical_path_internal(path);
    // cache it :
    i = the_map->insert(map_t::value_type(path, mapped)).first;
  }
  
  //
  return (*i).second;
}

vcl_string vbl_canonical_path_uncached(vcl_string path)
{
  return vbl_canonical_path_internal(path);
}


#endif
