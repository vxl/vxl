// This is core/vil/io/vil_io_smart_ptr.txx
#ifndef vil_io_smart_ptr_txx_
#define vil_io_smart_ptr_txx_
//:
// \file
// \brief Serialised binary IO functions for vil_smart_ptr<T>
// \author Tim Cootes/Ian Scott (Manchester)

#include "vil_io_smart_ptr.h"
#include <vsl/vsl_binary_io.h>
#include <vil/vil_smart_ptr.h>

//=========================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream & os, const vil_smart_ptr<T> &p)
{
  // write version number
  const short io_version_no = 2;
  vsl_b_write(os, io_version_no);

  if (p.ptr() == 0)  // Deal with Null pointers first.
  {
    vsl_b_write(os, true);
    vsl_b_write(os, 0ul); // Use 0 to indicate a null pointer.
                          // True serialisation IDs are always 1 or more.
    return;
  }

  // Get a serial_number for object being pointed to
  unsigned long id = os.get_serial_number(p.ptr());
  // Find out if this is the first time the object being pointed to is
  // being saved
  if (id == 0)
  {
    id = os.add_serialisation_record(p.ptr());

      // Say that this is the first time
      // that this object is being saved.
      // This isn't really necessary but
      // it is useful as an error check
    vsl_b_write(os, true);
    vsl_b_write(os, id);     // Save the serial number
// If you get an error in the next line, it could be because your type T
// has no vsl_b_write(vsl_b_ostream &,const T*)  defined on it.
// See the documentation in the .h file to see how to add it.
    vsl_b_write(os, p.ptr());    // Only save the actual object if it
                                  //hasn't been saved before to this stream
  }
  else
  {
      // Say that this is not the first time
      // that this object is being saved.
      // This isn't really necessary but
      // it is useful as an error check

    vsl_b_write(os, false);
    vsl_b_write(os, id);         // Save the serial number
  }
}

//=====================================================================
//: Binary load self from stream.
template<class T>
void vsl_b_read(vsl_b_istream &is, vil_smart_ptr<T> &p)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
   case 2:
   {
    bool first_time; // true if the object is about to be loaded
    vsl_b_read(is, first_time);

    unsigned long id; // Unique serial number indentifying object
    vsl_b_read(is, id);

    if (id == 0) // Deal with Null pointers first.
    {
      p = 0;
      return;
    }

    T * pointer = static_cast<T *>( is.get_serialisation_pointer(id));
    if (first_time != (pointer == 0))
    {
      // This checks that the saving stream and reading stream
      // both agree on whether or not this is the first time they
      // have seen this object.
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil_smart_ptr<T>&)\n"
               << "           De-serialisation failure\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }

    if (pointer == 0)
    {
      // If you get an error in the next line, it could be because your type T
      // has no vsl_b_read(vsl_b_ostream&,T*&)  defined on it.
      // See the documentation in the .h file to see how to add it.
      vsl_b_read(is, pointer);
      is.add_serialisation_record(id, pointer);
    }

    p = pointer; // This operator method will set the internal
                 //pointer in vil_smart_ptr.
    break;
   }
   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vil_smart_ptr<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//=====================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(vcl_ostream & os,const vil_smart_ptr<T> & p)
{
  os << "Smart ptr to ";
  if (p.ptr())
  {
    // If you get an error in the next line, it could be because your type T
    // has no vsl_print_summary(vsl_b_ostream &, const T*)  defined on it.
    // See the documentation in the .h file to see how to add it.
    vsl_print_summary(os, (p.ptr()));
  }
  else
    os << "NULL";
}


#if 0 // commented out
//===========================================
// Deal with base class pointers
template<class T>
void vsl_b_read(vsl_b_istream& is, vil_smart_ptr<T> * &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    p = new vil_smart_ptr<T>;
    vsl_b_read(is, *p);
  }
  else
    p = 0;
}

template<class T>
void vsl_b_write(vsl_b_ostream& os, const vil_smart_ptr<T> *p)
{
  if (p==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*p);
  }
}

template<class T>
void vsl_print_summary(vcl_ostream, const vil_smart_ptr<T> *p)
{
  if (p==0)
    os << "NULL PTR";
  else
  {
    os << "vil_smart_ptr: ";
    vsl_print_summary(*p);
  }
}
#endif

#undef VIL_IO_SMART_PTR_INSTANTIATE
#define VIL_IO_SMART_PTR_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, const vil_smart_ptr<T > &); \
template void vsl_b_read(vsl_b_istream &, vil_smart_ptr<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vil_smart_ptr<T > &)

#endif // vil_io_smart_ptr_txx_
