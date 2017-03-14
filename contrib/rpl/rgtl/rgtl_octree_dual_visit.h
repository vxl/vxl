/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_octree_dual_visit_h
#define rgtl_octree_dual_visit_h

//:
// \file
// \brief Octree dual cell visitation function.
// \author Brad King
// \date March 2007

#include "rgtl_octree_dual.h"
#include "rgtl_octree_dual_element.h"

namespace rgtl
{
  namespace detail
  {
    namespace octree_dual
    {
      // Hold arguments for one procedure call.
      template <typename Visitor, unsigned int d, unsigned int m>
      class procedure_args: public rgtl_octree_dual_element_oriented<
        Visitor::tree_type::dimension, (Visitor::tree_type::dimension-d), m>
      {
      public:
        typedef rgtl_octree_dual_element_oriented<
          Visitor::tree_type::dimension, (Visitor::tree_type::dimension-d), m>
          derived;
        typedef typename Visitor::tree_type tree_type;

        procedure_args(Visitor& v): visitor_(v), flags_(0) {}

        template <unsigned int h, unsigned int l>
          procedure_args(procedure_args<Visitor, h, l> const& in):
          visitor_(in.visitor()), flags_(0)
          {
          RGTL_STATIC_ASSERT(h >= d); RGTL_STATIC_ASSERT_USE();
          }

        tree_type const& tree() const { return visitor_.tree(); }
        Visitor& visitor() const { return visitor_; }
        void visit() const { visitor_(*static_cast<derived const*>(this)); }

        bool is_leaf(unsigned int j) const { return (flags_>>j)&1; }
        void set_leaf(unsigned int j) { flags_ |= (1<<j); }
        bool all_leaves() const { return flags_ == ((1<<derived::num_primal)-1); }
      private:
        Visitor& visitor_;
        unsigned int flags_;
        RGTL_STATIC_ASSERT(rgtl::meta::bits_c<m>::value ==
                           (tree_type::dimension-d));
      };

      // Construct arguments for recursive calls.
      template <typename Visitor,
                unsigned int d, unsigned int m,
                unsigned int k, unsigned int n>
      struct get_arg
      {
        typedef procedure_args<Visitor, d, m> in_args_type;
        typedef procedure_args<Visitor, k, n> out_args_type;
        typedef typename Visitor::tree_type tree_type;
        typedef typename tree_type::child_index_type child_index_type;
        typedef in_args_type const& arg1_type;
        typedef out_args_type& arg2_type;
        template <unsigned int outarg, unsigned int inarg, unsigned int child>
        struct apply
        {
          static void invoke(arg1_type in, arg2_type out)
            {
            if(in.is_leaf(inarg))
              {
              // The input cell is a leaf.  Just use it directly in
              // the output.
              out.primal_location(outarg) = in.primal_location(inarg);
              out.primal_index(outarg) = in.primal_index(inarg);
              out.set_leaf(outarg);
              }
            else
              {
              // The input cell is a node.  Get the child requested.
              child_index_type child_index(child);

              // Get the logical child location.
              out.primal_location(outarg) =
                in.primal_location(inarg).get_child(child_index);

              // Get the octree representation index of the child.
              out.primal_index(outarg) =
                in.tree().get_child(in.primal_index(inarg),
                                    child_index_type(child_index));

              // Check whether the child is a leaf.
              if(!out.tree().has_children(out.primal_index(outarg)))
                {
                out.set_leaf(outarg);
                }
              }
            }
        private:
          RGTL_STATIC_ASSERT(outarg < out_args_type::num_primal);
          RGTL_STATIC_ASSERT(inarg < in_args_type::num_primal);
          RGTL_STATIC_ASSERT(child < (1<<tree_type::dimension));
        };
      };

      template <typename Visitor, unsigned int d, unsigned int m>
      struct procedure
      {
        typedef typename Visitor::tree_type tree_type;

        // Procedure interface required by octree_dual::recurse.
        RGTL_STATIC_CONST(unsigned int, value_D = tree_type::dimension);
        RGTL_STATIC_CONST(unsigned int, value_d = d);
        RGTL_STATIC_CONST(unsigned int, value_m = m);
        typedef procedure_args<Visitor, d, m> argument_type;
        template <unsigned int k, unsigned int n> struct other
        { typedef procedure<Visitor, k, n> type; };
        template <unsigned int k, unsigned int n> struct argument_get
        { typedef get_arg<Visitor, d, m, k, n> type; };

        static void invoke(argument_type const& in)
          {
          if(in.all_leaves())
            {
            // All input cells are leaves.  Terminate recursion and
            // call the user dual cell visitor.
            in.visit();
            }
          else
            {
            // At least one input cell is not a leaf.  Recurse.
            rgtl::octree_dual::recurse<procedure>::invoke(in);
            }
          }
      private:
        RGTL_STATIC_ASSERT(rgtl::meta::bits_c<m>::value ==
                           (tree_type::dimension-d));
      };

      // Implementation of rgtl_octree_dual_visit
      template <typename Visitor>
      void visit(Visitor& v)
      {
      typedef typename Visitor::tree_type tree_type;
      procedure_args<Visitor, tree_type::dimension, 0> in(v);
      procedure<Visitor, tree_type::dimension, 0>::invoke(in);
      }

    } // namespace rgtl::detail::octree_dual
  } // namespace rgtl::detail
} // namespace rgtl

//: Visit all dual cells in an octree.
//
// The visitor given must provide the following interface:
//
//  struct Visitor
//  {
//    typedef ... tree_type;
//    tree_type const& tree() const;
//    void operator()(rgtl_octree_dual_element<D, 0> const&);
//                                               ...
//    void operator()(rgtl_octree_dual_element<D, D> const&);
//  };
//
// where D = tree_type::dimension.
template <typename Visitor>
void rgtl_octree_dual_visit(Visitor& v)
{
  rgtl::detail::octree_dual::visit(v);
}

#endif
