// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_HEADER_H
#define VIL_NITF2_HEADER_H

#include "vil_nitf2_tagged_record.h"
#include "vil_nitf2_classification.h"

class vil_stream;


/// Parses a NITF 2.1 file header for vil_nitf2_image. 
/// Use get_property() to get a field value.
//
class vil_nitf2_header 
{
public:
  vil_nitf2_header();

  virtual ~vil_nitf2_header();
  
  // Read the image header starting at stream's current position. Return success.
  virtual bool read( vil_stream* stream );
  //virtual bool write( vil_stream* stream );

  // Sets out_value to the value of field specified by tag. 
  // Returns 0 if such a field is not found or is of the wrong type.
  template< class T >
  bool get_property(vcl_string tag, T& out_value) const
  {
    if ( m_field_sequence_classification && m_field_sequence_classification->get_value(tag, out_value) ) return true;
    else if ( m_field_sequence2 && m_field_sequence2->get_value(tag, out_value) ) return true;
    else return m_field_sequence1.get_value(tag, out_value);
  }

  // Sets out_value to the value of vcl_vector field element specified by tag and index.
  // Returns 0 if such a field is not found or is of the wrong type.
  template< class T >
  bool get_property(vcl_string tag, int i, T& out_value) const
  {
    if ( m_field_sequence_classification && m_field_sequence_classification->get_value(tag, i, out_value) ) return true;
    else if ( m_field_sequence2 && m_field_sequence2->get_value(tag, i, out_value) ) return true;
    else return m_field_sequence1.get_value(tag, i, out_value);
  }

  vil_nitf2_classification::file_version file_version() const;

protected:
  vil_nitf2_field_sequence m_field_sequence1;
  vil_nitf2_field_sequence* m_field_sequence2;
  vil_nitf2_field_sequence* m_field_sequence_classification;
  static vil_nitf2_field_definitions* get_field_definitions_1();
  static vil_nitf2_field_definitions* get_field_definitions_2( vil_nitf2_classification::file_version version );
private:
  static vil_nitf2_field_definitions* s_field_definitions_1;
  static vil_nitf2_field_definitions* s_field_definitions_21;
  static vil_nitf2_field_definitions* s_field_definitions_20;
};


#endif // VIL_NITF2_HEADER_H
