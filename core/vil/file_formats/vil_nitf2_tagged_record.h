// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_TAGGED_RECORD_H
#define VIL_NITF2_TAGGED_RECORD_H

#include <list>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <set>
#include <iostream>
#include <vcl_sstream.h>
#include <vil/vil_stream.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
#include <vcl_istream.h>
#include <vcl_ostream.h>
#include <vcl_cassert.h>

#include "vil_nitf2_field_functor.h"

// Forward declarations
class vil_nitf2_field_formatter;
class vil_nitf2_tagged_record;
class vil_nitf2_tagged_record_definition;
class vil_nitf2_integer_formatter;
class vil_nitf2_string_formatter;
class vil_nitf2_index_vector;
class vil_nitf2_scalar_field;

/* Overview of NITF tagged record code
  
This code is used to define and read National Imagery Transmission
Format (NITF) tagged record extensions (TREs), also known as 
controlled extensions. It is intended to eventually support 
writing and verifying TREs also.  

A TRE is stored as a sequence of ASCII fields, each of a vcl_fixed length,
that are appended together without any tag names or delimiters. Some
fields' existence depends on the values of previous fields; other
fields are repeated by a value of a previous field. The TRE definition
code allows the programmer to specify such conditions, and the NITF
file reader evaluates such conditions as it parses the file.

The classes that represent TRE definitions were designed so that a
programmer can succinctly define a TRE in a format similar to the
tables that appear in the NITF documentation. This was felt to be
important goal to help reduce errors, since NITF TREs can contain
hundred of fields, and many users will need to define their own TRE's
depending on their application and environment.

Here an example that implements part of the definition of TRE "MTIRPB" 
from NIMA STDI-0002 version 2.1:

vil_nitf2_tagged_record_definition::define("MTIRPB", fields()

  .field("MTI_DP",           "Destination Point",   NITF_INT(2))
  .field("DATIME",           "Scan Date & Time",    NITF_DAT(14))
  .field("ACFT_LOC",         "Aircraft Position",   NITF_LOC(21)) 
  .field("SQUINT_ANGLE",     "Squint Angle",        NITF_DBL(6, 2, true),  true)
  .field("NO_VALID_TARGETS", "Number of Targets",   NITF_INT(3))

  .repeat("NO_VALID_TARGETS", fields()

    .field("TGT_n_SPEED",    "Target Ground Speed", NITF_INT(4),  true)
    .field("TGT_n_CAT",      "Target Classification Category",
           NITF_ENUM(1, vil_nitf2_enum_values()
             .value("H", "Helicopter")
             .value("T", "Tracked")
             .value("U", "Unknown")
             .value("W", "Wheeled")),
            true))

  .field("TEST_POS_COND",    "Test True Condition", NITF_STR_BCSA(14),  false,
         NitfGreaterThan<int>("MTI_DP", 1))
  .end();

A TaggedRecordDefintion consists of the TRE's name (here, "MTIRPB") 
and an ordered list of FieldDefinitions. Each field definition is 
specified by these arguments to method "field" above: tag, 
pretty_name, format; followed by optional arguments: value_optional, 
repeat_count, condition, units, description.

In this example the first field is a two-digit integer; the second a
is a date-time structure, specified to 14 "digits" precision
(YYYYMMDDHHMMSS); the next field is a location specified as 21
characters, stored either as a latitude-longitude pair of decimal
degrees or deg/min/sec/hemisphere (depending on the TRE, various
formats are allowed, sometimes with variable precision). The next
field, SQUINT_ANGLE, is a vcl_fixed-point decimal number stored using 6
characters, including 2 decimal places and a sign. The "true"
following the empty "units" argument indicates that field
SQUINT_ANGLE's value is optional, i.e., the field may be blank. Unless
a value is defined as optional, the parser will issue a warning in
such cases (although it will continue parsing the rest of the TRE if
possible).

The next field, NO_VALID_TARGETS, is an integer field. This is followed
by a repeating integer field, TGT_n_SPEED; the last argument of its
definition indicates that TGT_n_SPEED is repeated by the value of field
NO_VALID_TARGETS. The code here represents TGT_n_SPEED as a single field
whose value is a vcl_vector of integer pointers.  The reason for using
pointers is in case some of the field values are blank; for such entries,
the corresponding vcl_vector element is null.

The next field, TGT_n_CAT, is a repeating enumerated string field, whose
valid values are "H", "T", "U", "W", and blank.

The final field, TEST_POS_COND, is a conditional field. It exists if
and only if its condition is satisifed, in this case, if the value of
field MTI_DP is greater than 1. Conditional fields along with
repeating fields therefore cause instances of a TRE to vary in length.

That summarizes how TRE's are defined. Some possible extensions in the
future include value range checks and the ability to specify which formats 
among a set of alternatives (such as geolocation) are allowed.

When a TRE is defined, it is added to a dictionary of known TREs that
is used to read (and eventually to write) NITF files. As a NITF file 
is read in, at the start of a TRE the tag and length of the TRE are read. 
If the tag is recognized, its data is parsed into field using tag 
definition, creating a TaggedRecord. If not, the tag's data is skipped 
over (according to its declared length) and the next tag is processed.
  
Values of NITF fields are passed back to the application as built-in
types and as NITF structure types. Built-in types include integer,
double (used to represent all vcl_fixed-point decimal fields), string, and
character. NITF structure types include date_time and Location.

Here is an overview of the class hierarchy:

  vil_nitf2_tagged_record_definition: defines a TRE, consisting of a name and a
  list of vil_nitf2_field_definition.

  vil_nitf2_field_definition: defines a field; includes a vil_nitf2_field_formatter.

  vil_nitf2_field_formatter and its derived classes vil_nitf2_typed_field_formatter<T>:
  define the type and format of a field; used to read and write a field.

  vil_nitf2_tagged_record: an instance of a tagged record. Points to a
  TaggedRecordDefinition and owns a list of vil_nitf2_field.

  vil_nitf2_field: an instance of a field. Contains a pointer to a
  vil_nitf2_field_definition. See derived classes for its value.

  vil_nitf2_typed_scalar_field<T>: derived from vil_nitf2_field to represent a scalar
  value of type T (where T=integer, double, vcl_string, char,
  vil_nitf2_date_time, or vil_nitf2_location*)

  vil_nitf2_typed_array_field<T*>: derived from vil_nitf2_field to represent a
  repeating field of type T.

  vil_nitf2_field_functor<T>: A function object base class used to derive
  functions that compute values of tags from a TRE for the purpose
  of representing conditions and repeat counts.
*/

//-----------------------------------------------------------------------------
// vil_nitf2_tagged_record is an instance of a single NITF tagged record extension,
// consisting of a list of vil_nitf2_fields, as read from (or to be written to) a
// NITF file.
//
class vil_nitf2_tagged_record 
{
public:
  // Return record identifier
  vcl_string name() const;

  // Return record length in bytes
  int length() const { return m_length; }

  //bool validate(vil_nitf2_ostream& = vcl_cerr);

  // Attempts to read a NITF tagged record from input stream
  static vil_nitf2_tagged_record* create(vil_nitf2_istream& input);

  // Attempts to write a NITF tagged record to the input stream
  virtual bool write(vil_nitf2_ostream&);

  // Implements operator <<
  vcl_ostream& output(vcl_ostream&) const;
 
  // Sets out_value to the value of the integer-value field specified by tag. 
  // Returns 0 if such a field is not found.
  bool get_value(vcl_string tag, long long& out_value) const;
  bool get_value(vcl_string tag, int& out_value) const;
  bool get_value(vcl_string tag, double& out_value) const;
  bool get_value(vcl_string tag, char& out_value) const;
  bool get_value(vcl_string tag, void*& out_value) const;
  bool get_value(vcl_string tag, vcl_string& out_value) const;
  bool get_value(vcl_string tag, vil_nitf2_location*& out_value) const;
  bool get_value(vcl_string tag, vil_nitf2_date_time& out_value) const;
  
  //bool get_value(vcl_string tag, int i, long long& out_value) const;
  //bool get_value(vcl_string tag, int i, int& out_value) const;
  //bool get_value(vcl_string tag, int i, double& out_value) const;
  //bool get_value(vcl_string tag, int i, char& out_value) const;
  //bool get_value(vcl_string tag, int i, void*& out_value) const;
  //bool get_value(vcl_string tag, int i, vcl_string& out_value) const;
  //bool get_value(vcl_string tag, int i, vil_nitf2_location*& out_value) const;
  //bool get_value(vcl_string tag, int i, vil_nitf2_date_time& out_value) const;

  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, long long& out_value) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, int& out_value) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, double& out_value) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, char& out_value) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, void*& out_value) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, vcl_string& out_value) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, vil_nitf2_location*& out_value) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, vil_nitf2_date_time& out_value) const;

  // Sets the value of the integer-valued field specified by tag to value.
  // Not yet implemented.
  //bool set_value(vcl_string tag, int value) {
  //  return m_field_sequence->set_value(tag, value); }

  // Returns a field with specified tag, or 0 if not found.
  vil_nitf2_field* get_field(vcl_string tag) const {
    return m_field_sequence->get_field(tag); }

  // Removes a field with specified tag, returning whether successful.
  // Use this method to "undefine" an existing field.
  // Not yet implemented.
  //bool remove_field(vcl_string tag) {
  //  return m_field_sequence->remove_field(tag); }

  // Test method. Prints results. Returns true if no errors.
  static bool test();
 
  // Destructor
  virtual ~vil_nitf2_tagged_record();

private:
  // Default constructor
  vil_nitf2_tagged_record();

  // Reads tagged record members from input stream
  bool read(vil_nitf2_istream& input);

  // Static variables
  static vil_nitf2_field_definition* s_length_definition;
  static vil_nitf2_field_definition* s_tag_definition;
  static vil_nitf2_integer_formatter* s_length_formatter;
  static vil_nitf2_string_formatter* s_tag_formatter;

  // Member variables
  vil_nitf2_scalar_field* m_length_field;
  vil_nitf2_scalar_field* m_tag_field;
  int m_length;
  vil_nitf2_tagged_record_definition* m_definition;
  vil_nitf2_field_sequence* m_field_sequence;
};

vcl_ostream& operator << (vcl_ostream& os, const vil_nitf2_tagged_record& record);

#endif // VIL_NITF2_TAGGED_RECORD_H
