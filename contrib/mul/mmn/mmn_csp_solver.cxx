#include <iostream>
#include <algorithm>
#include "mmn_csp_solver.h"
//:
// \file
// \brief see if the Constraint Satisfaction Problem is satisfiable
// \author Martin Roberts

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Default constructor
mmn_csp_solver::mmn_csp_solver():nnodes_(0),verbose_(false)
{
    init();
}

//: Construct with arcs
mmn_csp_solver::mmn_csp_solver(unsigned num_nodes,const std::vector<mmn_arc>& arcs):
        nnodes_(num_nodes),verbose_(false)
{
    init();
    set_arcs(num_nodes,arcs);
}

void mmn_csp_solver::init()
{
}

//: Pass in the arcs, which are then used to build the graph object
void mmn_csp_solver::set_arcs(unsigned num_nodes,const std::vector<mmn_arc>& arcs)
{
    nnodes_=num_nodes;
    arcs_ = arcs;
    //Verify consistency
    unsigned max_node=0;
    for (auto arc : arcs)
    {
        max_node=std::max(max_node,arc.max_v());
    }
    if (nnodes_ != max_node+1)
    {
        std::cerr<<"Arcs appear to be inconsistent with number of nodes in mmn_csp_solver::set_arcs\n"
                <<"Max node in Arcs is: "<<max_node<<" but number of nodes= "<<nnodes_ << '\n';
    }

    graph_.build(nnodes_,arcs_);
}


//: Run the algorithm
bool mmn_csp_solver::operator()(const std::vector<mmn_csp_solver::label_subset_t >& node_labels_subset,
                                const std::vector<mmn_csp_solver::arc_labels_subset_t >& links_subset)
{
    init();

    node_labels_present_ = node_labels_subset;
    initialise_arc_labels_linked(links_subset);


    bool deletedNode=false;
    bool deletedArc=false;

    do
    {
        deletedNode = check_for_node_deletions();
        deletedArc = check_for_arc_deletions();
    }
    while (deletedNode || deletedArc);

    bool emptyKernel=true;

    for (unsigned i=0; i<nnodes_;i++)
    {
        emptyKernel = emptyKernel && node_labels_present_[i].empty();
    }

    return !emptyKernel;
}

void  mmn_csp_solver::initialise_arc_labels_linked(const std::vector<mmn_csp_solver::arc_labels_subset_t >& links_subset)
{
    arc_labels_linked1_.clear();
    arc_labels_linked2_.clear();
    unsigned narcs=arcs_.size();
    arc_labels_linked1_.resize(narcs);
    arc_labels_linked2_.resize(narcs);

    for (unsigned iarc=0;iarc<narcs;++iarc)
    {
        arc_labels_subset_t1& labels_linked1=arc_labels_linked1_[iarc];
        arc_labels_subset_t2& labels_linked2=arc_labels_linked2_[iarc];

        auto linkIter=links_subset[iarc].begin();
        auto linkIterEnd=links_subset[iarc].end();
        while (linkIter != linkIterEnd)
        {
            labels_linked1.insert(*linkIter);
            labels_linked2.insert(*linkIter);
            ++linkIter;
        }
    }
}

bool mmn_csp_solver::check_for_node_deletions()
{
    bool deleted=false;
    for (unsigned inode=0;inode<nnodes_;++inode)
    {
        auto labelIter=node_labels_present_[inode].begin();
        auto labelIterEnd=node_labels_present_[inode].end();
        const std::vector<std::pair<unsigned,unsigned> >& neighbourhood = graph_.node_data()[inode];
        while (labelIter != labelIterEnd)
        {
            unsigned label = *labelIter; //this label value
            //Now loop over all arcs in the node's neighbourhood
            auto neighIter=neighbourhood.begin();
            auto neighIterEnd=neighbourhood.end();
            bool found=true;
            while (neighIter != neighIterEnd)
            {
                bool foundThisNeighbour=false;
                unsigned arcId=neighIter->second;
                if (inode<neighIter->first)
                {
                    std::pair<unsigned ,unsigned > sought(label,0);
                    auto linkIter=arc_labels_linked1_[arcId].lower_bound(sought);
                    if (linkIter != arc_labels_linked1_[arcId].end())
                    {
                        if (linkIter->first==label)
                        {
                            foundThisNeighbour=true;
                        }
                    }
                }
                else
                {
                    std::pair<unsigned ,unsigned > sought(0,label);
                    auto linkIter=arc_labels_linked2_[arcId].lower_bound(sought);
                    if (linkIter != arc_labels_linked2_[arcId].end())
                    {
                        if (linkIter->second==label)
                        {
                            foundThisNeighbour=true;
                        }
                    }
                }
                found = found && foundThisNeighbour;
                if (!foundThisNeighbour)
                {
                    if (verbose_)
                    {
                        std::cout<<"Found no arc linking labels for node "<<inode<<" label "<<label<<" to node "<<neighIter->first<<"along arc ID "<<arcId<<std::endl;
                    }
                    break;
                }
                ++neighIter;
            } //loop over all neighbours (pencils)

            if (!found)
            {
                //Found no links from this label to anywhere
                //So delete it
                auto labelIterNext=labelIter;
                ++labelIterNext;
                if (verbose_)
                {
                    std::cout<<"Have removed label "<<*labelIter<<" for node "<<inode<<" as it has no linking arcs"<<std::endl;
                }
                node_labels_present_[inode].erase(labelIter);
                labelIter=labelIterNext;
                deleted=true;
            }
            else
            {
                ++labelIter;
            }
        } //next label of this node (object)
    } //next node (object)
    return deleted;
}

bool mmn_csp_solver::check_for_arc_deletions()
{
    bool deleted=false;
    for (unsigned iarc=0;iarc<arcs_.size();++iarc)
    {
        arc_labels_subset_t1& labels_linked1=arc_labels_linked1_[iarc];
        arc_labels_subset_t2& labels_linked2=arc_labels_linked2_[iarc];
        auto linkIter=labels_linked1.begin();
        auto linkIterEnd=labels_linked1.end();
        while (linkIter != linkIterEnd)
        {
            unsigned label1=linkIter->first;
            unsigned label2=linkIter->second;
            unsigned node1=arcs_[iarc].min_v();
            unsigned node2=arcs_[iarc].max_v();
            assert(node1<node2);
            std::set<unsigned>& labelSet1=node_labels_present_[node1];

            bool found=false;
            if (labelSet1.find(label1)!=labelSet1.end())
            {
                std::set<unsigned>& labelSet2=node_labels_present_[node2];
                found = labelSet2.find(label2)!=labelSet2.end();
            }
            if (!found) //failed to find at least one of target labels
            {
                deleted=true;
                //std::pair<unsigned,unsigned > pair2(linkIter->second,linkIter->first); //transpose
                std::pair<unsigned,unsigned > pair2(linkIter->first,linkIter->second); //transpose

                labels_linked1.erase(linkIter++); //remove from multset 1
                //Find all possible instances in multiset 2 for removal
                std::pair<arc_labels_subset_t2::iterator,arc_labels_subset_t2::iterator> range=
                    labels_linked2.equal_range(pair2);
                auto killer=range.first;

                while (killer != range.second)
                {
                    if (killer->first==pair2.first)
                    {
                        //remove target link
                        labels_linked2.erase(killer++);
                        if (verbose_)
                        {
                            std::cout<<"Have removed arc Id "<<iarc<<" Linking nodes "<<node1<<'\t'<<node2
                                    <<" for respective labels "<<pair2.second<<'\t'<<pair2.first<<std::endl;
                        }
                    }
                    else
                    {
                        ++killer;
                    }
                }
            }
            else
            {
                ++linkIter;
            }
        }
    }
    return deleted;
}
