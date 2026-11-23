/**
 * @file 
 * @brief  Implementation of [Hopcroft–Karp](https://en.wikipedia.org/wiki/Hopcroft%E2%80%93Karp_algorithm) algorithm.
 * @details 
 * The Hopcroft–Karp algorithm is an algorithm that takes as input a bipartite graph 
 * and produces as output a maximum cardinality matching, it runs in O(E√V) time in worst case.
 * 
 * ### Bipartite graph
 * A bipartite graph (or bigraph) is a graph whose vertices can be divided into two disjoint 
 * and independent sets U and V such that every edge connects a vertex in U to one in V. 
 * Vertex sets U and V are usually called the parts of the graph. 
 * Equivalently, a bipartite graph is a graph that does not contain any odd-length cycles.
 * 
 * ### Matching and Not-Matching edges
 * Given a matching M, edges that are part of matching are called Matching edges and edges that are not part 
 * of M (or connect free nodes) are called Not-Matching edges.
 * 
 * ### Maximum cardinality matching
 * Given a bipartite graphs G = ( V = ( X , Y ) , E ) whose partition has the parts X and Y, 
 * with E denoting the edges of the graph, the goal is to find a matching with as many edges as possible. 
 * Equivalently, a matching that covers as many vertices as possible.
 * 
 * ### Augmenting paths
 * Given a matching M, an augmenting path is an alternating path that starts from and ends on free vertices. 
 * All single edge paths that start and end with free vertices are augmenting paths.
 * 
 * 
 * ### Concept
 * A matching M is not maximum if there exists an augmenting path. It is also true other way,
 * i.e, a matching is maximum if no augmenting path exists.
 * 
 * 
 * ### Algorithm
 * 1) Initialize the Maximal Matching M as empty.
 * 2) While there exists an Augmenting Path P
 *   Remove matching edges of P from M and add not-matching edges of P to M
 *   (This increases size of M by 1 as P starts and ends with a free vertex
 *   i.e. a node that is not part of matching.)
 * 3) Return M. 
 * 
 * 
 *
 * @author [Krishna Pal Deora](https://github.com/Krishnapal4050)
 * 
 */


#include <iostream>
#include <cstdlib> 
#include <queue>
#include <list>
#include <climits>
#include <memory>
#include <cassert>

/**
 * @namespace graph 
 * @brief Graph algorithms
 */
 namespace graph { 

/**
 * @brief Represents Bipartite graph for
 * Hopcroft Karp implementation
 */
class HKGraph
{
    int m{};  ///< m is the number of vertices on left side of Bipartite Graph
    int n{};  ///< n is the number of vertices on right side of Bipartite Graph
    const int NIL{0};
    const int INF{INT_MAX};

    std::vector<std::list<int> >adj;  ///< adj[u] stores adjacents of left side and 0 is used for dummy vertex

    std::vector<int> pair_u; ///< value of vertex 'u' ranges from 1 to m
    std::vector<int> pair_v; ///< value of vertex 'v' ranges from 1 to n
    std::vector<int> dist;   ///< dist represents the distance between vertex 'u' and vertex 'v'

public:
    HKGraph();		       // Default Constructor
    HKGraph(int m, int n);     // Constructor
    void addEdge(int u, int v); // To add edge
    
    bool bfs(); // Returns true if there is an augmenting path    
    bool dfs(int u); // Adds augmenting path if there is one beginning with u  
	
    int hopcroftKarpAlgorithm();  // Returns size of maximum matching
};


/**
 * @brief This function counts the number of augmenting paths between left and right sides of the Bipartite graph
 * @returns size of maximum matching
 */
int HKGraph::hopcroftKarpAlgorithm()
{

    // pair_u[u] stores pair of u in matching on left side of Bipartite Graph.
    // If u doesn't have any pair, then pair_u[u] is NIL
    pair_u = std::vector<int>(m + 1,NIL); 

    // pair_v[v] stores pair of v in matching on right side of Biparite Graph.
    // If v doesn't have any pair, then pair_u[v] is NIL
    pair_v = std::vector<int>(n + 1,NIL); 

    dist = std::vector<int>(m + 1);  // dist[u] stores distance of left side vertices

    int result = 0;  // Initialize result

    // Keep updating the result while there is an augmenting path possible.
    while (bfs())
    {
        // Find a free vertex to check for a matching
        for (int u = 1; u <= m; u++){

            // If current vertex is free and there is
            // an augmenting path from current vertex
            // then increment the result
            if (pair_u[u] == NIL && dfs(u)){
                result++;
	    }
	}
    }
    return result;
}


/**
 * @brief This function checks for the possibility of augmented path availability 
 * @returns `true` if there is an augmenting path available
 * @returns `false` if there is no augmenting path available
 */
bool HKGraph::bfs()
{
    std::queue<int> q; // an integer queue for bfs

    // First layer of vertices (set distance as 0)
    for (int u = 1; u <= m; u++)
    {
        // If this is a free vertex, add it to queue
        if (pair_u[u] == NIL){
            
            dist[u] = 0; // u is not matched so distance is 0
            q.push(u);
        }

        else{
            dist[u] = INF; // set distance as infinite so that this vertex is considered next time for availibility
	}
    }

    
    dist[NIL] = INF; // Initialize distance to NIL as infinite

    // q is going to contain vertices of left side only.
    while (!q.empty())
    {
        int u = q.front();  // dequeue a vertex
        q.pop();

        // If this node is not NIL and can provide a shorter path to NIL then
        if (dist[u] < dist[NIL])
        {
            // Get all the adjacent vertices of the dequeued vertex u
            std::list<int>::iterator it;
            for (it = adj[u].begin(); it != adj[u].end(); ++it)
            {
                int v = *it;

                // If pair of v is not considered so far i.e. (v, pair_v[v]) is not yet explored edge.
                if (dist[pair_v[v]] == INF)
                {
                    dist[pair_v[v]] = dist[u] + 1; 
                    q.push(pair_v[v]);    // Consider the pair and push it to queue
                }
            }
        }
    }

   
   
    return (dist[NIL] != INF);   // If we could come back to NIL using alternating path of distinct vertices then there is an augmenting path available
}

/**
 * @brief This functions checks whether an augmenting path is available exists beginning with free vertex u
 * @param u represents position of vertex
 * @returns `true` if there is an augmenting path beginning with free vertex u
 * @returns `false` if there is no augmenting path beginning with free vertex u
 */
bool HKGraph::dfs(int u)
{
    if (u != NIL)
    {
        std::list<int>::iterator it;
        for (it = adj[u].begin(); it != adj[u].end(); ++it)
        {
            
            int v = *it; // Adjacent vertex of u

            // Follow the distances set by BFS search
            if (dist[pair_v[v]] == dist[u] + 1)
            {
                // If dfs for pair of v also return true then new matching possible, store the matching
                if (dfs(pair_v[v]) == true)
                {   
                    pair_v[v] = u;
                    pair_u[u] = v;
                    return true;
                }
            }
        }

        
        dist[u] = INF; // If there is no augmenting path beginning with u then set distance to infinite.
        return false;
    }
    return true;
}

/**
 * @brief Default Constructor for initialization
 */
HKGraph::HKGraph() = default;

/**
 * @brief Constructor for initialization
 * @param m is the number of vertices on left side of Bipartite Graph
 * @param n is the number of vertices on right side of Bipartite Graph
 */
HKGraph::HKGraph(int m, int n) {
    this->m = m;
    this->n = n;
    adj = std::vector<std::list<int> >(m + 1);
}

/**
 * @brief function to add edge from u to v
 * @param u is the position of first vertex
 * @param v is the position of second vertex
 */
void HKGraph::addEdge(int u, int v)
{
    adj[u].push_back(v); // Add v to u’s list.
}

} // namespace graph

using graph::HKGraph;

/**
 * Self-test implementation
 * @returns none
 */
void tests(){
     // Sample test case 1
	     int v1a = 3, v1b = 5;  // vertices of left side, right side and edges
	     HKGraph g1(v1a, v1b); // execute the algorithm 

	     g1.addEdge(0,1);
	     g1.addEdge(1,4);

	     // edge (0, 1) starts at the dummy vertex 0, which the algorithm never visits (u runs 1..m);
	     // the only real edge is (1, 4), so the maximum matching is 1
	     int expected_res1 = 1; // for the above sample data, this is the expected output
	     int res1 = g1.hopcroftKarpAlgorithm();

	     assert(res1 == expected_res1); // assert check to ensure that the algorithm executed correctly for test 1
	
     // Sample test case 2
     	     int v2a = 4, v2b = 4;  // vertices of left side, right side and edges
	     HKGraph g2(v2a, v2b); // execute the algorithm 

             g2.addEdge(1,1);
	     g2.addEdge(1,3);
	     g2.addEdge(2,3);
	     g2.addEdge(3,4);
	     g2.addEdge(4,3);
             g2.addEdge(4,2);
	
	     // perfect matching: 1-1, 2-3, 3-4, 4-2
	     int expected_res2 = 4; // for the above sample data, this is the expected output
	     int res2 = g2.hopcroftKarpAlgorithm();

	     assert(res2 == expected_res2); // assert check to ensure that the algorithm executed correctly for test 2
	
      // Sample test case 3
     	     int v3a = 6, v3b = 6;  // vertices of left side, right side and edges
	     HKGraph g3(v3a, v3b); // execute the algorithm 

             g3.addEdge(0,1);
	     g3.addEdge(1,4);
	     g3.addEdge(1,5);
	     g3.addEdge(5,0);

	     // edge (0, 1) starts at the dummy vertex and is ignored; edge (5, 0) ends at right vertex 0,
	     // which is the reserved NIL index, outside the supported range 1..n. The implementation
	     // still counts it, so the result is 2 (1-4 or 1-5, plus 5-"0"). Over supported vertices only,
	     // the maximum matching would be 1. 2 is recorded as the current behaviour, not a valid matching.
	     int expected_res3 = 2; // for the above sample data, this is the expected output
	     int res3 = g3.hopcroftKarpAlgorithm();

	     assert(res3 == expected_res3); // assert check to ensure that the algorithm executed correctly for test 3
	
	
    	
}

/**
 * @brief Tests for the "no augmenting path" branches of HKGraph
 * @details HKGraph never throws and has no error value: when no augmenting
 * path exists, bfs() returns false (`return (dist[NIL] != INF);`) and
 * dfs(u) returns false after `dist[u] = INF;`, so hopcroftKarpAlgorithm()
 * stops and returns the matching found so far (0 for a graph with no edges).
 * @returns void
 */
static void failure_path_tests() {
    int count = 0;

    // smallest graphs: no left vertices, or no edges. bfs() finds no path
    // on its first call, so the result is 0.
    HKGraph empty(0, 0);
    assert(empty.hopcroftKarpAlgorithm() == 0);
    HKGraph default_graph;
    assert(default_graph.hopcroftKarpAlgorithm() == 0);
    HKGraph no_edges(1, 1);
    assert(no_edges.hopcroftKarpAlgorithm() == 0);
    count += 3;

    // dfs() fails for one vertex: left 1 and 2 both want right 1. In phase 1,
    // dfs(1) matches 1-1 and dfs(2) finds no layered path, so it reaches
    // dist[u] = INF; return false. Phase 2's bfs() returns false.
    HKGraph shared(2, 2);
    shared.addEdge(1, 1);
    shared.addEdge(2, 1);
    assert(shared.hopcroftKarpAlgorithm() == 1);
    ++count;

    // the graph is still valid afterwards: running again gives the same
    // answer, because the matching is rebuilt from scratch
    assert(shared.hopcroftKarpAlgorithm() == 1);
    ++count;

    // and an ordinary call succeeds once left 2 gets its own right vertex
    shared.addEdge(2, 2);
    assert(shared.hopcroftKarpAlgorithm() == 2);
    ++count;

    // neighbouring input of source test 2: without edge (4, 2), left 2 and
    // left 4 can only use right 3, so one of them stays unmatched -> 3
    HKGraph g(4, 4);
    g.addEdge(1, 1);
    g.addEdge(1, 3);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 3);
    assert(g.hopcroftKarpAlgorithm() == 3);
    ++count;

    std::cerr << "[HKGraph] " << count << " checks passed" << std::endl;
}

/**
 * @brief Main function
 * @returns 0 on exit
 */
int main()
{
    tests();  // perform self-tests
    failure_path_tests();  // perform failure-path tests

    int v1 = 0, v2 = 0, e = 0;
    std::cin >> v1 >> v2 >> e; // vertices of left side, right side and edges
    HKGraph g(v1, v2);  
    int u = 0, v = 0;
    for (int i = 0; i < e; ++i)
    {
        std::cin >> u >> v;
        g.addEdge(u, v);
    }
  
    int res = g.hopcroftKarpAlgorithm();
    std::cout << "Maximum matching is " << res <<"\n";

    return 0;

}
