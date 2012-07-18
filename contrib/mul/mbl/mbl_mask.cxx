// This is mul/mbl/mbl_mask.cxx

//:
// \file
// \author Barry Skellern
// \brief Class representing a binary mask, and related functions

#include "mbl_mask.h"
#include <vcl_set.h>
#include <vcl_map.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vul/vul_string.h>
#include <mbl/mbl_exception.h>



    //: Given a collection of indices, produce a collection of masks that isolate each indexed set
    //    The input index set does not need to be zero based or continuous
    //    The output vector of masks is sorted such that
    //    for example: (1,4,2,1,2) will make three masks: (1,0,0,1,0), (0,0,1,0,1) and (0,1,0,0,0)
    //    which correspond to the sorted index sets 1,2,4
void mbl_masks_from_index_set(const vcl_vector<unsigned> & indices,
                              vcl_vector<mbl_mask> & masks)
{
  masks.clear();
  unsigned n = indices.size(), n_masks = 0;
  vcl_set<unsigned> used_indices;
  vcl_map<unsigned, unsigned> ordering;

  for (unsigned i = 0 ; i < n ; ++i)
    used_indices.insert(indices[i]);

  for (vcl_set<unsigned>::const_iterator it = used_indices.begin(),
                                         end = used_indices.end();
                                         it != end; ++it)
  {
    ordering[*it] = n_masks++;
    masks.push_back(mbl_mask(n));
  }

  for (unsigned i = 0 ; i < n ; ++i)
    masks[ordering[indices[i]]][i] = true;
}


    //: Replace 'true' values in B with values taken from A. size of A must match 'true' count in B
void mbl_mask_on_mask(const mbl_mask & A, mbl_mask & B)
{
  unsigned nA = A.size();
  unsigned nB = 0;
  for (unsigned i = 0 ; i < B.size() ; ++i) nB += B[i];
  if (nA != nB)
    throw vcl_out_of_range("mbl_mask: Length of A mismatch with number of true elements of B");

  for (unsigned i = 0, j = 0 ; i < B.size() ; ++i)
    if (B[i]) B[i] = A[j++];
}


    //: Apply an "AND" (rule 0001) logical operation between two masks
void mbl_mask_logic_and(const mbl_mask & A, mbl_mask & B)
{
  mbl_mask_logic(A, B, "0001");
}

//: Apply an "OR" (rule 0111) logical operation between two masks
void mbl_mask_logic_or(const mbl_mask & A, mbl_mask & B)
{
  mbl_mask_logic(A, B, "0111");
}

    //: Apply an "XOR" (rule 0110) logical operation between two masks
void mbl_mask_logic_xor(const mbl_mask & A, mbl_mask & B)
{
  mbl_mask_logic(A, B, "0110");
}

    //: Apply a "NOR" (rule 1000) logical operation between two masks
void mbl_mask_logic_nor(const mbl_mask & A, mbl_mask & B)
{
  mbl_mask_logic(A, B, "1000");
}

    //: Apply an "XNOR" (rule 1001) logical operation between two masks
void mbl_mask_logic_xnor(const mbl_mask & A, mbl_mask & B)
{
  mbl_mask_logic(A, B, "1001");
}

    //: Apply an "NAND" (rule 1110) logical operation between two masks
void mbl_mask_logic_nand(const mbl_mask & A, mbl_mask & B)
{
  mbl_mask_logic(A, B, "1110");
}


    //: Apply a general logical operation between two masks
void mbl_mask_logic(const mbl_mask & A, mbl_mask & B, const vcl_string & operation)
{
  if (A.size() != B.size())
    throw vcl_out_of_range("mbl_mask_logic: Mask lengths differ");

  // Validate the operation to perform and parse into vector

  if (operation.length() != 4)
    throw vcl_length_error("mbl_mask_logic: Operation must be of length 4");
  vcl_vector<bool> op_rule(4);
  for (unsigned i = 0 ; i < 4 ; ++i)
  {
    if (operation[i] == '0') op_rule[i] = false;
    else if (operation[i] == '1') op_rule[i] = true;
    else throw vcl_invalid_argument("mbl_mask_logic: Invalid character in operation string - must contain only '0' or '1'");
  }

  // Apply the operation in place
  for (unsigned i = 0 ; i < A.size() ; ++i)
    B[i] = op_rule[2*A[i] + B[i]]; // consider AB as 2bit binary, converted to decimal index into rule
}


    //: Save to file
void mbl_save_mask(const mbl_mask & mask, vcl_ostream & stream)
{
  vcl_vector<bool>::const_iterator it = mask.begin();
  const vcl_vector<bool>::const_iterator & end = mask.end();
  for (; it != end; ++it)
    stream << *it << vcl_endl;
}

    //: Save to file
void mbl_save_mask(const mbl_mask & mask, const char * filename)
{
  vcl_ofstream stream(filename);
  if (!stream)
    mbl_exception_throw_os_error(filename);
  mbl_save_mask(mask, stream);
}

    //: Save to file
void mbl_save_mask(const mbl_mask & mask, const vcl_string &filename)
{
  vcl_ofstream stream(filename.c_str());
  if (!stream)
    mbl_exception_throw_os_error(filename);
  mbl_save_mask(mask, stream);
}

    //: Load from file
void mbl_load_mask(mbl_mask & mask, vcl_istream & stream)
{
  mask.clear();
  vcl_string line;
  while (stream.good())
  {
    char c='X';
    stream >> vcl_ws >> c;
    if (stream.eof()) break;
    if (c == '0') mask.push_back(false);
    else if (c == '1') mask.push_back(true);
    else
    {
      mask.clear();
      throw mbl_exception_parse_file_error(vcl_string("Unable to parse mask value " +
        vul_string_escape_ctrl_chars(vcl_string(1,c)) ), "" );
    }
  }
}

    //: Load from file
void mbl_load_mask(mbl_mask & mask, const char * filename)
{
  vcl_ifstream stream(filename);
  if (!stream)
    mbl_exception_throw_os_error(filename);
  try
  {
    mbl_load_mask(mask, stream);
  }
  catch (mbl_exception_parse_file_error & e)
  {
    throw mbl_exception_parse_file_error(e.what(), filename);
  }
}

    //: Load from file
void mbl_load_mask(mbl_mask & mask, const vcl_string &filename)
{
  mbl_load_mask( mask, filename.c_str() );
}


//: Convert a mask to a list of indices.
void mbl_mask_to_indices(const mbl_mask& mask, vcl_vector<unsigned>& inds)
{
  inds.clear();
  for (unsigned i=0,n=mask.size(); i<n; ++i)
  {
    if (mask[i]) inds.push_back(i);
  }
}


//: Convert a list of indices to a mask.
void mbl_indices_to_mask(const vcl_vector<unsigned>& inds,
                         const unsigned n,
                         mbl_mask& mask)
{
  mask.resize(n, false);
  for (unsigned i=0, m=inds.size(); i<m; ++i)
  {
    mask[inds[i]]=true;
  }
}




