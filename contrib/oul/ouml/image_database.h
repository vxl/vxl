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
#define OTAGO__image_database_INCLUDED_ 1

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <multimap.h>
#include <utility>
#include <vil/vil_memory_image.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

class ImageDatabase
{
	
	// the comparison object for the map
public:
	struct ltstr
	{
		bool operator()(const char* s1, const char* s2) const
		{
			return strcasecmp(s1, s2) < 0;
		}
	};

private:
	// the multimap to store label/image pairs
	multimap<const char*, vil_memory_image *, ltstr> image_db;

public:
	// typedefs for access to the image_db
	typedef multimap<const char*, vil_memory_image *, ltstr>::iterator iterator;
	typedef multimap<const char*, vil_memory_image *, ltstr>::const_iterator 
	const_iterator;
	typedef pair<const char *, vil_memory_image *> value_type;

	ImageDatabase(){};
	~ImageDatabase();
	inline iterator insert(const char *label, vil_memory_image *image)
		{char *new_label=new char[200]; strcpy(new_label, label);
		value_type ins(new_label, image); return(image_db.insert(ins));};
	inline iterator begin(){return image_db.begin();};
	inline iterator end(){return image_db.end();};
	inline const_iterator begin() const {return image_db.begin();};
	inline const_iterator end() const {return image_db.end();};
	inline pair<iterator, iterator> equal_range(const char *&label)
		{return(image_db.equal_range(label));};
	inline pair<const_iterator, const_iterator> equal_range(const char *&label)
		const {return(image_db.equal_range(label));};
	inline bool label_exists(const char *label)
		{return(image_db.count(label)>0);};
	void clear();
	
	bool save(const char *name, const char *imagetype);
	bool load(const char *name);

};

#endif
