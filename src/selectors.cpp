#include "selectors.hpp"

#include <json_spirit_reader.h>
#include <json_spirit_stream_reader.h>
#include <boost/foreach.hpp>

using namespace json_spirit;

/*
 * utility functions
 */
Interval get_chunk_coordinate(int chunk_coord){
	return Interval(chunk_coord * 16, (chunk_coord + 1) * 16 - 1);
}

std::ostream & operator<<(std::ostream & out, Interval & i){
	out << "[" << i.getmin() << "," << i.getmax() << "]" <<endl;
	return out;
}

std::ostream & operator<<(std::ostream & out,point_surface  & p){
	out << "[ x:" << p.x << ", z: " << p.z << "]" << endl;
	return out;
}

template<typename Y>
	Y sqr(Y val){ 
		return val*val; 
	}


/*
 * Predicte selectors implementations
 */

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

	return distance_sqr <= size * size ; 
}


bool in_circle_predicate::operator()(const point & pt){
	int distance_sqr = sqr(pt.x - center.x) + sqr ( pt.z - center.z);
	return distance_sqr <= sqr(size); 
}


/*
 * Chunk selector generators from JSon Spirit Values functions
 */ 

pchunksel analyse_json_unknown(wmValue & v, bool & error);

// json point : needs (optionaly) a "X","Y","Z" triple of integers, defaults to 0.
point analyse_point(wmValue & v, bool & error){
	wmObject obj = v.get_obj();

	std::cout << "analysing point" <<std::endl;	
	int X,Y,Z ;
	X=Y=Z=0;
	std::wstring str_x(L"X") ; 
	X = obj[std::wstring(str_x)].get_int();
	std::wstring str_y(L"Y") ; 
	Y = obj[std::wstring(str_y)].get_int();
	std::wstring str_z(L"Z") ; 
	Z = obj[std::wstring(str_z)].get_int();

	return point(X,Y,Z);	
}



// circle : needed a "center" point and a "diameter" integer attributes
pchunksel analyse_json_circle(wmValue & v, bool & error){
	std::cout << "analysing circle" << std::endl;
	
	point p = analyse_point(v.get_obj()[L"center"],error);
	point_surface p_s(p.x,p.z);	
	int size = v.get_obj()[L"diameter"].get_int();
	in_circle_predicate in_c(p_s,size);
		
	return pchunksel(new in_circle_criterium(in_c)) ;
}

// not : needs a sub selector specification
pchunksel analyse_json_not(wmValue & v, bool & error){
	pchunksel inverse( analyse_json_unknown(v,error));
	pchunksel sel(new not_in_criterium_chunk_selector(inverse));
	return sel;
}

// and : needs an array of selectors
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
			cout << "Json Spec error in \"and\" selector, table waited";
		error=true;
	}
	return andsel;
}

// line : needs an array of point
pchunksel analyse_json_line (wmValue & v,bool & error){
	std::cout << "analysing line" << std::endl ;
	std::list<point_surface> point_list;
	switch (v.type()) {
		case array_type:
			{
				wmArray array = v.get_array();
				if(array.size()<2){
					cout << "Json Spec line error in \"line\" : not enough points in line, needs at list 2." << endl;
					error=true;
					break;
				}
				BOOST_FOREACH(wmValue val, array ){
					point p = analyse_point(val,error);

					if(error) 
						break;

					point_list.push_back(point_surface(p.x,p.z));
				}
				break;
			}
		default:
			cout << "JSon Spec Error in \"line\" : Table waited."; 
			error=true;
		break;
	}
	pchunksel sel;
	if(! error ) 
		sel=selector_factory::from_line_point_list(point_list);
	return sel;
}


// or : needs an array of selectors
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
			cout << "JSon Spec Error parsing \"or\", Table waited." << std::endl;
		error=true;
	}
	return anysel;
}

// unknown : guessing the type of selector and generating it accordingly.
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


/* Chunk selector Factory : users API */

pchunksel selector_factory::from_json_spec(std::string & filename){
	std::wifstream json_file(filename.c_str());
	bool error = false;
	if (! json_file.good()){
		std::cout << "bad json spec selector path : "<< filename <<" " << std::endl <<"Exiting." << std::endl; 
		exit(0);
	}
	wmValue value;

	read_stream(json_file,value);
	pchunksel p(analyse_json_and(value,error));
	if(error){
		std::cout << "Error analysing Json file selector. Exiting." << std::endl;
		exit(0);
	}

	return p;
}

pchunksel selector_factory::from_line_point_list(const std::list<point_surface> & point_list){
	
	typedef boost::iterator_range< std::list<point_surface>::const_iterator > ilist;
	point_surface begin_line = point_list.front(); 
	panychunksel line_selector(new any_criterium_chunk_selector());

	ilist points = boost::make_iterator_range(
		(point_list.begin())++, 
		point_list.end()
	);

	BOOST_FOREACH(point_surface end_line, points) {
		is_chunk_on_line is(begin_line,end_line);
		pchunksel p(new predicate_criterium<is_chunk_on_line>(is));
		line_selector->add_criterium(p);
		begin_line=end_line;
	}
	return line_selector;
}

