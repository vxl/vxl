// This is oul/ouml/image_database.h
#ifndef OTAGO__image_database_INCLUDED_
#define OTAGO__image_database_INCLUDED_
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
//
//----------------------------------------------------------------------

#include <iostream>
#include <cstring>
#include <map>
#include <utility>
#include <vcl_sys/types.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_memory_image.h>

class ImageDatabase
{
  // the comparison object for the map
 public:
  struct ltstr
  {
    bool operator()(const char* s1, const char* s2) const;
  };

 private:
  // the multimap to store label/image pairs
  std::multimap<const char*, vil1_memory_image *, ltstr> image_db;

 public:
  // typedefs for access to the image_db
  typedef std::multimap<const char*, vil1_memory_image *, ltstr>::iterator iterator;
  typedef std::multimap<const char*, vil1_memory_image *, ltstr>::const_iterator const_iterator;
  typedef std::pair<const char *, vil1_memory_image *> value_type;

  ImageDatabase()= default;
  ~ImageDatabase();
  inline iterator insert(const char *label, vil1_memory_image *image)
    {char *new_label=new char[200]; std::strcpy(new_label, label);
    value_type ins(new_label, image); return image_db.insert(ins);}
  inline iterator begin(){return image_db.begin();}
  inline iterator end(){return image_db.end();}
  inline const_iterator begin() const {return image_db.begin();}
  inline const_iterator end() const {return image_db.end();}
  inline std::pair<iterator, iterator> equal_range(const char *&label)
    {return image_db.equal_range(label);}
  inline std::pair<const_iterator, const_iterator> equal_range(const char *&label)
    const {return image_db.equal_range(label);}
  inline bool label_exists(const char *label) const
    {return image_db.count(label)>0;}
  void clear();

  bool save(const char *name, const char *imagetype);
  bool load(const char *name);
};

#endif // OTAGO__image_database_INCLUDED_
