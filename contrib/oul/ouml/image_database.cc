//-*-c++-*--------------------------------------------------------------
/** \file image_database.cc
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
		delete((*i).second);
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
 * \param const char *name: the name of the database file (a directory
 * called name.d is also created).
 *
 * \param const char *imagetype: a valid file extension type (eg "ppm");
 *
 * \author Brendan McCane 
 */
//----------------------------------------------------------------------

bool ImageDatabase::save(const char *name, const char *imagetype)
{
	char dirname[200];
	sprintf(dirname, "%s.d", name);

	DIR *d;
	// check if the directory exists
	if ((d = opendir(dirname))==NULL)
	{
		int fd;
		// if it doesn't exist
		if (errno==ENOENT)
		{
			// try and create it
			if ((fd=mkdir(dirname, S_IRWXU))==-1)
			{
				cerr << "can't open directory " << dirname << endl;
				return(false);
			}
			else close(fd);
		}
		else perror("ImageDatabase::save: Can't open directory for saving");
	} else closedir(d);

	// now open the database file
	FILE *dbfile;
	if (!(dbfile=fopen(name, "w")))
	{
		cerr << "Can't open database file " << name << endl;
		return(false);
	}

	int index=0;
	for (iterator i=begin(); i!=end(); i++)
	{
		char filename[200];
		sprintf(filename, "%s/%s_%03d.%s", dirname, (*i).first, index++, 
				imagetype);
		vil_save(*((*i).second), filename);

		printf("db: %s %s\n", (*i).first, filename);
		fprintf(dbfile, "%s %s\n", (*i).first, filename);
	}
	fclose(dbfile);
	return(true);
}


//----------------------------------------------------------------------
/** load
 *
 * Load a database from file. 
 *
 * \param const char *name: the name of the database file (a directory
 * called name.d should also exist).
 *
 * \author Brendan McCane 
 */
//----------------------------------------------------------------------

bool ImageDatabase::load(const char *name)
{
	// now open the database file
	FILE *db;
	if (!(db=fopen(name, "r")))
	{
		cerr << "Can't open database file " << name << endl;
		return(false);
	}

	char label[200], filename[200];
	while (fscanf(db, "%s%s", label, filename)!=EOF)
	{
		vil_image im = vil_load(filename);
		if (!im) return(false);
		vil_memory_image *image = new vil_memory_image(im);
		insert(label, image);
	}
	fclose(db);
	return(true);
}
