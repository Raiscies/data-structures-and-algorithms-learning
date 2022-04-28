
// #include <linked_list.h>
#include <cstddef>


namespace rais::study {

using std::size_t;

struct polynormial_item {
	//a * x^n
	double a = 1;
	size_t n;
};

// class polynormial_function: private linked_list<polynormial_item> {

// public:
// 	using container_t = linked_list<polynormial_item>;
// 	using container_t::node_t;

// 	polynormial_function(initializer_list list): container_t(list) {
// 		this->sort();
// 	}
// };


}; //namespace rais::study

using namespace rais::study;

int main() {
	const auto t = polynormial_item{};
	double a = t.a;
	return 0;
}