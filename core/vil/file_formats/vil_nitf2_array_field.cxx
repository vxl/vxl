// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include <cstddef>
#include <sstream>
#include <cstdlib>
#include "vil_nitf2_array_field.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_index_vector.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_stream_core.h>

int vil_nitf2_array_field::num_dimensions() const
{
  return m_num_dimensions;
}

int vil_nitf2_array_field::
next_dimension(const vil_nitf2_index_vector& index) const
{
  auto dimension_bounds_entry = m_dimensions_map.find(index);
  if (dimension_bounds_entry != m_dimensions_map.end()) {
    return dimension_bounds_entry->second;
  } else {
    return 0;
  }
}

void vil_nitf2_array_field::
set_next_dimension(const vil_nitf2_index_vector& index, int bound)
{
  if ((int)index.size() >= m_num_dimensions) {
    std::cerr << "vil_nitf2_array_field::set_next_dimension"
             << index << ": invalid partial index!\n";
    return;
  }
  if (next_dimension(index) > 0) {
    std::cerr << "vil_nitf2_array_field::set_next_dimension"
             << index << ": bound previously set!\n";
  }
  m_dimensions_map[index] = bound;
}

bool vil_nitf2_array_field::
check_index(const vil_nitf2_index_vector& indexes) const
{
  if ((int)indexes.size() != m_num_dimensions) {
    std::cerr << "index length does not match value dimensions!\n";
    return false;
  }
  // Remove the last element from index and look it up in the dimensions map.
  // Then check if the removed element is less than the returned dimension bound.
  // If no dimension bound is found, then one of the other index values is
  // invalid.
  vil_nitf2_index_vector dimension_index;
  for (int dim=0; dim < m_num_dimensions-1; ++dim) {
    dimension_index.push_back(indexes[dim]);
  }
  int dimension_bound = next_dimension(dimension_index);
  int last_index = indexes[indexes.size()-1];
  if (last_index < dimension_bound) {
    return true;
  } else {
    std::cerr << "Tag " << tag() << indexes << ": index out of bounds!\n";
    return false;
  }
}

std::string int_to_string( int i )
{
  std::stringstream s;
  s << i;
  return s.str();
}

std::string index_string( const vil_nitf2_index_vector& indices )
{
  std::string ret_val = "";
  for ( unsigned int i = 0 ; i < indices.size() ; i++ ){
    ret_val += "[" + int_to_string( indices[i] ) + "]";
  }
  return ret_val;
}

std::string vil_nitf2_array_field::get_value_string(const vil_nitf2_index_vector& in_indices) const
{
  auto* str = new vil_stream_core;
  write_vector_element( *str, in_indices, -1 );
  vil_streampos num_to_read = str->tell();
  str->seek( 0 );
  char* buffer;
  buffer = (char*)std::malloc( (std::size_t) num_to_read+1 );
  str->read( (void*)buffer, num_to_read );
  buffer[(std::size_t) num_to_read] = 0;
  return std::string( buffer );
}

void vil_nitf2_array_field::do_dimension( const vil_nitf2_index_vector& in_indices,
                                          vil_nitf2_field::field_tree* inTree ) const
{
  int dim = next_dimension( in_indices );
  for ( int i = 0 ; i < dim ; i++ )
  {
    //this is the index list we're dealing with in this loop
    vil_nitf2_index_vector curr_indices = in_indices;
    curr_indices.push_back( i );
    //create our tree node and add it to inTree's child list
    auto* tr = new vil_nitf2_field::field_tree;
    std::string tag_str = tag();
    std::string index_str = index_string( curr_indices );
    std::string p_name;
    if ( index_str == "" ) p_name = pretty_name();
    else p_name = "";
    tr->columns.push_back( tag_str + index_str );
    tr->columns.push_back( p_name );
    if( check_index( curr_indices ) ) {
      tr->columns.push_back( get_value_string( curr_indices ) );
    }
    inTree->children.push_back( tr );
    //recursive call
    do_dimension( curr_indices, tr );
  }
}

vil_nitf2_field::field_tree* vil_nitf2_array_field::get_tree() const
{
  field_tree* tr = vil_nitf2_field::get_tree();
  do_dimension( vil_nitf2_index_vector(), tr );
  return tr;
}
