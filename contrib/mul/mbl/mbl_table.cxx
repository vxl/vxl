//:
// \file
// \author Kevin de Souza
// \date 05-Aug-2004
// \brief Container for tabulated data suitable for reading/writing to delimited text files

#include "mbl_table.h"
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>

//==================================================================================
//! Constructor
//==================================================================================
mbl_table::mbl_table(const char delim)
  : delimiter_(delim)
{
}


//==================================================================================
//! Get the column of data corresponding to a particular heading.
//! \param header String identifying the desired column.
//! \return true if there is a column with the specified heading.
//! \retval values A vector containing the values of the requested column.
//==================================================================================
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


//==================================================================================
//! Load this table's data from specified text stream.
//! Any existing data is lost.
//! \return true if table was read successfully from the stream.
//==================================================================================
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
          if (col==columns_.size())
            col = 0;
        }
      }
    }

    success = true;
  }

  return success;
}


//==================================================================================
//! Save this table's data to specified text stream.
//==================================================================================
void mbl_table::write(vcl_ostream& os) const
{
  // How many columns are there?
  unsigned int ncols = columns_.size();

  // How many rows of data do we expect? Assume all columns are the same as the first.
  unsigned int nrows = columns_[0].size();

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


//==================================================================================
//! Read a series of characters from the stream until a delimiter character or eol.
//! \return true if a non-empty string was successfully read.
//==================================================================================
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


//==================================================================================
//! Is another table identical to this one?
//==================================================================================
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

