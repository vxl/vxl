// This is tbl/vipl/section/vipl_section_iterator.h
#ifndef vipl_section_iterator_h_
#define vipl_section_iterator_h_

template < class DataType > class vipl_section_descriptor; //template forward reference
template < class DataType > class vipl_section_container; //template forward reference

template < class DataType >
class vipl_section_iterator
{
  friend class vipl_section_container< DataType > ; //declare a friend class
  friend class vipl_section_descriptor< DataType > ; //declare a friend class
 protected:
  vipl_section_descriptor< DataType >* hsreal_descriptor;
  vipl_section_container< DataType >* hscontainer;
  int hsincr_count;
  // Assigns the pointer directly. Does not deep copy them.
  vipl_section_iterator(
       vipl_section_descriptor< DataType >* desc ,
       vipl_section_container< DataType >* container);
  // Deep copies the pointers.
  vipl_section_iterator(
       vipl_section_descriptor< DataType >* desc ,
       vipl_section_container< DataType >* container ,
       int t);
 public:
  virtual ~vipl_section_iterator(); //low_level destructor
  vipl_section_iterator(); //low_level c++ constructor
  //user accessed low_level c++ copy constructor
  vipl_section_iterator(const vipl_section_iterator< DataType > &);
  // begin method list for class section_iterator
  // prefix
  virtual vipl_section_iterator< DataType >& operator++(); // postfix
  virtual vipl_section_iterator< DataType > operator++( int a);
  // Get the descriptor for the current section
  virtual vipl_section_descriptor< DataType > operator*();

  //The equality test is, if this and p have the same address TRUE;
  //if this and p both have null descriptors TRUE if this XOR p
  //have null descriptor FALSE if this and p have linked containers
  //which are equal, AND the iterator's ++ counts are the same,
  //TRUE; otherwise FALSE
  virtual bool operator==(const vipl_section_iterator< DataType >& p) const;
  virtual bool operator!=( const vipl_section_iterator< DataType >& p) const;
  virtual vipl_section_iterator< DataType >& operator=( const vipl_section_iterator< DataType >& p) ;
  vipl_section_descriptor< DataType >* real_descriptor() const { return hsreal_descriptor; }
  vipl_section_descriptor< DataType >* & ref_real_descriptor() { return hsreal_descriptor; }
  void put_real_descriptor( vipl_section_descriptor< DataType >* v) { hsreal_descriptor = v; }
  vipl_section_container< DataType >* container() const { return hscontainer; }
  vipl_section_container< DataType >* & ref_container() { return hscontainer; }
  void put_container( vipl_section_container< DataType >* v) { hscontainer = v; }
  int incr_count() const { return hsincr_count; }
  int & ref_incr_count() { return hsincr_count; }
  void put_incr_count( int v) { hsincr_count = v; }

}; // end of class definition

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_section_iterator.hxx"
#endif

#endif // vipl_section_iterator_h_
