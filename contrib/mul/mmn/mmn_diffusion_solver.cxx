#include <iostream>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <cmath>
#include "mmn_diffusion_solver.h"
//:
// \file
// \brief Run diffusion algorithm to solve max sum problem
// \author Martin Roberts
//
// See  T Werner. A Linear Programming Approach to Max-sum problem: A review;
// IEEE Trans on Pattern Recog & Machine Intell, July 2007

#include <mmn/mmn_csp_solver.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector_ref.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_stl.h>
#include <mbl/mbl_stl_pred.h>

//Static magic numbers
unsigned mmn_diffusion_solver::gNCONVERGED=3;
unsigned mmn_diffusion_solver::gACS_CHECK_PERIOD=10;

//: Default constructor
mmn_diffusion_solver::mmn_diffusion_solver()
  : nnodes_(0), max_iterations_(2000), min_iterations_(200), epsilon_(1.0E-5),
    verbose_(false)
{
    init();
}

//: Construct with arcs
mmn_diffusion_solver::mmn_diffusion_solver(unsigned num_nodes,const std::vector<mmn_arc>& arcs)
: max_iterations_(2000), min_iterations_(200), epsilon_(1.0E-5), verbose_(false)
{
    init();
    set_arcs(num_nodes,arcs);
}

void mmn_diffusion_solver::init()
{
    count_=0;
    max_delta_=-1.0;
    nConverging_=0;
    soln_val_prev_=-1.0E30;
}

//: Pass in the arcs, which are then used to build the graph object
void mmn_diffusion_solver::set_arcs(unsigned num_nodes,const std::vector<mmn_arc>& arcs)
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
        std::cerr<<"Arcs appear to be inconsistent with number of nodes in mmn_diffusion_solver::set_arcs\n"
                <<"Max node in Arcs is: "<<max_node<<" but number of nodes= "<<nnodes_ << '\n';
    }

    graph_.build(nnodes_,arcs_);

    arc_costs_.clear();
    arc_costs_phi_.clear();
    node_costs_.clear();
    node_costs_phi_.clear();
    phi_.clear();
    phi_upd_.clear();
    u_.clear();

    phi_.resize(nnodes_);
    phi_upd_=phi_;
    arc_costs_.resize(nnodes_);
    arc_costs_phi_.resize(nnodes_);
    u_.resize(nnodes_);

    //Set max iterations, somewhat arbitrarily, increasing with nodes and arcs
    max_iterations_ = min_iterations_ + 2*(nnodes_ * arcs_.size());
}

//: Run the algorithm
std::pair<bool,double> mmn_diffusion_solver::operator()(const std::vector<vnl_vector<double> >& node_costs,
                                  const std::vector<vnl_matrix<double> >& pair_costs,
                                  std::vector<unsigned>& x)
{
    init();

    x.resize(nnodes_);
    std::fill(x.begin(),x.end(),0);

    node_costs_.resize(nnodes_);
    for (unsigned i=0;i<nnodes_;++i)
    {
        //: Negate costs to convert to log prob (i.e. internally we do maximisation)
        node_costs_[i] = - node_costs[i];
    }
    node_costs_phi_ = node_costs_;
    //Initialise potential structure and neighbourhood cost representation
    const std::vector<std::vector<std::pair<unsigned,unsigned> > >& neighbourhoods=graph_.node_data();
    for (unsigned inode=0; inode<neighbourhoods.size();++inode)
    {
        const std::vector<std::pair<unsigned,unsigned> >& neighbours=neighbourhoods[inode];
        auto neighIter=neighbours.begin();
        auto neighIterEnd=neighbours.end();
        while (neighIter != neighIterEnd) //do all neighbours of this node
        {
            unsigned arcId=neighIter->second;
            vnl_matrix<double>& linkCosts = arc_costs_[inode][neighIter->first];
            const vnl_matrix<double >& srcArcCosts=pair_costs[arcId];
            mmn_arc& arc=arcs_[arcId];
            unsigned v1=arc.v1;
            unsigned v2=arc.v2;
            unsigned minv=arc.min_v();
            if (inode!=v1 && inode!=v2)
            {
                std::string msg("Graph inconsistency in mmn_diffusion_solver::operator()\n");
                std::ostringstream os;
                os <<"Source node is "<<inode<<" but arc to alleged neighbour joins nodes "<<v1<<"\t to "<<v2<<'\n';
                msg+= os.str();
                std::cerr<<msg<<std::endl;
                throw mbl_exception_abort(msg);
            }

            if (inode==minv)
            {
                linkCosts=srcArcCosts;
            }
            else //transpose arc costs as this node is the 2nd element in the pair_costs matrix
            {
                linkCosts=srcArcCosts.transpose();
            }
            linkCosts*= -1.0; //convert to maximising log prob (not min -log prob)
            unsigned nstates=linkCosts.rows(); //number of source states
            phi_[inode][neighIter->first].set_size(nstates);
            phi_[inode][neighIter->first].fill(0.0); //set all initial potentials to zero

            u_[inode][neighIter->first].set_size(nstates);
            u_[inode][neighIter->first].fill(0.0); //set all initial potential changes to zero
                                                   //
            ++neighIter; //next neighbour of this node
        }
    } //next node

    arc_costs_phi_ = arc_costs_;
    node_costs_phi_ = node_costs_;
    phi_upd_ = phi_;

    //Now keep repeating node-pencil averaging
    std::vector<unsigned > random_indices(nnodes_,0);
    mbl_stl_increments(random_indices.begin(),random_indices.end(),0);
    do
    {
        max_delta_=-1.0;
        std::random_shuffle(random_indices.begin(),random_indices.end());
        //Randomise the order of pencils
        for (unsigned knode=0; knode<nnodes_;++knode)
        {
            unsigned inode=random_indices[knode];
            //Do all pencils from this node
            update_potentials_to_neighbours(inode,node_costs_phi_[inode]);

            phi_[inode]=phi_upd_[inode];
            //Update costs for node and its pencils given phi
            transform_costs(inode);
        }

        if (verbose_)
        {
            std::cout<<"Max potential delta at iteration "<<count_<<"\t is "<<max_delta_<<std::endl;
        }
    }
    while (continue_diffusion());

    //Now check final "trivial" solution is arc consistent
    bool ok = arc_consistent_solution(x);

    return std::pair<bool,double>(ok,-solution_cost(x));
}

void mmn_diffusion_solver::transform_costs()
{
    for (unsigned inode=0; inode<nnodes_;++inode)
    {
        transform_costs(inode);
    }
}

void mmn_diffusion_solver::transform_costs(unsigned inode)
{
    //Add on potentials to nodes and subtract from arcs to transform to equivalent problem
    // with (hopefully) a tighter upper bound
    unsigned nStates=node_costs_[inode].size();
    for (unsigned xlabel=0; xlabel<nStates;++xlabel) //Loop over labels of node
    {
        const std::vector<std::pair<unsigned,unsigned> >& neighbours=graph_.node_data()[inode];

        auto neighIter=neighbours.begin();
        auto neighIterEnd=neighbours.end();
        double phiTot=0.0; //total added to node cost
        while (neighIter != neighIterEnd) //Loop over all my neighbours
        {
            double& phix=phi_[inode][neighIter->first][xlabel];
            phiTot+= phix; //add contribution for this arc
            vnl_matrix<double>& linkCosts = arc_costs_[inode][neighIter->first];
            vnl_matrix<double>& linkCostsPhi = arc_costs_phi_[inode][neighIter->first];
            unsigned nNeighStates=linkCosts.cols();
            const vnl_vector<double >& phiTransposed=phi_[neighIter->first][inode];
            for (unsigned xprime=0; xprime<nNeighStates;++xprime) //Loop over labels of neighbour
            {
                // Update equivalent link costs given phi
                linkCostsPhi(xlabel,xprime) = linkCosts(xlabel,xprime) -(phix + phiTransposed[xprime]);
            }
            ++neighIter;
        }
        //Update equivalent node costs given phi
        node_costs_phi_[inode][xlabel] = node_costs_[inode][xlabel] + phiTot;
    } //labels of this node
}

double mmn_diffusion_solver::solution_cost(std::vector<unsigned>& x)
{
    //: Calculate objective function for solution x
    double sumNodes=0.0;
    //Sum over all nodes
    std::vector<vnl_vector<double> >::const_iterator nodeIter=node_costs_.begin();
    std::vector<vnl_vector<double> >::const_iterator nodeIterEnd=node_costs_.end();
    std::vector<unsigned >::const_iterator stateIter=x.begin();
    while (nodeIter != nodeIterEnd)
    {
        const vnl_vector<double>& ncosts = *nodeIter;
        sumNodes+=ncosts[*stateIter];
        ++nodeIter;++stateIter;
    }

    // Sum over all arcs
    std::vector<mmn_arc>::const_iterator arcIter=arcs_.begin();
    std::vector<mmn_arc>::const_iterator arcIterEnd=arcs_.end();
    double sumArcs=0.0;
    while (arcIter != arcIterEnd)
    {
        unsigned nodeId1=arcIter->v1;
        unsigned nodeId2=arcIter->v2;
        sumArcs += arc_costs_[nodeId1][nodeId2](x[nodeId1],x[nodeId2]);
        ++arcIter;
    }
    return sumNodes+sumArcs;
}


void mmn_diffusion_solver::update_potentials_to_neighbours(unsigned inode,
                                                           const vnl_vector<double>& node_cost)
{
    //Update all potentials from this node to its neighbours
    unsigned nStates=node_cost.size();
    const std::vector<std::pair<unsigned,unsigned> >& neighbours=graph_.node_data()[inode];
    for (unsigned xlabel=0; xlabel<nStates;++xlabel) //loop over my labels (i.e. each pencil)
    {
        auto neighIter=neighbours.begin();
        auto neighIterEnd=neighbours.end();
        double du=node_cost[xlabel];
        while (neighIter != neighIterEnd) //Loop over all my neighbours
        {
            //Compute contribution of this neighbour to the node-pencil averaging
            vnl_vector<double>& uToNeigh = u_[inode][neighIter->first];
            vnl_matrix<double>& linkCosts = arc_costs_phi_[inode][neighIter->first];
            double* pgRow=linkCosts[xlabel];
            uToNeigh[xlabel] = *(std::max_element(pgRow,pgRow+linkCosts.cols())); //max arc cost of pencil

            du += uToNeigh[xlabel];
            ++neighIter;
        }

        du /= (double(1.0+neighbours.size())); //average

        //Now update potentials given du
        neighIter=neighbours.begin();
        while (neighIter != neighIterEnd) //Loop over all my neighbours
        {
            vnl_vector<double>& uToNeigh = u_[inode][neighIter->first];
            double delta = (uToNeigh[xlabel] - du);
            phi_upd_[inode][neighIter->first][xlabel] += delta;
            max_delta_ = std::max(max_delta_,delta);
            ++neighIter;
        }
    }
}

bool mmn_diffusion_solver::arc_consistent_solution(std::vector<unsigned>& x)
{
    // Find for each node the maximum label(s), and the maximal connecting arcs
    // Check if this set form an arc consistent solution
    // If so set x to kernel
    // Otherwise x is set to the first maximal node label

    std::vector<mmn_csp_solver::label_subset_t > node_labels_subset(nnodes_);
    std::vector<mmn_csp_solver::arc_labels_subset_t > links_subset(arcs_.size());

    const double epsilon_cost = 1.0E-6;
    for (unsigned inode=0; inode<nnodes_;++inode) //Loop over nodes
    {
        vnl_vector<double> labelCosts=node_costs_phi_[inode];
        //: Find (possibly non-unique) maximal label value
        double lmax=*std::max_element(labelCosts.begin(),labelCosts.end());
        //: Then compile vector of all node indices with label value "near" this
        std::vector<unsigned  > index(labelCosts.size(),0) ;
        mbl_stl_increments(index.begin(),index.end(),0);
        //Insert all indices of elements = (or very close to) max value
        mbl_stl_copy_if(index.begin(),index.end(),
                        std::inserter(node_labels_subset[inode],node_labels_subset[inode].end()),
                        mbl_stl_pred_create_index_adapter(labelCosts,
                                                          mbl_stl_pred_is_near(lmax,epsilon_cost)));
    }
    for (unsigned arcId=0;arcId<arcs_.size();++arcId)
    {
        //Now find maximal edges by looping over all arcs
        double umax=-1.0E30;
        unsigned srcId=arcs_[arcId].min_v();
        unsigned targetId=arcs_[arcId].max_v();

        vnl_vector<double>& uToNeigh = u_[srcId][targetId];
        vnl_matrix<double>& linkCosts = arc_costs_phi_[srcId][targetId];

        unsigned nStates=linkCosts.rows();
        for (unsigned xlabel=0; xlabel<nStates;++xlabel) //Loop over labels of source node
        {
            double* pgRow=linkCosts[xlabel];
            double u = *(std::max_element(pgRow,pgRow+linkCosts.cols())); //max arc cost of pencil
            //And now look for max cost pencil
            umax = std::max(u,umax);
            uToNeigh[xlabel] = u;
        }


        std::vector<unsigned  > xindex(linkCosts.rows(),0) ;
        mbl_stl_increments(xindex.begin(),xindex.end(),0);
        std::vector<unsigned> maxRows;
        //Insert all indices of elements (source node labels) = (or very close to) max value
        mbl_stl_copy_if(xindex.begin(),xindex.end(),
                        std::back_inserter(maxRows),
                        mbl_stl_pred_create_index_adapter(uToNeigh,
                                                          mbl_stl_pred_is_near(umax,epsilon_cost)));
        auto rowIter=maxRows.begin();
        auto rowIterEnd=maxRows.end();
        while (rowIter != rowIterEnd)
        {
            //And for each such pencil locate the index of the maximising label to which it connects
            unsigned xlabel=*rowIter;
            vnl_vector_ref<double > row(linkCosts.cols(),linkCosts[xlabel]);
            mbl_stl_pred_is_near nearMax(umax,epsilon_cost);
            for (unsigned xprime=0;xprime<linkCosts.cols();++xprime)
            {
                if (nearMax(row[xprime]))
                {
                    links_subset[arcId].insert(std::pair<unsigned ,unsigned >(xlabel,xprime));
                }
            }

            ++rowIter;
        }
    }//Arcs

    //CSP checker
    mmn_csp_solver cspSolver(nnodes_,arcs_);
    bool arcConsistent=cspSolver(node_labels_subset,links_subset);
    if (arcConsistent)
    {
        const std::vector<mmn_csp_solver::label_subset_t >& kernel_node_labels=cspSolver.kernel_node_labels();
        for (unsigned inode=0; inode<nnodes_;++inode)
        {
            x[inode]=*(kernel_node_labels[inode].begin());
        }
    }
    else
    {
        //Transformed problem is not arc consistent but fill up the solution vector anyway
        //Set to first maximal node label in each case
        for (unsigned inode=0; inode<nnodes_;++inode)
        {
            x[inode] = *(node_labels_subset[inode].begin());
        }
    }
    return arcConsistent;
}

bool mmn_diffusion_solver::continue_diffusion()
{
    ++count_;
    bool retstate=true;
    if (max_delta_<epsilon_ || count_>max_iterations_)
    {
        //Terminate on either convergence or max iteration count reached
        retstate = false;
    }
    else if (count_>min_iterations_)
    {
        //Final convergence can be slow, but the final stages may well not affect the highest layer
        //So periodically check if we have reached an arc consistent top layer solution with non-increasing value
        if (count_ % gACS_CHECK_PERIOD==0)
        {
            std::vector<unsigned> x(nnodes_,0);
            bool ok = arc_consistent_solution(x);
            if (ok)
            {
                double soln_val=solution_cost(x);
                if (verbose_)
                {
                    std::cout<<"Arc consistent solution reached. "
                            <<"\tSolution value= "<<soln_val<<"\tprev soln val= "<<soln_val_prev_<<std::endl;
                }
                if (std::fabs(soln_val-soln_val_prev_)<epsilon_)
                {
                    ++nConverging_;
                    if (nConverging_>gNCONVERGED)
                    {
                        retstate = false;
                    }
                }
                else
                {
                    nConverging_=0;
                }

                soln_val_prev_=soln_val;
            }
            else
            {
                if (verbose_)
                {
                    std::cout<<"Solution is not yet arc consistent."<<std::endl;
                }
                nConverging_=0;
            }
        }
    }
    return retstate;
}
