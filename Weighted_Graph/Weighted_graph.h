/*****************************************
 * UW User ID:  uwuserid
 * Submitted for ECE 250
 * Semester of Submission:  (Winter|Spring|Fall) 20NN
 *
 * By submitting this file, I affirm that
 * I am the author of all modifications to
 *  the provided code.
 *****************************************/

#ifndef WEIGHTED_GRAPH_H
#define WEIGHTED_GRAPH_H

#ifndef nullptr
#define nullptr 0
#endif

#include <iostream>
#include <limits>
#include <queue>

#include "Exception.h"

// used as temporary structure with priority queue
class Weighted_edge {
public:
	int				vertex;
	double			distance;
	Weighted_edge(int v, double d) { vertex = v; distance = d; }
	// to override operator so that it can be ordered according to distance
	inline bool operator<(const Weighted_edge& rhs) const { return distance > rhs.distance; }
};

class Weighted_graph {
	private:
		// your implementation here
		//  you can add both private member variables and private member functions
		
		static const double					INF;
		// stores all the edges
		// this is an O(N^2) structure
		double								**m_edges;
		// holds all the calculated distances
		double								*m_distances;
		// the size of the graph, is the same as the value N
		int									m_capacity;
		// the size as the m_edge[1]s array
		int									m_size;
		// true if the distance algorithm has started before
		bool								m_started;
		// true is distance algorithm has already finished before
		bool								m_finished;
		// indicates which vertices have been found
		bool								*found;

	public:
		Weighted_graph( int = 50 );
		~Weighted_graph();

		int degree( int ) const;
		int edge_count() const;
		double adjacent( int, int ) const;
		double distance( int, int );

		void insert( int, int, double );

	// Friends

	friend std::ostream &operator<<( std::ostream &, Weighted_graph const & );
};

const double Weighted_graph::INF = std::numeric_limits<double>::infinity();

Weighted_graph::Weighted_graph( int size ) {
	// initialize members
	m_capacity = size;
	m_size = size*(size - 1)/2;
	m_started = false;
	m_finished = false;
	// allocate array space
	found = new bool[m_capacity];
	m_distances = new double[m_capacity];
	m_edges = new double*[m_capacity];
	// allocate adjacency matrix
	m_edges[0] = nullptr;
	m_edges[1] = new double[m_size];
	for( int i = 2; i < m_capacity; ++i ) {
	    m_edges[i] = m_edges[i - 1] + i - 1;
	}
	// set all edge weights to infinity
	for(int i = 0; i < m_size; i++)
		m_edges[1][i] = INF;
}

// destructor
Weighted_graph::~Weighted_graph() {
	// delete all allocated memory
	delete [] m_edges[1];
	delete [] m_edges;
	delete [] m_distances;
	delete [] found;
}

// returns the degree of a vertex
int Weighted_graph::degree( int n ) const {
	if(n < 0 || n >= m_capacity) // not a valid vertex
		throw illegal_argument();

	int deg = -1; // start with -1 since vertex will count itself
	for (int i = 0; i < m_capacity; i++) {
		if (adjacent(n,i) < INF)
			deg++; // if edge exists
	}
	return deg;
}

// count the number of edges in the adjacency matrix
int Weighted_graph::edge_count() const {
	int sum = 0;
	for (int i = 0; i < m_size; i++) {
		if(m_edges[1][i] < INF)
			sum += 1;
	}
	return sum;
}

// return the edge weight between 2 vertices
double Weighted_graph::adjacent( int m, int n ) const {
	if(n < 0 || n >= m_capacity)
		throw illegal_argument();
	if(m < 0 || m >= m_capacity)
		throw illegal_argument();
	if(m == n) // weight between vertex and itself
		return 0;
	
	// larger vertex should be the row
	if ( m < n ) {
		return m_edges[n][m];
	} else {
		return m_edges[m][n];
	}
}

// uses dijkstra's algorithm to return the shortest path between the vertices
double Weighted_graph::distance( int m, int n ) {
	if(n < 0 || n >= m_capacity)
		throw illegal_argument(); // bad value
	if(m < 0 || m >= m_capacity)
		throw illegal_argument(); // bad value
	if(m == n) { 
		return 0.0; // distance between node and itself
	}
	
	// temporary instance so doesnt have to be created every iteration
	double distance;
	Weighted_edge temp(0,0);
	Weighted_edge edge(0,0);
	// priority queue for finding vertex with shortest distance
	std::priority_queue<Weighted_edge> pqu; 

	if(m_started && m_distances[m] == 0) {
	// if algorithm has already been started and with the same source node
		if(m_finished || found[n])
			return m_distances[n]; // if distance to n has already been calculated
		// else, need to continue from where it left off last time
		for (int i = 0; i < m_capacity; i++) {
			if(!found[i] && m_distances[i] < INF) {
				temp.distance = m_distances[i]; temp.vertex = i;
				pqu.push(temp); 
			}
		}
	}
	else {
	// have to start the algorithm fresh
		m_started = false;
		for (int i = 0; i < m_capacity; i++) {
			// reset distance array
			m_distances[i] = INF;
			found[i] = false;
		}
		// place source vertex into queue
		temp.distance = 0.0; temp.vertex = m; 
		pqu.push(temp);
		//pqu.emplace(m, 0.0); // doesnt compile on ecelinux, requires c++ 11
		m_distances[m] = 0.0;
	}
	/*if(degree(m) == 0 || degree(n) == 0)
		return INF;*/
	m_finished = false; // indicate no finished

	// pop each vertex from queue until it is empty or n is found
	while (!pqu.empty()) {
		edge = pqu.top();
		if(found[edge.vertex]) {
		// vertex is found 
			pqu.pop();
			continue;
		}
		found[edge.vertex] = true;
		pqu.pop();
		// for each edge of the vertex check the path length to that vertex
		for (int i = 0; i < m_capacity; i++) {
			if(found[i])
				continue; // vertex already found
			distance = edge.distance + adjacent( edge.vertex, i );
			if(distance >= m_distances[i])
				continue; // path to vertex is longer from this direction
			// else, distance from here is shorter
			// update distance array and enque vertex with updated distance
			m_distances[i] = distance;
			temp.distance = distance; temp.vertex = i; 
			pqu.push(temp);
			//pqu.emplace(i, distance);
		}
		if(n == edge.vertex)
			break;
	}
	if (pqu.empty())
		m_finished = true; // only finished if queue is empty, this only happens if n is not found

	m_started = true; // indicate that algorithm has started
	return m_distances[n]; // return distance of n from array
}

// update edge weight in adjacency matrix
void Weighted_graph::insert( int m, int n, double w ) {
//
	if(n < 0 || n >= m_capacity)
		throw illegal_argument();
	if(m < 0 || m >= m_capacity)
		throw illegal_argument();
	if(m == n)
		throw illegal_argument();
	if(w <= 0)
		throw illegal_argument();

	if ( m < n ) {
		m_edges[n][m] = w;
	} else {
		m_edges[m][n] = w;
	}
	m_started = false; // graph is updated, path algorithm should restart
}

// You can modify this function however you want:  it will not be tested

std::ostream &operator<<( std::ostream &out, Weighted_graph const &graph ) {
	out << "";
	return out;
}

// Is an error showing up in ece250.h or elsewhere?
// Did you forget a closing '}' ?

#endif
