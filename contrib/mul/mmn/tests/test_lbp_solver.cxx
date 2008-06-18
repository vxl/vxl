// This is mul/mmn/tests/test_lbp_solver.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <mmn/mmn_graph_rep1.h>
#include <mmn/mmn_lbp_solver.h>
#include <mmn/mmn_dp_solver.h>
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
#include <pdf1d/pdf1d_sampler.h>
#include <vpdfl/vpdfl_sampler_base.h>


struct point_data
{
  vgl_point_2d<double> loc;
  double amplitude; //DP "value" is a function of this
};

void convert_to_minus_log_probs(vcl_vector<vnl_vector<double> >& node_cost)
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

void test_lbp_solver_a()
{
    vcl_cout<<"==== test test_lbp_solver (chain) ====="<<vcl_endl;

    unsigned n=5;
    // Generate linked list
    vcl_vector<mmn_arc> arc(n-1);
    for (unsigned i=0;i<n-1;++i)
        arc[i]=mmn_arc(i,i+1);

    mmn_graph_rep1 graph;
    graph.build(n,arc);
    vcl_vector<mmn_dependancy> deps;
    graph.compute_dependancies(deps);

    vcl_vector<vnl_vector<double> > node_cost(n);
    vcl_vector<vnl_matrix<double> > pair_cost(arc.size());

    vcl_cout<<"Set up trivial problem. Optimal node=i, pair_costs all flat"<<vcl_endl;

    for (unsigned i=0;i<n;++i)
    {
        node_cost[i].set_size(5+i);
        node_cost[i].fill(10);
        node_cost[i][i]=20;
    }

    convert_to_minus_log_probs(node_cost);
    double arc_cost=-vcl_log(0.25);

    for (unsigned a=0;a<arc.size();++a)
    {
        unsigned v1=arc[a].min_v();
        unsigned v2=arc[a].max_v();
        pair_cost[a].set_size(node_cost[v1].size(), node_cost[v2].size());
        pair_cost[a].fill(arc_cost);
    }

    mmn_lbp_solver solver;
    solver.set_arcs(n,arc);

    vcl_cout<<"Run solver."<<vcl_endl;

    vcl_vector<unsigned> x;
    double min_cost = solver(node_cost,pair_cost,x);
    vcl_cout<<"LBP Solver Iteration Count Is: "<<solver.count()<<vcl_endl;
    double cost=0.0;
    for (unsigned i=0; i<node_cost.size();i++)
    {
        cost += node_cost[i][i];
    }
    cost += double(n-1)*arc_cost;
    TEST_NEAR("Optimum value",min_cost,cost,1e-6);
    TEST("Correct number of nodes",x.size(),n);
    for (unsigned i=0;i<n;++i)
    {
        TEST("Correct node value",x[i],i);
        vcl_cout<<"x["<<i<<"]="<<x[i]<<vcl_endl;
    }
}

void test_lbp_solver_b()
{
    vcl_cout<<"==== test test_lbp_solver (reversed chain) ====="<<vcl_endl;

    unsigned n=5;
    // Generate linked list
    vcl_vector<mmn_arc> arc(n-1);
    for (unsigned i=0;i<n-1;++i)
        arc[i]=mmn_arc(i+1,i);

    mmn_graph_rep1 graph;
    graph.build(n,arc);
    vcl_vector<mmn_dependancy> deps;
    graph.compute_dependancies(deps);

    vcl_vector<vnl_vector<double> > node_cost(n);
    vcl_vector<vnl_matrix<double> > pair_cost(arc.size());

    vcl_cout<<"Set up trivial problem. Optimal node=i, pair_costs all flat"<<vcl_endl;
    for (unsigned i=0;i<n;++i)
    {
        node_cost[i].set_size(5+i);
        node_cost[i].fill(10);
        node_cost[i][i]=20;
    }
    convert_to_minus_log_probs(node_cost);

    double arc_cost=-vcl_log(0.25);
    for (unsigned a=0;a<arc.size();++a)
    {
        unsigned v1=arc[a].min_v();
        unsigned v2=arc[a].max_v();
        pair_cost[a].set_size(node_cost[v1].size(), node_cost[v2].size());
        pair_cost[a].fill(arc_cost);
    }

    mmn_lbp_solver solver;
    solver.set_arcs(n,arc);

    vcl_cout<<"Run solver."<<vcl_endl;

    vcl_vector<unsigned> x;
    double min_cost = solver(node_cost,pair_cost,x);
    vcl_cout<<"LBP Solver Iteration Count Is: "<<solver.count()<<vcl_endl;
    double cost=0.0;
    for (unsigned i=0; i<node_cost.size();i++)
    {
        cost += node_cost[i][i];
    }
    cost += double(arc.size())*arc_cost;

    TEST_NEAR("Optimum value",min_cost,cost,1e-6);
    TEST("Correct number of nodes",x.size(),n);
    for (unsigned i=0;i<n;++i)
    {
        TEST("Correct node value",x[i],i);
        vcl_cout<<"x["<<i<<"]="<<x[i]<<vcl_endl;
    }
}

void test_lbp_solver_loop_a(unsigned n)
{
    vcl_cout << "==== test test_lbp_solver (loop) =====\n"
             << n << " nodes." << vcl_endl;

    // Generate arcs
    vcl_vector<mmn_arc> arc(n);
    for (unsigned i=0;i<n;++i)
        arc[i]=mmn_arc(i,(i+1)%n);

    mmn_graph_rep1 graph;
    graph.build(n,arc);
    vcl_vector<mmn_dependancy> deps;
    graph.compute_dependancies(deps);

    vcl_vector<vnl_vector<double> > node_cost(n);
    vcl_vector<vnl_matrix<double> > pair_cost(arc.size());

    vcl_cout<<"Set up trivial problem. Optimal node=i, pair_costs all flat"<<vcl_endl;
    for (unsigned i=0;i<n;++i)
    {
        node_cost[i].set_size(5+i);
        node_cost[i].fill(10);
        node_cost[i][i]=20;
    }
    convert_to_minus_log_probs(node_cost);
    double arc_cost=-vcl_log(0.25);
    for (unsigned a=0;a<arc.size();++a)
    {
        unsigned v1=arc[a].min_v();
        unsigned v2=arc[a].max_v();
        pair_cost[a].set_size(node_cost[v1].size(), node_cost[v2].size());
        pair_cost[a].fill(arc_cost);
    }

    mmn_lbp_solver solver;
    solver.set_arcs(n,arc);

    vcl_cout<<"Run solver."<<vcl_endl;

    vcl_vector<unsigned> x;
    double min_cost = solver(node_cost,pair_cost,x);
    vcl_cout<<"LBP Solver Iteration Count Is: "<<solver.count()<<vcl_endl
            <<"LOOP GRAPH SOLUTION IS:"<<vcl_endl;
    vcl_copy(x.begin(),x.end(),vcl_ostream_iterator<unsigned>(vcl_cout,"\t"));
    vcl_cout<<vcl_endl;
    double cost=0.0;
    for (unsigned i=0; i<node_cost.size();i++)
    {
        cost += node_cost[i][i];
    }
    cost += double(arc.size())*arc_cost;

    TEST_NEAR("Optimum value",min_cost,cost,1e-6);
    TEST("Correct number of nodes",x.size(),n);
    for (unsigned i=0;i<n;++i)
    {
        TEST("Correct node value",x[i],i);
        vcl_cout<<"x["<<i<<"]="<<x[i]<<vcl_endl;
    }
}

void test_lbp_solver_loop_b(unsigned n)
{
    vcl_cout<<"==== test test_lbp_solver (loop) ====="<<vcl_endl;

    // Generate arcs
    vcl_vector<mmn_arc> arc(n);
    for (unsigned i=0;i<n;++i)
        arc[i]=mmn_arc(i,(i+1)%n);

    mmn_graph_rep1 graph;
    graph.build(n,arc);
    vcl_vector<mmn_dependancy> deps;
    graph.compute_dependancies(deps);

    vcl_vector<vnl_vector<double> > node_cost(n);
    vcl_vector<vnl_matrix<double> > pair_cost(arc.size());

    vcl_cout << "Set up trivial problem. Optimal node=i\n"
             << "node_costs all flat"<<vcl_endl;
    for (unsigned i=0;i<n;++i)
    {
        node_cost[i].set_size(5+i);
        node_cost[i].fill(1.0);
    }
    convert_to_minus_log_probs(node_cost);
    double arc_cost_good=-vcl_log(0.5);
    double arc_cost_bad=-vcl_log(0.001);

    for (unsigned a=0;a<arc.size();++a)
    {
        unsigned v1=arc[a].min_v();
        unsigned v2=arc[a].max_v();
        pair_cost[a].set_size(node_cost[v1].size(), node_cost[v2].size());
        pair_cost[a].fill(arc_cost_bad);
        pair_cost[a](v1,v2)=arc_cost_good;
    }

    mmn_lbp_solver solver;
    solver.set_arcs(n,arc);

    vcl_cout<<"Run solver."<<vcl_endl;

    vcl_vector<unsigned> x;
    double min_cost = solver(node_cost,pair_cost,x);
    vcl_cout<<"LBP Solver Iteration Count Is: "<<solver.count()<<vcl_endl;
    double cost=double (arc.size())*arc_cost_good;
    for (unsigned i=0;i<n;++i)
    {
        cost+= node_cost[i][0]; //as node cost is flat just use the zero state
    }
    TEST_NEAR("Optimum value",min_cost,cost,1e-6);
    TEST("Correct number of nodes",x.size(),n);
    for (unsigned i=0;i<n;++i)
    {
        TEST("Correct node value",x[i],i);
        vcl_cout<<"x["<<i<<"]="<<x[i]<<vcl_endl;
    }
}

void test_best_xy_line()
{
    const unsigned NSTAGES=5;
    const unsigned NPOINTS_PER_STAGE=10;
    vcl_cout<<"==== test test_lbp_solver best y line ====="<<vcl_endl;

    unsigned n=NSTAGES;
    // Generate linked list
    vcl_vector<mmn_arc> arc(n-1);
    for (unsigned i=0;i<n-1;++i)
        arc[i]=mmn_arc(i,i+1);

    vcl_vector<vnl_vector<double> > node_cost(n);
    vcl_vector<vnl_matrix<double> > pair_cost(arc.size());

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

    mmn_lbp_solver solver;
    solver.set_arcs(n,arc);

    vcl_cout<<"Run solver."<<vcl_endl;

    vcl_vector<unsigned> x;
    double min_cost = solver(node_cost,pair_costs_neg,x);
    vcl_cout<<"LBP Solver Iteration Count Is: "<<solver.count()<<vcl_endl;
    TEST("Correct number of nodes",x.size(),n);

    mmn_graph_rep1 graph;
    graph.build(n,arc);
    vcl_vector<mmn_dependancy> deps;
    graph.compute_dependancies(deps);

    mmn_dp_solver dpSolver;
    dpSolver.set_dependancies(deps,n,graph.max_n_arcs());

    vcl_cout<<"Run DP solver."<<vcl_endl;

    vcl_vector<unsigned> xDP;
    double min_costdp = dpSolver.solve(node_cost,pair_costs_neg,xDP);
    TEST_NEAR("Optimum value",min_cost,min_costdp,1e-6);
    for (unsigned i=0;i<n;++i)
    {
        TEST("Correct node value",x[i],xDP[i]);
        vcl_cout<<"x["<<i<<"]="<<x[i]<<'\t'<<locations[i][x[i]]<<"\t prior prob: "<<vcl_exp(-node_cost[i][x[i]])<<vcl_endl;
    }
}

void test_5x5grid_easy()
{
    const unsigned NSTAGES=5;
    const unsigned NPOINTS_PER_NODE=10;
    vcl_cout<<"==== test test_lbp_solver 5x5 grid ====="<<vcl_endl;

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
    double twoSigma = 2.0*sigma;
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
                    assert(false);
                }
                delta[0] = d.x()-dgx; delta[1]=d.y()-dgy;
                double linkProb=pdf_model(delta);
                pairCost=vcl_log(linkProb);
            }
        }
    }

    vcl_cout<<"Have computed all node and arc costs"<<vcl_endl;
    convert_to_minus_log_probs(node_costs);

    for (unsigned i=0;i<n;++i)
    {
        vcl_copy(node_costs[i].begin(),node_costs[i].end(),
                 vcl_ostream_iterator<double >(vcl_cout,"\t"));
        vcl_cout<<vcl_endl;
    }
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

    mmn_lbp_solver solver;
    solver.set_arcs(n,arcs);
    solver.set_verbose(true);

    vcl_cout<<"Run solver."<<vcl_endl;

    vcl_vector<unsigned> x;
    double min_cost = solver(node_costs,pair_costs_neg,x);
    vcl_cout<<"LBP Solver Iteration Count Is: "<<solver.count()<<" solution value is "<<min_cost<<vcl_endl;
    TEST("Correct number of nodes",x.size(),n);

    unsigned badCount=0;
    for (unsigned iy=0;iy<NSTAGES;++iy)
    {
        for (unsigned ix=0;ix<NSTAGES;++ix)
        {
            unsigned nodeId=iy*NSTAGES + ix;
            unsigned nodeIdRight=nodeId+1;
            unsigned nodeIdAbove=nodeId+NSTAGES;

            vcl_cout<<"x["<<nodeId<<"]="<<x[nodeId]<<'\t'<<locations[nodeId][x[nodeId]]<<"\t prior prob: "<<vcl_exp(-node_costs[nodeId][x[nodeId]])<<vcl_endl;

            unsigned i0=x[nodeId];
            if (ix<NSTAGES-1)
            {
                unsigned iR=x[nodeIdRight];

                vgl_vector_2d<double> dR=locations[nodeIdRight][iR]-locations[nodeId][i0];
                TEST_NEAR("Grid Point dx to Right", vcl_fabs(dR.x()-DG),0.0,2.0*twoSigma);
                TEST_NEAR("Grid Point dy to Right", vcl_fabs(dR.y()),0.0,2.0*twoSigma);
                if (vcl_fabs(dR.x()-DG)>twoSigma || vcl_fabs(dR.y())>twoSigma)
                {
                    ++badCount;
                }
            }
            if (iy<NSTAGES-1)
            {
                unsigned iA=x[nodeIdAbove];
                vgl_vector_2d<double> dA=locations[nodeIdAbove][iA]-locations[nodeId][i0];
                TEST_NEAR("Grid Point dx to Above", vcl_fabs(dA.x()),0.0,2.0*twoSigma);
                TEST_NEAR("Grid Point dy to Above", vcl_fabs(dA.y()-DG),0.0,2.0*twoSigma);

                if (vcl_fabs(dA.y()-DG)>twoSigma || vcl_fabs(dA.x())>twoSigma)
                {
                    ++badCount;
                }
            }
        }
    }
    vcl_cout<<"Number of unusual grid point separations= "<<badCount<<vcl_endl;
    TEST("Unusual grid point separation count",badCount<3,true);
}

void test_5x5grid_hard()
{
    const unsigned NSTAGES=5;
    const unsigned NPOINTS_PER_NODE=10;
    vcl_cout<<"==== test test_lbp_solver 5x5 grid ====="<<vcl_endl;

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
    vpdfl_gaussian pdf_data_outlier;
    vpdfl_gaussian pdf_model;
    vnl_matrix<double> covar(2,2);
    double sigma = 10.0;
    double twoSigma = 2.0*sigma;
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

    covar.put(0,0, 25.0*var);
    covar.put(0,0, 25.0*var);
    gbuilder.buildFromCovar(pdf_data_outlier,means,covar);

    vpdfl_sampler_base* loc_sampler = pdf_data.new_sampler();
    vpdfl_sampler_base* loc_sampler_outlier = pdf_data.new_sampler();
    pdf1d_sampler* amp_sampler = pdf_amp.new_sampler();


    //Samplers are go......

    //--------- Now loop over all grid points and create some raw data, then transform it to input data form


    unsigned NALL=n*NPOINTS_PER_NODE;
    vcl_vector< vgl_point_2d<double > > locations;
    vcl_vector<double> all_responses;
    all_responses.reserve(NALL);
    locations.reserve(NALL);

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
        node_costs[nodeId].set_size(NALL);
        const double STRENGTH_FACTOR=1/20.0;
        for (unsigned int ipt=0;ipt<NPOINTS_PER_NODE;++ipt)
        {
            if (ipt+2<NPOINTS_PER_NODE)
            //if (true)
                loc_sampler->sample(error);
            else
                loc_sampler_outlier->sample(error);
            double amplitude = amps[ipt];
            //place it on a semi random grid,

            vgl_point_2d<double > location = vgl_point_2d<double>(gridPoint.x()+error[0],
                                                                  gridPoint.y()+error[1]);

            locations.push_back(location);
            double ampprob=1.0-vcl_exp(-amplitude * STRENGTH_FACTOR);
            if (ampprob<1.0E-8)
                ampprob=1.0E-8;
            all_responses.push_back(ampprob);
        }

        //Raw data points are go.......
    }

    assert(all_responses.size()==NALL);
    assert(locations.size()==NALL);
    vcl_cout<<"Have created points grid and node costs...\n"
            <<"Now doing dumb assignment of all responses to every node..."<<vcl_endl;
    for (unsigned int inode=0;inode<n;++inode)
    {
        node_costs[inode].set_size(NALL);
        vcl_copy(all_responses.begin(),all_responses.end(),
                 node_costs[inode].begin());
    }
    for (unsigned i=0; i<arcs.size();i++)
    {
        unsigned node1=arcs[i].min_v();
        unsigned node2=arcs[i].max_v();
        unsigned arcId=i;
        pair_costs[arcId].set_size(NALL,NALL);
        for (unsigned iLabel=0;iLabel<locations.size();++iLabel)
        {
            for (unsigned jLabel=0;jLabel<locations.size();++jLabel)
            {
                double& pairCost=pair_costs[arcId](iLabel,jLabel);
                //if (iLabel==jLabel)
                if (false)
                {
                    pairCost=-1.0E-20;
                }
                else
                {
                    vgl_vector_2d<double> d=locations[jLabel]-locations[iLabel];
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
                        assert(false);
                    }
                    delta[0] = d.x()-dgx; delta[1]=d.y()-dgy;
                    pairCost=pdf_model.log_p(delta);
                }
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

    mmn_lbp_solver solver;
    solver.set_arcs(n,arcs);
    solver.set_smooth_on_cycling(true);
    solver.set_verbose(true);
    vcl_cout<<"Run solver."<<vcl_endl;

    vcl_vector<unsigned> x;
    double min_cost = solver(node_costs,pair_costs_neg,x);
    vcl_cout<<"LBP Solver Iteration Count Is: "<<solver.count()<<" solution value is "<<min_cost<<vcl_endl;
    TEST("Correct number of nodes",x.size(),n);

    unsigned badCount=0;
    for (unsigned iy=0;iy<NSTAGES;++iy)
    {
        for (unsigned ix=0;ix<NSTAGES;++ix)
        {
            unsigned nodeId=iy*NSTAGES + ix;
            unsigned nodeIdRight=nodeId+1;
            unsigned nodeIdAbove=nodeId+NSTAGES;

            vcl_cout<<"x["<<nodeId<<"]="<<x[nodeId]<<'\t'<<locations[x[nodeId]]<<"\t prior prob: "<<vcl_exp(-node_costs[nodeId][x[nodeId]])<<vcl_endl;

            unsigned i0=x[nodeId];
            if (ix<NSTAGES-1)
            {
                unsigned iR=x[nodeIdRight];

                vgl_vector_2d<double> dR=locations[iR]-locations[i0];
                TEST_NEAR("Grid Point dx to Right", vcl_fabs(dR.x()-DG),0.0,2.0*twoSigma);
                TEST_NEAR("Grid Point dy to Right", vcl_fabs(dR.y()),0.0,2.0*twoSigma);
                if (vcl_fabs(dR.x()-DG)>twoSigma || vcl_fabs(dR.y())>twoSigma)
                {
                    ++badCount;
                }
            }
            if (iy<NSTAGES-1)
            {
                unsigned iA=x[nodeIdAbove];
                vgl_vector_2d<double> dA=locations[iA]-locations[i0];
                TEST_NEAR("Grid Point dx to Above", vcl_fabs(dA.x()),0.0,2.0*twoSigma);
                TEST_NEAR("Grid Point dy to Above", vcl_fabs(dA.y()-DG),0.0,2.0*twoSigma);

                if (vcl_fabs(dA.y()-DG)>twoSigma || vcl_fabs(dA.x())>twoSigma)
                {
                    ++badCount;
                }
            }
        }
    }
    vcl_cout<<"Number of unusual grid point separations= "<<badCount<<vcl_endl;
    TEST("Unusual grid point separation count",badCount<3,true);
}

void test_lbp_solver()
{
    test_lbp_solver_a();
    test_lbp_solver_b();
    test_lbp_solver_loop_a(3);
    test_lbp_solver_loop_a(4);
    test_lbp_solver_loop_a(5);
    test_lbp_solver_loop_b(4);
    test_best_xy_line();
    test_5x5grid_easy();
    test_5x5grid_hard();
}

TESTMAIN(test_lbp_solver);
