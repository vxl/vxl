// This is mul/clsfy/clsfy_binary_tree.cxx
#include "clsfy_binary_tree.h"
//:
// \file
// \brief Binary tree classifier
// \author Martin Roberts

#include <vcl_string.h>
#include <vcl_deque.h>
#include <vcl_algorithm.h>
#include <vcl_iterator.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <mbl/mbl_stl.h>


clsfy_binary_tree::clsfy_binary_tree(const clsfy_binary_tree& srcTree)
: clsfy_classifier_base(srcTree)
{
    root_=cache_node_=0;
    copy(srcTree);
}

clsfy_binary_tree& clsfy_binary_tree::operator=(const clsfy_binary_tree& srcTree)
{
    if (&srcTree != this)
    {
        copy(srcTree);
    }
    return *this;
}

void clsfy_binary_tree::copy(const clsfy_binary_tree& srcTree)
{
    remove_tree(root_);
    //Then copy into the classifier
    if (srcTree.root_)
    {
        root_ = new clsfy_binary_tree_node(0,srcTree.root_->op_);
        root_->prob_ = srcTree.root_->prob_;
        copy_children(srcTree.root_,root_);
    }
    else
        root_=0;
    cache_node_ = root_;
}

void clsfy_binary_tree::copy_children(clsfy_binary_tree_node* pSrcNode,clsfy_binary_tree_node* pNode)
{
    bool left=true;
    pNode->prob_ = pSrcNode->prob_;
    if (pSrcNode->left_child_)
    {
        pNode->add_child(pSrcNode->left_child_->op_,left);
        copy_children(pSrcNode->left_child_,
                      pNode->left_child_);
    }
    if (pSrcNode->right_child_)
    {
        pNode->add_child(pSrcNode->right_child_->op_,!left);
        copy_children(pSrcNode->right_child_,
                      pNode->right_child_);
    }
}

//=======================================================================
//: Return the classification of the given probe vector.
unsigned clsfy_binary_tree::classify(const vnl_vector<double> &input) const
{
    unsigned outClass=0;
    //Traverse the tree
    clsfy_binary_tree_node* pNode=root_;
    if (!pNode)
    {
        vcl_cerr<<"WARNING - empty tree in clsfy_binary_tree::classify\n"
                <<"Return default classification zero\n";
        return 0;
    }
    clsfy_binary_tree_node* pChild=0;
    do //Keep dropping down the tree till reach base level
    {
        pNode->op_.set_data(input);
        unsigned indicator=pNode->op_.classify();
        if (indicator==0)
        {
            pChild=pNode->left_child_;
        }
        else
        {
            pChild=pNode->right_child_;
        }
        if (pChild)
            pNode=pChild;
        else
        {
            cache_node_ = pNode; //Store final node (in case probability accessed)
            outClass=(pNode->prob_>0.5 ? 1 : 0);
        }
    }while (pChild);

    return outClass;
}

//=======================================================================
//: Return a probability like value that the input being in each class.
// output(i) i<<nClasses, contains the probability that the input is in class i
void clsfy_binary_tree::class_probabilities(vcl_vector<double>& outputs,
                                            vnl_vector<double>const& input) const
{
    outputs.resize(1);
    classify(input);
    outputs[0] = cache_node_->prob_;
}


//=======================================================================
//: The dimensionality of input vectors.
unsigned clsfy_binary_tree::n_dims() const
{
    clsfy_binary_tree_node* pNode=root_;
    if (pNode)
        return pNode->op_.ndims();
    else
        return 0;
}

//=======================================================================
//: This value has properties of a Log likelihood of being in class (binary classifiers only)
// class probability = exp(logL) / (1+exp(logL))
double clsfy_binary_tree::log_l(const vnl_vector<double> &input) const
{
    vcl_vector<double > probs;
    class_probabilities(probs,input);
    double p1=probs[0];
    double p0=1-p1;
    const double epsilon=1.0E-8;
    if (p0<epsilon) p0=epsilon;
    double L=vcl_log(p1/p0);

    return L;
}


//=======================================================================

vcl_string clsfy_binary_tree::is_a() const
{
    return vcl_string("clsfy_binary_tree");
}

//=======================================================================

bool clsfy_binary_tree::is_class(vcl_string const& s) const
{
    return s == clsfy_binary_tree::is_a() || clsfy_classifier_base::is_class(s);
}

//=======================================================================

short clsfy_binary_tree::version_no() const
{
    return 1;
}

//=======================================================================

clsfy_classifier_base* clsfy_binary_tree::clone() const
{
    return new clsfy_binary_tree(*this);
}

//=======================================================================

void clsfy_binary_tree::print_summary(vcl_ostream& /*os*/) const
{
}

//=======================================================================

void clsfy_binary_tree::b_write(vsl_b_ostream& bfs) const
{
    vsl_b_write(bfs,version_no());
    int nodeId=0; //used numeric ids for parent child relations
    // -1 means none
    vcl_deque<clsfy_binary_tree_node*> stack;
    vcl_deque<clsfy_binary_tree_node*> outlist;
    vcl_vector<graph_rep> arcs;
    clsfy_binary_tree_node* pNode=root_;

    stack.push_back(pNode);
    pNode->nodeId_=0;
    while (!stack.empty())
    {
        pNode=stack.front();
        stack.pop_front();
        outlist.push_back(pNode);
        graph_rep link;
        link.me=pNode->nodeId_;
        link.left_child = link.right_child = -1;

        if (pNode)
        {
            if (pNode->left_child_)
            {
                stack.push_back(pNode->left_child_);
                pNode->left_child_->nodeId_= ++nodeId;
                link.left_child=nodeId;
            }
            if (pNode->right_child_)
            {
                stack.push_back(pNode->right_child_);
                pNode->right_child_->nodeId_= ++nodeId;
                link.right_child=nodeId;
            }

            arcs.push_back(link);
        }
    }

    unsigned N=outlist.size();
    vsl_b_write(bfs,N);

    vcl_deque<clsfy_binary_tree_node*>::iterator outIter=outlist.begin();
    vcl_deque<clsfy_binary_tree_node*>::iterator outIterEnd=outlist.end();
    while (outIter != outIterEnd)
    {
        clsfy_binary_tree_node* pNode=*outIter;
        vsl_b_write(bfs,pNode->nodeId_);
        pNode->op_.b_write(bfs);
        vsl_b_write(bfs,pNode->prob_);
        ++outIter;
    }

    //Now write out the links graph
    N=arcs.size();
    vsl_b_write(bfs,N);

    vcl_vector<graph_rep>::iterator arcIter=arcs.begin();
    vcl_vector<graph_rep>::iterator arcIterEnd=arcs.end();

    while (arcIter != arcIterEnd)
    {
        vsl_b_write(bfs,arcIter->me);
        vsl_b_write(bfs,arcIter->left_child);
        vsl_b_write(bfs,arcIter->right_child);
        ++arcIter;
    }
}

//=======================================================================

void clsfy_binary_tree::b_read(vsl_b_istream& bfs)
{
    if (!bfs) return;

    remove_tree(root_);
    root_=0;

    short version;
    vsl_b_read(bfs,version);
    switch (version)
    {
        case (1):
        {
            vcl_map<int,clsfy_binary_tree_node*> workmap;
            vcl_vector<graph_rep> arcs;

            clsfy_binary_tree_node* pNull=0;
            unsigned N;
            vsl_b_read(bfs,N);
            for (unsigned i=0;i<N;++i)
            {
                int nodeId=-1;
                vsl_b_read(bfs,nodeId);
                clsfy_binary_tree_op op;
                op.b_read(bfs);
                clsfy_binary_tree_node* pNode=new clsfy_binary_tree_node(pNull,op);
                pNode->nodeId_=nodeId;
                vsl_b_read(bfs,pNode->prob_);
                workmap[nodeId]=pNode;
            }
            vsl_b_read(bfs,N);
            arcs.reserve(N);
            for (unsigned i=0;i<N;++i)
            {
                graph_rep link;
                vsl_b_read(bfs,link.me);
                vsl_b_read(bfs,link.left_child);
                vsl_b_read(bfs,link.right_child);
                arcs.push_back(link);
            }
            root_=workmap[0];
            for (unsigned i=0;i<N;++i)
            {
                graph_rep link=arcs[i];
                if (link.me!= -1)
                {
                    clsfy_binary_tree_node* parent=workmap[link.me];
                    clsfy_binary_tree_node* left_child=0;
                    clsfy_binary_tree_node* right_child=0;
                    if (link.left_child != -1)
                        left_child=workmap[link.left_child];
                    if (link.right_child != -1)
                        right_child=workmap[link.right_child];

                    if (!parent || parent->nodeId_ != link.me)
                    {
                        vcl_cerr<<"ERROR - Inconsistent parent in tree set up in clsfy_binary_tree::b_read\n";
                        assert(0);
                    }
                    if ((link.left_child != -1) &&
                        (!left_child || left_child->nodeId_ != link.left_child))
                                        {
                        vcl_cerr<<"ERROR - Inconsistent left child in tree set up in clsfy_binary_tree::b_read\n";
                        assert(0);
                    }
                    if ((link.right_child != -1) &&
                        (!right_child || right_child->nodeId_ != link.right_child))
                                        {
                        vcl_cerr<<"ERROR - Inconsistent right child in tree set up in clsfy_binary_tree::b_read\n";
                        assert(0);
                    }

                    //And link these into the tree
                    parent->left_child_=left_child;
                    if (left_child)
                        left_child->parent_=parent;

                    parent->right_child_=right_child;
                    if (right_child)
                        right_child->parent_=parent;
                }
            }

            //Validate the tree
            assert(root_);
            vcl_map<int,clsfy_binary_tree_node*>::iterator nodeIter =workmap.begin();
            vcl_map<int,clsfy_binary_tree_node*>::iterator nodeIterEnd =workmap.end();
            while (nodeIter != nodeIterEnd)
            {
                clsfy_binary_tree_node* pNode=nodeIter->second;
                assert(pNode->nodeId_==nodeIter->first);
                if (pNode != root_)
                {
                    assert(pNode->parent_);
                    assert(pNode->parent_->left_child_==pNode ||
                           pNode->parent_->right_child_ == pNode);
                }
                if (pNode->left_child_)
                    assert(pNode->left_child_->parent_==pNode);
                if (pNode->right_child_)
                    assert(pNode->right_child_->parent_==pNode);

                //Check all nodes connect back up to root
                while (pNode->parent_)
                {
                    pNode=pNode->parent_;
                }
                assert(pNode==root_);

                ++nodeIter;
            }
        }
        break;

        default:
            vcl_cerr << "I/O ERROR: clsfy_binary_tree::b_read(vsl_b_istream&)\n"
                     << "           Unknown version number "<< version << '\n';
            bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
}

clsfy_binary_tree::~clsfy_binary_tree()
{
    remove_tree(root_);
    root_=0;
}

void  clsfy_binary_tree::remove_tree(clsfy_binary_tree_node* root)
{
    vcl_deque<clsfy_binary_tree_node*> stack;
    vcl_deque<clsfy_binary_tree_node*> killset;
    stack.push_back(root);
    while (!stack.empty())
    {
        clsfy_binary_tree_node* pNode=stack.front();
        stack.pop_front();

        if (pNode)
        {
            killset.push_back(pNode);
            if (pNode->left_child_)
            {
                stack.push_back(pNode->left_child_);
            }
            if (pNode->right_child_)
            {
                stack.push_back(pNode->right_child_);
            }
        }
    }

    mbl_stl_clean(killset.begin(),killset.end());
}

void clsfy_binary_tree::set_root(  clsfy_binary_tree_node* root)
{
    if ((root != root_) && root_)
        remove_tree(root_);
    root_=root;
}


//--------------- HELPER CLASSES---------------------------------------------------------

clsfy_binary_tree_node* clsfy_binary_tree_node::create_child(const clsfy_binary_tree_op& op)
{
    return new clsfy_binary_tree_node(this,op);
}

void clsfy_binary_tree_op::b_write(vsl_b_ostream& bfs) const
{
    vsl_b_write(bfs,version_no());
    vsl_b_write(bfs,data_index_);
    vsl_b_write(bfs,classifier_);
}

//: Load the class from a Binary File Stream
void clsfy_binary_tree_op::b_read(vsl_b_istream& bfs)
{
    short version;
    vsl_b_read(bfs,version);
    if (version != 1)
    {
        vcl_cerr << "I/O ERROR: clsfy_binary_tree::b_read(vsl_b_istream&)\n"
                 << "           Unknown version number "<< version << '\n';
        bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    }
    else
    {
        vsl_b_read(bfs,data_index_);
        vsl_b_read(bfs,classifier_);
    }
}

