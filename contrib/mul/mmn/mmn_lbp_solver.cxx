#include "mmn_lbp_solver.h"
#include <vcl_algorithm.h>
#include <vcl_functional.h>
#include <vcl_iterator.h>
#include <vcl_sstream.h>
#include <mbl/mbl_exception.h>
//:
// \file
// \brief //: Run loopy belief propagation to estimate maximum marginal probabilities of all node states
// \author Martin Roberts


//: Default constructor
mmn_lbp_solver::mmn_lbp_solver():nnodes_(0),epsilon_(1.0E-8),max_iterations_(100)
{
    init();
        
}
    //: Construct with arcs
mmn_lbp_solver::mmn_lbp_solver(unsigned num_nodes,const vcl_vector<mmn_arc>& arcs):
        epsilon_(1.0E-8),max_iterations_(100)
{
    init();
    set_arcs(num_nodes,arcs);
}
void mmn_lbp_solver::init()
{
    count_=0;
    max_delta_=-1.0;
}

//: Pass in the arcs, which are then used to build the graph object
void mmn_lbp_solver::set_arcs(unsigned num_nodes,const vcl_vector<mmn_arc>& arcs)
{
    nnodes_=num_nodes;
    arcs_ = arcs;
    //Verify consistency
    unsigned max_node=0;
    for(unsigned i=0; i<arcs.size();++i)
    {
        max_node=vcl_max(max_node,arcs[i].max_v());
    }
    if(nnodes_ != max_node+1)
    {
        vcl_cerr<<"Arcs appear to be inconsistent with number of nodes in mmn_lbp_solver::set_arcs"<<vcl_endl;
    }

    graph_.build(nnodes_,arcs_);

    const vcl_vector<vcl_vector<vcl_pair<unsigned,unsigned> > >& neighbourhoods=graph_.node_data();
    messages_.resize(nnodes_);
    messages_upd_=messages_;
    arc_costs_.resize(nnodes_);
    
}

//: Run the algorithm
double mmn_lbp_solver::operator()(const vcl_vector<vnl_vector<double> >& node_costs,
                                  const vcl_vector<vnl_matrix<double> >& pair_costs,
                                  vcl_vector<unsigned>& x)
{

    init();
    x.resize(nnodes_);
    vcl_fill(x.begin(),x.end(),0);
    belief_.resize(nnodes_);
    node_costs_.resize(nnodes_);
    for(unsigned i=0;i<nnodes_;++i)
    {
        //: Negate costs to convert to log prob (i.e. internallly we do maximisation)
        node_costs_[i] = -1.0*node_costs[i];
    }
    
    //Initialise message structure and neighbourhood cost representation
    const vcl_vector<vcl_vector<vcl_pair<unsigned,unsigned> > >& neighbourhoods=graph_.node_data();
    for(unsigned inode=0; inode<neighbourhoods.size();++inode)
    {
        unsigned nbstates=node_costs_[inode].size();
        belief_[inode].set_size(nbstates);
        
        const vcl_vector<vcl_pair<unsigned,unsigned> >& neighbours=neighbourhoods[inode];
        vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator neighIter=neighbours.begin();
        vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator neighIterEnd=neighbours.end();
        while(neighIter != neighIterEnd)
        {
            vnl_vector<double>& msgsToNeigh = messages_[inode][neighIter->first];
            unsigned arcId=neighIter->second;
            vnl_matrix<double>& linkCosts = arc_costs_[inode][neighIter->first];
            const vnl_matrix<double >& srcArcCosts=pair_costs[arcId];
            mmn_arc& arc=arcs_[arcId];
            unsigned v1=arc.v1;
            unsigned v2=arc.v2;
            unsigned minv=arc.min_v();
            bool swapRef=((v2==minv) ? true : false);
            unsigned rows,cols;
            if(inode!=v1 && inode!=v2)
            {
                vcl_string msg("Graph inconsistency in mmn_lbp_solver::operator() \n");
                vcl_ostringstream os;
                os <<"Source node is "<<inode<<" but arc to alleged neighbour joins nodes "<<v1<<"\t to "<<v2<<vcl_endl;
                msg+= os.str();
                vcl_cerr<<msg<<vcl_endl;
                throw mbl_exception_abort(msg);
            }
            

            if(inode==minv)
            {
                linkCosts=srcArcCosts;
            }
            else //transpose arc costs as this node is the 2nd element in the pair_costs matrix
            {
                linkCosts=srcArcCosts.transpose();
            }
            linkCosts*= -1.0; //convert to maximising log prob (not min -log prob)
            unsigned nstates=linkCosts.cols();
            double dnstates=double(nstates);
            messages_[inode][neighIter->first].set_size(nstates);
            messages_[inode][neighIter->first].fill(vcl_log(1.0/dnstates)); //set all initial messages to uniform prob
            
            ++neighIter; //next neighbour of this node
        }
                
    } //next node
   
    messages_upd_ = messages_;
    
    do
    {
        max_delta_=-1.0;
        for(unsigned inode=0; inode<nnodes_;++inode)
        {
            update_messages_to_neighbours(inode,node_costs_[inode]);
        }

        messages_ = messages_upd_;

//        vcl_cout<<"Max message delta at iteration "<<count_<<"\t is "<<max_delta_<<vcl_endl;        
    }while (continue_propagation());

    //Now calculate final belief levels of each node's states and select the maximising ones
    belief_.resize(nnodes_);
    for(unsigned inode=0; inode<neighbourhoods.size();++inode)
    {
        unsigned bestState=0;
        double best=-1.0E012;
        unsigned nstates=node_costs_[inode].size();
        for(unsigned istate=0; istate<nstates;++istate)
        {
            double b=node_costs_[inode][istate];
            //Now loop over neighbourhood
            const vcl_vector<vcl_pair<unsigned,unsigned> >& neighbours=graph_.node_data()[inode];
            vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator neighIter=neighbours.begin();
            vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator neighIterEnd=neighbours.end();
            while(neighIter != neighIterEnd)
            {
                vnl_vector<double>& msgsFromNeigh = messages_[neighIter->first][inode];
                b+= msgsFromNeigh[istate];
                ++neighIter;
            }
            belief_[inode][istate]=b;
            if(b>best)
            {
                best=b;
                bestState=istate;
            }

        }
        x[inode]=bestState;
            
        renormalise_log(belief_[inode]);
        for(unsigned i=0; i<belief_[inode].size();i++)
        {
            belief_[inode][i]=vcl_exp(belief_[inode][i]);
        }

     }

    //Return -best solution value (i.e. minimised form)
    return -solution_cost(x);
}

double mmn_lbp_solver::solution_cost(vcl_vector<unsigned>& x)
{
    //: Calculate best (max log prob) of solution x
    double sumNodes=0.0;
    //Sum over all nodes
    vcl_vector<vnl_vector<double> >::const_iterator nodeIter=node_costs_.begin();
    vcl_vector<vnl_vector<double> >::const_iterator nodeIterEnd=node_costs_.end();
    vcl_vector<unsigned >::const_iterator stateIter=x.begin();
    while(nodeIter != nodeIterEnd)
    {
        const vnl_vector<double>& ncosts = *nodeIter;
        sumNodes+=ncosts[*stateIter];
        ++nodeIter;++stateIter;
    }

    // Sum over all arcs
    vcl_vector<mmn_arc>::const_iterator arcIter=arcs_.begin();
    vcl_vector<mmn_arc>::const_iterator arcIterEnd=arcs_.end();
    double sumArcs=0.0;
    while(arcIter != arcIterEnd)
    {
        unsigned nodeId1=arcIter->v1;
        unsigned nodeId2=arcIter->v2;
        sumArcs += arc_costs_[nodeId1][nodeId2](x[nodeId1],x[nodeId2]);
        ++arcIter;
    }
    
    return (sumNodes+sumArcs);
    
}
void mmn_lbp_solver::update_messages_to_neighbours(unsigned inode,
                                                   const vnl_vector<double>& node_cost)
{
    //Update all messages from this node to its neighbours
    
    const vcl_vector<vcl_pair<unsigned,unsigned> >& neighbours=graph_.node_data()[inode];
    
    vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator neighIter=neighbours.begin();
    vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator neighIterEnd=neighbours.end();
    while(neighIter != neighIterEnd) //Loop over all my neighbours
    {
        vnl_vector<double>& msgsToNeigh = messages_[inode][neighIter->first];
        vnl_matrix<double>& linkCosts = arc_costs_[inode][neighIter->first];
        unsigned nTargetStates=msgsToNeigh.size();
        unsigned nSrcStates=linkCosts.rows(); //number of source states for this node
        if(nSrcStates!=node_cost.size())
        {
            vcl_string msg("Inconsistent array sizes in mmn_lbp_solver::update_messages_to_neighbours \n");
            msg+= "Inconsistent array sizes in mmn_lbp_solver::update_messages_to_neighbours ";
                
            vcl_cerr<<msg<<vcl_endl;
            throw mbl_exception_abort(msg);
        }
        double outerSum=0.0;
        for(unsigned jstate=0;jstate<nTargetStates;++jstate) //do each state of the target neighbour
        {
            double max_istates= -1.0E6;
            for(unsigned istate=0; istate<nSrcStates;++istate)
            {
                double logProdIncoming=0.0;
                //Compute product of all incoming messages to this node i from elsewhere (excluding target node j)
                vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator tomeIter=neighbours.begin();
                vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator tomeIterEnd=neighbours.end();
                while(tomeIter != tomeIterEnd)
                {
                    if(tomeIter != neighIter)
                    {
                        unsigned k=tomeIter->first;
                        double m_ki_xi=messages_[k][inode][istate];
                        logProdIncoming += m_ki_xi;
                    }
                    ++tomeIter;
                }
                double acost=linkCosts(istate,jstate);
                double ncost=node_cost[istate];
                double logMij=acost+ncost+logProdIncoming;
                max_istates = vcl_max(max_istates,logMij);
            }
            messages_upd_[inode][neighIter->first][jstate]=max_istates;

        }
        renormalise_log(messages_upd_[inode][neighIter->first]);
        #if 0
        vcl_cout<<"Iteration "<<count_<<"Msg Upd Node \t"<<inode<<" \tto node "<<neighIter->first<<"\t";
        
        vcl_copy(messages_upd_[inode][neighIter->first].begin(),
                 messages_upd_[inode][neighIter->first].end(),
                 vcl_ostream_iterator<double>(vcl_cout,"\t"));
        vcl_cout<<vcl_endl;
        vcl_cout<<"Iteration "<<count_<<"Msg Prv Node \t"<<inode<<" \tto node "<<neighIter->first<<"\t";
        
        vcl_copy(messages_[inode][neighIter->first].begin(),
                 messages_[inode][neighIter->first].end(),
                 vcl_ostream_iterator<double>(vcl_cout,"\t"));


        vcl_cout<<vcl_endl;
        vcl_cout<<vcl_endl;
        #endif

        //: Compute max change during iteration
        vnl_vector<double > delta_message=(messages_upd_[inode][neighIter->first]-
                                           messages_[inode][neighIter->first]);
        double delta=delta_message.inf_norm();
        max_delta_ = vcl_max(max_delta_,delta);

        ++neighIter;
    }
}
void mmn_lbp_solver::renormalise_log(vnl_vector<double >& logMessageVec)
{
    
    vnl_vector<double >::iterator stateIter=logMessageVec.begin();
    vnl_vector<double >::iterator stateIterEnd=logMessageVec.end();
    double probSum=0.0;
    while(stateIter != stateIterEnd)
    {
        probSum+=vcl_exp(*stateIter);
        ++stateIter;
    }

    //normalise so probabilities sum to 1
    double alpha = 1.0/probSum;
    //But now rather than multiplying by alpha, add log(alpha);
    double logAlpha=vcl_log(alpha);
    vcl_transform(logMessageVec.begin(),logMessageVec.end(),
                  logMessageVec.begin(),
                  vcl_bind2nd(vcl_plus<double>(),logAlpha));

}

bool mmn_lbp_solver::continue_propagation()
{
    ++count_;
    if(max_delta_<epsilon_ || count_>max_iterations_)
    {
        return false;
    }
    else
    {
        return true;
    }
}

                                                   
                                                   
                                                   

