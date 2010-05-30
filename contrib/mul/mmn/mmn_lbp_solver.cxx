#include "mmn_lbp_solver.h"
#include <vcl_algorithm.h>
#include <vcl_functional.h>
#include <vcl_iterator.h>
#include <vcl_sstream.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_stl.h>
#include <mbl/mbl_parse_block.h>
#include <mbl/mbl_read_props.h>

//:
// \file
// \brief Run loopy belief propagation to estimate maximum marginal probabilities of all node states
// \author Martin Roberts

const unsigned mmn_lbp_solver::NHISTORY_=5;
const unsigned mmn_lbp_solver::NCYCLE_DETECT_=7;

//: Default constructor
mmn_lbp_solver::mmn_lbp_solver()
    : nnodes_(0), max_iterations_(100), min_simple_iterations_(25),
      epsilon_(1E-6), alpha_(0.6), smooth_on_cycling_(true),
      max_cycle_detection_count_(3), verbose_(false),
      msg_upd_mode_(mmn_lbp_solver::eRANDOM_SERIAL)
{
    init();
}

//: Construct with arcs
mmn_lbp_solver::mmn_lbp_solver(unsigned num_nodes,const vcl_vector<mmn_arc>& arcs)
    : max_iterations_(100), min_simple_iterations_(25),
      epsilon_(1E-6), alpha_(0.6), smooth_on_cycling_(true),
      max_cycle_detection_count_(3), verbose_(false),
      msg_upd_mode_(mmn_lbp_solver::eRANDOM_SERIAL)
{
    init();
    set_arcs(num_nodes,arcs);
}

void mmn_lbp_solver::init()
{
    count_=0;
    max_delta_=-1.0;
    soln_history_.clear();
    max_delta_history_.clear();
    isCycling_ = false;
    nrevisits_=0;
    cycle_detection_count_=0;
    zbest_on_cycle_detection_=0.0;
}

//: Pass in the arcs, which are then used to build the graph object
void mmn_lbp_solver::set_arcs(unsigned num_nodes,const vcl_vector<mmn_arc>& arcs)
{
    nnodes_=num_nodes;
    arcs_ = arcs;
    //Verify consistency
    unsigned max_node=0;
    for (unsigned i=0; i<arcs.size();++i)
    {
        max_node=vcl_max(max_node,arcs[i].max_v());
    }
    if (nnodes_ != max_node+1)
    {
        vcl_cerr<<"Arcs appear to be inconsistent with number of nodes in mmn_lbp_solver::set_arcs\n"
                <<"Max mode in Arcs is: "<<max_node<<" but number of nodes= "<<nnodes_<<'\n';
    }

    graph_.build(nnodes_,arcs_);

    messages_.resize(nnodes_);
    messages_upd_=messages_;
    arc_costs_.resize(nnodes_);

    //Set max iterations, somewhat arbitrarily, increasing with nodes and arcs
    max_iterations_ = min_simple_iterations_ + nnodes_ + arcs_.size();
}

double mmn_lbp_solver::solve(
                 const vcl_vector<vnl_vector<double> >& node_cost,
                 const vcl_vector<vnl_matrix<double> >& pair_cost,
                 vcl_vector<unsigned>& x)
{
    return (*this)(node_cost,pair_cost,x);
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
    for (unsigned i=0;i<nnodes_;++i)
    {
        //: Negate costs to convert to log prob (i.e. internally we do maximisation)
        node_costs_[i] = - node_costs[i];
    }

    //Initialise message structure and neighbourhood cost representation
    const vcl_vector<vcl_vector<vcl_pair<unsigned,unsigned> > >& neighbourhoods=graph_.node_data();
    for (unsigned inode=0; inode<neighbourhoods.size();++inode)
    {
        unsigned nbstates=node_costs_[inode].size();
        belief_[inode].set_size(nbstates);

        double priorb=vcl_log(1.0/double(nbstates));
        belief_[inode].fill(priorb);

        const vcl_vector<vcl_pair<unsigned,unsigned> >& neighbours=neighbourhoods[inode];
        vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator neighIter=neighbours.begin();
        vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator neighIterEnd=neighbours.end();
        while (neighIter != neighIterEnd)
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
                vcl_string msg("Graph inconsistency in mmn_lbp_solver::operator()\n");
                vcl_ostringstream os;
                os <<"Source node is "<<inode<<" but arc to alleged neighbour joins nodes "<<v1<<"\t to "<<v2<<'\n';
                msg+= os.str();
                vcl_cerr<<msg<<vcl_endl;
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
            unsigned nstates=linkCosts.cols();
            double dnstates=double(nstates);
            messages_[inode][neighIter->first].set_size(nstates);
            messages_[inode][neighIter->first].fill(vcl_log(1.0/dnstates)); //set all initial messages to uniform prob

            ++neighIter; //next neighbour of this node
        }
    } //next node

    messages_upd_ = messages_;

    //Now keep repeating message passing
    vcl_vector<unsigned > random_indices(nnodes_,0);
    mbl_stl_increments(random_indices.begin(),random_indices.end(),0);

    do
    {
        max_delta_=-1.0;
        switch (msg_upd_mode_)
        {
            case eALL_PARALLEL:
            {
                //Calculate all updates in parallel using only previous iteration messages
                for (unsigned inode=0; inode<nnodes_;++inode)
                {
                    update_messages_to_neighbours(inode,node_costs_[inode]);
                }
                messages_ = messages_upd_;
            }
            break;

            case eRANDOM_SERIAL:
            default:
            {
                vcl_random_shuffle(random_indices.begin(),random_indices.end());
                //Randomise the order of inter-node messages
                //May help avoid looping
                for (unsigned knode=0; knode<nnodes_;++knode)
                {
                    unsigned inode=random_indices[knode];
                    update_messages_to_neighbours(inode,node_costs_[inode]);
                    messages_[inode] = messages_upd_[inode]; //immediate update for this node
                }
            }
        }

        if (verbose_)
        {
            vcl_cout<<"Max message delta at iteration "<<count_<<"\t is "<<max_delta_<<vcl_endl;
        }
        //Now calculate belief levels of each node's states
        calculate_beliefs(x);
    }
    while (continue_propagation(x));

    //Now calculate final belief levels of each node's states and select the maximising ones
    calculate_beliefs(x);

    for (unsigned inode=0; inode<nnodes_;++inode)
    {
        renormalise_log(belief_[inode]);
        for (unsigned i=0; i<belief_[inode].size();i++)
        {
            belief_[inode][i]=vcl_exp(belief_[inode][i]);
        }
    }

    //Return -best solution value (i.e. minimised form)
    //vcl_cout<<"Calculating solution cost..."<<vcl_endl;

    if (!isCycling_)
    {
        return -solution_cost(x);
    }
    else
    {
        double zbest=best_solution_cost_in_history(x);
        if (verbose_)
        {
            vcl_cout<<"Best solution when cycling condition first detected was: "<<zbest_on_cycle_detection_<<vcl_endl
                    <<"Final Best solution : "<<zbest<<vcl_endl;
        }
        return -zbest;
    }
}

void mmn_lbp_solver::calculate_beliefs(vcl_vector<unsigned>& x)
{
    //Now calculate belief levels of each node's states
    //NB calculates log belief actually

    const vcl_vector<vcl_vector<vcl_pair<unsigned,unsigned> > >& neighbourhoods=graph_.node_data();
    for (unsigned inode=0; inode<neighbourhoods.size();++inode)
    {
        unsigned bestState=0;
        double best=-1.0E012;
        unsigned nstates=node_costs_[inode].size();
        for (unsigned istate=0; istate<nstates;++istate)
        {
            double b=node_costs_[inode][istate];
            //Now loop over neighbourhood
            const vcl_vector<vcl_pair<unsigned,unsigned> >& neighbours=graph_.node_data()[inode];
            vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator neighIter=neighbours.begin();
            vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator neighIterEnd=neighbours.end();
            while (neighIter != neighIterEnd)
            {
                vnl_vector<double>& msgsFromNeigh = messages_[neighIter->first][inode];
                b+= msgsFromNeigh[istate];
                ++neighIter;
            }
            belief_[inode][istate]=b;
            if (b>best)
            {
                best=b;
                bestState=istate;
            }
        }
        x[inode]=bestState;

        renormalise_log(belief_[inode]);
    }
}

double mmn_lbp_solver::solution_cost(vcl_vector<unsigned>& x)
{
    //: Calculate best (max log prob) of solution x
    double sumNodes=0.0;
    //Sum over all nodes
    vcl_vector<vnl_vector<double> >::const_iterator nodeIter=node_costs_.begin();
    vcl_vector<vnl_vector<double> >::const_iterator nodeIterEnd=node_costs_.end();
    vcl_vector<unsigned >::const_iterator stateIter=x.begin();
    while (nodeIter != nodeIterEnd)
    {
        const vnl_vector<double>& ncosts = *nodeIter;
        sumNodes+=ncosts[*stateIter];
        ++nodeIter;++stateIter;
    }

    // Sum over all arcs
    vcl_vector<mmn_arc>::const_iterator arcIter=arcs_.begin();
    vcl_vector<mmn_arc>::const_iterator arcIterEnd=arcs_.end();
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

double mmn_lbp_solver::best_solution_cost_in_history(vcl_vector<unsigned>& x)
{
    double zbest=solution_cost(x);
    vcl_vector<double> solution_vals(soln_history_.size());
    vcl_deque<vcl_vector<unsigned> >::iterator xIter=soln_history_.begin();
    vcl_deque<vcl_vector<unsigned> >::iterator xIterEnd=soln_history_.end();
    vcl_deque<vcl_vector<unsigned> >::iterator xIterBest=soln_history_.end()-1;
    while (xIter != xIterEnd)
    {
        double z = solution_cost(*xIter);
        if (z>zbest)
        {
            zbest=z;
            xIterBest=xIter;
        }
        ++xIter;
    }
    x=*xIterBest;
    return zbest;
}

void mmn_lbp_solver::update_messages_to_neighbours(unsigned inode,
                                                   const vnl_vector<double>& node_cost)
{
    //Update all messages from this node to its neighbours

    const vcl_vector<vcl_pair<unsigned,unsigned> >& neighbours=graph_.node_data()[inode];

    vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator neighIter=neighbours.begin();
    vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator neighIterEnd=neighbours.end();
    while (neighIter != neighIterEnd) //Loop over all my neighbours
    {
        vnl_vector<double>& msgsToNeigh = messages_[inode][neighIter->first];
        vnl_matrix<double>& linkCosts = arc_costs_[inode][neighIter->first];
        unsigned nTargetStates=msgsToNeigh.size();
        unsigned nSrcStates=linkCosts.rows(); //number of source states for this node
        if (nSrcStates!=node_cost.size())
        {
            vcl_string msg("Inconsistent array sizes in mmn_lbp_solver::update_messages_to_neighbours\n");
            msg+= "Inconsistent array sizes in mmn_lbp_solver::update_messages_to_neighbours ";

            vcl_cerr<<msg<<vcl_endl;
            throw mbl_exception_abort(msg);
        }
        for (unsigned jstate=0;jstate<nTargetStates;++jstate) //do each state of the target neighbour
        {
            double max_istates= -1E99; // minus infinity, as initialisation for a maximum
            for (unsigned istate=0; istate<nSrcStates;++istate)
            {
                double logProdIncoming=0.0;
                //Compute product of all incoming messages to this node i from elsewhere (excluding target node j)
                vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator tomeIter=neighbours.begin();
                vcl_vector<vcl_pair<unsigned,unsigned> >::const_iterator tomeIterEnd=neighbours.end();
                while (tomeIter != tomeIterEnd)
                {
                    if (tomeIter != neighIter)
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
            if (cycle_detection_count_>0 && smooth_on_cycling_)
            {
                messages_upd_[inode][neighIter->first][jstate]=alpha_*max_istates+(1.0-alpha_)*messages_[inode][neighIter->first][jstate];
            }
            else
            {
                messages_upd_[inode][neighIter->first][jstate]=max_istates;
            }
        }
        renormalise_log(messages_upd_[inode][neighIter->first]);
#if 0
        vcl_cout<<"Iteration "<<count_<<"Msg Upd Node\t"<<inode<<"\tto node "<<neighIter->first<<'\t';

        vcl_copy(messages_upd_[inode][neighIter->first].begin(),
                 messages_upd_[inode][neighIter->first].end(),
                 vcl_ostream_iterator<double>(vcl_cout,"\t"));
        vcl_cout<<vcl_endl
                <<"Iteration "<<count_<<"Msg Prv Node\t"<<inode<<"\tto node "<<neighIter->first<<'\t';

        vcl_copy(messages_[inode][neighIter->first].begin(),
                 messages_[inode][neighIter->first].end(),
                 vcl_ostream_iterator<double>(vcl_cout,"\t"));

        vcl_cout<<vcl_endl<<vcl_endl;
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
    while (stateIter != stateIterEnd)
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

bool mmn_lbp_solver::continue_propagation(vcl_vector<unsigned>& x)
{
    ++count_;
    bool retstate=true;
    if (max_delta_<epsilon_ || count_>max_iterations_)
    {
        //Terminate on either convergence or max iteration count reached
        retstate = false;
    }
    else if (count_ < min_simple_iterations_)
    {
        //always do at least this many if not converged in delta
        retstate = true;
    }
    else if (cycle_detection_count_<2 &&
             vcl_count_if(max_delta_history_.begin(),max_delta_history_.end(),
                          vcl_bind1st(vcl_less<double >(),max_delta_))
             == int(max_delta_history_.size()))
    {
        retstate =true; //delta is definitely decreasing so keep going unless we've had >2 cycles already
    }
    else
    {
        isCycling_=false;
        //Check for cycling condition
        vcl_deque<vcl_vector<unsigned  > >::iterator finder=vcl_find(soln_history_.begin(),soln_history_.end(),x);
        if (finder != soln_history_.end())
        {
            ++nrevisits_;
        }
        else
        {
            nrevisits_=0;
        }
        if (nrevisits_>NCYCLE_DETECT_)
        {
            isCycling_=true;
            ++cycle_detection_count_;
            vcl_cout<<"!!!! Loopy Belief is CYCLING... "<<vcl_endl;
        }
        if (isCycling_)
        {
            if (cycle_detection_count_==1)
            {
                vcl_vector<unsigned > xdummy=x;
                zbest_on_cycle_detection_=best_solution_cost_in_history(xdummy);
            }
            if (smooth_on_cycling_ && cycle_detection_count_<max_cycle_detection_count_)
            {
                nrevisits_=0;
                vcl_cout<<"Initiating message alpha smoothing to try and break cycling..."<<vcl_endl;
                soln_history_.clear();
            }
            else
            {
                vcl_cout<<"Abort and pick best solution in history."<<vcl_endl;
                retstate= false;
            }
        }
    }

    max_delta_history_.push_back(max_delta_);
    if (max_delta_history_.size()>NHISTORY_)
    {
        max_delta_history_.pop_front();
    }
    soln_history_.push_back(x);
    if (soln_history_.size()>NHISTORY_)
    {
        soln_history_.pop_front();
    }

    return retstate;
}


//=======================================================================
// Method: set_from_stream
//=======================================================================
//: Initialise from a string stream
bool mmn_lbp_solver::set_from_stream(vcl_istream &is)
{
  // Cycle through stream and produce a map of properties
  vcl_string s = mbl_parse_block(is);
  vcl_istringstream ss(s);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  // No properties expected.

  // Check for unused props
  mbl_read_props_look_for_unused_props(
      "mmn_lbp_solver::set_from_stream", props, mbl_read_props_type());
  return true;
}


//=======================================================================
// Method: version_no
//=======================================================================

short mmn_lbp_solver::version_no() const
{
  return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mmn_lbp_solver::is_a() const
{
  return vcl_string("mmn_lbp_solver");
}

//: Create a copy on the heap and return base class pointer
mmn_solver* mmn_lbp_solver::clone() const
{
  return new mmn_lbp_solver(*this);
}

//=======================================================================
// Method: print
//=======================================================================

void mmn_lbp_solver::print_summary(vcl_ostream& os) const
{
    os<<"This is a "<<is_a()<<'\t'<<"with "<<nnodes_<<" nodes"<<vcl_endl;
}

//=======================================================================
// Method: save
//=======================================================================

void mmn_lbp_solver::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
}

//=======================================================================
// Method: load
//=======================================================================

void mmn_lbp_solver::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
    case (1):
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&)\n"
               << "           Unknown version number "<< version << vcl_endl;
      bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}

