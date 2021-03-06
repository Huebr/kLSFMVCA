#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <boost/graph/graph_traits.hpp> // for creation of descriptors vertex and edges.
#include <boost/graph/adjacency_list.hpp> //for usage of adjacency list
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/graphml.hpp>
#include <vector>

#include <unordered_set>
#include <boost/dynamic_bitset.hpp>


using namespace boost;

//basic definitions
typedef typename adjacency_list<vecS, vecS, undirectedS, no_property, property<edge_color_t,int>> graph_t;
typedef dynamic_bitset<> db;

template <typename EdgeColorMap, typename ValidColorsMap>
struct valid_edge_color {
	valid_edge_color() { }
	valid_edge_color(EdgeColorMap color, ValidColorsMap v_colors) : m_color(color), v_map(v_colors) { }
	template <typename Edge>
	bool operator()(const Edge& e) const {
		return v_map.test(get(m_color, e));
	}
	EdgeColorMap m_color;
	ValidColorsMap v_map;
};



template<class Graph, class Mask>
void print_filtered_graph(Graph &g, Mask valid) { //pay atention to the position of the bits and the colors positions in array
	typedef typename property_map<Graph, edge_color_t>::type EdgeColorMap;
	typedef typename boost::dynamic_bitset<> db;
	typedef filtered_graph<Graph, valid_edge_color<EdgeColorMap, db> > fg;

	valid_edge_color<EdgeColorMap, Mask> filter(get(edge_color, g), valid);
	fg tg(g, filter);
	print_edges(edges(tg).first, edges(tg).second, tg);
}

template<class Graph, class Mask>
int get_components(Graph &g, Mask &m, std::vector<int> &components) {
	typedef typename property_map<Graph, edge_color_t>::type EdgeColorMap;
	typedef typename boost::dynamic_bitset<> db;
	typedef filtered_graph<Graph, valid_edge_color<EdgeColorMap, db> > fg;

	valid_edge_color<EdgeColorMap, Mask> filter(get(edge_color, g), m);
	fg tg(g, filter);
	int num = connected_components(tg, &components[0]);
	return num;
}

template<class Graph>
property_map<graph_t, edge_color_t>::type get_colors(Graph &g) {
	typedef typename property_map<Graph, edge_color_t>::type ColorMap;
	ColorMap colors = get(edge_color, g);
	//make color type generic
	return colors;
}







//template function to print edges.
template<class EdgeIter, class Graph>
void print_edges(EdgeIter first, EdgeIter last, const Graph& G) {
	typedef typename property_map<Graph, edge_color_t>::const_type ColorMap;
	ColorMap colors = get(edge_color, G);
	//make color type generic
	//typedef typename property_traits<ColorMap>::value_type ColorType;
	//ColorType edge_color;
	for (auto it = first; it != last; ++it) {
		std::cout << "Edge: " << "(" << source(*it, G) << "," << target(*it, G) << ") " << " Color: " << colors[*it] << "\n";
		std::cout << "Edge: " << "(" << target(*it, G) << "," << source(*it, G) << ") " << " Color: " << colors[*it] << "\n";
	}
	std::cout << " Number of vertex: " << num_vertices(G) << std::endl;
	std::cout << " Number of edges: " << num_edges(G) << std::endl;
	std::vector<int> components(num_vertices(G));
	int num = connected_components(G, &components[0]);
	std::vector<int>::size_type i;
	std::cout << "Total number of components: " << num << std::endl;
	for (i = 0; i != components.size(); ++i)
		std::cout << "Vertex " << i << " is in component " << components[i] << std::endl;
	std::cout << std::endl;
}



int main()
{
	graph_t g;
	std::ifstream ifn("C://Users//pedro//source//repos//kLSFMVCA//x64//Release//sample.graphml");
	try {
		dynamic_properties dp;
		dp.property("color",get(edge_color,g));
		read_graphml(ifn, g, dp);
	}catch (const boost::property_tree::xml_parser::xml_parser_error& ex) {
		std::cout << "Exception opening/reading/closing file\n";
		ifn.close();
		exit(EXIT_FAILURE);
	}
	ifn.close();
	//print_edges(edges(g).first, edges(g).second,g);
	int k_sup=4;
	int n_labels=25;
	std::vector<int> components(num_vertices(g));
	db temp(n_labels);
	int num_c = get_components(g, temp, components);
	int num_c_best = num_c;
	while (num_c_best > 1 && temp.count() < k_sup) {//peharps temp.count() < k_sup
		int best_label = 0;
		for (int i = 0; i < n_labels; ++i) {
			if (!temp.test(i)) {
				temp.set(i);
				int nc = get_components(g, temp, components);
				if (nc < num_c_best) {
					num_c_best = nc;
					best_label = i;
				}
				temp.flip(i);
			}
		}
		temp.set(best_label);
	}
	print_filtered_graph(g, temp);
	return 0;
}