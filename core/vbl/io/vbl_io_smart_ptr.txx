// This is core/vbl/io/vbl_io_smart_ptr.txx
#ifndef vbl_io_smart_ptr_txx_
#define vbl_io_smart_ptr_txx_
//:
// \file
// \brief Serialised binary IO functions for vbl_smart_ptr<T>
// \author Ian Scott (Manchester)
// \date 26-Mar-2001

#include "vbl_io_smart_ptr.h"
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_cstdlib.h> // vcl_abort()

//=========================================================================
//: Binary save self to stream.
template<class T>
void vsl_b_write(vsl_b_ostream & os, const vbl_smart_ptr<T> &p)
{
  // write version number
  const short io_version_no = 2;
  vsl_b_write(os, io_version_no);
  vsl_b_write(os, p.is_protected());

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
    // <rant> IMS
    // It is not clear how to deal fully satisfactorily with unprotected
    // smart_ptrs. For example is we save and reload them without any error
    // checks we could restore the object with a reference count of 0.
    // To be honest, I think the idea of an unprotected smart_ptr is
    // not so smart. Either it is a smart pointer, in which case it should
    // be protected, or it is unprotected in which case you should use a
    // ordinary pointer. Cycles in the pointer network, are best dealt with
    // by avoiding them. You have to be aware they are happening to unprotect
    // the pointer anyway.
    // <\rant>
    if (!p.is_protected())
    {
        vcl_cerr << "vsl_b_write(vsl_b_ostream & os, const vbl_smart_ptr<T>:"
                 << " You cannot\nsave unprotected smart pointers before saving"
                 << " a protected smart pointer\nto the same object. Either do"
                 << " not save unprotected smart pointers, or\nbe very careful"
                 << " about the order.\n";
        vcl_abort();
    }

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
void vsl_b_read(vsl_b_istream &is, vbl_smart_ptr<T> &p)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
   case 2:
   {
    bool is_protected; // true if the smart_ptr is to be
                       //responsible for the object
    vsl_b_read(is, is_protected);

    bool first_time; // true if the object is about to be loaded
    vsl_b_read(is, first_time);

    if (first_time && !is_protected)  // This should have been
    {                                  //checked during saving
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vbl_smart_ptr<T>&)\n"
               << "           De-serialisation failure of non-protected smart_ptr\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
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
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vbl_smart_ptr<T>&)\n"
               << "           De-serialisation failure\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }

    if (pointer == 0)
    {
      // If you get an error in the next line, it could be because your type T
      // has no vsl_b_read(vsl_b_istream&,T*&)  defined on it.
      // See the documentation in the .h file to see how to add it.
      vsl_b_read(is, pointer);
      is.add_serialisation_record(id, pointer);
    }

    p = pointer; // This operator method will set the internal
                 //pointer in vbl_smart_ptr.
    if (!is_protected)
      p.unprotect();

    break;
   }
   default:
    vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, vbl_smart_ptr<T>&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}

//=====================================================================
//: Output a human readable summary to the stream
template<class T>
void vsl_print_summary(vcl_ostream & os,const vbl_smart_ptr<T> & p)
{
  if (p.is_protected())
    os <<"Unprotected ";
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


#undef VBL_IO_SMART_PTR_INSTANTIATE
#define VBL_IO_SMART_PTR_INSTANTIATE(T) \
template void vsl_print_summary(vcl_ostream &, const vbl_smart_ptr<T > &); \
template void vsl_b_read(vsl_b_istream &, vbl_smart_ptr<T > &); \
template void vsl_b_write(vsl_b_ostream &, const vbl_smart_ptr<T > &)

#endif // vbl_io_smart_ptr_txx_
