
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>

#ifndef image_convert__INCLUDED
#define image_convert__INCLUDED 1

template <class T1, class T2>
vil_memory_image_of<T2> *convert_image
(
	const vil_memory_image_of<T1> &from,
	T2 &var
);

template <> vil_memory_image_of<unsigned char> *
convert_image<vil_rgb<unsigned char>, unsigned char>
( 
	const vil_memory_image_of<vil_rgb<unsigned char> > &from,
	unsigned char &var
);

template <> vil_memory_image_of<double> *
convert_image<vil_rgb<unsigned char>, double>
( 
	const vil_memory_image_of<vil_rgb<unsigned char> > &from,
	double &var
);
#endif
