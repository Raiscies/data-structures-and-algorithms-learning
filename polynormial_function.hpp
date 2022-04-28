#pragma once

#include <linked_list.hpp>

namespace rais::study {



struct polynormial_item {
	// a * x^n
	double a = 1.0;
	size_t n;

};

class polynormial_function;
using polyfunc = polynormial_function;
using polyitem = polynormial_item;

class polynormial_function: protected linked_list<polynormial_item> {

public:
	// using linked_list<item_t> = linked_list<polynormial_item>;
	using item_t = polynormial_item;
	using linked_list<item_t>::iterator_t;
	using linked_list<item_t>::const_iterator_t;

	using linked_list<item_t>::begin;
	using linked_list<item_t>::end;
	using linked_list<item_t>::length;
private:

	using linked_list<item_t>::head;
	using linked_list<item_t>::sort;
	using linked_list<item_t>::shift;
	using linked_list<item_t>::erase_after;
	using linked_list<item_t>::erase;
	using linked_list<item_t>::node_t;
	using linked_list<item_t>::merge;
public:

	polynormial_function(const polyfunc& other): linked_list<item_t>(other.data()) {}
	polynormial_function(polyfunc&& other): linked_list<item_t>(move(other.data())) {}

	template <typename U> 
	requires same_as<remove_cvref_t<U>, linked_list<item_t>>
	polynormial_function(U&& other): linked_list<item_t>(forward<U>(other)) {
		regularize();
	}
	polynormial_function(initializer_list<item_t> list): linked_list<item_t>(list) {
		regularize();	
	}

	void regularize() {
		if(length == 0) return; 
		sort([](const item_t& item1, const item_t& item2) noexcept{ return item1.n < item2.n; });
		//merge same items
		node_t** pos = &head;

		while(*pos != nullptr and (*pos)->next != nullptr) {
			if((*pos)->data.a == 0) erase(pos);
			else if((*pos)->data.n == (*pos)->next->data.n) {
				//merge the same n's item
				(*pos)->data.a += (*pos)->next->data.a;
				erase_after(iterator_t{*pos});
			}else {
				pos = &((*pos)->next);
			}
		}
		if(*pos != nullptr and (*pos)->data.a == 0) erase(pos);
	
	}	

	linked_list<item_t>& data() noexcept{return static_cast<linked_list<item_t>&>(*this); }
	const linked_list<item_t>& data() const noexcept{ return static_cast<const linked_list<item_t>&>(*this); }

	//TODO...
	friend polyfunc operator-(const polyfunc& f) {
		auto temp = f;
		
	}
 
	friend polyfunc operator+(const polyfunc& f1, const polyfunc& f2) {
		return polyfunc(merge(f1.data(), f2.data(), [](const polyitem& a, const polyitem& b) noexcept{return a.n < b.n;} ));
	}
	friend polyfunc operator-(const polyfunc& f1, const polyfunc& f2) {

	}

};



std::ostream& operator<<(std::ostream& os, const polynormial_item& item) {
	os << (item.a < 0 ? " - " : " + ") << item.a;
	if(item.n == 0) return os;
	else if(item.n == 1) return os << "x";
	else return os << "x^(" << item.n << ')';
}
std::ostream& operator<<(std::ostream& os, const polynormial_function& func) {
	for(const auto& item: func) {
		os << item;
	}
	return os;
}

} //namespace rais::study

void test_polynormial_function() {
	polyfunc func1 = {{0, 34}, {2, 3}, {18, 3}, {8, 1}, {9, 4}, {0, 2}, {4, 1}, {1, 2}},
	     func2 = {{0, 3}, {13, 2}, {18, 2}, {8, 0}, {9, 4}, {0, 2}, {4, 1}, {1, 2}};
	std::cout << "func1: " << func1 << '\n';
	std::cout << "func2: " << func2 << '\n';
	std::cout << "func1 + func2: " << (func1 + func2) << '\n';


}