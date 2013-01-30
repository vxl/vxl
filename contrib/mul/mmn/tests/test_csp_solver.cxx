// This is mul/mmn/tests/test_csp_solver.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <mmn/mmn_graph_rep1.h>
#include <mmn/mmn_csp_solver.h>
#include <mmn/mmn_dp_solver.h>
#include <mmn/mmn_lbp_solver.h>
#include <vcl_algorithm.h>
#include <vcl_numeric.h>
#include <vcl_iterator.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

#include <vpdfl/vpdfl_gaussian.h>
#include <vpdfl/vpdfl_gaussian_builder.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <pdf1d/pdf1d_flat.h>
#include <pdf1d/pdf1d_sampler.h>
#include <vpdfl/vpdfl_sampler_base.h>

namespace test_csp_bits
{
    struct point_data
    {
        vgl_point_2d<double> loc;
        double amplitude; //DP "value" is a function of this
    };
    void convert_to_minus_log_probs(vcl_vector<vnl_vector<double> >& node_cost);
};

using namespace test_csp_bits;
void test_csp_bits::convert_to_minus_log_probs(vcl_vector<vnl_vector<double> >& node_cost)
{
    for (unsigned i=0; i<node_cost.size();++i)
    {
        double sum=vcl_accumulate(node_cost[i].begin(),
                                  node_cost[i].end(),
                                  0.0);
        node_cost[i]/=sum;
        for (unsigned j=0; j<node_cost[i].size();j++)
        {
            node_cost[i][j] = -vcl_log(node_cost[i][j]);
        }
    }
}


void test_csp_solver_a()
{
    vcl_cout<<"==== test test_csp_solver (reversed chain with one loop) ====="<<vcl_endl;

    unsigned n=5;
    // Generate linked list
    vcl_vector<mmn_arc> arcs(n-1);
    for (unsigned i=0;i<n-1;++i)
        arcs[i]=mmn_arc(i+1,i);

    arcs.push_back(mmn_arc(0,n-1));
    vcl_cout<<"Set up trivial problem. Optimal node=i, pair_costs all flat"<<vcl_endl;

    vcl_vector<mmn_csp_solver::label_subset_t > node_labels_subset(n);
    for (unsigned i=0; i<n;i++)
    {
        node_labels_subset[i].insert(i);
        node_labels_subset[i].insert(0);
        node_labels_subset[i].insert(1);
        node_labels_subset[i].insert(4+i);
    }

    vcl_vector<mmn_csp_solver::arc_labels_subset_t > links_subset(arcs.size());
    for (unsigned a=0;a<arcs.size();++a)
    {
        unsigned v1=arcs[a].min_v();
        unsigned v2=arcs[a].max_v();
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(v1,v2)); //ensure consistency
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(3+v1,3+v2));
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(2,1));
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(1,2));
    }

    mmn_csp_solver solver(n,arcs);
    solver.set_verbose(true);
    vcl_cout<<"Run CSP solver."<<vcl_endl;
    bool arcConsistent=solver(node_labels_subset,links_subset);

    TEST("CSP is Arc Consistent",arcConsistent,true);

    const vcl_vector<mmn_csp_solver::label_subset_t >& kernel_node_labels=solver.kernel_node_labels();
    assert(kernel_node_labels.size()==n);
    for (unsigned i=0; i<n;i++)
    {
        vcl_cout<<vcl_endl<<"Node "<<i<<vcl_endl;
        TEST("Unique label on each node ",kernel_node_labels[i].size(),1);
        if (!kernel_node_labels[i].empty())
        {
            TEST("Correct max label on each node",(*(kernel_node_labels[i].begin())),i);
            if (kernel_node_labels[i].size()>1)
            {
                vcl_cout<<"Dumping all labels for node "<<i<<vcl_endl;
                vcl_copy(kernel_node_labels[i].begin(),kernel_node_labels[i].end(),
                         vcl_ostream_iterator<unsigned >(vcl_cout,"\t"));
                vcl_cout<<vcl_endl;
            }
        }
        else
        {
            vcl_cout<<"kernel_node_labels is empty"<<vcl_endl;
        }
    }

    unsigned middle=3;
    node_labels_subset[middle].erase(middle);
    vcl_cout<<"Now erase the middle nodes max label to create inconsistent problem"<<vcl_endl;
    arcConsistent=solver(node_labels_subset,links_subset);

    TEST("CSP is now NOT Arc Consistent",arcConsistent,false);
    const vcl_vector<mmn_csp_solver::label_subset_t >& kernel_node_labels2=solver.kernel_node_labels();
    assert(kernel_node_labels2.size()==n);
    for (unsigned i=0; i<n;i++)
    {
        TEST("Empty label on each node ",kernel_node_labels2[i].empty(),true);
    }
}


void test_csp_solver_loop_b(unsigned n)
{
    vcl_cout<<"==== test test_csp_solver (loop) ====="<<vcl_endl;

    // Generate arcs
    vcl_vector<mmn_arc> arcs(n);
    for (unsigned i=0;i<n;++i)
        arcs[i]=mmn_arc(i,(i+1)%n);

    vcl_vector<mmn_csp_solver::label_subset_t > node_labels_subset(n);
    for (unsigned i=0; i<n;i++)
    {
        node_labels_subset[i].insert(i);
        node_labels_subset[i].insert(0);
        node_labels_subset[i].insert(1);
        node_labels_subset[i].insert(5+i);
    }

    vcl_vector<mmn_csp_solver::arc_labels_subset_t > links_subset(arcs.size());
    for (unsigned a=0;a<arcs.size();++a)
    {
        unsigned v1=arcs[a].min_v();
        unsigned v2=arcs[a].max_v();
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(v1,v2)); //ensure consistency
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(3+v1,3+v2));
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(2,1));
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(1,2));
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(999,0)); //definitely invalid
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(0,999));
    }

    mmn_csp_solver solver(n,arcs);
    solver.set_verbose(true);
    vcl_cout<<"Run CSP solver."<<vcl_endl;
    bool arcConsistent=solver(node_labels_subset,links_subset);

    TEST("CSP is Arc Consistent",arcConsistent,true);

    const vcl_vector<mmn_csp_solver::label_subset_t >& kernel_node_labels=solver.kernel_node_labels();
    assert(kernel_node_labels.size()==n);
    for (unsigned i=0; i<n;i++)
    {
        TEST("Unique label on each node ",kernel_node_labels[i].size(),1);
        TEST("Correct max label on each node",(*(kernel_node_labels[i].begin())),i);
    }

    unsigned middle=2;
    node_labels_subset[middle].erase(middle);
    vcl_cout<<"Now erase the middle nodes arc to create inconsistent problem"<<vcl_endl;
    arcConsistent=solver(node_labels_subset,links_subset);
    TEST("now NOT Arc Consistent",arcConsistent,false);
    {
        unsigned v1=arcs[middle].min_v();
        unsigned v2=arcs[middle].max_v();
        links_subset[middle].erase(vcl_pair<unsigned ,unsigned >(v1,v2)); //ensure consistency
    }
    vcl_cout<<"Run CSP solver."<<vcl_endl;
    arcConsistent=solver(node_labels_subset,links_subset);

    TEST("CSP is now NOT Arc Consistent",arcConsistent,false);
    const vcl_vector<mmn_csp_solver::label_subset_t >& kernel_node_labels2=solver.kernel_node_labels();
    assert(kernel_node_labels2.size()==n);
    for (unsigned i=0; i<n;i++)
    {
        TEST("Empty label on each node ",kernel_node_labels2[i].empty(),true);
    }
}

void test_best_xydp_line()
{
    const unsigned NSTAGES=5;
    const unsigned NPOINTS_PER_STAGE=10;
    vcl_cout<<"==== test test_csp_solver best y line ====="<<vcl_endl;

    unsigned n=NSTAGES;
    // Generate linked list
    vcl_vector<mmn_arc> arcs(n-1);
    for (unsigned i=0;i<n-1;++i)
        arcs[i]=mmn_arc(i,i+1);

    vcl_vector<vnl_vector<double> > node_cost(n);
    vcl_vector<vnl_matrix<double> > pair_cost(arcs.size());

    //Create some point data

    //First create some random samplers
    vpdfl_gaussian_builder gbuilder;
    pdf1d_gaussian pdf_amp(10.0,16.0);

    vpdfl_gaussian pdf_data;
    vpdfl_gaussian pdf_model;
    vnl_matrix<double> covar(2,2);
    covar.put(0,0, 200);
    covar.put(1,1, 50.0);
    covar.put(0,1, 0.0);
    covar.put(1,0, 0.0);
    vnl_vector<double> means(2,0.0);
    gbuilder.buildFromCovar(pdf_data,means,covar);

    means[1] = 100.0; //try and separate them by this much in y
    covar.put(0,0, 25.0);
    covar.put(1,1, 4.0);
    gbuilder.buildFromCovar(pdf_model,means,covar);

    vpdfl_sampler_base* loc_sampler = pdf_data.new_sampler();
    pdf1d_sampler* amp_sampler = pdf_amp.new_sampler();

    //Samplers are go......

    //--------- Now loop over all stages and create some raw data, then transform it to input data form

    vcl_vector<vcl_vector<vgl_point_2d<double > > > locations(NSTAGES);

    vcl_vector<point_data> prev_raw_data(NPOINTS_PER_STAGE);
    for (unsigned int istage=0;istage<NSTAGES;++istage)
    {
        vcl_vector<point_data> raw_data(NPOINTS_PER_STAGE);
        vnl_vector<double> amps(NPOINTS_PER_STAGE);
        vnl_vector<double> error(2);
        amp_sampler->get_samples(amps);

        for (unsigned int ipt=0;ipt<NPOINTS_PER_STAGE;++ipt)
        {
            loc_sampler->sample(error);
            raw_data[ipt].amplitude = amps[ipt];
            //place it on a semi random grid, which widens in x as each stage goes up in y
//            raw_data[ipt].loc = vgl_point_2d<double>((100.0-2*double(istage))*double(ipt-5) + error[0],
//                                                    100.0*istage + error[1]);
            raw_data[ipt].loc = vgl_point_2d<double>((100.0-2*(double(ipt)-5.0)  + 4.0*error[0]),
                                                     100.0*istage + error[1]);

            locations[istage].push_back(raw_data[ipt].loc);
        }

        //Raw data points are go.......

        //----------------------Prepare DP stage vector ----------------------------
        node_cost[istage].set_size(NPOINTS_PER_STAGE);
        for (unsigned j=0;j<NPOINTS_PER_STAGE;++j)
        {
            const double STRENGTH_FACTOR=1/20.0;
            double ampprob=1.0-vcl_exp(-raw_data[j].amplitude * STRENGTH_FACTOR);
            if (ampprob<1.0E-8)
                ampprob=1.0E-8;
            node_cost[istage][j]=ampprob;
        }

        if (istage>=1)
        {
            unsigned arcId=istage-1;
            pair_cost[arcId].set_size(NPOINTS_PER_STAGE,NPOINTS_PER_STAGE);
            for (unsigned kprev=0;kprev<NPOINTS_PER_STAGE;++kprev)
            {
                vgl_point_2d<double >& pt=prev_raw_data[kprev].loc;
                for (unsigned k=0;k<NPOINTS_PER_STAGE;++k)
                {
                    vgl_vector_2d<double> d=raw_data[k].loc-pt;
                    vnl_vector<double > delta(2);
                    delta[0] = d.x(); delta[1]=d.y();
                    double linkProb=pdf_model(delta);
                    pair_cost[arcId][kprev][k]=vcl_log(linkProb);
                }
            }
        }
        prev_raw_data=raw_data;
        //This stage is prepared
    }

    convert_to_minus_log_probs(node_cost);

    //Also test using Markov alg
    vcl_vector<vnl_matrix<double  > > pair_costs_neg=pair_cost;

    for (unsigned i=0;i<pair_costs_neg.size();++i)
    {
        for (unsigned j=0; j<pair_costs_neg[i].rows();j++)
        {
            for (unsigned k=0; k<pair_costs_neg[i].cols();k++)
            {
                pair_costs_neg[i](j,k)= -1.0*pair_costs_neg[i](j,k);
            }
        }
    }

    mmn_graph_rep1 graph;
    graph.build(n,arcs);
    vcl_vector<mmn_dependancy> deps;
    graph.compute_dependancies(deps);

    mmn_dp_solver dpSolver;
    dpSolver.set_dependancies(deps,n,graph.max_n_arcs());

    vcl_cout<<"Run DP solver."<<vcl_endl;

    vcl_vector<unsigned> xDP;
    /* double min_costdp = */ dpSolver.solve(node_cost,pair_costs_neg,xDP);

    vcl_vector<mmn_csp_solver::label_subset_t > node_labels_subset(n);
    for (unsigned i=0; i<n;i++)
    {
        node_labels_subset[i].insert(i);
        node_labels_subset[i].insert(0);
        node_labels_subset[i].insert(9);
        node_labels_subset[i].insert(xDP[i]);
    }

    vcl_vector<mmn_csp_solver::arc_labels_subset_t > links_subset(arcs.size());
    for (unsigned a=0;a<arcs.size();++a)
    {
        unsigned v1=arcs[a].min_v();
        unsigned v2=arcs[a].max_v();
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(xDP[v1],xDP[v2])); //ensure consistency
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(3+v1,3+v2));
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(2,1));
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(1,2));
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(999,0)); //definitely invalid
    }

    mmn_csp_solver solver(n,arcs);
    //solver.set_verbose(true);
    vcl_cout<<"Run CSP solver."<<vcl_endl;
    bool arcConsistent=solver(node_labels_subset,links_subset);

    TEST("CSP is Arc Consistent",arcConsistent,true);

    const vcl_vector<mmn_csp_solver::label_subset_t >& kernel_node_labels=solver.kernel_node_labels();
    assert(kernel_node_labels.size()==n);
    for (unsigned i=0; i<n;i++)
    {
        TEST("Unique label on each node ",kernel_node_labels[i].size(),1);
        TEST("Correct max label on each node",(*(kernel_node_labels[i].begin())),xDP[i]);
    }

    unsigned middle=2;
    vcl_cout<<"Now erase the middle nodes arc to create inconsistent problem"<<vcl_endl;
    {
        unsigned v1=arcs[middle].min_v();
        unsigned v2=arcs[middle].max_v();
        links_subset[middle].erase(vcl_pair<unsigned ,unsigned >(xDP[v1],xDP[v2])); //ensure inconsistency
    }
    vcl_cout<<"Run CSP solver."<<vcl_endl;
    arcConsistent=solver(node_labels_subset,links_subset);

    TEST("CSP is now NOT Arc Consistent",arcConsistent,false);
    const vcl_vector<mmn_csp_solver::label_subset_t >& kernel_node_labels2=solver.kernel_node_labels();
    assert(kernel_node_labels2.size()==n);
    for (unsigned i=0; i<n;i++)
    {
        TEST("Empty label on each node ",kernel_node_labels2[i].empty(),true);
    }
}

void test_5x5grid()
{
    const unsigned NSTAGES=5;
    const unsigned NPOINTS_PER_NODE=10;
    vcl_cout<<"==== test test_csp_solver 5x5 grid ====="<<vcl_endl;

    unsigned n=NSTAGES*NSTAGES;
    // Generate linked list
    vcl_vector<mmn_arc> arcs;
    const double DG=100.0;
    for (unsigned iy=0;iy<NSTAGES;++iy)
    {
        for (unsigned ix=0;ix<NSTAGES;++ix)
        {
            unsigned nodeId= NSTAGES*iy+ix;
            unsigned nodeIdRight=nodeId+1;
            unsigned nodeIdAbove=nodeId+NSTAGES;

            if (ix<NSTAGES-1)
            {
                arcs.push_back(mmn_arc(nodeId,nodeIdRight));
            }
            if (iy<NSTAGES-1)
            {
                arcs.push_back(mmn_arc(nodeId,nodeIdAbove));
            }
        }
    }

    vcl_vector<vnl_vector<double> > node_costs(n);
    vcl_vector<vnl_matrix<double> > pair_costs(arcs.size());

    vcl_cout<<"All arcs added, total number of arcs= "<<arcs.size()<<vcl_endl;

    //Create some point data

    //First create some random samplers
    vpdfl_gaussian_builder gbuilder;
    pdf1d_gaussian pdf_amp(10.0,16.0);

    vpdfl_gaussian pdf_data;
    vpdfl_gaussian pdf_model;
    vnl_matrix<double> covar(2,2);
    double sigma = 10.0;
    double var=sigma*sigma;
    covar.put(0,0, 4.0*var);
    covar.put(1,1, 4.0*var);
    covar.put(0,1, 0.0);
    covar.put(1,0, 0.0);
    vnl_vector<double> means(2,0.0);
    gbuilder.buildFromCovar(pdf_data,means,covar);

    covar.put(0,0, var);
    covar.put(1,1, var);
    gbuilder.buildFromCovar(pdf_model,means,covar);

    vpdfl_sampler_base* loc_sampler = pdf_data.new_sampler();
    pdf1d_sampler* amp_sampler = pdf_amp.new_sampler();

    pdf1d_flat uniform_pdf(0.001, 10.0);
    pdf1d_sampler* usampler = uniform_pdf.new_sampler();

    //Samplers are go......

    //--------- Now loop over all grid points and create some raw data, then transform it to input data form

    vcl_vector<vcl_vector<vgl_point_2d<double > > > locations(n);

    for (unsigned int inode=0;inode<n;++inode)
    {
        unsigned ix = inode % NSTAGES;
        unsigned iy = inode / NSTAGES;
        unsigned nodeId=NSTAGES*iy+ix;
        assert(nodeId==inode);

        vcl_vector<point_data> raw_data(NPOINTS_PER_NODE);
        vnl_vector<double> amps(NPOINTS_PER_NODE);
        vnl_vector<double> error(2);
        amp_sampler->get_samples(amps);

        vgl_point_2d<double> gridPoint(DG*double(ix),DG*double(iy));
        node_costs[nodeId].set_size(NPOINTS_PER_NODE);
        locations[nodeId].reserve(NPOINTS_PER_NODE);
        const double STRENGTH_FACTOR=1/20.0;
        for (unsigned int ipt=0;ipt<NPOINTS_PER_NODE;++ipt)
        {
            loc_sampler->sample(error);
            double amplitude = amps[ipt];
            //place it on a semi random grid,

            vgl_point_2d<double > location = vgl_point_2d<double>(gridPoint.x()+error[0],
                                                                  gridPoint.y()+error[1]);

            locations[nodeId].push_back(location);
            double ampprob=1.0-vcl_exp(-amplitude * STRENGTH_FACTOR);
            if (ampprob<1.0E-8)
                ampprob=1.0E-8;

            node_costs[nodeId][ipt]=ampprob;
        }

        //Raw data points are go.......
    }
    vcl_cout<<"Have created points grid and node costs..."<<vcl_endl;

    for (unsigned i=0; i<arcs.size();i++)
    {
        unsigned node1=arcs[i].min_v();
        unsigned node2=arcs[i].max_v();
        unsigned arcId=i;
        pair_costs[arcId].set_size(NPOINTS_PER_NODE,NPOINTS_PER_NODE);
        for (unsigned iLabel=0;iLabel<NPOINTS_PER_NODE;++iLabel)
        {
            for (unsigned jLabel=0;jLabel<NPOINTS_PER_NODE;++jLabel)
            {
                double& pairCost=pair_costs[arcId](iLabel,jLabel);
                vgl_vector_2d<double> d=locations[node2][jLabel]-locations[node1][iLabel];
                vnl_vector<double > delta(2);
                double dgx=0.0;
                double dgy=0.0;
                if (node1 % NSTAGES == node2 % NSTAGES) //x coord same so must be a y arc
                {
                    dgy=100.0;
                }
                else if (node2==node1+1)
                {
                    dgx=100.0;
                }
                else
                {
                    vcl_cout<<"WARNING - inconsistent node numbering on arc "<<i<<"\tlinking\t"<<node1<<'\t'<<node2<<vcl_endl;
                    assert(0);
                }
                delta[0] = d.x()-dgx; delta[1]=d.y()-dgy;
                double linkProb=pdf_model(delta);
                pairCost=vcl_log(linkProb);
            }
        }
    }

    vcl_cout<<"Have computed all node and arc costs"<<vcl_endl;
    convert_to_minus_log_probs(node_costs);

    vcl_vector<vnl_matrix<double  > > pair_costs_neg=pair_costs;

    for (unsigned i=0;i<pair_costs_neg.size();++i)
    {
        for (unsigned j=0; j<pair_costs_neg[i].rows();j++)
        {
            for (unsigned k=0; k<pair_costs_neg[i].cols();k++)
            {
                pair_costs_neg[i](j,k)= -1.0*pair_costs_neg[i](j,k);
            }
        }
    }

    mmn_lbp_solver LBPsolver;
    LBPsolver.set_arcs(n,arcs);

    vcl_cout<<"Run Loopy Belief Solver."<<vcl_endl;

    vcl_vector<unsigned> x;
    /* double min_cost = */ LBPsolver(node_costs,pair_costs_neg,x);

    vnl_vector<double> states(n);
    usampler->get_samples(states);

    vcl_vector<mmn_csp_solver::label_subset_t > node_labels_subset(n);
    for (unsigned i=0; i<n;i++)
    {
        node_labels_subset[i].insert(unsigned(states[i]+0.49999));
        node_labels_subset[i].insert(0);
        node_labels_subset[i].insert(9);
        node_labels_subset[i].insert(x[i]);
    }

    vcl_vector<mmn_csp_solver::arc_labels_subset_t > links_subset(arcs.size());
    for (unsigned a=0;a<arcs.size();++a)
    {
        vnl_vector<double> states(2);

        unsigned v1=arcs[a].min_v();
        unsigned v2=arcs[a].max_v();
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(x[v1],x[v2])); //ensure consistency
        links_subset[a].insert(vcl_pair<unsigned ,unsigned >(9,0));
        for (unsigned k=0;k<5;++k)
        {
            usampler->get_samples(states);
            links_subset[a].insert(vcl_pair<unsigned ,unsigned >(unsigned(states[0]+0.5),unsigned(states[1]+0.5)));
        }
    }

    mmn_csp_solver solver(n,arcs);
    //solver.set_verbose(true);
    vcl_cout<<"Run CSP solver."<<vcl_endl;
    bool arcConsistent=solver(node_labels_subset,links_subset);

    TEST("CSP is Arc Consistent",arcConsistent,true);

    const vcl_vector<mmn_csp_solver::label_subset_t >& kernel_node_labels=solver.kernel_node_labels();
    assert(kernel_node_labels.size()==n);
    for (unsigned i=0; i<n;i++)
    {
        TEST("Unique label on each node ",kernel_node_labels[i].size(),1);
        TEST("Correct max label on each node",(*(kernel_node_labels[i].begin())),x[i]);
    }

    //------------------------------------ Erase middle arc and test for consistency
    unsigned middle=2;
    vcl_cout<<"Now erase the middle nodes arc to create inconsistent problem"<<vcl_endl;
    {
        unsigned v1=arcs[middle].min_v();
        unsigned v2=arcs[middle].max_v();
        links_subset[middle].erase(vcl_pair<unsigned ,unsigned >(x[v1],x[v2])); //ensure inconsistency
        if (x[v1] != x[v2])
            links_subset[middle].insert(vcl_pair<unsigned ,unsigned >(x[v2],x[v1])); //
    }

    vcl_cout <<"Now retest for inconsistency with one middle arc removed"<<vcl_endl;
    arcConsistent=solver(node_labels_subset,links_subset);

    TEST("CSP is now NOT Arc Consistent",arcConsistent,false);
    const vcl_vector<mmn_csp_solver::label_subset_t >& kernel_node_labels2=solver.kernel_node_labels();
    assert(kernel_node_labels2.size()==n);
    for (unsigned i=0; i<n;i++)
    {
        TEST("Empty label on each node ",kernel_node_labels2[i].empty(),true);
    }

    //----------------------- remove node label and test for inconsistency
    {
        unsigned v1=arcs[middle].min_v();
        unsigned v2=arcs[middle].max_v();
        links_subset[middle].insert(vcl_pair<unsigned ,unsigned >(x[v1],x[v2])); //ensure consistency in arcs
        node_labels_subset[v2].erase(x[v2]); //But now erase target node state
    }

    vcl_cout <<"Now retest for inconsistency with one target node label removed"<<vcl_endl;
    arcConsistent=solver(node_labels_subset,links_subset);

    TEST("CSP is now NOT Arc Consistent",arcConsistent,false);
    {
        const vcl_vector<mmn_csp_solver::label_subset_t >& kernel_node_labels2=solver.kernel_node_labels();
        assert(kernel_node_labels2.size()==n);
        for (unsigned i=0; i<n;i++)
        {
            TEST("Empty label on each node ",kernel_node_labels2[i].empty(),true);
        }
    }
}

void test_csp_solver()
{
    test_csp_solver_a();
    test_csp_solver_loop_b(7);
    test_best_xydp_line();
    test_5x5grid();
}

TESTMAIN(test_csp_solver);
