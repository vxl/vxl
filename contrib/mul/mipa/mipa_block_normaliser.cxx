#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstddef>
#include "mipa_block_normaliser.h"
//:
// \file
// \brief Class to independently normalise sub-blocks with a region
// \author Martin Roberts

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/vnl_vector.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_exception.h>

//=======================================================================
//: Apply transform to texture vector
void mipa_block_normaliser::normalise(vnl_vector<double>& v) const
{
    //Assume layout is cell values (e.g. histogram), the columns most contiguous, then rows.

    //Note any leftover regions not spanned by an integral number of blocks remain unnormalised!!!
    //e.g. a 17x17 region with 4x4 blocks has the final row and column not normalised

    unsigned nblockCols=ni_region_/nc_per_block_; //Number of blocks across region
    unsigned nblockRows=nj_region_/nc_per_block_; //Number of blocks down region

    unsigned nTargetColEls=nA_*nc_per_block_; //Number of elements in one row of a block
    unsigned nblockEls=nTargetColEls*nc_per_block_;  //Total number of elements  a block
    //Working vector for a contiguous memory single block

    vnl_vector<double> vblock(nblockEls,0.0);
    std::ptrdiff_t colStep=nA_; //Columns are contiguous
    std::ptrdiff_t rowStep=nA_*ni_region_;

    for (unsigned jbRows=0;jbRows<nblockRows;++jbRows) //move down rows of region in block chunks
    {
        for (unsigned ibCols=0;ibCols<nblockCols;++ibCols) //move down rows of regionc_per_block_n in block chunks
        {
            {
                unsigned rowNum=jbRows*nc_per_block_;
                unsigned colNum=ibCols*nc_per_block_;
                double* pBlockTopLeft=v.data_block() + rowNum*rowStep + colNum*colStep; //Source
                double* pTarget=vblock.data_block();
                for (unsigned j=0;j<nc_per_block_;++j) //rows of this target block
                {
                    //Copy all elements across the columns of the block
                    std::copy(pBlockTopLeft,pBlockTopLeft+nTargetColEls,pTarget);
                    pBlockTopLeft+= rowStep; //Next row of block source
                    pTarget+= nTargetColEls; //Next row of block target
                }
                normaliser_->normalise(vblock);
            }
            //Now copy back into input vector
            {
                unsigned rowNum=jbRows*nc_per_block_;
                unsigned colNum=ibCols*nc_per_block_;
                double* pBlockTopLeft=v.data_block() + rowNum*rowStep + colNum*colStep; //Source
                double* pTarget=vblock.data_block();
                for (unsigned j=0;j<nc_per_block_;++j) //rows of this target block
                {
                    //Copy all elements across the columns of the block
                    std::copy(pTarget,pTarget+nTargetColEls,pBlockTopLeft);
                    pBlockTopLeft+= rowStep; //Next row of block in overall normalised vector
                    pTarget+= nTargetColEls; //Next row of block
                }
            }
        }
    }
}

//=======================================================================

std::string mipa_block_normaliser::is_a() const
{
    return std::string("mipa_block_normaliser");
}

//=======================================================================

mipa_vector_normaliser* mipa_block_normaliser::clone() const
{
    return new mipa_block_normaliser(*this);
}

//=======================================================================

    // required if data is present in this base class
void mipa_block_normaliser::print_summary(std::ostream& os) const
{
    os<<"mipa_block_normaliser:\n"
      <<"\tni_region_\t"<<ni_region_
      <<"\tnj_region_\t"<<nj_region_
      <<"\tnA_\t"<<nA_
      <<"\tnc_per_block_\t"<<nc_per_block_
      <<'\n'
      <<"Block normaliser summary is:"<<'\n';
    normaliser_->print_summary(os);
}

//=======================================================================

// required if data is present in this base class
void mipa_block_normaliser::b_write(vsl_b_ostream& bfs) const
{
    constexpr short version_no = 1;
    vsl_b_write(bfs, version_no);

    vsl_b_write(bfs, ni_region_);
    vsl_b_write(bfs, nj_region_);
    vsl_b_write(bfs, nA_);
    vsl_b_write(bfs, nc_per_block_);

    vsl_b_write(bfs, normaliser_);
}

//=======================================================================

// required if data is present in this base class
void mipa_block_normaliser::b_read(vsl_b_istream& bfs)
{
    if (!bfs) return;

    short version;
    vsl_b_read(bfs,version);
    switch (version)
    {
        case (1):
        {
            vsl_b_read(bfs, ni_region_);
            vsl_b_read(bfs, nj_region_);
            vsl_b_read(bfs, nA_);
            vsl_b_read(bfs, nc_per_block_);

            vsl_b_read(bfs,normaliser_);
            break;
        default:
            std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, mipa_block_normaliser&)\n"
                     << "           Unknown version number "<< version << '\n';
            bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
            return;
        }
    }
}

//=======================================================================
//: Initialise from a text stream.
// The next non-ws character in the stream should be a '{'
// syntax
// \verbatim
// {
//   normaliser: mipa_l2norm_vector_normaliser
//   ni: 16
//   nj: 16
//   nc_per_block: 4
// }
// \endverbatim

void mipa_block_normaliser::config_from_stream(
    std::istream &is, const mbl_read_props_type &extra_props)
{
    std::string s = mbl_parse_block(is);

    std::istringstream ss(s);
    mbl_read_props_type props = mbl_read_props(ss);

    props = mbl_read_props_merge(props, extra_props, true);

    if (props["normaliser"].empty())
    {
        mbl_exception_parse_error x(std::string("mipa_block_normaliser could not find a normaliser property"));
        mbl_exception_error(x);
    }
    std::istringstream ss2(props["normaliser"]);
    std::unique_ptr<mipa_vector_normaliser> norm = new_normaliser_from_stream(ss2, extra_props);
    normaliser_=norm.release();
    props.erase("normaliser");

    {
        if (props["ni"].empty())
        {
            mbl_exception_parse_error x(std::string("mipa_block_normaliser could not find a ni  property"));
            mbl_exception_error(x);
        }

        //Decode the feature indices using a string stream
        std::string sfi=props["ni"];
//    std::istringstream ssInner(sfi.substr(1,sfi.size()-2));
        std::istringstream ssInner(sfi);
        int n=0;
        ssInner>>n;
        if (ssInner.bad() || n<=0)
        {
            mbl_exception_parse_error x(std::string("mipa_block_normaliser - string stream read error of ni property"));
            mbl_exception_error(x);
        }
        ni_region_ =n;

        props.erase("ni");
    }

    {
        if (props["nj"].empty())
        {
            mbl_exception_parse_error x(std::string("mipa_block_normaliser could not find a nj  property"));
            mbl_exception_error(x);
        }

        //Decode the feature indices using a string stream
        std::string sfi=props["nj"];
//    std::istringstream ssInner(sfi.substr(1,sfi.size()-2));
        std::istringstream ssInner(sfi);
        int n=0;
        ssInner>>n;
        if (ssInner.bad() || n<=0)
        {
            mbl_exception_parse_error x(std::string("mipa_block_normaliser - string stream read error of nj property"));
            mbl_exception_error(x);
        }
        nj_region_ =n;

        props.erase("nj");
    }
    {
        if (props["nA"].empty())
        {
            mbl_exception_parse_error x(std::string("mipa_block_normaliser could not find a nA  property"));
            mbl_exception_error(x);
        }

        std::string sfi=props["nA"];
        std::istringstream ssInner(sfi);
        int n=0;
        ssInner>>n;
        if (ssInner.bad() || n<=0)
        {
            mbl_exception_parse_error x(std::string("mipa_block_normaliser - string stream read error of nj property"));
            mbl_exception_error(x);
        }
        nA_ =n;
        props.erase("nA");
    }
    {
        if (props["block_size"].empty())
        {
            mbl_exception_parse_error x(std::string("mipa_block_normaliser could not find a block_size  property"));
            mbl_exception_error(x);
        }

        std::string sfi=props["block_size"];
        std::istringstream ssInner(sfi);
        int n=0;
        ssInner>>n;
        if (ssInner.bad() || n<=0)
        {
            mbl_exception_parse_error x(std::string("mipa_block_normaliser - string stream read error of block_size property"));
            mbl_exception_error(x);
        }
        nc_per_block_ =n;

        props.erase("block_size");
    }

    mbl_read_props_look_for_unused_props(
            "mipa_block_normaliser::config_from_stream", props, extra_props);
}
