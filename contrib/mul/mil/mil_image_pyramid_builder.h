#ifndef mil_image_pyramid_builder_h_
#define mil_image_pyramid_builder_h_
#ifdef __GNUC__
#pragma interface
#endif

//: \file
//  \brief Base class for objects which build image pyramids.
//  \author Tim Cootes

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>

class mil_image;
class mil_image_pyramid;

//: Base class for objects which build image pyramids
class mil_image_pyramid_builder {

public:

		//: Dflt ctor
	mil_image_pyramid_builder();

		//: Destructor
	virtual ~mil_image_pyramid_builder();
	
		//: Create new (empty) pyramid on heap
		//  Caller responsible for its deletion
	virtual mil_image_pyramid* newImagePyramid() const =0;
	
		//: Build pyramid
	virtual void build(mil_image_pyramid&, const mil_image&) = 0;
	
		//: Scale step between levels
	virtual double scaleStep() const = 0;
	
		//: Version number for I/O 
	short version_no() const;

		//: Name of the class
	virtual vcl_string is_a() const = 0;

		//: Create a copy on the heap and return base class pointer
	virtual mil_image_pyramid_builder* clone() const = 0;

		//: Print class to os
	virtual void print_summary(vcl_ostream& os) const = 0;
		
		//: Save class to binary file stream
		//!in: bfs: Target binary file stream
	virtual void b_write(vsl_b_ostream& bfs) const = 0;

		//: Load class from binary file stream
		//!out: bfs: Target binary file stream
	virtual void b_read(vsl_b_istream& bfs) = 0;
};

	//: Allows derived class to be loaded by base-class pointer
	//  A loader object exists which is invoked by calls
	//  of the form "vsl_b_read(bfs,base_ptr);".  This loads derived class
	//  objects from the disk, places them on the heap and 
	//  returns a base class pointer.  
	//  In order to work the loader object requires
	//  an instance of each derived class that might be
	//  found.  This function gives the model class to
	//  the appropriate loader.
void vsl_add_to_binary_loader(const mil_image_pyramid_builder& b);

	//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mil_image_pyramid_builder& b);

	//: Binary file stream output operator for pointer to class
void vsl_b_write(vsl_b_ostream& bfs, const mil_image_pyramid_builder* b);

	//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mil_image_pyramid_builder& b);

	//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mil_image_pyramid_builder& b);

	//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const mil_image_pyramid_builder* b);


#endif



