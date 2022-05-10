#include <double_list.hpp>
#include <iostream>

void test_double_list() {
	using namespace rais::study;
	using std::cout;
	constexpr char lf = '\n';
	double_list<int> list = {9, 6, 4, 1, 3, 2, 2, 3, 8, 4, 5 ,22, 18, 6, 5};
	cout << list << lf;
	cout << list.push(11).unshift(22) << lf << list[0] << ", " << list[1] << ", " << list[list.size() - 1] << ", " << list[7] << lf;
	for(size_t i = 0; i < list.size(); i++) {
		cout << list[i] << ", ";
	}
	cout << lf;
	list.clear();
	cout << list << lf;
	double_list<const char*> slist = {"slist", "test", "a", "bb", "ccc", "rais", "@@"};
	cout << slist << lf;
	slist.insert(2, "insert str");
	slist.insert(0, "insert head");
	cout << slist << lf;
	cout << slist.shift() << lf << slist.pop() << lf << slist << lf; 
	slist.erase(3);
	cout << slist << lf << "reverse list: \n";
	slist.reverse();
	cout << slist << lf;
	cout << "reverse list2: \n";
	double_list<int> list2{9, 6, 4, 1, 3, 2, 2, 3, 8, 4, 5 ,22, 18, 6, 5};
	cout << list2 << lf;
	list2.reverse();
	cout << list2 << lf;
	cout << "finished test\n";
}

int main() {
	test_double_list();
}