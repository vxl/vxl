//-*-c++-*--------------------------------------------------------------
/** \file image_database.h
 *
 * An image database. Basically maintains a list of labels and
 * associated images. And allows for saving and loading a database.
 * Ideally, this would form an inheritance hierarchy or be a templated
 * class, but I'm looking for simplicity at the moment.
 *
 * The images inserted into the database WILL be deleted on
 * destruction of the database. So only insert things you don't want
 * to persist.
 *
 * \author Brendan McCane
 * \date 17/7/01
 *
 * Copyright (c) 2001 Brendan McCane
 * University of Otago, Dunedin, New Zealand
 * Reproduction rights limited as described in the COPYRIGHT file.
 */
//----------------------------------------------------------------------

#ifndef OTAGO__image_database_INCLUDED_
#define OTAGO__image_database_INCLUDED_

#include <vcl_sys/types.h>
#include <vcl_cstring.h> // for strcpy(), strcmp()
#include <vcl_cctype.h> // for tolower()
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_algorithm.h>
#include <vil/vil_memory_image.h>

class ImageDatabase
{
  // the comparison object for the map
public:
  struct ltstr
  {
    bool operator()(const char* s1, const char* s2) const
    {
      // do a case insensitive comparision. Can't use strcasecmp
      // because it's not standard.
      vcl_string tmp1( s1 );
      vcl_string tmp2( s2 );
      vcl_transform( tmp1.begin(), tmp1.end(), tmp1.begin(), vcl_tolower );
      vcl_transform( tmp2.begin(), tmp2.end(), tmp2.begin(), vcl_tolower );
      return vcl_strcmp( tmp1.c_str(), tmp2.c_str() ) < 0;
    }
  };

private:
  // the multimap to store label/image pairs
  vcl_multimap<const char*, vil_memory_image *, ltstr> image_db;

public:
  // typedefs for access to the image_db
  typedef vcl_multimap<const char*, vil_memory_image *, ltstr>::iterator iterator;
  typedef vcl_multimap<const char*, vil_memory_image *, ltstr>::const_iterator const_iterator;
  typedef vcl_pair<const char *, vil_memory_image *> value_type;

  ImageDatabase(){}
  ~ImageDatabase();
  inline iterator insert(const char *label, vil_memory_image *image)
    {char *new_label=new char[200]; vcl_strcpy(new_label, label);
    value_type ins(new_label, image); return image_db.insert(ins);}
  inline iterator begin(){return image_db.begin();}
  inline iterator end(){return image_db.end();}
  inline const_iterator begin() const {return image_db.begin();}
  inline const_iterator end() const {return image_db.end();}
  inline vcl_pair<iterator, iterator> equal_range(const char *&label)
    {return image_db.equal_range(label);}
  inline vcl_pair<const_iterator, const_iterator> equal_range(const char *&label)
    const {return image_db.equal_range(label);}
  inline bool label_exists(const char *label)
    {return image_db.count(label)>0;}
  void clear();

  bool save(const char *name, const char *imagetype);
  bool load(const char *name);
};

#endif // OTAGO__image_database_INCLUDED_
