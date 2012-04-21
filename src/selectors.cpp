#include "selectors.hpp"

#include "json_spirit/json_spirit_reader_template.h"
#include "json_spirit/json_spirit_writer_template.h"

#include <boost/foreach.hpp>

using namespace json_spirit;

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

	// std::cout <<" in circle ?" <<std::endl;
	distance_sqr = dist_x * dist_x + dist_y * dist_y ; 

	// std::cout <<" d sqr :"<< distance_sqr << ", size" << size * size<<std::endl;
	return distance_sqr <= size * size ; 
	// distance of the chunk from the circle
}

template<typename Y>
Y sqr(Y val)
{ return val*val; }


bool in_circle_predicate::operator()(const point & pt){
	// std::cout << "in circle ?" << std::endl;
	int distance_sqr = sqr(pt.x - center.x) + sqr ( pt.z - center.z);
	// std::cout << pt.x << " " << pt.y << " "<< pt.z <<" " << distance_sqr << " " << sqr(size) << std::endl;
	// std::cout << pt.x << " " << pt.z <<" " << distance_sqr << " " << sqr(size) << std::endl;
	return distance_sqr <= sqr(size); 
}

std::ostream & operator<<(std::ostream & out, Interval & i){
	out << "[" << i.getmin() << "," << i.getmax() << "]" <<endl;
	return out;
}

std::ostream & operator<<(std::ostream & out,point_surface  & p){
	out << "[ x:" << p.x << ", z: " << p.z << "]" << endl;
	return out;
}

pchunksel analyse_json_unknown(wmValue & v, bool & error);

point analyse_point(wmValue & v, bool & error){
	wmObject obj = v.get_obj();

	std::cout << "analysing point" <<std::endl;	
	int X,Y,Z ;
	X=Y=Z=0;
	std::wstring str_x(L"X") ; 
	X = obj[std::wstring(str_x)].get_int(); // .get("X").get_int();
	std::wstring str_y(L"Y") ; 
	Y = obj[std::wstring(str_y)].get_int();
	std::wstring str_z(L"Z") ; 
	Z = obj[std::wstring(str_z)].get_int();

	return point(X,Y,Z);	
}

pchunksel analyse_json_circle(wmValue & v, bool & error){
	// pallchunksel sel(new all_criterium_chunk_selector());
	std::cout << "analysing circle" << std::endl;
	
	point p = analyse_point(v.get_obj()[L"center"],error);
	point_surface p_s(p.x,p.z);	
	int size = v.get_obj()[L"diameter"].get_int();
	in_circle_predicate in_c(p_s,size);
		
	return pchunksel(new in_circle_criterium(in_c)) ;
}

pchunksel analyse_json_not(wmValue & v, bool & error){
	pchunksel inverse( analyse_json_unknown(v,error));
	pchunksel sel(new not_in_criterium_chunk_selector(inverse));
	return sel;
}


pchunksel analyse_json_and (wmValue & v,bool & error){
	std::cout << "analysing and" << std::endl ;
	pallchunksel andsel( new all_criterium_chunk_selector());
	switch (v.type()) {
		case array_type:
			BOOST_FOREACH(wmValue val, v.get_array() ){
				andsel->add_criterium(analyse_json_unknown(val, error));
			}
		break;
		default:
			cout << "erreur de parsing dans le \"and\" : attendu un tableau";
		error=true;
	}
	return andsel;
}

pchunksel analyse_json_line (wmValue & v,bool & error){
	std::cout << "analysing line" << std::endl ;
	panychunksel orsel( new any_criterium_chunk_selector());
	switch (v.type()) {
		case array_type:
			BOOST_FOREACH(wmValue val, v.get_array() ){
				orsel->add_criterium(analyse_json_unknown(val, error));
			}
		break;
		default:
			cout << "JSon Spec Error in \"line\" : attendu un tableau"; error=true;
	}
	return orsel;
}


pchunksel analyse_json_or (wmValue & v,bool & error){
	std::cout << "analysing unknown" << std::endl;
	panychunksel anysel( new any_criterium_chunk_selector());
	switch (v.type()) {
		case array_type:
			BOOST_FOREACH(wmValue val, v.get_array() ){
				anysel->add_criterium(analyse_json_unknown(val, error));
			}

		break;
		default:
		cout << "or error parsing" << std::endl;
		error=true;
	}
	return anysel;
}

pchunksel analyse_json_unknown(wmValue & v, bool & error){
	pchunksel sel;
	wmObject obj;
	std::cout << "analysing unknown" << std::endl;
	switch (v.type()){
		case obj_type:
		obj=v.get_obj();
		BOOST_FOREACH(wmConfig::Pair_type obj_,obj ){
			wstring first = obj_.first ;
			std::cout << "plop";
			std::wcout << first << std::endl;
		}
		std::cout << "is an obj" <<std::endl;
		if (obj.count( L"circle") > 0 ){
			sel = analyse_json_circle(obj[L"circle"],error);
		}else if (obj.count( L"and") > 0 ){
			sel = analyse_json_and(obj[L"and"],error);
		}else if(obj.count( L"or") > 0 ){
			sel = analyse_json_or(obj[L"or"],error);
		}else if(obj.count( L"not") > 0 ){
			sel = analyse_json_not(obj[L"not"],error);
		}else if(obj.count( L"line") > 0 ){
			sel = analyse_json_line(obj[L"line"],error);
		}else {
		
			cout << "JSon SPec Error : not niether a and, not or circle" << std::endl; 
			error=true;
		}
		break;
		default:
			cout << "Json Spec Error : Object waited";
			error=true;
	}
	return sel;
}

pchunksel selector_factory::from_json_spec(std::string & filename){
	std::wifstream json_file(filename.c_str());
	bool error = false;
	if (! json_file.good()){
		std::cout << "bad json spec selector file : "<< filename <<" exiting" << std::endl; 
		exit(0);
	}
	wmValue value;

	read_stream(json_file,value);
	pchunksel p(analyse_json_and(value,error));
	if(error){
		std::cout << "erreur de parsing" << std::endl;
		exit(0);
	}

	return p;
}

