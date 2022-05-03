
#include <random>
#include <chrono>
#include <iostream>
#include <linked_list.hpp>

void test_linked_list_basic() {
	using namespace rais::study;

	linked_list<std::string> list;
	// test push
	list.push("1. Test push");
	// test unshift
	list.unshift("2. Test unshift");
	// test insert
	list.insert(0, "3. Test insert");
	// test operator[]
	std::cout << "4. Test operator[]{" << list[2] << "}\n";
	// test get_ptr
	std::cout << "5. Test get_ptr{" << *list.get_ptr(0) << "}\n";
	std::cout << list << '\n';
	// test erase
	std::cout << "6. Test erase\n";
	list.erase(1);
	std::cout << list << '\n';
	// test clear
	std::cout << "7. Test clear\n";
	list.clear();
	std::cout << list << '\n';
	// test reverse
	std::cout << "8. Test reverse from:";
	list.push("1").push("2").push("3").push("4").push("5").push("6").unshift("0").unshift("-1");
	std::cout << list << "\n";
	list.reverse();
	std::cout << "to: " << list << '\n';
	// test shift
	std::cout << "9. Test shift: " << list.shift() << '\n';
	//test pop
	std::cout << "10. Test pop: "  << list.pop() << '\n';
	std::cout << "current list: " << list << '\n';
	// test copy constructor
	auto list2 = list;
	std::cout << "list2: " << list2 << '\n';
	// test swap 
	list2.push("this is from list2");
	swap(list, list2);
	std::cout << "list: " << list << '\n' << "list2: " << list2 << '\n';
	// test merge
	linked_list<int> list3, list4;
	list3.push(1).push(4).push(7).push(10).push(14).push(22).push(25).push(31);
	list4.push(1).push(2).push(10).push(10).push(11).push(15).push(20).push(26);
	std::cout << "11. test merge:\nlist3: " << list3 << "\nlist4: " << list4 << '\n';
	list4.merge(move(list3), [](int a, int b){return a < b;});
	std::cout << "merged. \nlist3: " << list3 << "\nlist4: " << list4 << '\n';
	std::cout << "12. test initializer_list constructor: " << linked_list<char>{'2','3', '4', 'c', 'x', 'n'} << "\n\n";
	auto list5 = linked_list<int>{52, 99, 7, 3, 5, 7, 2, 2, 34, 53, 53, 12, 42, 94, 53, 81, 1, 4, 9};
	std::cout << "13. test sort: \nbefore sort: " << list5 << '\n';
	list5.sort();
	std::cout << " after sort: " << list5 << '\n';
	std::cout << "14. test is_sorted: " << std::boolalpha << list5.is_sorted() << '\n';
	std::cout << "15. test merge(const list&, const list&, CompareT&&): " << merge(list4, list5) << '\n';
	auto list6 = linked_list<int>{52, 99, 7, 3, 5, 7, 2, 2, 34, 53, 53, 12, 42, 94, 53, 81, 1, 4, 9};
	std::cout << "16. test quick_sort: \nbefore quick_sort: " << list6 << '\n';
	list6.quick_sort();
	std::cout << "after quick_sort: " << list6 << '\n';
}

void test_sort() {
	using namespace rais::study;

	std::minstd_rand randint{std::random_device{}()};
	std::uniform_int_distribution mask;

	linked_list<int> list;
	for(int i = 0; i < 1000'0000; i++) {
		list.unshift(mask(randint));
	}
	std::cout << "length: " << list.size() << '\n';
	auto start = std::chrono::steady_clock::now();
	list.sort();
	auto end = std:: chrono::steady_clock::now();
	std::cout << std::boolalpha << list.is_sorted() << '\n';
	std::cout << std::chrono::duration<double>(end-start).count() << "s\n";


}

int main() {
	// std::ios::sync_with_stdio();



	// test_linked_list();
	test_sort();
}