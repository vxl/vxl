// This is mul/clsfy/clsfy_random_forest.h
#ifndef clsfy_random_forest_h_
#define clsfy_random_forest_h_
//:
// \file
// \brief Binary tree classifier
// \author Martin Roberts
#include <clsfy/clsfy_classifier_base.h>
#include <clsfy/clsfy_binary_tree.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <vcl_iosfwd.h>


//: Forest of clsfy_binary_tree trees
// Note as it uses clsfy_binary_tree this is a 2-state (binary) classifier also
class clsfy_random_forest : public clsfy_classifier_base
{
  public:
    
  //: Constructor
    
    clsfy_random_forest(); 
    
    virtual ~clsfy_random_forest();

    
  //: Return the classification of the given probe vector.
    virtual unsigned classify(const vnl_vector<double> &input) const;

    //: Provides a probability-like value that the input being in each class.
    // output(i) i<nClasses, contains the probability that the input is in class i
    virtual void class_probabilities(vcl_vector<double> &outputs, const vnl_vector<double> &input) const;

    //: This value has properties of a Log likelihood of being in class (binary classifiers only)
    // class probability = exp(logL) / (1+exp(logL))
    virtual double log_l(const vnl_vector<double> &input) const;

    //: The number of possible output classes.
    virtual unsigned n_classes() const {return 1;}

    //: The dimensionality of input vectors.
    virtual unsigned n_dims() const;

    //: Storage version number
    virtual short version_no() const;

    //: Name of the class
    virtual vcl_string is_a() const;

    //: Name of the class
    virtual bool is_class(vcl_string const& s) const;

    //: Create a copy on the heap and return base class pointer
    virtual clsfy_classifier_base* clone() const;

    //: Print class to os
    virtual void print_summary(vcl_ostream& os) const;

    //: Save class to binary file stream
    virtual void b_write(vsl_b_ostream& bfs) const;

    //: Load class from binary file stream
    virtual void b_read(vsl_b_istream& bfs);

    unsigned ntrees() const {return trees_.size();}

    void prune();
    
    //Append the tres of forest2 onto this
    clsfy_random_forest& operator+=(const clsfy_random_forest& forest2);

    //: Merge the sub-forests in the input filenames into a single larger one
    friend void merge_sub_forests(const vcl_vector<vcl_string>& filenames,
                           clsfy_random_forest& large_forest);
    //: Merge the sub-forests pointed to the input vector a single larger one
    friend void merge_sub_forests(const vcl_vector< clsfy_random_forest*>& sub_forests,
                           clsfy_random_forest& large_forest);

    //: Merge the two input forests 
    friend clsfy_random_forest operator+(const clsfy_random_forest& forest1,
                                         const clsfy_random_forest& forest2);


    //: Provides a probability-like value that the input being in each class.
    // output(i) i<nClasses, contains the probability that the input is in class i
    // Do Out of bag estimates, i.e. only use trees whose vector of indices does not include this_indexs
    virtual void class_probabilities_oob(vcl_vector<double> &outputs,
                                         const vnl_vector<double> &input,
                                         const vcl_vector<vcl_vector<unsigned > >& oobIndices,
                                         unsigned this_index) const;
    

    //: Return the classification of the given probe vector using out of bag trees only.
    //See also class_probabilities_oob
    virtual unsigned classify_oob(const vnl_vector<double> &input,
                                  const vcl_vector<vcl_vector<unsigned > >& oobIndices,
                                  unsigned this_index) const;

  private:

    //: The trees in this forest
    vcl_vector<mbl_cloneable_ptr<clsfy_classifier_base> > trees_;

    friend class clsfy_random_forest_builder;
};

#endif // clsfy_random_forest_h_
