#ifndef _SELECTORS_HPP_
#define _SELECTORS_HPP_

#include "mc/world.hpp"
#include "mc/blocks.hpp"
#include "mc/utils.hpp"

#include "global.hpp"


using namespace std;

class chunk_selector {
	public:
	virtual bool select(const mc::utils::level_coord & coord)=0;
};


typedef boost::shared_ptr<chunk_selector> pchunksel;
typedef std::list<pchunksel> lpchunk;
typedef lpchunk::iterator iter;

class all_criterium_chunk_selector : public chunk_selector {
	lpchunk list_sel;
	public:
	all_criterium_chunk_selector(){
	}
	void add_criterium(pchunksel s){
		list_sel.push_back(s);
	}
	virtual bool select(const mc::utils::level_coord & coord ){
		for(iter it = list_sel.begin() ; it != list_sel.end() ; ++it){
			if (! (*it)->select(coord)){
				return false;
			}
		}
		return true;
	}
};


class any_criterium_chunk_selector : public chunk_selector {
	lpchunk list_sel;
	public:
	void add_criterium(pchunksel s){
		list_sel.push_back(s);
	} 
	bool select(const mc::utils::level_coord & coord){
		for(iter it = list_sel.begin() ; it != list_sel.end() ; ++it){
			if ((*it)->select(coord)){
				return true;
			}
		}
		return false;
	}
};

template<class T> 
	class predicate_criterium : public chunk_selector {
		public:
		T _predicate;
		predicate_criterium<T>(T predicate):_predicate(predicate){
		}
		bool select(const mc::utils::level_coord & coord){
			return _predicate(coord);
		}
};

class Interval{
	
	int _min,_max;
	public:
	Interval(int min,int max){
		_min=std::min(min,max);
		_max=std::max(min,max);
	}
	bool intersects(Interval & i2){
		if (empty()||i2.empty()){
			return false;
		}
		return bound_intersect(i2) || i2.bound_intersect(*this);
	}
	bool empty(){
		return _max< _min;
	}
	int getmin(){
		return _min;
	}
	int getmax(){
		return _max;
	}
	int width(){
		return _max-_min;
	}
	bool includes(int val){
		return val >= getmin() && val <=getmax();
	}
	int distance(int val){
		if( includes(val) ) return 0;
		else return std::min( std::abs(getmin() -val), 
				      std::abs(getmax()-val));
	}
	private:
	bool bound_intersect(Interval & i2){
		return (_min <= i2._min && _max >= i2._min) ||(_min <= i2._max && _max >=i2._max);
	}
	
};



std::ostream & operator<<(std::ostream & out, Interval & i);
std::ostream & operator<<(std::ostream & out,point_surface  & p);

class is_chunk_on_line{
	point_surface p1;
	point_surface p2;

	public:
	is_chunk_on_line(point_surface _p1,point_surface _p2):p1(_p1),p2(_p2) {
		int x1 = std::min<int>(p1.x,p2.x);
		if(x1!=p1.x){
			point_surface inter=p2;
			p2=p1;
			p1=inter;
		}
		
	}
	public:	
	bool operator()(const mc::utils::level_coord & coord){
		//out << "is chunk_on line ?" << endl;
		Interval chunk_x = Interval(int(coord.get_x())*16 - 1,(((int(coord.get_x())+1)*16) + 1));
		Interval chunk_z = Interval(int(coord.get_z())*16 - 1,(((int(coord.get_z())+1)*16) + 1));
		
		//out << " chunk_x" << chunk_x << "chunk_z" << chunk_z << " " << p1 << " " << p2 << endl;

		// intersection of the chunk xs and the segment x domain
		int minx = std::max(chunk_x.getmin(),p1.x);
		int maxx = std::min(chunk_x.getmax(),p2.x);
		
		Interval x_domain(minx,maxx);	
		// empty intersection : 	
		if (minx>maxx){
			//out << "seems not !"<< endl;
			return false;
		}
		bool result;
		if (p1.x==p2.x){
			// straight line : does the line also intersects
			// the z domain ?
			//out << "cas ou les x sont identiques" << endl;
			Interval zline(p1.z,p2.z);
			//out << "zline" << zline << "chunk_z" << chunk_z << endl; 
			result = zline.intersects(chunk_z);
		}else{
			//out << "cas ou il y a une pente" << endl;
			double pente=double(p2.z-p1.z)/double(p2.x-p1.x);

			int zboxmin=std::floor((minx - p1.x) * pente +p1.z);
			int zboxmax=std::floor((maxx - p1.x) * pente +p1.z)+1;

			Interval zdomain(zboxmin,zboxmax);
			//out << "domain Zs" << ""
			result = zdomain.intersects(chunk_z);
		}
		// out << "result :" << result <<endl;
		return result;
	 
	}
};

class in_range_predicate {
	settings_t & _s;
	public:
	in_range_predicate(settings_t & s):_s(s){
	}
     	 
	bool operator()(const mc::utils::level_coord & coord) {

	uint64_t x2 = coord.get_x() * coord.get_x();
      	uint64_t z2 = coord.get_z() * coord.get_z();
     	uint64_t r2 = _s.max_radius * _s.max_radius;
      

	 bool out_of_range = coord.get_x() < _s.min_x
          || coord.get_x() > _s.max_x
          || coord.get_z() < _s.min_z
          || coord.get_z() > _s.max_z
          || x2 + z2 >= r2;

        if (out_of_range) {
          if (_s.debug) {
            // out_log << ": position out of limit (" << coord.get_z() << "," << coord.get_z() << ")" << std::endl;
          }
        }
 	  return not(out_of_range);
	}       
};

class in_circle_predicate {
	private:
	int size;
	point_surface center;

	public : 
	bool operator()(const mc::utils::level_coord & coord) ;
	in_circle_predicate(point_surface &, int size) ; 
};

typedef predicate_criterium<in_circle_predicate> in_circle_criterium;

class selector_factory {
	public:
	static pchunksel from_json_spec(std::string & );

};

/* template<class predicate> void operator||(predicate_criterium<predicate> ) { 

}*/

#endif
