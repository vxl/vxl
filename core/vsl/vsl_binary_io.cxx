// This is core/vsl/vsl_binary_io.cxx
#include <cstddef>
#include <map>
#include <cstdlib>
#include "vsl_binary_io.h"
//:
// \file
// \brief Functions to perform consistent binary IO within vsl
// \author Tim Cootes and Ian Scott

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_explicit_io.h>

template <typename TYPE>
void  local_vsl_b_write(vsl_b_ostream& os, const TYPE n)
{
  const size_t MAX_INT_BUFFER_LENGTH = VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(TYPE));
  unsigned char buf[ MAX_INT_BUFFER_LENGTH ] = {0};
  const auto nbytes = (std::size_t)vsl_convert_to_arbitrary_length(&n, buf);
  os.os().write((char*)buf, nbytes );
}

template <typename TYPE>
void local_vsl_b_read(vsl_b_istream &is,TYPE & n)
{
  const size_t MAX_INT_BUFFER_LENGTH = VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(TYPE));
  unsigned char buf[MAX_INT_BUFFER_LENGTH] = {0};
  unsigned char *ptr=buf;
  do
  {
    vsl_b_read(is, *ptr);
    const std::ptrdiff_t ptr_offset_from_begin = ptr-buf;
    if (ptr_offset_from_begin >= (std::ptrdiff_t)MAX_INT_BUFFER_LENGTH)
    {
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream &, int& )\n"
               << "           Integer too big. Likely cause either file corruption, or\n"
               << "           file was created on platform with larger integer sizes.\n"
               << "ptr_offset_from_begin: " << ptr_offset_from_begin << " >= " << MAX_INT_BUFFER_LENGTH << std::endl;
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      n = 0; //If failure occurs, then set n=0 for number of reads.
      return;
    }
  }
  while (!(*(ptr++) & 128));
  vsl_convert_from_arbitrary_length(buf, &n);
}

#define MACRO_MAKE_INTEGER_READ_WRITE( TYPEIN ) \
void vsl_b_write(vsl_b_ostream& os,TYPEIN n )   \
{                                               \
  local_vsl_b_write<TYPEIN>(os,n);              \
}                                               \
                                                \
void vsl_b_read(vsl_b_istream &is,TYPEIN& n )   \
{                                               \
  local_vsl_b_read<TYPEIN>(is,n);               \
}

MACRO_MAKE_INTEGER_READ_WRITE(int);
MACRO_MAKE_INTEGER_READ_WRITE(unsigned int);
MACRO_MAKE_INTEGER_READ_WRITE(short);
MACRO_MAKE_INTEGER_READ_WRITE(unsigned short);
MACRO_MAKE_INTEGER_READ_WRITE(long);
MACRO_MAKE_INTEGER_READ_WRITE(unsigned long);
#if VXL_INT_64_IS_LONGLONG
MACRO_MAKE_INTEGER_READ_WRITE(vxl_int_64);
MACRO_MAKE_INTEGER_READ_WRITE(vxl_uint_64);
#endif
#if 0
MACRO_MAKE_INTEGER_READ_WRITE(std::ptrdiff_t);
// When the macro is ready, this test will be
// #if ! VCL_SIZE_T_IS_A_STANDARD_TYPE
MACRO_MAKE_INTEGER_READ_WRITE(std::size_t);
// #endif
#endif

#undef MACRO_MAKE_INTEGER_READ_WRITE

void vsl_b_write(vsl_b_ostream& os, char n )
{
  os.os().write( reinterpret_cast<char *>(&n), sizeof( n ) );
}

void vsl_b_read(vsl_b_istream &is, char& n )
{
  const int value = is.is().get();
  n = static_cast<signed char>(value);
}

void vsl_b_write(vsl_b_ostream& os, signed char n )
{
  os.os().write( reinterpret_cast<char *>(&n), sizeof( n ) );
}

void vsl_b_read(vsl_b_istream &is, signed char& n )
{
  const int value = is.is().get();
  n = static_cast<signed char>(value);
}


void vsl_b_write(vsl_b_ostream& os,unsigned char n )
{
  os.os().write( reinterpret_cast<char *>(&n), 1 );
}

void vsl_b_read(vsl_b_istream &is,unsigned char& n )
{
  const int value = is.is().get();
  n = static_cast<unsigned char>(value);
}


void vsl_b_write(vsl_b_ostream& os, const std::string& str )
{
    std::string::const_iterator          it;

    vsl_b_write(os,(short)str.length());
    for ( it = str.begin(); it != str.end(); ++it )
        vsl_b_write(os,*it);
}

void vsl_b_read(vsl_b_istream &is, std::string& str )
{
    std::string::iterator                it;
    std::string::size_type               length;

    vsl_b_read(is,length);
    str.resize( length );
    for ( it = str.begin(); it != str.end(); ++it )
        vsl_b_read(is,*it);
}

// deprecated in favour of std::string version.
void vsl_b_write(vsl_b_ostream& os,const char *s )
{
  int i = -1;
  do {
     i++;
     vsl_b_write(os,s[i]);
  } while ( s[i] != 0 );
}

// deprecated in favour of std::string version.
// note You must preallocate enough space at \p s for expected length of string.
// This function is easy to crash mith a malformed data file.
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

void vsl_b_write(vsl_b_ostream& os,float n )
{
  vsl_swap_bytes(reinterpret_cast<char *>(&n), sizeof( n ) );
  os.os().write( reinterpret_cast<char *>(&n), sizeof( n ) );
}

void vsl_b_read(vsl_b_istream &is,float& n )
{
  is.is().read( reinterpret_cast<char *>(&n), sizeof( n ) );
  vsl_swap_bytes(reinterpret_cast<char *>(&n), sizeof( n ) );
}

void vsl_b_write(vsl_b_ostream& os,double n )
{
  vsl_swap_bytes(reinterpret_cast<char *>(&n), sizeof( n ) );
  os.os().write( reinterpret_cast<char *>(&n), sizeof( n ) );
}

void vsl_b_read(vsl_b_istream &is,double& n )
{
  is.is().read( reinterpret_cast<char *>(&n), sizeof( n ) );
  vsl_swap_bytes(reinterpret_cast<char *>(&n), sizeof( n ) );
}


static const unsigned short vsl_magic_number_part_1=0x2c4e;
static const unsigned short vsl_magic_number_part_2=0x472b;


//: Create this adaptor using an existing stream
// The stream (os) must be open (i.e. ready to be written to) so that the
// IO version number can be written by this constructor.
// User is responsible for deleting os after deleting the adaptor
vsl_b_ostream::vsl_b_ostream(std::ostream *o_s): os_(o_s)
{
  assert(os_ != nullptr);
  vsl_b_write_uint_16(*this, version_no_);
  vsl_b_write_uint_16(*this, vsl_magic_number_part_1);
  vsl_b_write_uint_16(*this, vsl_magic_number_part_2);
}

//: A reference to the adaptor's stream
std::ostream& vsl_b_ostream::os() const
{
  assert(os_ != nullptr);
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
  assert(pointer != nullptr);
  assert(serialisation_records_.find(pointer) == serialisation_records_.end());
  unsigned long id = (unsigned long)serialisation_records_.size() + 1;
  serialisation_records_[pointer] = std::make_pair(id, other_data);
  return id;
}


//: Returns a unique identifier for the object.
// Returns 0 if there is no record of the object.
unsigned long vsl_b_ostream::get_serial_number(void *pointer) const
{
  auto entry =
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
  auto entry =
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
  auto entry =
    serialisation_records_.find(pointer);
  if (entry == serialisation_records_.end())
  {
    std::cerr << "vsl_b_ostream::set_serialisation_other_data():\n"
             << "No such value " << pointer << "in records.\n";
    std::abort();
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
  assert(os_ != nullptr);
  ((std::ofstream *)os_)->close();
  clear_serialisation_records();
}

//: Create this adaptor using an existing stream.
// The stream (is) must be open (i.e. ready to be read from) so that the
// IO version number can be read by this constructor.
// User is responsible for deleting is after deleting the adaptor
vsl_b_istream::vsl_b_istream(std::istream *i_s): is_(i_s)
{
  assert(is_ != nullptr);
  if (!(*is_)) return;
  unsigned long v=0, m1=0, m2=0;
  vsl_b_read_uint_16(*this, v);
  vsl_b_read_uint_16(*this, m1);
  vsl_b_read_uint_16(*this, m2);

  // If this test fails, either the file is missing, or it is not a
  // Binary VXL file, or it is a corrupted Binary VXL file
  if (m2 != vsl_magic_number_part_2 || m1 != vsl_magic_number_part_1)
  {
    std::cerr << "\nI/O ERROR: vsl_b_istream::vsl_b_istream(std::istream *is)\n"
             <<   "           The input stream does not appear to be a binary VXL stream.\n"
             <<   "           Can't find correct magic number.\n";
    is_->clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }

  if (v != 1)
  {
    std::cerr << "\nI/O ERROR: vsl_b_istream::vsl_b_istream(std::istream *is)\n"
             << "             The stream's leading version number is "
             << v << ". Expected value 1.\n";
    is_->clear(std::ios::badbit); // Set an unrecoverable IO error on stream
  }
  version_no_ = (unsigned short)v;
}

//: A reference to the adaptor's stream
std::istream & vsl_b_istream::is() const
{
  assert(is_ != nullptr);
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
  assert(pointer != nullptr);
  assert(serialisation_records_.find(serial_number) == serialisation_records_.end());
  serialisation_records_[serial_number] = std::make_pair(pointer, other_data);
}

//: Returns the pointer to the object identified by the unique serial number.
// Returns 0 if no record has been added.
void* vsl_b_istream::get_serialisation_pointer(unsigned long serial_number) const
{
  auto entry =
        serialisation_records_.find(serial_number);
  if (entry == serialisation_records_.end())
  {
    return nullptr;
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
  auto entry =
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
    std::cerr << "vsl_b_istream::set_serialisation_other_data():\n"
             << "  No such value " << serial_number << "in records.\n";
    std::abort();
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
  assert(is_ != nullptr);
  ((std::ifstream *)is_)->close();
  clear_serialisation_records();
}



//: Test to see if a stream really is a binary vsl file.
// \return false if we can't find magic numbers and correct version number.
// The file pointer is reset to the beginning on leaving this function.
bool vsl_b_istream_test(std::istream &is)
{
  if (!is) return false;
  is.seekg(0);
  unsigned long v=0, m1=0, m2=0;

//  vsl_b_read_uint_16(is, v);
//  vsl_b_read_uint_16(is, m1);
//  vsl_b_read_uint_16(is, m2);

  is.read( ( char* )&v, 2 );
  vsl_swap_bytes(( char* )&v, sizeof(long) );
  is.read( ( char* )&m1, 2 );
  vsl_swap_bytes(( char* )&m1, sizeof(long) );
  is.read( ( char* )&m2, 2 );
  vsl_swap_bytes(( char* )&m2, sizeof(long) );

  is.seekg(0);

  if (!is || m2 != vsl_magic_number_part_2 || m1 != vsl_magic_number_part_1 || v>1)
    return false;


  return true;
}
