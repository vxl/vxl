// This is core/vsl/vsl_binary_io.cxx
#include "vsl_binary_io.h"
//:
// \file
// \brief Functions to perform consistent binary IO within vsl
// \author Tim Cootes and Ian Scott

#include <vcl_cstddef.h>
#include <vcl_cassert.h>
#include <vcl_map.txx>
#include <vcl_cstdlib.h> // abort()
#include <vsl/vsl_binary_explicit_io.h>

void vsl_b_write(vsl_b_ostream& os, char n )
{
  os.os().write( ( char* )&n, sizeof( n ) );
}

void vsl_b_read(vsl_b_istream &is, char& n )
{
  is.is().read( ( char* )&n, sizeof( n ) );
}

void vsl_b_write(vsl_b_ostream& os, signed char n )
{
  os.os().write( ( char* )&n, sizeof( n ) );
}

void vsl_b_read(vsl_b_istream &is, signed char& n )
{
  is.is().read( ( char* )&n, sizeof( n ) );
}


void vsl_b_write(vsl_b_ostream& os,unsigned char n )
{
  os.os().write( ( char* )&n, 1 );
}

void vsl_b_read(vsl_b_istream &is,unsigned char& n )
{
  is.is().read( ( char* )&n, 1 );
}


void vsl_b_write(vsl_b_ostream& os, const vcl_string& str )
{
    vcl_string::const_iterator          it;

    vsl_b_write(os,(short)str.length());
    for ( it = str.begin(); it != str.end(); ++it )
        vsl_b_write(os,*it);
}

void vsl_b_read(vsl_b_istream &is, vcl_string& str )
{
    vcl_string::iterator                it;
    vcl_string::size_type               length;

    vsl_b_read(is,length);
    str.resize( length );
    for ( it = str.begin(); it != str.end(); ++it )
        vsl_b_read(is,*it);
}


void vsl_b_write(vsl_b_ostream& os,const char *s )
{
  int i = -1;
  do {
     i++;
     vsl_b_write(os,s[i]);
  } while ( s[i] != 0 );
}

void vsl_b_read(vsl_b_istream &is,char *s )
{
  int i = -1;
  do {
    i++;
    vsl_b_read(is,s[i]);
  } while ( s[i] != 0 );
}


void vsl_b_write(vsl_b_ostream& os,bool b)
{
  if (b)
    vsl_b_write(os, (signed char) -1);
  else
    vsl_b_write(os, (signed char) 0);
}

void vsl_b_read(vsl_b_istream &is,bool& b)
{
  signed char c;
  vsl_b_read(is, c);
  b = (c != 0);
}


void vsl_b_write(vsl_b_ostream& os,int n )
{
  unsigned char buf[ VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(int)) ];
  unsigned long nbytes = vsl_convert_to_arbitrary_length(&n, buf);
  os.os().write((char*)buf, nbytes );
}

void vsl_b_read(vsl_b_istream &is,int& n )
{
  unsigned char buf[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(int))];
  unsigned char *ptr=buf;
  do
  {
    vsl_b_read(is, *ptr);
    if (ptr-buf >= (vcl_ptrdiff_t)VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(int)))
    {
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream &, int& )\n"
               << "           Integer too big. Likely cause either file corruption, or\n"
               << "           file was created on platform with larger integer sizes.\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
  while (!(*(ptr++) & 128));
  vsl_convert_from_arbitrary_length(buf, &n);
}


void vsl_b_write(vsl_b_ostream& os,unsigned int n )
{
  unsigned char
    buf[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned int))];
  unsigned long nbytes = vsl_convert_to_arbitrary_length(&n, buf);
  os.os().write((char*)buf, nbytes );
}

void vsl_b_read(vsl_b_istream &is,unsigned int& n )
{
  unsigned char buf[
    VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned int))];
  unsigned char *ptr=buf;
  do
  {
    vsl_b_read(is, *ptr);
    if (ptr-buf >= (vcl_ptrdiff_t)VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned int)))
    {
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream &, unsigned int& )\n"
               << "           Integer too big. Likely cause either file corruption, or\n"
               << "           file was created on platform with larger integer sizes.\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
  while (!(*(ptr++) & 128));
  vsl_convert_from_arbitrary_length(buf, &n);
}


void vsl_b_write(vsl_b_ostream& os,short n )
{
  unsigned char buf[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(short))];
  unsigned long nbytes = vsl_convert_to_arbitrary_length(&n, buf);
  os.os().write((char*)buf, nbytes );
}

void vsl_b_read(vsl_b_istream &is,short& n )
{
  unsigned char buf[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(short))];
  unsigned char *ptr=buf;
  do
  {
    vsl_b_read(is, *ptr);

    if (ptr-buf >= (vcl_ptrdiff_t)VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(short)))
    {
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream &, short& )\n"
               << "           Integer too big. Likely cause either file corruption, or\n"
               << "           file was created on platform with larger integer sizes.\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
  while (!(*(ptr++) & 128));
  vsl_convert_from_arbitrary_length(buf, &n);
}

void vsl_b_write(vsl_b_ostream& os, unsigned short n )
{
  unsigned char buf[
    VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned short))];
  unsigned long nbytes = vsl_convert_to_arbitrary_length(&n, buf);
  os.os().write((char*)buf, nbytes );
}

void vsl_b_read(vsl_b_istream &is, unsigned short& n )
{
  unsigned char buf[
    VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned short))];
  unsigned char *ptr=buf;
  do
  {
    vsl_b_read(is, *ptr);
    if (ptr-buf >= (vcl_ptrdiff_t)VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned short)))
    {
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream &, unsigned short& )\n"
               << "           Integer too big. Likely cause either file corruption, or\n"
               << "           file was created on platform with larger integer sizes.\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
  while (!(*(ptr++) & 128));
  vsl_convert_from_arbitrary_length(buf, &n);
}


void vsl_b_write(vsl_b_ostream& os,long n )
{
  unsigned char buf[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(long))];
  unsigned long nbytes = vsl_convert_to_arbitrary_length(&n, buf);
  os.os().write((char*)buf, nbytes );
}

void vsl_b_read(vsl_b_istream &is,long& n )
{
  unsigned char buf[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(long))];
  unsigned char *ptr=buf;
  do
  {
    vsl_b_read(is, *ptr);
    if (ptr-buf >= (vcl_ptrdiff_t)VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(long)))
    {
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream &, long& )\n"
               << "           Integer too big. Likely cause either file corruption, or\n"
               << "           file was created on platform with larger integer sizes.\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
  while (!(*(ptr++) & 128));
  vsl_convert_from_arbitrary_length(buf, &n);
}

void vsl_b_write(vsl_b_ostream& os,unsigned long n )
{
  unsigned char buf[
    VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned long))];
  unsigned long nbytes = vsl_convert_to_arbitrary_length(&n, buf);
  os.os().write((char*)buf, nbytes );
}

void vsl_b_read(vsl_b_istream &is,unsigned long& n )
{
  unsigned char buf[
    VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned long))];
  unsigned char *ptr=buf;
  do
  {
    vsl_b_read(is, *ptr);
    if (ptr-buf >= (vcl_ptrdiff_t)VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned long)))
    {
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream &, unsigned long& )\n"
               << "           Integer too big. Likely cause either file corruption, or\n"
               << "           file was created on platform with larger integer sizes.\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
  while (!(*(ptr++) & 128));
  vsl_convert_from_arbitrary_length(buf, &n);
}

#if VXL_HAS_INT_64 && !VXL_INT_64_IS_LONG

void vsl_b_write(vsl_b_ostream& os, vxl_int_64 n )
{
  unsigned char buf[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vxl_int_64))];
  unsigned long nbytes = vsl_convert_to_arbitrary_length(&n, buf);
  os.os().write((char*)buf, nbytes );
}

void vsl_b_read(vsl_b_istream &is,vxl_int_64& n )
{
  unsigned char buf[VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vxl_int_64))];
  unsigned char *ptr=buf;
  do
  {
    vsl_b_read(is, *ptr);
    if (ptr-buf >= (vcl_ptrdiff_t)VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vxl_int_64)))
    {
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream &, vxl_int_64& )\n"
               << "           Integer too big. Likely cause either file corruption, or\n"
               << "           file was created on platform with larger integer sizes.\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
  while (!(*(ptr++) & 128));
  vsl_convert_from_arbitrary_length(buf, &n);
}

void vsl_b_write(vsl_b_ostream& os, vxl_uint_64 n )
{
  unsigned char buf[
    VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vxl_uint_64))];
  unsigned long nbytes = vsl_convert_to_arbitrary_length(&n, buf);
  os.os().write((char*)buf, nbytes );
}

void vsl_b_read(vsl_b_istream &is,vxl_uint_64& n )
{
  unsigned char buf[
    VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vxl_uint_64))];
  unsigned char *ptr=buf;
  do
  {
    vsl_b_read(is, *ptr);
    if (ptr-buf >= (vcl_ptrdiff_t)VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vxl_uint_64)))
    {
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream &, vxl_uint_64& )\n"
               << "           Integer too big. Likely cause either file corruption, or\n"
               << "           file was created on platform with larger integer sizes.\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
  while (!(*(ptr++) & 128));
  vsl_convert_from_arbitrary_length(buf, &n);
}

#endif // VXL_HAS_INT_64

#if 0
// When the macro is ready, this test will be
// #if ! VCL_PTRDIFF_T_IS_A_STANDARD_TYPE

void vsl_b_write(vsl_b_ostream& os, vcl_ptrdiff_t n )
{
  unsigned char buf[
    VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vcl_ptrdiff_t))];
  unsigned long nbytes = vsl_convert_to_arbitrary_length(&n, buf);
  os.os().write((char*)buf, nbytes );
}

void vsl_b_read(vsl_b_istream &is, vcl_ptrdiff_t& n )
{
  unsigned char buf[
    VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vcl_ptrdiff_t))];
  unsigned char *ptr=buf;
  do
  {
    vsl_b_read(is, *ptr);
    if (ptr-buf >= (vcl_ptrdiff_t)VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vcl_ptrdiff_t)))
    {
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream &, vcl_ptrdiff_t& )\n"
               << "           Integer too big. Likely cause either file corruption, or\n"
               << "           file was created on platform with larger integer sizes\n"
               << "           and represents a very large data structure.\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
  while (!(*(ptr++) & 128));
  vsl_convert_from_arbitrary_length(buf, &n);
}
#endif // 0

#if 0
// When the macro is ready, this test will be
// #if ! VCL_SIZE_T_IS_A_STANDARD_TYPE

void vsl_b_write(vsl_b_ostream& os, vcl_size_t n )
{
  unsigned char buf[
    VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vcl_size_t))];
  unsigned long nbytes = vsl_convert_to_arbitrary_length(&n, buf);
  os.os().write((char*)buf, nbytes );
}

void vsl_b_read(vsl_b_istream &is, vcl_size_t& n )
{
  unsigned char buf[
    VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vcl_size_t))];
  unsigned char *ptr=buf;
  do
  {
    vsl_b_read(is, *ptr);
    if (ptr-buf >= (vcl_ptrdiff_t)VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(vcl_size_t)))
    {
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream &, vcl_size_t& )\n"
               << "           Integer too big. Likely cause either file corruption, or\n"
               << "           file was created on platform with larger integer sizes\n"
               << "           and represents a very large data structure.\n";
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
    }
  }
  while (!(*(ptr++) & 128));
  vsl_convert_from_arbitrary_length(buf, &n);
}
#endif // 0


void vsl_b_write(vsl_b_ostream& os,float n )
{
  vsl_swap_bytes(( char* )&n, sizeof( n ) );
  os.os().write( ( char* )&n, sizeof( n ) );
}

void vsl_b_read(vsl_b_istream &is,float& n )
{
  is.is().read( ( char* )&n, sizeof( n ) );
  vsl_swap_bytes(( char* )&n, sizeof( n ) );
}

void vsl_b_write(vsl_b_ostream& os,double n )
{
  vsl_swap_bytes(( char* )&n, sizeof( n ) );
  os.os().write( ( char* )&n, sizeof( n ) );
}

void vsl_b_read(vsl_b_istream &is,double& n )
{
  is.is().read( ( char* )&n, sizeof( n ) );
  vsl_swap_bytes(( char* )&n, sizeof( n ) );
}


const unsigned short vsl_b_ostream::version_no_ = 1;
const vcl_streamoff vsl_b_ostream::header_length = 6;
static const unsigned short vsl_magic_number_part_1=0x2c4e;
static const unsigned short vsl_magic_number_part_2=0x472b;


//: Create this adaptor using an existing stream
// The stream (os) must be open (i.e. ready to be written to) so that the
// IO version number can be written by this constructor.
// User is responsible for deleting os after deleting the adaptor
vsl_b_ostream::vsl_b_ostream(vcl_ostream *os): os_(os)
{
  assert(os_ != 0);
  vsl_b_write_uint_16(*this, version_no_);
  vsl_b_write_uint_16(*this, vsl_magic_number_part_1);
  vsl_b_write_uint_16(*this, vsl_magic_number_part_2);
}

//: A reference to the adaptor's stream
vcl_ostream& vsl_b_ostream::os() const
{
  assert(os_ != 0);
  return *os_;
}

//: Returns true if the underlying stream has its fail bit set.
bool vsl_b_ostream::operator!() const
{
  return os_->operator!();
}


//: Clear the stream's record of any serialisation operations
// Calling this function while outputting serialisable things to stream,
// will mean that a second copy of an object may get stored to the stream.
void vsl_b_ostream::clear_serialisation_records()
{
  serialisation_records_.clear();
}


//: Adds an object pointer to the serialisation records.
// Returns a unique identifier for the object.
//
// \a pointer must be non-null, so you should handle null pointers separately.
//
// You can optionally add some user-defined integer with each record
// If error checking is on, and the object pointer is null or already in the records,
// this function will abort()
unsigned long vsl_b_ostream::add_serialisation_record
                    (void *pointer, int other_data /*= 0*/)
{
  assert(pointer != 0);
  assert(serialisation_records_.find(pointer) == serialisation_records_.end());
  unsigned long id = serialisation_records_.size() + 1;
  serialisation_records_[pointer] = vcl_make_pair(id, other_data);
  return id;
}


//: Returns a unique identifier for the object.
// Returns 0 if there is no record of the object.
unsigned long vsl_b_ostream::get_serial_number(void *pointer) const
{
  serialisation_records_type::const_iterator entry =
    serialisation_records_.find(pointer);
  if (entry == serialisation_records_.end())
  {
    return 0;
  }
  else
  {
    return (*entry).second.first;
  }
}

//: Set the user-defined data associated with the object
// If there is no record of the object, this function will return 0.
// However a retval of 0 does not necessarily imply that the object is
// unrecorded.
int vsl_b_ostream::get_serialisation_other_data(void *pointer) const
{
  serialisation_records_type::const_iterator entry =
    serialisation_records_.find(pointer);
  if (entry == serialisation_records_.end())
  {
    return 0;
  }
  else
  {
    return (*entry).second.second;
  }
}

//: Modify the user-defined data associated with the object.
// If there is no record of the object, this function will abort.
int vsl_b_ostream::set_serialisation_other_data
    (void *pointer, int /*other_data*/)
{
  serialisation_records_type::iterator entry =
    serialisation_records_.find(pointer);
  if (entry == serialisation_records_.end())
  {
    vcl_cerr << "vsl_b_ostream::set_serialisation_other_data():\n"
             << "No such value " << pointer << "in records.\n";
    vcl_abort();
  }
  return (*entry).second.second;
}


//: destructor.
vsl_b_ofstream::~vsl_b_ofstream()
{
  if (os_) delete os_;
}


//: Close the stream
void vsl_b_ofstream::close()
{
  assert(os_ != 0);
  ((vcl_ofstream *)os_)->close();
  clear_serialisation_records();
}

//: Create this adaptor using an existing stream.
// The stream (is) must be open (i.e. ready to be read from) so that the
// IO version number can be read by this constructor.
// User is responsible for deleting is after deleting the adaptor
vsl_b_istream::vsl_b_istream(vcl_istream *is): is_(is)
{
  assert(is_ != 0);
  if (!(*is_)) return;
  unsigned long v, m1, m2;
  vsl_b_read_uint_16(*this, v);
  vsl_b_read_uint_16(*this, m1);
  vsl_b_read_uint_16(*this, m2);

  // If this test fails, either the file is missing, or it is not a
  // Binary VXL file, or it is a corrupted Binary VXL file
  if (m2 != vsl_magic_number_part_2 || m1 != vsl_magic_number_part_1)
  {
    vcl_cerr << "\nI/O ERROR: vsl_b_istream::vsl_b_istream(vcl_istream *is)\n"
             <<   "           The input stream does not appear to be a binary VXL stream.\n"
             <<   "           Can't find correct magic number.\n";
    is_->clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }

  if (v != 1)
  {
    vcl_cerr << "\nI/O ERROR: vsl_b_istream::vsl_b_istream(vcl_istream *is)\n"
             << "             The stream's leading version number is "
             << v << ". Expected value 1.\n";
    is_->clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  }
  version_no_ = (unsigned short)v;
}

//: A reference to the adaptor's stream
vcl_istream & vsl_b_istream::is() const
{
  assert(is_ != 0);
  return *is_;
}


//: Return the version number of the IO format of the file being read.
unsigned short vsl_b_istream::version_no() const
{
  return version_no_;
}

//: Returns true if the underlying stream has its fail bit set.
bool vsl_b_istream::operator!() const
{
  return is_->operator!();
}

//: Clear the stream's record of any serialisation operations
// Calling this function while inputting serialisable things from a stream,
// could cause errors during loading unless the records were cleared at a
// similar point during output.
void vsl_b_istream::clear_serialisation_records()
{
  serialisation_records_.clear();
}

//: Adds record of object's unique serial number, and location in memory.
// \a pointer must be non-null, so you should handle null pointers separately.
//
// Adding a null pointer or one that already exists will cause the function to abort(),
// if debugging is turned on;
//
// You can also store a single integer as other data.
// Interpretation of this data is entirely up to the client code.
void vsl_b_istream::add_serialisation_record(unsigned long serial_number,
                                             void *pointer, int other_data /*= 0*/)
{
  assert(pointer != 0);
  assert(serialisation_records_.find(serial_number) == serialisation_records_.end());
  serialisation_records_[serial_number] = vcl_make_pair(pointer, other_data);
}

//: Returns the pointer to the object identified by the unique serial number.
// Returns 0 if no record has been added.
void* vsl_b_istream::get_serialisation_pointer(unsigned long serial_number) const
{
  serialisation_records_type::const_iterator entry =
        serialisation_records_.find(serial_number);
  if (entry == serialisation_records_.end())
  {
    return 0;
  }
  else
  {
    return (*entry).second.first;
  }
}

//: Returns the user defined data associated with the unique serial number
// Returns 0 if no record has been added.
int vsl_b_istream::get_serialisation_other_data
    (unsigned long serial_number) const
{
  serialisation_records_type::const_iterator entry =
    serialisation_records_.find(serial_number);
  if (entry == serialisation_records_.end())
  {
    return 0;
  }
  else
  {
    return (*entry).second.second;
  }
}

//: Modify the user-defined data associated with the unique serial number
// If there is no record of the object, this function will  abort.
int vsl_b_istream::set_serialisation_other_data
    (unsigned long serial_number, int /*other_data*/)
{
  serialisation_records_type::const_iterator entry =
    serialisation_records_.find(serial_number);
  if (entry == serialisation_records_.end())
  {
    vcl_cerr << "vsl_b_istream::set_serialisation_other_data():\n"
             << "  No such value " << serial_number << "in records.\n";
    vcl_abort();
  }
  return (*entry).second.second;
}


//: destructor.so that it can be overloaded
vsl_b_ifstream::~vsl_b_ifstream()
{
  if (is_) delete is_;
}

//: Close the stream
void vsl_b_ifstream::close()
{
  assert(is_ != 0);
  ((vcl_ifstream *)is_)->close();
  clear_serialisation_records();
}
