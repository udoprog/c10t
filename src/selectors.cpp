#include "selectors.hpp"

#include <boost/property_tree/json_parser.hpp>

Interval get_chunk_coordinate(int chunk_coord){
	return Interval(chunk_coord * 16, (chunk_coord + 1) * 16 - 1);
}

in_circle_predicate::in_circle_predicate(point_surface & ps, int _size):
	size(_size),center(ps){

}


bool in_circle_predicate::operator()(const mc::utils::level_coord & coord) {
	Interval x_chunk(get_chunk_coordinate(coord.get_x()));
	Interval z_chunk(get_chunk_coordinate(coord.get_z()));

	point_surface  ps=center;
	int distance_sqr;

	int dist_x = x_chunk.distance(ps.x);
	int dist_y = z_chunk.distance(ps.z);

	distance_sqr = dist_x * dist_x + dist_y * dist_y ; 
	return distance_sqr > size * size ; 
	// distance of the chunk from the circle
};

std::ostream & operator<<(std::ostream & out, Interval & i){
	out << "[" << i.getmin() << "," << i.getmax() << "]" <<endl;
	return out;
}

std::ostream & operator<<(std::ostream & out,point_surface  & p){
	out << "[ x:" << p.x << ", z: " << p.z << "]" << endl;
	return out;
}


pchunksel selector_factory::from_json_spec(std::string & filename){
	pchunksel p(new all_criterium_chunk_selector()); 
	return p;
};

