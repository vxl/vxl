#ifndef mbl_table_h
#define mbl_table_h
//:
// \file
// \author Kevin de Souza
// \date 05-Aug-2004
// \brief Container for tabulated data suitable for reading/writing to delimited text files

#include <vcl_map.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_string.h>

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

  //! Constructor
  //! \param delim The delimiter character.
  mbl_table(const char delim);

  //! Get the column of data corresponding to a particular heading.
  //! \param header String identifying the desired column.
  //! \return true if there is a column with the specified heading.
  //! \retval column A vector containing the values of the requested column.
  bool get_column(const vcl_string& header,
                  vcl_vector<double>& column) const;

  //! Load this table's data from specified text stream.
  //! Any existing data is lost.
  //! \return true if table was read successfully from the stream.
  bool read(vcl_istream& is);

  //! Save this table's data to specified text stream.
  void write(vcl_ostream& os) const;

  //! Is another table identical to this one?
  bool operator==(const mbl_table& rhs) const;

protected:

  //! Read a series of characters from the stream until a delimiter character or eol.
  //! \param is The input stream.
  //! \return true if a non-empty string was successfully read.
  //! \retval str The string which was read.
  //! \retval eol Whether the stream is at end-of-line after reading the string.
  //! \retval eof Whether the stream is at end-of-file after reading the string.
  bool read_delimited_string(vcl_istream& is,
                             vcl_string& str,
                             bool& eol,
                             bool& eof);

  //! The character delimiting each column.
  char delimiter_;

  //! The column headers (in order)
  vcl_vector<vcl_string> column_headers_;

  //! Map a column header string to column index
  vcl_map<vcl_string, unsigned> header_to_column_index_;

  //! The table data, arranged as column vectors of double data.
  vcl_vector<vcl_vector<double> > columns_;
};

#endif // mbl_table_h
