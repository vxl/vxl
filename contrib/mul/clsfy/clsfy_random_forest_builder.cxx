// This is mul/clsfy/clsfy_random_forest_builder.cxx
//:
// \file
// \brief Implement a random_forest classifier builder
// \author Martin Roberts

#include <iostream>
#include <string>
#include <algorithm>
#include <numeric>
#include <iterator>
#include "clsfy_random_forest_builder.h"
#include <vxl_config.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_stl.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <clsfy/clsfy_binary_tree_builder.h>
#include "clsfy_random_forest.h"

//=======================================================================

clsfy_random_forest_builder::clsfy_random_forest_builder()
  : ntrees_(100),
    max_depth_(-1), min_node_size_(-1),
    poob_indices_(nullptr),
    calc_test_error_(true)
{
    unsigned long default_seed=123654987;
    seed_sampler(default_seed);
}

clsfy_random_forest_builder::clsfy_random_forest_builder(unsigned ntrees,
                                                         int max_depth,
                                                         int min_node_size)
  : ntrees_(ntrees),
    max_depth_(max_depth), min_node_size_(min_node_size),
    poob_indices_(nullptr),
    calc_test_error_(true)
{
    unsigned long default_seed=123654987;
    seed_sampler(default_seed);
}

clsfy_random_forest_builder::~clsfy_random_forest_builder() = default;
//=======================================================================

short clsfy_random_forest_builder::version_no() const
{
    return 1;
}

//=======================================================================

std::string clsfy_random_forest_builder::is_a() const
{
    return std::string("clsfy_random_forest_builder");
}

//=======================================================================

bool clsfy_random_forest_builder::is_class(std::string const& s) const
{
    return s == clsfy_random_forest_builder::is_a() || clsfy_builder_base::is_class(s);
}

//=======================================================================

clsfy_builder_base* clsfy_random_forest_builder::clone() const
{
    return new clsfy_random_forest_builder(*this);
}

//=======================================================================

void clsfy_random_forest_builder::print_summary(std::ostream& os) const
{
    os << "Num trees = "<<ntrees_<<"\tmax_depth = " << max_depth_;
}

//=======================================================================

void clsfy_random_forest_builder::b_write(vsl_b_ostream& bfs) const
{
    vsl_b_write(bfs, version_no());
    vsl_b_write(bfs, ntrees_);
    vsl_b_write(bfs, max_depth_);
    vsl_b_write(bfs, min_node_size_);
    vsl_b_write(bfs,calc_test_error_);
    std::cerr << "clsfy_random_forest_builder::b_write() NYI\n";
}

//=======================================================================

void clsfy_random_forest_builder::b_read(vsl_b_istream& bfs)
{
    if (!bfs) return;

    short version;
    vsl_b_read(bfs,version);
    switch (version)
    {
        case (1):
            vsl_b_read(bfs, ntrees_);
            vsl_b_read(bfs, max_depth_);
            vsl_b_read(bfs, min_node_size_);
            vsl_b_read(bfs,calc_test_error_);
            break;
        default:
            std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_random_forest_builder&)\n"
                     << "           Unknown version number "<< version << '\n';
            bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    }
}

//=======================================================================

//: Build model from data
// return the mean error over the training set.
// For many classifiers, you may use nClasses==1 to
// indicate a binary classifier
double clsfy_random_forest_builder::build(clsfy_classifier_base& classifier,
                                          mbl_data_wrapper<vnl_vector<double> >& inputs,
                                          unsigned nClasses,
                                          const std::vector<unsigned> &outputs) const
{
    assert(classifier.is_class("clsfy_random_forest")); // equiv to dynamic_cast<> != 0
    assert(inputs.size()==outputs.size());
    assert(nClasses=1);


    auto &random_forest = static_cast<clsfy_random_forest&>(classifier);
    unsigned npoints=inputs.size();
    std::vector<vnl_vector<double> > vin(npoints);

    inputs.reset();
    unsigned i=0;
    do
    {
        vin[i++] = inputs.current();
    } while (inputs.next());

    assert(i==inputs.size());

    unsigned ndims=vin[0].size();
    int nbranch_params=select_nbranch_params(ndims);

    //Start with all parameter indices
    std::cout<<"npoints= "<<npoints<<"\tndims= "<<ndims<<std::endl;
    std::vector<unsigned> indices(ndims,0);

    mbl_stl_increments(indices.begin(),indices.end(),0);

    //Clean any old trees
    random_forest.prune();

    if (poob_indices_)
    {
        poob_indices_->clear();
        poob_indices_->reserve(ntrees_);
    }


    std::vector<vnl_vector<double> > bootstrapped_inputs;
    std::vector<unsigned  > bootstrapped_outputs;

    for (i=0;i<ntrees_;++i)
    {
        select_data(vin,outputs,bootstrapped_inputs,bootstrapped_outputs);

        clsfy_binary_tree_builder builder;
        builder.set_calc_test_error(false);

        clsfy_classifier_base* pBaseClassifier=builder.new_classifier();
        auto* pTreeClassifier=dynamic_cast<clsfy_binary_tree*>(pBaseClassifier);
        assert(pTreeClassifier);
        builder.set_nbranch_params(nbranch_params);

        unsigned long seed=get_tree_builder_seed();
//        std::cout<<"The seed is "<<seed<<std::endl;
        builder.seed_sampler(seed);

        builder.set_max_depth(max_depth_);
        builder.set_min_node_size(min_node_size_);
        mbl_data_array_wrapper<vnl_vector<double> > bootstrapped_inputs_mbl(bootstrapped_inputs);

        builder.build(*pTreeClassifier,
                      bootstrapped_inputs_mbl,
                      1,
                      bootstrapped_outputs);

        mbl_cloneable_ptr<clsfy_classifier_base> treeClassifier(pTreeClassifier);
        random_forest.trees_.push_back(treeClassifier);
    }

    if (calc_test_error_)
        return clsfy_test_error(classifier, inputs, outputs);
    else
        return 0.0;
}
//=======================================================================
//: Create empty classifier
// Caller is responsible for deletion
clsfy_classifier_base* clsfy_random_forest_builder::new_classifier() const
{
    return new clsfy_random_forest();
}


void clsfy_random_forest_builder::select_data(std::vector<vnl_vector<double> >& inputs,
                                              const std::vector<unsigned> &outputs,
                                              std::vector<vnl_vector<double> >& bootstrapped_inputs,
                                              std::vector<unsigned> & bootstrapped_outputs) const
{
    unsigned npoints=inputs.size();
    bootstrapped_inputs.resize(npoints);
    bootstrapped_outputs.resize(npoints);
    unsigned ndims=  inputs.front().size();
    if (poob_indices_)
    {
        poob_indices_->push_back(std::vector<unsigned>());
        poob_indices_->back().reserve(npoints);
    }
    for (unsigned i=0;i<npoints;++i)
    {
        bootstrapped_inputs[i].set_size(ndims);
        unsigned index=random_sampler_(npoints);
        bootstrapped_inputs[i]=inputs[index];
        bootstrapped_outputs[i]=outputs[index];
        if (poob_indices_)
            poob_indices_->back().push_back(index); //store index of point for later OOB estimates
    }
}

unsigned  clsfy_random_forest_builder::select_nbranch_params(unsigned ndims) const
{
    unsigned nbranch_params=1;
    if (ndims>2)
    {
        double dnbranch_params=std::sqrt(double(ndims)+0.1); //round up if close
        nbranch_params=unsigned (dnbranch_params); //round
    }
    return nbranch_params;
}

void clsfy_random_forest_builder::seed_sampler(unsigned long seed)
{
    random_sampler_.reseed(seed);
}

unsigned long clsfy_random_forest_builder::get_tree_builder_seed() const
{
    //generate some bytes from the original seeded random number generator
    unsigned long N=256;
    unsigned nbytes=sizeof(unsigned long);
    std::vector<vxl_byte> seedAsBytes(nbytes,1);

    for (unsigned ib=0;ib<nbytes;++ib)
    {
        seedAsBytes[ib]=static_cast<vxl_byte>(random_sampler_(N));
    }

    auto* pSeed=reinterpret_cast<unsigned long*>(&seedAsBytes[0]);
    return *pSeed;
}
