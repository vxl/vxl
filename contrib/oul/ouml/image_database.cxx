//-*-c++-*--------------------------------------------------------------
/**
 * \file
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

#include "image_database.h"
#include <vcl_iostream.h>
#include <vcl_cerrno.h> // for EEXIST
#include <vcl_cctype.h> // for tolower()
#include <vcl_cstdio.h> // for fscanf()
#include <vpl/vpl.h> // for vpl_mkdir
#include <vil/vil_load.h>
#include <vil/vil_save.h>

//----------------------------------------------------------------------
/** destructor
 *
 * Iterate through all images in the database and delete them.
 *
 * \side Memory is actually deallocated
 *
 * \author Brendan McCane 
 */
//----------------------------------------------------------------------

ImageDatabase::~ImageDatabase()
{
  clear();
}

//----------------------------------------------------------------------
/** clear
 *
 * clear all the images from the database (deletion is performed)
 *
 * \author Brendan McCane 
 */
//----------------------------------------------------------------------
void ImageDatabase::clear()
{
  for (iterator i=begin(); i!=end(); i++)
  {
    delete (*i).second;
  }
  image_db.clear();
}

//----------------------------------------------------------------------
/** save
 *
 * Save all images in the database. To do this, I create a single
 * database file that has a list of label/filename pairs. I also
 * create a subdirectory in which all the images are stored. The
 * images are stored based on the imagetype parameter, which should be
 * the extension of a valid image file type (eg "ppm", "pgm", "png",
 * etc).
 *
 * \param name  the name of the database file (a directory called name.d is also created).
 *
 * \param imagetype  a valid file extension type (eg "ppm");
 *
 * \author Brendan McCane 
 */
//----------------------------------------------------------------------

bool ImageDatabase::save(const char *name, const char *imagetype)
{
  char dirname[200];
  vcl_sprintf(dirname, "%s.d", name);

  int err;

  err = vpl_mkdir( dirname, 0755 );
  if (err != 0 && err != EEXIST)
  {
    vcl_cerr << "can't open directory " << dirname << vcl_endl;
    return false;
  }

  // now open the database file
  FILE *dbfile;
  if (!(dbfile=vcl_fopen(name, "w")))
  {
    vcl_cerr << "Can't open database file " << name << vcl_endl;
    return false;
  }

  int index=0;
  for (iterator i=begin(); i!=end(); i++)
  {
    char filename[200];
    vcl_sprintf(filename, "%s/%s_%03d.%s", dirname, (*i).first, index++, imagetype);
    vil_save(*((*i).second), filename);

    vcl_printf("db: %s %s\n", (*i).first, filename);
    vcl_fprintf(dbfile, "%s %s\n", (*i).first, filename);
  }
  vcl_fclose(dbfile);
  return true;
}


//----------------------------------------------------------------------
/** load
 *
 * Load a database from file. 
 *
 * \param name  the name of the database file (a directory called name.d should also exist).
 *
 * \author Brendan McCane 
 */
//----------------------------------------------------------------------

bool ImageDatabase::load(const char *name)
{
  // now open the database file
  FILE *db;
  if (!(db=vcl_fopen(name, "r")))
  {
    vcl_cerr << "Can't open database file " << name << vcl_endl;
    return false;
  }

  char label[200], filename[200];
  while (vcl_fscanf(db, "%s%s", label, filename)!=EOF)
  {
    vil_image im = vil_load(filename);
    if (!im) return false;
    vil_memory_image *image = new vil_memory_image(im);
    insert(label, image);
  }
  vcl_fclose(db);
  return true;
}

bool ImageDatabase::ltstr::operator()(const char* s1, const char* s2) const
{
  // do a case insensitive comparison. Can't use strcasecmp
  // because it's not standard.
  vcl_string tmp1( s1 );
  vcl_string tmp2( s2 );
  vcl_transform( tmp1.begin(), tmp1.end(), tmp1.begin(), ::tolower );
  vcl_transform( tmp2.begin(), tmp2.end(), tmp2.begin(), ::tolower );
  return vcl_strcmp( tmp1.c_str(), tmp2.c_str() ) < 0;
}
