#ifndef mbl_table_h
#define mbl_table_h
//:
// \file
// \author Kevin de Souza
// \date 05-Aug-2004
// \brief Container for tabulated data suitable for reading/writing to delimited text files

#include <map>
#include <iostream>
#include <iosfwd>
#include <vector>
#include <string>
#include <vcl_compiler.h>

//: Container for tabulated data suitable for reading/writing to delimited text files.
//
// Table comprises a number of columns of double-precision data.
// Each column is headed by a text string (which may contain whitespace but not end-of-line).
// Column headers must be unique.
// Columns are separated by a delimiter character (e.g. "\t").
// The delimiter character must not occur in any of the entries!
// The delimiter character is optional at the end of a row.
// Each row must be terminated by an end-of-line (including the last row).
// Each column of data is stored as a vector of doubles and associated with the column header string.
class mbl_table
{
public:

  //: Constructor
  // \param delim The delimiter character.
  mbl_table(const char delim='\t');


  //: Constructor
  // \param delim The delimiter character.
  // \param headers The column headers (in order).
  mbl_table(const char delim,
            const std::vector<std::string>& headers);


  //: Return the number of columns
  unsigned num_cols() const;


  //: Return the number of rows
  unsigned num_rows() const;


  //: Get the column of data corresponding to a particular heading.
  // \param header String identifying the desired column.
  // \return true if there is a column with the specified heading.
  // \retval column A vector containing the values of the requested column.
  bool get_column(const std::string& header,
                  std::vector<double>& column) const;


  //: Returns true if column exists
  bool column_exists(const std::string& header) const;


  //: Get a specified row of data.
  // \param r Index of the desired row.
  // \return true if there is a row with the specified index.
  // \retval row A vector containing the values of the requested row.
  bool get_row(const unsigned& r,
               std::vector<double>& row) const;


  //: Get the list of column headers (in column order).
  // \retval headers The list of column headers.
  void get_column_headers(std::vector<std::string>& headers) const;


  //: Append a column of data with its own heading.
  // \param header String identifying the column.
  // \param column A vector containing the values of the column.
  // \return true If the column was added.
  // \note The new column must be the same length as existing columns.
  bool append_column(const std::string& header,
                     const std::vector<double>& column);


  //: Append an empty column with its own heading.
  // \param header String identifying the column.
  // \param val Default value to initialize all elements of the new column.
  // \return true If the column was added.
  // \note The new column will be the same length as existing columns.
  bool append_column(const std::string& header,
                     const double val=0);


  //: Append a row of data.
  // \return true if the row was added.
  // \param row A vector containing the values of the new row.
  // \note The new row must be the same length as existing rows.
  bool append_row(const std::vector<double>& row);


  //: Append an empty row.
  // \return true If the row was added.
  // \param val Default value to initialize all elements of the new row.
  // \note The new row will be the same length as existing rows.
  bool append_row(const double val=0);


  //: Set the value of an existing element.
  // \param header The string identifying the column to be modified.
  // \param r The row index of the element to be modified.
  // \param value The new value to use for the modified element.
  // \return False if the column does not exist or the row index is not valid.
  // \note This function is intended only for existing elements.
  bool set_element(const std::string& header,
                   const unsigned r,
                   const double value);


  //: Get the value of an existing element.
  // \param header The string identifying the column of interest.
  // \param r The row index of the element of interest.
  // \return The value of the requested element (undefined if the element
  //         does not exist).
  // \retval success If provided, will be used to indicate whether the
  //                 specified element existed.
  // \note This function is intended only for existing elements.
  // \note It is recommended that you provide and check the parameter success.
  double get_element(const std::string& header,
                     const unsigned r,
                     bool* success=0) const;


  //: Load this table's data from specified text stream.
  // Any existing data is lost.
  // \return true if table was read successfully from the stream.
  bool read(std::istream& is);


  //: Save this table's data to specified text stream.
  void write(std::ostream& os) const;


  //: Create a new table of subset of columns defined by headers
  // \retval new_table a subtable
  // \param headers subset of column headers
  // \return true if all columns in headers existed in "this" and were copied
  bool subtable(mbl_table &new_table,  const std::vector<std::string> &headers) const;


  //: Is another table identical to this one?
  // \note The normal behaviour of this function is to return false as soon
  // as a discrepancy is found. However, if verbosity>=2 this function will
  // compare all corresponding elements (providing that the table dimensions match).
  bool operator==(const mbl_table& rhs) const;


  //: Is another table different from this one?
  // \sa operator==()
  bool operator!=(const mbl_table& rhs) const;


  //: Set the tolerance used to determine whether table entries are equal.
  // \param tol The tolerance should be a small positive number, eg 1e-19.
  // \param frac Whether the tolerance is applied as a fractional difference.
  static void set_tolerance(const double& tol,
                            const bool& fract=false);


  //: Set the level of verbosity used for error output.
  // \param v The verbosity should be a small integer, eg -3 to 3.
  // Larger values cause more detailed output.
  // Default level is 0.
  // \note Currently only levels 1 and 2 are defined: level 1 provides some
  // feedback for equality testing; level 2 also causes the equality operator
  // to check all corresponding elements of a table.
  static void set_verbosity(const int& v);


protected:

  //: Read a series of characters from the stream until a delimiter character or eol.
  // \param is The input stream.
  // \return true if a non-empty string was successfully read.
  // \retval str The string which was read.
  // \retval eol Whether the stream is at end-of-line after reading the string.
  // \retval eof Whether the stream is at end-of-file after reading the string.
  bool read_delimited_string(std::istream& is,
                             std::string& str,
                             bool& eol,
                             bool& eof);

  //: The character delimiting each column.
  char delimiter_;

  //: The column headers (in order)
  std::vector<std::string> column_headers_;

  //: Map a column header string to column index
  std::map<std::string, unsigned> header_to_column_index_;

  //: The table data, arranged as column vectors of double data.
  std::vector<std::vector<double> > columns_;

};

#endif // mbl_table_h
