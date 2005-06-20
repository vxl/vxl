//:
// \file
// \author Kevin de Souza
// \date 05-Aug-2004
// \brief Container for tabulated data suitable for reading/writing to delimited text files


#include <mbl/mbl_table.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>


// Tolerance used to determine whether table entries are equal
static double tolerance_ = 1e-15;

// Whether the tolerance is applied as a fractional difference
static bool fractional_tolerance_ = false;

// Level of verbosity used for error output.
static int verbosity_ = 0;


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
  for (unsigned int c=0; c<ncols; ++c)
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
    int nrows=0;
    // If there are at least 1 column, assume all columns are the same as the first
    if (columns_.size()>0) nrows=columns_[0].size();
    return nrows;
}


//========================================================================
// Returns true if column exists
//========================================================================
bool mbl_table::column_exists(const vcl_string& header)
{
    // Does the map contain this header?
    vcl_map<vcl_string, unsigned>::const_iterator iter =
        header_to_column_index_.find(header);

    return  (iter != header_to_column_index_.end());
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
  else
  {
    vcl_cerr << "ERROR: mbl_table::get_column(): column \""
             << header << "\" does not exist in the table.\n";
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
    unsigned int ncols = num_cols();
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
             << r << " does not exist in the table.\n";
    return false;
  }
}


//========================================================================
// Set the value of an existing element.
//========================================================================
bool mbl_table::set_element(const vcl_string& header,
                            const unsigned r,
                            const double value)
{
  bool success = false;

  // Does the map contain this column header?
  vcl_map<vcl_string, unsigned>::const_iterator iter =
    header_to_column_index_.find(header);

  if (iter != header_to_column_index_.end())
  {
    // Does the column have sufficient rows?
    vcl_vector<double>& col = columns_[iter->second];
    if (col.size()>r)
    {
      // Set the value
      col[r] = value;
      success = true;
    }
    else
    {
      vcl_cerr << "ERROR: mbl_table::set_element(): row "
               << r << " does not exist in the table.\n";
    }
  }
  else
  {
    vcl_cerr << "ERROR: mbl_table::set_element(): column \""
             << header << "\" does not exist in the table.\n";
  }

  return success;
}


//========================================================================
// Get the value of an existing element.
//========================================================================
double mbl_table::get_element(const vcl_string& header,
                              const unsigned r,
                              bool* success/*=0*/) const
{
  double value = 1e-19;
  if (success)
    *success = false;

  // Does the map contain this column header?
  vcl_map<vcl_string, unsigned>::const_iterator iter =
    header_to_column_index_.find(header);

  if (iter != header_to_column_index_.end())
  {
    // Does the column have sufficient rows?
    const vcl_vector<double>& col = columns_[iter->second];
    if (col.size()>r)
    {
      // Get the value
      value = col[r];
      if (success)
        *success = true;
    }
    else
    {
      vcl_cerr << "ERROR: mbl_table::get_element(): row "
               << r << " does not exist in the table.\n";
    }
  }
  else
  {
    vcl_cerr << "ERROR: mbl_table::get_element(): column \""
             << header << "\" does not exist in the table.\n";
  }

  return value;
}


//========================================================================
// Append a column of data with its own heading.
//========================================================================
bool mbl_table::append_column(const vcl_string& header,
                              const vcl_vector<double>& column)
{
  // Check whether there is already a column with this heading
  if (header_to_column_index_.find(header) == header_to_column_index_.end())
  {
    // Check that the length of the new column matches the existing columns
    if (num_rows()==0 || num_rows()==column.size())
    {
      column_headers_.push_back(header);
      columns_.push_back(column);
      header_to_column_index_[header] = columns_.size()-1;
      return true;
    }
    else
    {
      vcl_cerr << "ERROR: mbl_table::append_column(): "
               << "new column is different length from existing columns.\n"
               << "Column not appended.\n";
      return false;
    }
  }
  else
  {
    vcl_cerr << "ERROR: mbl_table::append_column(): a column with header \""
             << header << "\" already exists.\n"
             << "Column not appended.\n";
    return false;
  }
}


//========================================================================
//: Append an empty column with its own heading.
//========================================================================
bool mbl_table::append_column(const vcl_string& header,
                              const double val/*=0*/)
{
  // Check whether there is already a column with this heading
  if (header_to_column_index_.find(header) == header_to_column_index_.end())
  {
    // Append a new column of the same length as existing columns
    column_headers_.push_back(header);
    unsigned c = columns_.size();
    header_to_column_index_[header] = c;
    columns_.push_back(vcl_vector<double>());
    columns_[c].resize(num_rows(), val);
    return true;
  }
  else
  {
    vcl_cerr << "ERROR: mbl_table::append_column(): a column with header \""
             << header << "\" already exists.\n"
             << "Column not appended.\n";
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
             << "Row not appended.\n";
    return false;
  }
}


//========================================================================
// Append an empty row.
//========================================================================
bool mbl_table::append_row(const double val/*=0*/)
{
  // Append a new element to each column
  unsigned ncols = num_cols();
  for (unsigned c=0; c<ncols; ++c)
  {
    columns_[c].push_back(val);
  }

  return true;
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
//: Create a new table with as subset of columnsdefined by headers
//========================================================================
bool mbl_table::subtable(mbl_table &new_table,  const vcl_vector<vcl_string> &headers) const
{
    bool ret = true;

    new_table = mbl_table();


    // Write column headers row
    for (unsigned c=0; c<headers.size(); ++c)
    {
        // get the column for the header if available
        vcl_map<vcl_string, unsigned>::const_iterator iter =
            header_to_column_index_.find(headers[c]);

        if (iter != header_to_column_index_.end())
        {
            new_table.append_column(headers[c],columns_[iter->second]);
        }
        else
        {
            ret=false;
        }
    }

    return ret;
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
  // Are both tables the same object - do they occupy the same memory location?
  if (this == &rhs)
  {
    if (verbosity_>0)
      vcl_cout << "Both tables are actually the same memory object!" << vcl_endl;
    return true;
  }

  // Is the delimiter the same?
  if (delimiter_ != rhs.delimiter_)
  {
    if (verbosity_>0)
      vcl_cout << "Tables have different delimiter characters" << vcl_endl;
    return false;
  }

  // Is the column headers vector the same?
  if (column_headers_ != rhs.column_headers_)
  {
    if (verbosity_>0)
      vcl_cout << "Tables have different column headers" << vcl_endl;
    return false;
  }

  // Is the header to index map the same?
  if (header_to_column_index_ != rhs.header_to_column_index_)
  {
    if (verbosity_>0)
      vcl_cout << "Tables have different header-to-column index map" << vcl_endl;
    return false;
  }

  // Are the numbers of columns the same?
  unsigned ncols = columns_.size();
  if (ncols != rhs.columns_.size())
  {
    if (verbosity_>0)
      vcl_cout << "Tables have different number of columns" << vcl_endl;
    return false;
  }

  // Is the table data the same (within the current tolerance)?
  bool values_different = false;
  for (unsigned c=0; c<ncols; ++c)
  {
    // Are the numbers of rows in this column the same?
    unsigned nrows = columns_[c].size();
    if (nrows != rhs.columns_[c].size())
    {
      if (verbosity_>0)
        vcl_cout << "Tables have different number of elements in some columns"
                 << vcl_endl;
      return false;
    }

    // Compare all data values in this column
    for (unsigned r=0; r<nrows; ++r)
    {
      double diff = columns_[c][r] - rhs.columns_[c][r];
      if (fractional_tolerance_)
      {
        diff /= columns_[c][r];
      }
      if (vcl_fabs(diff) > tolerance_)
      {
        if (verbosity_>0)
          vcl_cout << "Tables have different values in column " << c
                   << " (" << column_headers_[c] << "), row " << r
                   << ":  " << columns_[c][r] << ",  "
                   << rhs.columns_[c][r]
                   << "  (diff=" << diff << ") "
                   << vcl_endl;

        if (verbosity_<=1)
          return false;     // Don't bother checking any more elements
        else
          values_different = true; // Proceed to check all elements
      }
    }
  }
  if (values_different) return false;

  // Passed all tests, table is identical to this one.
  return true;
}


//========================================================================
// Is another table different from this one?
//========================================================================
bool mbl_table::operator!=(const mbl_table& rhs) const
{
  return !(*this==rhs);
}


//========================================================================
// Set the tolerance used to determine whether table entries are equal.
//========================================================================
void mbl_table::set_tolerance(const double& tol, 
                              const bool& fract/*=false*/)
{
  tolerance_ = tol;
  fractional_tolerance_ = fract;
}


//========================================================================
// Set the level of verbosity used for error output.
//========================================================================
void mbl_table::set_verbosity(const int& v)
{
  verbosity_ = v;
}
