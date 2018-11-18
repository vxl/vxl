#include <iostream>
#include <sstream>
#include "mipa_ms_block_normaliser.h"
//:
// \file
// \brief Class to independently normalise sub-blocks with a region at several (SIFT-like) scales
// \author Martin Roberts

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/vnl_vector_ref.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_exception.h>

//=======================================================================
//: Apply transform to texture vector
void mipa_ms_block_normaliser::normalise(vnl_vector<double>& v) const
{
    //Apply the base class normalisation to each scale in turn.
    //It is assumed that the vector (v's) memory layout is lowest scale, then ordered by scale
    // There may be an overall histogram left over at the end

    double* pDataForScale=v.data_block();

    unsigned nTotal=0;
    unsigned ni_region_low=ni_region_;
    unsigned nj_region_low=nj_region_;
    unsigned nc_per_block_low=nc_per_block_;
    auto& ni_region=const_cast<unsigned&>(this->ni_region_);
    auto& nj_region=const_cast<unsigned&>(this->nj_region_);
    auto& nc_per_block=const_cast<unsigned&>(this->nc_per_block_);
    for (unsigned iscale=0;iscale<nscales_;++iscale)
    {
        //Do each scale
        unsigned nForScale=ni_region_ * nj_region_ * nA_;
        vnl_vector_ref<double> vscale(nForScale,pDataForScale);
        //Normalise this scale's sub-vector
        mipa_block_normaliser::normalise(vscale);

        //Increment sub-vector pointer and reduce number of elements for next scale
        pDataForScale += nForScale;
        ni_region/=2;
        nj_region/=2;
        nc_per_block/=2;
        nTotal += nForScale;
    }
    if (include_overall_histogram_ && (nTotal+nA_) <= v.size())
    {
        vnl_vector_ref<double> vOverall(nA_,pDataForScale);
        //Normalise the overall histogram
        this->normaliser().normalise(vOverall);
    }
    //Restore region cell count to that of lowest scale
    ni_region = ni_region_low;
    nj_region = nj_region_low;
    nc_per_block = nc_per_block_low;
}

//=======================================================================

std::string mipa_ms_block_normaliser::is_a() const
{
    return std::string("mipa_ms_block_normaliser");
}

//=======================================================================

mipa_vector_normaliser* mipa_ms_block_normaliser::clone() const
{
    return new mipa_ms_block_normaliser(*this);
}

//=======================================================================

// required if data is present in this base class
void mipa_ms_block_normaliser::print_summary(std::ostream& os) const
{
    os<<"mipa_ms_block_normaliser:\n"
      <<"\tnscales:\t"<<nscales_
      <<"\tinclude_overall_histogram:\t"<<include_overall_histogram_
      <<'\n';
    mipa_block_normaliser::print_summary(os);
}

//=======================================================================

// required if data is present in this base class
void mipa_ms_block_normaliser::b_write(vsl_b_ostream& bfs) const
{
    constexpr short version_no = 1;
    vsl_b_write(bfs, version_no);

    vsl_b_write(bfs, nscales_);
    vsl_b_write(bfs, include_overall_histogram_);
    mipa_block_normaliser::b_write(bfs);
}

//=======================================================================

// required if data is present in this base class
void mipa_ms_block_normaliser::b_read(vsl_b_istream& bfs)
{
    if (!bfs) return;

    short version;
    vsl_b_read(bfs,version);
    switch (version)
    {
        case (1):
        {
            vsl_b_read(bfs, nscales_);
            vsl_b_read(bfs, include_overall_histogram_);
            mipa_block_normaliser::b_read(bfs);
        break;
        default:
            std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, mipa_ms_block_normaliser&)\n"
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
//   nscales: 2
//   include_overall_histogram: true
//   normaliser: mipa_l2norm_vector_normaliser
//   ni: 16
//   nj: 16
//   nc_per_block: 4
// }
// \endverbatim

void mipa_ms_block_normaliser::config_from_stream(
    std::istream &is, const mbl_read_props_type &extra_props)
{
    std::string s = mbl_parse_block(is);


    std::istringstream ss(s);
    mbl_read_props_type props = mbl_read_props(ss);

    props = mbl_read_props_merge(props, extra_props, true);

    if (!(props["nscales"].empty()))
    {
        std::string sfi=props["nscales"];
        std::istringstream ssInner(sfi);
        int n=0;
        ssInner>>n;
        if (ssInner.bad() || n<=0)
        {
            mbl_exception_parse_error x(std::string("mipa_ms_block_normaliser - string stream read error of nscales property"));
            mbl_exception_error(x);
        }

        nscales_ =n;
    }
    props.erase("nscales");

    if (!(props["include_overall_histogram"].empty()))
    {
        std::string strBool=props["include_overall_histogram"];
        if (strBool=="true" || strBool=="TRUE" || strBool=="t" || strBool=="T" )
        {
            include_overall_histogram_ = true;
        }
        else
        {
            include_overall_histogram_ = false;
        }
    }
    props.erase("include_overall_histogram");

    std::string dummy;
    std::istringstream ssDummy(dummy);
    mipa_block_normaliser::config_from_stream(ssDummy,props);
}
