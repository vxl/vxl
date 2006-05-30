// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_DES_H
#define VIL_NITF2_DES_H

class vil_nitf2_field_definitions;
class vil_stream;
#include "vil_nitf2_classification.h"
#include "vil_nitf2_field_sequence.h"

#include <vcl_map.h>

// Class for representing a single data extension segment (DES) in 
// a NITF 2.x file.  This class can handle TRE overflow DES (DESID=TRE_OVERFLOW)
// as well as custom defined data extension segments (DESID=<something else>).  Use
// the static define() function to tell this class about custom DES' that you want
// to support.
// It does not,however, handle streaming DES' (DESID=STREAMING_FILE_HEADER).  I'm not
// sure that those des' are relevant to the nitf files that vil supports
class vil_nitf2_des 
{
public:
  vil_nitf2_des( vil_nitf2_classification::file_version version, int data_width );
  ///read the des starting at stream's current position
  ///returns false if failed
  virtual bool read( vil_stream* stream );
  //virtual bool write( vil_stream* stream );

  virtual ~vil_nitf2_des();

  // Sets out_value to the value of field specified by tag. 
  // Returns 0 if such a field is not found or is of the wrong type.
  template< class T >
  bool get_property(vcl_string tag, T& out_value) const
  {
    if ( ! m_field_sequence1->get_value( tag, out_value ) && m_field_sequence2 ){
      return m_field_sequence2->get_value( tag, out_value );
    }
  }

  // Sets out_value to the value of vcl_vector field element specified by tag and index.
  // Returns 0 if such a field is not found or is of the wrong type.
  template< class T >
  bool get_property(vcl_string tag, int i, T& out_value) const
  {
    if( ! m_field_sequence1->get_value( tag, i, out_value ) && m_field_sequence2 ){
      return m_field_sequence2->get_value( tag, i, out_value );
    }
  }

  // I allocate the return value, but you own it after I return it to you
  // so you need to delete it.  
  virtual vil_nitf2_field::field_tree* get_tree( int i = 0 ) const;

  // Call this function to register a DES with this class.  Once you've 
  // done this, then this class will be able to parse your custom DES.
  static vil_nitf2_field_definitions& define( vcl_string desId );
protected:
  typedef vcl_map<vcl_string, vil_nitf2_field_definitions*> 
    field_definition_map;
  static field_definition_map & all_definitions();

  static void add_shared_field_defs_1( vil_nitf2_field_definitions* defs );
  static void add_shared_field_defs_2( vil_nitf2_field_definitions* defs, int data_width );
  static vil_nitf2_field_definitions* create_field_definitions( vil_nitf2_classification::file_version ver, int data_width);

  vil_nitf2_field_sequence* m_field_sequence1;
  vil_nitf2_field_sequence* m_field_sequence2;
};

#endif //VIL_NITF2_DES_H
