// miscellaneous io operators

#include "io.h"

template <class T>
ostream &operator <<(ostream &os, const vnl_vector<T> &vec)
{
	vnl_vector<T>::const_iterator iter, end_iter;

	end_iter = vec.end();
	cout << "<Vector: ";
	for (iter=vec.begin(); iter!=end_iter; iter++)
	{
		cout << " " << *iter;
	}
	cout << "> ";

	return(os);
}

template <class T>
ostream &operator <<(ostream &os, const vcl_vector<T> &vec)
{
	vcl_vector<T>::const_iterator iter, end_iter;

	end_iter = vec.end();
	cout << "<Vector: ";
	for (iter=vec.begin(); iter!=end_iter; iter++)
	{
		cout << " " << *iter;
	}
	cout << "> ";

	return(os);
}

template <class T, class S>
ostream &operator<<(ostream &os, const vcl_map<T, S> &omap)
{
	vcl_map<T, S>::const_iterator iter, end_iter;

	end_iter = omap.end();
	os << "<Map: ";
	for (iter=omap.begin(); iter!=end_iter; iter++)
	{
		os << " <" << (*iter).first << ", " << (*iter).second << ">";
	}
	os << "> ";

	return(os);
}
	
template <class T>
ostream &operator <<(ostream &os, const vcl_set<T> &vec)
{
	vcl_set<T>::const_iterator iter, end_iter;

	end_iter = vec.end();
	os << "<Set: ";
	for (iter=vec.begin(); iter!=end_iter; iter++)
	{
		os << " " << *iter;
	}
	os << "> ";

	return(os);
}


ostream &operator<<(ostream &os, const vil_rgb<unsigned char> &colour)
{
	os << "<RGB = " << (int)colour.R_ << ", " << (int)colour.G_ << ", " 
	   << (int)colour.B_ << ">\n";
	return os;
}
