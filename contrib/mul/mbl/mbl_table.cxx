//:
// \file
// \author Kevin de Souza
// \date 05-Aug-2004
// \brief Container for tabulated data suitable for reading/writing to delimited text files


#include "mbl_table.h"
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>


//========================================================================
// Constructor
//========================================================================
mbl_table::mbl_table(const char delim)
  : delimiter_(delim)
{
}


//========================================================================
//: Constructor
//========================================================================
mbl_table::mbl_table(const char delim,
                     const vcl_vector<vcl_string>& headers)
  : delimiter_(delim),
    column_headers_(headers)
{
  unsigned ncols = column_headers_.size();
  
  // Allocate column vectors
  columns_.resize(ncols);

  // Map each column header string to column index
  for (int c=0; c<ncols; ++c)
  {
    header_to_column_index_[headers[c]] = c;
  }
}


//========================================================================
// Return the number of columns
//========================================================================
unsigned mbl_table::num_cols() const
{
  return columns_.size();
}


//========================================================================
// Return the number of rows
//========================================================================
unsigned mbl_table::num_rows() const
{
  // Assume all columns are the same as the first
  return columns_[0].size();
}


//========================================================================
// Get the column of data corresponding to a particular heading.
//========================================================================
bool mbl_table::get_column(const vcl_string& header,
                           vcl_vector<double>& column) const
{
  bool success = false;
  column.clear();

  // Does the map contain this header?
  vcl_map<vcl_string, unsigned>::const_iterator iter =
    header_to_column_index_.find(header);

  if (iter != header_to_column_index_.end())
  {
    // Get the corresponding column vector and copy it to the output vector
    column = columns_[iter->second];
    success = true;
  }

  return success;
}


//========================================================================
// Get a specified row of data.
//========================================================================
bool mbl_table::get_row(const unsigned& r,
                        vcl_vector<double>& row) const
{
  // Clear output data
  row.clear();
  
  // Check that the specified row index is valid
  if (r < num_rows())
  {
    int ncols = num_cols();
    row.resize(ncols);
    for (unsigned c=0; c<ncols; ++c)
    {
      row[c] = columns_[c][r];
    }
    return true;
  }
  else
  {
    vcl_cerr << "ERROR: mbl_table::get_row(): row "
      << r << " does not exist in the table."
      << vcl_endl;
    return false;
  }
}


//========================================================================
// Append a column of data with its own heading.
//========================================================================
bool mbl_table::append_column(const vcl_string& header,
                              const vcl_vector<double>& column)
{
  // Check whether there is already a column with this heading  
  if (header_to_column_index_.find(header) != header_to_column_index_.end())
  {
    // Check that the length of the new column matches the existing columns
    if (num_rows()==column.size())
    {
      column_headers_.push_back(header);
      columns_.push_back(column);
      header_to_column_index_[header] = columns_.size();
      return true;
    }
    else
    {
      vcl_cerr << "ERROR: mbl_table::append_column(): "
        << "new column is different length from existing columns.\n"
        << "Column not appended."
        << vcl_endl;
      return false;
    }
  }
  else
  {
    vcl_cerr << "ERROR: mbl_table::append_column(): a column with header "
      << header << " already exists.\n"
      << "Column not appended."
      << vcl_endl;
    return false;
  }
}


//========================================================================
// Append a row of data.
//========================================================================
bool mbl_table::append_row(const vcl_vector<double>& row)
{
  // Check that the length of the new row matches the existing rows
  unsigned ncols = num_cols();
  if (ncols==row.size())
  {
    for (unsigned c=0; c<ncols; ++c)
    {
      columns_[c].push_back(row[c]);
    }
    return true;
  }
  else
  {
    vcl_cerr << "ERROR: mbl_table::append_row(): "
      << "new row is different length from existing row.\n"
      << "Row not appended."
      << vcl_endl;
    return false;
  }
}


//========================================================================
// Load this table's data from specified text stream.
//========================================================================
bool mbl_table::read(vcl_istream& is)
{
  bool success = false;

  if (is.good() && !is.eof())
  {
    // Read header row
    bool eol = false;
    bool eof = false;
    unsigned col = 0;
    while (!eol && !eof)
    {
      vcl_string str;
      if (read_delimited_string(is, str, eol, eof))
      {
        // Create an empty column vector and enter it into the map
        column_headers_.push_back(str);
        header_to_column_index_[str] = col;
        columns_.push_back(vcl_vector<double>(0));

        col++;
      }
    }

    // Read table data
    while (!is.eof())
    {
      // Read one row
      eol = false;
      eof = false;
      unsigned col = 0;

      while (!eol && !eof)
      {
        vcl_string str;
        if (read_delimited_string(is, str, eol, eof))
        {
          // Convert string to double (NB sets to 0 if string is non-numeric)
          double val = vcl_atof(str.c_str());

          // Add this double value to the current column vector
          columns_[col].push_back(val);

          // Advance to the next column or back to the first column
          col++;
          if (col==num_cols()) col = 0;
        }
      }
    }

    success = true;
  }

  return success;
}


//========================================================================
// Save this table's data to specified text stream.
//========================================================================
void mbl_table::write(vcl_ostream& os) const
{
  // How many columns are there?
  unsigned int ncols = num_cols();

  // How many rows of data do we expect?
  unsigned int nrows = num_rows();

  // Write column headers row
  for (unsigned c=0; c<ncols; ++c)
  {
    os << column_headers_[c] << delimiter_;
  }
  os << vcl_endl;

  // Write data rows
  for (unsigned r=0; r<nrows; ++r)
  {
    for (unsigned c=0; c<ncols; ++c)
    {
      os << columns_[c][r] << delimiter_;
    }
    os << vcl_endl;
  }
}


//========================================================================
// Read a series of characters from the stream until a delimiter character or eol.
//========================================================================
bool mbl_table::read_delimited_string(vcl_istream& is,
                                      vcl_string& str,
                                      bool& eol,
                                      bool& eof)
{
  str = "";
  eol = false;
  bool success = false;

  char c = 0;
  bool eos = false;
  while (!eos && is.good() && !is.eof())
  {
    is.get(c);
    if (c=='\n')
    {
      eol = true;
      eos = true;
    }
    else if (c==delimiter_)
    {
      eos = true;
    }
    else if (c==0)  // We sometimes get this at end-of-file
    {
      eof = true;
    }
    else
    {
      str += c;
    }
  }

  if (eos && str.length()>0)
  {
    success = true;
  }

  return success;
}


//========================================================================
// Is another table identical to this one?
//========================================================================
bool mbl_table::operator==(const mbl_table& rhs) const
{
  // Is the delimiter the same?
  if (delimiter_ != rhs.delimiter_) return false;

  // Is the column headers vector the same?
  if (column_headers_ != rhs.column_headers_) return false;

  // Is the header to index map the same?
  if (header_to_column_index_ != rhs.header_to_column_index_) return false;

  // Is the table data the same?
  if (columns_ != rhs.columns_) return false;

  // Passed all tests, table is identical to this one.
  return true;
}

