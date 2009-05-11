
#ifndef TEST_DIRECTION_HPP
#define TEST_DIRECTION_HPP

#include <algorithm>
#include <boost/range.hpp>

/** @name Test Out-Directed Graph
 * Test all graphs that have directed out edges.
 */
//@{
template <typename Graph, typename VertexSet>
void test_outdirected_graph(Graph const& g, VertexSet const& verts, boost::mpl::true_) {
    typedef typename boost::graph_traits<Graph>::out_edge_iterator OutIter;
    typedef std::pair<OutIter, OutIter> OutRange;
    typedef std::vector<OutRange> OutSet;

    // Collect all of the out edge ranges from the graph.
    OutSet outs(verts.size());
    for(size_t i = 0; i < verts.size(); ++i) {
        outs[i] = boost::out_edges(verts[i], g);
    }

    BOOST_ASSERT(boost::distance(outs[0]) == 0);
    BOOST_ASSERT(boost::distance(outs[1]) == 1);
    BOOST_ASSERT(boost::distance(outs[2]) == 1);
    BOOST_ASSERT(boost::distance(outs[3]) == 2);
    BOOST_ASSERT(boost::distance(outs[4]) == 2);
    BOOST_ASSERT(boost::distance(outs[5]) == 1);

    // Verify that the edges are actually correct.
    // TODO: Find a better way of testing connectivity with multiple edges.
    BOOST_ASSERT(has_target(g, *outs[1].first, verts[0]));
    BOOST_ASSERT(has_target(g, *outs[2].first, verts[1]));
    // BOOST_ASSERT(has_target(g, *outs[3].first++, verts[4]));
    // BOOST_ASSERT(has_target(g, *outs[3].first, verts[2]));
    // BOOST_ASSERT(has_target(g, *outs[3].first++, verts[4]));
    // BOOST_ASSERT(has_target(g, *outs[3].first, verts[2]));
    BOOST_ASSERT(has_target(g, *outs[5].first, verts[3]));
}

template <typename Graph, typename VertexSet>
void test_outdirected_graph(Graph const& g, VertexSet const& verts, boost::mpl::false_)
{ }
//@}

/** @name Test In-Directed Graph
 * Test all graphs that support in-directed edges.
 */
//@{
template <typename Graph, typename VertexSet>
void test_indirected_graph(Graph const& g, VertexSet const& verts, boost::mpl::true_) {
    typedef typename boost::graph_traits<Graph>::in_edge_iterator InIter;
    typedef std::pair<InIter, InIter> InRange;
    typedef std::vector<InRange> InSet;

    // Collect all of the in edges from the graph.
    InSet ins(verts.size());
    for(size_t i = 0; i < verts.size(); ++i) {
        ins[i] = boost::in_edges(verts[i], g);
    }

    BOOST_ASSERT(distance(ins[0]) == 2);
    BOOST_ASSERT(distance(ins[1]) == 2);
    BOOST_ASSERT(distance(ins[2]) == 1);
    BOOST_ASSERT(distance(ins[3]) == 1);
    BOOST_ASSERT(distance(ins[4]) == 1);
    BOOST_ASSERT(distance(ins[5]) == 0);

    // Verify that the connected edges are correct.
    // TODO: Find a better way of testing connectivity with multiple edges.
    BOOST_ASSERT(has_source(g, *ins[2].first, verts[3]));
    BOOST_ASSERT(has_source(g, *ins[3].first, verts[5]));
    BOOST_ASSERT(has_source(g, *ins[4].first, verts[3]));
}

template <typename Graph, typename VertexSet>
void test_indirected_graph(Graph const& g, VertexSet const& verts, boost::mpl::false_)
{ }
//@}

/** @name Undirected Graphs
 * Test all graphs that have undirected edges.
 */
template <typename Graph, typename VertexSet>
void test_undirected_graph(Graph const& g, VertexSet const& verts, boost::mpl::true_) {
    typedef typename boost::graph_traits<Graph>::out_edge_iterator OutIter;
    typedef std::pair<OutIter, OutIter> OutRange;
    typedef std::vector<OutRange> OutSet;

    // The set of out edges is the same as the set of incident edges.
    OutSet outs(verts.size());
    for(size_t i = 0; i < verts.size(); ++i) {
        outs[i] = boost::out_edges(verts[i], g);
    }

    // TODO: Actually test the end connections to ensure that these are
    // definitely correct.
    BOOST_ASSERT(boost::distance(outs[0]) == 2);
    BOOST_ASSERT(boost::distance(outs[1]) == 3);
    BOOST_ASSERT(boost::distance(outs[2]) == 2);
    BOOST_ASSERT(boost::distance(outs[3]) == 3);
    BOOST_ASSERT(boost::distance(outs[4]) == 3);
    BOOST_ASSERT(boost::distance(outs[5]) == 1);
}

template <typename Graph, typename VertexSet>
void test_undirected_graph(Graph const& g, VertexSet const& verts, boost::mpl::false_)
{ }
//@}

#endif
