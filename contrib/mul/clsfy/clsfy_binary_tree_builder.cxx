// This is mul/clsfy/clsfy_binary_tree_builder.cxx
//:
// \file
// \brief Implement a binary_tree classifier builder
// \author Martin Roberts

#include <iostream>
#include <string>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <cstddef>
#include "clsfy_binary_tree_builder.h"
#include <clsfy/clsfy_binary_threshold_1d_gini_builder.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vsl/vsl_binary_loader.h>
#include <mbl/mbl_stl.h>
#include <clsfy/clsfy_k_nearest_neighbour.h>

//=======================================================================

clsfy_binary_tree_builder::clsfy_binary_tree_builder():
    max_depth_(-1),min_node_size_(5),nbranch_params_(-1),calc_test_error_(true)
{
    unsigned long default_seed=123654987;
    seed_sampler(default_seed);
}


//=======================================================================

short clsfy_binary_tree_builder::version_no() const
{
    return 1;
}

//=======================================================================

std::string clsfy_binary_tree_builder::is_a() const
{
    return std::string("clsfy_binary_tree_builder");
}

//=======================================================================

bool clsfy_binary_tree_builder::is_class(std::string const& s) const
{
    return s == clsfy_binary_tree_builder::is_a() || clsfy_builder_base::is_class(s);
}

//=======================================================================

clsfy_builder_base* clsfy_binary_tree_builder::clone() const
{
    return new clsfy_binary_tree_builder(*this);
}

//=======================================================================

void clsfy_binary_tree_builder::print_summary(std::ostream& os) const
{
    os << "max_depth = " << max_depth_;
}

//=======================================================================

void clsfy_binary_tree_builder::b_write(vsl_b_ostream& bfs) const
{
    vsl_b_write(bfs, version_no());
    vsl_b_write(bfs, max_depth_);
    vsl_b_write(bfs, min_node_size_);
    vsl_b_write(bfs, nbranch_params_);
    vsl_b_write(bfs,calc_test_error_);
    std::cerr << "clsfy_binary_tree_builder::b_write() NYI\n";
}

//=======================================================================

void clsfy_binary_tree_builder::b_read(vsl_b_istream& bfs)
{
    if (!bfs) return;

    short version;
    vsl_b_read(bfs,version);
    switch (version)
    {
        case (1):
            vsl_b_read(bfs, max_depth_);
            vsl_b_read(bfs, min_node_size_);
            vsl_b_read(bfs, nbranch_params_);
            vsl_b_read(bfs,calc_test_error_);

            break;
        default:
            std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, clsfy_binary_tree_builder&)\n"
                     << "           Unknown version number "<< version << '\n';
            bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    }
}

//=======================================================================

//: Build model from data
// return the mean error over the training set.
// For many classifiers, you may use nClasses==1 to
// indicate a binary classifier
double clsfy_binary_tree_builder::build(clsfy_classifier_base& classifier,
                                        mbl_data_wrapper<vnl_vector<double> >& inputs,
                                        unsigned nClasses,
                                        const std::vector<unsigned> &outputs) const
{
    assert(classifier.is_class("clsfy_binary_tree")); // equiv to dynamic_cast<> != 0
    assert(inputs.size()==outputs.size());
    assert(nClasses=1);


    auto &binary_tree = static_cast<clsfy_binary_tree&>(classifier);
    unsigned npoints=inputs.size();
    std::vector<vnl_vector<double> > vin(npoints);

    inputs.reset();
    unsigned i=0;
    do
    {
        vin[i++] = inputs.current();
    } while (inputs.next());

    assert(i==inputs.size());


    unsigned ndims=vin.front().size();
    base_indices_.resize(ndims);
    mbl_stl_increments(base_indices_.begin(),base_indices_.end(),0);

    clsfy_binary_tree_op rootOp;
    auto* root=new clsfy_binary_tree_bnode(nullptr,rootOp);

    // Start with all indices
    std::set<unsigned> indices;
    mbl_stl_increments_n(std::inserter(indices,indices.end()),npoints,0);
    // Build the root node starting from all indices
    build_a_node(vin,outputs,indices,root);

    bool left=true;
    bool right=false;

    bool pure=isNodePure(root->subIndicesL,outputs);
    if (!pure)
    { // Build the left branch children (recursively)
#if 0
        std::cout<<"Building the root left branch children"<<std::endl;
#endif
        build_children(vin,outputs,root,left);
    }
    else
    {
#if 0
        std::cout<<"Terminating the root left branch"<<std::endl;
#endif
        add_terminator(vin,outputs,root,left,true);
    }
    pure=isNodePure(root->subIndicesR,outputs);
    if (!pure)
    {    // Build the right branch children (recursively)
#if 0
        std::cout<<"Building the root right branch children"<<std::endl;
#endif
        build_children(vin,outputs,root,right);
    }
    else
    {
#if 0
        std::cout<<"Terminating the root right branch"<<std::endl;
#endif
        add_terminator(vin,outputs,root,right,true);
    }


    // Then copy into the classifier (like b_write, b_read)
    clsfy_binary_tree_node* classRoot=new clsfy_binary_tree_node(nullptr,root->op_);
    set_node_prob(classRoot,root);

    copy_children(root,classRoot);
    binary_tree.set_root(classRoot);

    clsfy_binary_tree::remove_tree(root);

    if (calc_test_error_)
        return clsfy_test_error(classifier, inputs, outputs);
    else
        return 0.0;
}

void clsfy_binary_tree_builder::build_children(
    const std::vector<vnl_vector<double> >& vin,
    const std::vector<unsigned>& outputs,
    clsfy_binary_tree_bnode* parent,bool left) const
{
    if (max_depth_>0)
    {
        // Validate depth
        int depth=1;
        clsfy_binary_tree_bnode* pNode=parent;
        while (pNode)
        {
            pNode=static_cast<clsfy_binary_tree_bnode*>(pNode->parent_);
            ++depth;
        }
        if (depth>=max_depth_)
        {
            // Can't go any deeper on this branch
            std::set<unsigned >& subIndices=(left ? parent->subIndicesL : parent->subIndicesR);
            bool pure=isNodePure(subIndices,outputs);
            add_terminator(vin,outputs,parent,left,pure);
            return;
        }
    }


    std::set<unsigned >& subIndices=(left ? parent->subIndicesL : parent->subIndicesR);
    clsfy_binary_tree_op dummyOp;
    parent->add_child(dummyOp,left);


    auto* pChild=dynamic_cast< clsfy_binary_tree_bnode*>(left ? parent->left_child_ : parent->right_child_);
    build_a_node(vin,outputs,subIndices,pChild);

    // Check that this actually managed to produce a split (in case we have homogeneous data)
    if (pChild->subIndicesL.empty() || pChild->subIndicesR.empty())
    {
        // We should not have added this child as it's not managed to produce a split
        // Backtrack
        delete pChild;
        if (left)
            parent->left_child_=nullptr;
        else
            parent->right_child_=nullptr;
        // Can't go any deeper on this branch
        std::set<unsigned >& subIndices=(left ? parent->subIndicesL : parent->subIndicesR);
        bool pure=isNodePure(subIndices,outputs);
        add_terminator(vin,outputs,parent,left,pure);
        return;
    }

    // May need to check min dataset size in next generation children
    if (min_node_size_>0)
    {
        if (pChild->subIndicesL.size() < static_cast<std::size_t>(min_node_size_) ||
            pChild->subIndicesR.size() < static_cast<std::size_t>(min_node_size_) )
        {
            // We should not have added this child as it's based on too small a split
            // Backtrack
            delete pChild;
            if (left)
                parent->left_child_=nullptr;
            else
                parent->right_child_=nullptr;
            // Can't go any deeper on this branch
            std::set<unsigned >& subIndices=(left ? parent->subIndicesL : parent->subIndicesR);
            bool pure=isNodePure(subIndices,outputs);
            add_terminator(vin,outputs,parent,left,pure);
            return;
        }
    }


    clsfy_binary_tree_bnode* pNode=pChild;
    // Go on down the left branch of the split we just introduced

    // Check if left node is pure
    bool pure=isNodePure(pNode->subIndicesL,outputs);
    bool myLeft=true;
    bool myRight=false;
    if (!pure)
    {
        build_children(vin,outputs,pNode,myLeft);
    }
    else // Add a dummy classifier on a pure node so it always returns that class
    {
        add_terminator(vin,outputs,pNode,myLeft,true);
    }

    // Go on down the right branch of the split we just introduced
    // Check if right node is pure
    pure=isNodePure(pNode->subIndicesR,outputs);
    if (!pure)
    {
        build_children(vin,outputs,pNode,myRight);
    }
    else
    {
        add_terminator(vin,outputs,pNode,myRight,true);
    }
}


void clsfy_binary_tree_builder::copy_children(clsfy_binary_tree_bnode* pBuilderNode,
                                              clsfy_binary_tree_node* pNode) const
{
    bool left=true;
    set_node_prob(pNode,pBuilderNode);

    if (pBuilderNode->left_child_)
    {
        pNode->add_child(pBuilderNode->left_child_->op_,left);
        copy_children(static_cast<clsfy_binary_tree_bnode*>(pBuilderNode->left_child_),
                      pNode->left_child_);
    }
    if (pBuilderNode->right_child_)
    {
        pNode->add_child(pBuilderNode->right_child_->op_,!left);
        copy_children(static_cast<clsfy_binary_tree_bnode*>(pBuilderNode->right_child_),
                      pNode->right_child_);
    }
}

void clsfy_binary_tree_builder::build_a_node(
    const std::vector<vnl_vector<double> >& vin,
    const std::vector<unsigned>& outputs,
    const std::set<unsigned >& subIndices,
    clsfy_binary_tree_bnode* pNode) const
{
    clsfy_binary_threshold_1d_gini_builder tbuilder;
    unsigned ndims=vin.front().size();
    unsigned ndimsUsed=ndims;
    std::vector<unsigned > param_indices;
    if (nbranch_params_>0) // Random forest style random subset selection
    {
        ndimsUsed=nbranch_params_;
        ndimsUsed=std::min(ndimsUsed,ndims);
        if (ndimsUsed<ndims)
        {
            // Note always do a full random permutation as the ones beyond ndimsUsed
            // may be needed as a fallback in exceptional cases where the initial random
            // subset cannot split the data
            randomise_parameters(ndims,param_indices);
        }
        else
        {
            ndimsUsed=ndims;
            param_indices.resize(ndims);
            mbl_stl_increments(param_indices.begin(),param_indices.end(),0);
        }
    }
    else
    {
        param_indices.resize(ndims);
        mbl_stl_increments(param_indices.begin(),param_indices.end(),0);
    }
    std::vector<clsfy_classifier_1d*> pBranchClassifiers(ndims,nullptr);
    vnl_vector<double > wts(subIndices.size());
    wts.fill(1.0/double (vin.size())-1.0E-12);
    unsigned npoints=subIndices.size();
    std::vector<unsigned > subOutputs;
    subOutputs.reserve(npoints);
    std::transform(subIndices.begin(),subIndices.end(),
                  std::back_inserter(subOutputs),
                  mbl_stl_index_functor<unsigned >(outputs));

    double minError=1.0E30;
    unsigned ibest=0;

    vnl_vector<double > data(npoints);

    // May need a second pass because there may be a subset of homogeneous parameters
    // if we are only using a random subset which cannot produce a split
    // So try first with the random subset, and then with all if that fails
    // Note this assumes param_indices contains the complete set
    unsigned npasses=1;
    if (ndimsUsed<ndims) npasses=2;
    for (unsigned ipass=0;ipass<npasses;++ipass)
    {
        unsigned istart=0;
        unsigned nmax=ndimsUsed;
        if (ipass>0)
        {
            istart=ndimsUsed;
            nmax=ndims;
        }
        for (unsigned idim=istart;idim<nmax;++idim)
        {
            pBranchClassifiers[idim] = tbuilder.new_classifier();
            auto indIter=subIndices.begin();
            auto indIterEnd=subIndices.end();
            unsigned ipt=0;
            while (indIter != indIterEnd)
            {
                data[ipt] = vin[*indIter][param_indices[idim]];
                ++ipt;
                ++indIter;
            }

            double error=tbuilder.build_gini(*pBranchClassifiers[idim],
                                             data,subOutputs);
            if (error<minError)
            {
                minError=error;
                ibest=idim;
            }
        }

        pNode->subIndicesL.clear();
        pNode->subIndicesR.clear();
        clsfy_binary_tree_op op(nullptr,param_indices[ibest]);
        op.classifier() = *(static_cast<clsfy_binary_threshold_1d*>(pBranchClassifiers[ibest]));

        pNode->op_=op;

        // Now reapply to all relevant data to construct the subset split
        auto indIter=subIndices.begin();
        auto indIterEnd=subIndices.end();
        std::set<unsigned >& subIndicesL=pNode->subIndicesL;
        std::set<unsigned >& subIndicesR=pNode->subIndicesR;
        while (indIter != indIterEnd)
        {
            double x = vin[*indIter][param_indices[ibest]];
            if (pBranchClassifiers[ibest]->classify(x)==0)
                subIndicesL.insert(*indIter);
            else
                subIndicesR.insert(*indIter);
            ++indIter;
        }

        if (!subIndicesL.empty() && !subIndicesR.empty()) // Success - it really has split
            break; // no second pass needed
    }
    mbl_stl_clean(pBranchClassifiers.begin(),pBranchClassifiers.end());
}

bool clsfy_binary_tree_builder::isNodePure(const std::set<unsigned >& subIndices,
                                           const std::vector<unsigned>& outputs) const
{
    if (subIndices.empty()) return true;
    auto indIter=subIndices.begin();
    auto indIterEnd=subIndices.end();

    unsigned class0=outputs[*indIter];
    while (indIter != indIterEnd)
    {
        if (outputs[*indIter] != class0)
            return false;
        ++indIter;
    }
    return true;
}

//: Add dummy node to represent a pure node
// The threshold is set either very low or very high
void clsfy_binary_tree_builder::add_terminator(
    const std::vector<vnl_vector<double> >&  /*vin*/,
    const std::vector<unsigned>& outputs,
    clsfy_binary_tree_bnode* parent,
    bool left, bool pure) const
{
    double thresholdBig=1.0E30;

    int dummyIndex=0;
    clsfy_binary_tree_op dummyOp(nullptr,dummyIndex);


    unsigned classification=0;
    double prob=0.5;
    if (pure)
    {
        if (left)
        {
            if (!(parent->subIndicesL.empty()))
                classification=outputs[*(parent->subIndicesL.begin())];
        }
        else
        {
            classification=1;
            if (!(parent->subIndicesR.empty()))
                classification=outputs[*(parent->subIndicesR.begin())];
        }
        prob = (classification==1 ? 1.0 : 0.0);
    }
    else // Mixed node - assess ratio of classes
    {
        std::set<unsigned >& indices=(left ? parent->subIndicesL : parent->subIndicesR);
        auto indexIter=indices.begin();
        auto indexIterEnd=indices.end();
        unsigned n1=0;
        while (indexIter != indexIterEnd)
        {
            if (outputs[*indexIter]==1)
                ++n1;
            ++indexIter;
        }
        prob=double (n1)/double (indices.size());
        classification = (prob>0.5 ? 1 : 0);
    }
    double parity=(classification==0 ? 1.0 : -1.0);
    dummyOp.classifier().set(1.0,parity*thresholdBig);

    parent->add_child(dummyOp,left);

    if (left)
        parent->left_child_->prob_=prob;
    else
        parent->right_child_->prob_=prob;
}

//=======================================================================
//: Create empty classifier
// Caller is responsible for deletion
clsfy_classifier_base* clsfy_binary_tree_builder::new_classifier() const
{
    return new clsfy_binary_tree();
}

void  clsfy_binary_tree_builder::randomise_parameters(unsigned  /*ndimsUsed*/,
                                                      std::vector<unsigned  >& param_indices) const
{
    // In fact it shuffles all indices (in case the random subset does not produce a split)
    param_indices.resize(base_indices_.size());

    std::random_shuffle(base_indices_.begin(),base_indices_.end(),random_sampler_);
    std::copy(base_indices_.begin(),base_indices_.end(),
             param_indices.begin());
}


void clsfy_binary_tree_builder::seed_sampler(unsigned long seed)
{
    random_sampler_.reseed(seed);
}

void clsfy_binary_tree_builder::set_node_prob(clsfy_binary_tree_node* pNode,
                                              clsfy_binary_tree_bnode* pBuilderNode) const
{
    pNode->prob_ = pBuilderNode->prob_;
}

//=========================== Helper Classes =============================
clsfy_binary_tree_node* clsfy_binary_tree_bnode::create_child(const clsfy_binary_tree_op& op)
{
    return new clsfy_binary_tree_bnode(this,op);
}

clsfy_binary_tree_bnode::~clsfy_binary_tree_bnode() = default;
