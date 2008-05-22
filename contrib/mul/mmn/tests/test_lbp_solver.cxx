// This is mul/mmn/tests/test_lbp_solver.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <mmn/mmn_graph_rep1.h>
#include <mmn/mmn_lbp_solver.h>
#include <mmn/mmn_dp_solver.h>
#include <vcl_algorithm.h>
#include <vcl_numeric.h>
#include <vcl_functional.h>
#include <vcl_iterator.h>
#include <vcl_cmath.h>
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
    for(unsigned i=0; i<node_cost.size();++i)
    {
        double sum=vcl_accumulate(node_cost[i].begin(),
                                  node_cost[i].end(),
                                  0.0);
        node_cost[i]/=sum;
        for(unsigned j=0; j<node_cost[i].size();j++)
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
    for(unsigned i=0; i<node_cost.size();i++)
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
    for(unsigned i=0; i<node_cost.size();i++)
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
    vcl_cout<<"LBP Solver Iteration Count Is: "<<solver.count()<<vcl_endl;
    vcl_cout<<"LOOP GRAPH SOLUTION IS:"<<vcl_endl;
    vcl_copy(x.begin(),x.end(),vcl_ostream_iterator<unsigned>(vcl_cout,"\t"));
    vcl_cout<<vcl_endl;
    double cost=0.0;
    for(unsigned i=0; i<node_cost.size();i++)
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
    for(int istage=0;istage<NSTAGES;istage++)
    {
        vcl_vector<point_data> raw_data(NPOINTS_PER_STAGE);
        vnl_vector<double> amps(NPOINTS_PER_STAGE);
        vnl_vector<double> error(2);
        amp_sampler->get_samples(amps);

        for(int ipt=0;ipt<NPOINTS_PER_STAGE;ipt++)
        {
            loc_sampler->sample(error);
            raw_data[ipt].amplitude = amps[ipt];
            //place it on a semi random grid, which widens in x as each stage goes up in y
//            raw_data[ipt].loc = vgl_point_2d<double>((100.0-2*double(istage))*double(ipt-5) + error[0], 
//                                                    100.0*istage + error[1]);
            raw_data[ipt].loc = vgl_point_2d<double>((100.0-2*double(ipt-5)  + 4.0*error[0]), 
                                                     100.0*istage + error[1]);
            
            locations[istage].push_back(raw_data[ipt].loc);

        }

        //Raw data points are go.......
        
        //----------------------Prepare DP stage vector ----------------------------
        node_cost[istage].set_size(NPOINTS_PER_STAGE);
        for(unsigned j=0;j<NPOINTS_PER_STAGE;++j)
        {
            const double STRENGTH_FACTOR=1/20.0;
            node_cost[istage][j]=1.0-vcl_exp(-raw_data[j].amplitude * STRENGTH_FACTOR);
            
        }
         
        if(istage>=1)
        {
            unsigned arcId=istage-1;
            pair_cost[arcId].set_size(NPOINTS_PER_STAGE,NPOINTS_PER_STAGE);
            for(unsigned kprev=0;kprev<NPOINTS_PER_STAGE;++kprev)
            {
                vgl_point_2d<double >& pt=prev_raw_data[kprev].loc;
                for(unsigned k=0;k<NPOINTS_PER_STAGE;++k)
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

    for(unsigned i=0;i<pair_costs_neg.size();++i)
    {
        for(unsigned j=0; j<pair_costs_neg[i].rows();j++)
        {
            for(unsigned k=0; k<pair_costs_neg[i].cols();k++)
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
        vcl_cout<<"x["<<i<<"]="<<x[i]<<"\t"<<locations[i][x[i]]<<"\t prior prob: "<<vcl_exp(node_cost[i][x[i]])<<vcl_endl;
    }
    

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
}

TESTMAIN(test_lbp_solver);
