//:
// \file
//
// An image database. Basically maintains a list of labels and
// associated images. And allows for saving and loading a database.
// Ideally, this would form an inheritance hierarchy or be a templated
// class, but I'm looking for simplicity at the moment.
//
// The images inserted into the database WILL be deleted on
// destruction of the database. So only insert things you don't want
// to persist.
//
// \author Brendan McCane
// \date 17 July 2001
//
// Copyright (c) 2001 Brendan McCane
// University of Otago, Dunedin, New Zealand
// Reproduction rights limited as described in the COPYRIGHT file.
//----------------------------------------------------------------------

#include <iostream>
#include <cerrno>
#include <cctype>
#include <cstdio>
#include <algorithm>
#include "image_database.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h> // for vpl_mkdir
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>

//----------------------------------------------------------------------
//: destructor
//
// Iterate through all images in the database and delete them.
//
// \remark Memory is actually deallocated
//
// \author Brendan McCane
//----------------------------------------------------------------------

ImageDatabase::~ImageDatabase()
{
  clear();
}

//----------------------------------------------------------------------
//: clear
//
// clear all the images from the database (deletion is performed)
//
// \author Brendan McCane
//----------------------------------------------------------------------
void ImageDatabase::clear()
{
  for (auto & i : *this)
  {
    delete i.second;
  }
  image_db.clear();
}

//----------------------------------------------------------------------
//: save
//
// Save all images in the database. To do this, I create a single
// database file that has a list of label/filename pairs. I also
// create a subdirectory in which all the images are stored. The
// images are stored based on the imagetype parameter, which should be
// the extension of a valid image file type (eg "ppm", "pgm", "png",
// etc).
//
// \param name  the name of the database file (a directory called name.d is also created).
//
// \param imagetype  a valid file extension type (eg "ppm");
//
// \author Brendan McCane
//----------------------------------------------------------------------

bool ImageDatabase::save(const char *name, const char *imagetype)
{
  char dirname[200];
  std::sprintf(dirname, "%s.d", name);

  int err;

  err = vpl_mkdir( dirname, 0755 );
  if (err != 0 && err != EEXIST)
  {
    std::cerr << "can't open directory " << dirname << std::endl;
    return false;
  }

  // now open the database file
  FILE *dbfile;
  if (!(dbfile=std::fopen(name, "w")))
  {
    std::cerr << "Can't open database file " << name << std::endl;
    return false;
  }

  int index=0;
  for (auto & i : *this)
  {
    char filename[200];
    std::sprintf(filename, "%s/%s_%03d.%s", dirname, i.first, index++, imagetype);
    vil1_save(*(i.second), filename);

    std::printf("db: %s %s\n", i.first, filename);
    std::fprintf(dbfile, "%s %s\n", i.first, filename);
  }
  std::fclose(dbfile);
  return true;
}


//----------------------------------------------------------------------
//: load
//
// Load a database from file.
//
// \param name  the name of the database file (a directory called name.d should also exist).
//
// \author Brendan McCane
//----------------------------------------------------------------------

bool ImageDatabase::load(const char *name)
{
  // now open the database file
  FILE *db;
  if (!(db=std::fopen(name, "r")))
  {
    std::cerr << "Can't open database file " << name << std::endl;
    return false;
  }

  char label[200], filename[200];
  while (std::fscanf(db, "%s%s", label, filename)!=EOF)
  {
    vil1_image im = vil1_load(filename);
    if (!im) return false;
    auto *image = new vil1_memory_image(im);
    insert(label, image);
  }
  std::fclose(db);
  return true;
}

bool ImageDatabase::ltstr::operator()(const char* s1, const char* s2) const
{
  // do a case insensitive comparison. Can't use strcasecmp
  // because it's not standard.
  std::string tmp1( s1 );
  std::string tmp2( s2 );
  std::transform( tmp1.begin(), tmp1.end(), tmp1.begin(), ::tolower );
  std::transform( tmp2.begin(), tmp2.end(), tmp2.begin(), ::tolower );
  return std::strcmp( tmp1.c_str(), tmp2.c_str() ) < 0;
}
