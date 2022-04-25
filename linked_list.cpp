#include <cstddef>
#include <utility>
#include <limits>
#include <initializer_list>
#include <functional>
#include <concepts>
#include <type_traits>
#include <string>   //for testing
#include <iostream> //for testing

using std::size_t;
using std::numeric_limits;
using std::initializer_list;
using std::less;
using std::less_equal;
using std::forward;
using std::move;
using std::same_as;
using std::predicate;
using std::convertible_to;
using std::remove_reference_t;

namespace rais::study {


template <typename T>
struct list_node {
	T data;
	list_node* next;

	template <typename U> 
	requires convertible_to<U, T>
	list_node(U&& val, list_node* next): data(forward<U>(val)), next(next) {}
	template <typename U>
	requires convertible_to<U, T>
	list_node(U&& val): data(forward<U>(val)) {}
};

template <typename T>
class linked_list {
public:

	using element_t = T;
	using node_t = list_node<T>;

	struct iterator {
	private:
		node_t* it;

	public:
		iterator(node_t* it): it{it} {}
		T& operator*() {return it->data; }
		const T& operator*() const{ return it->data; }
		T* operator->() noexcept{return &(it->data); } 
		const T* operator->() const noexcept{return &(it->data); }
		iterator& operator++() {it = it->next; return *this;}
		iterator operator++(int) {auto temp = iterator{it}; it = it->next; return temp;}
		bool operator==(const iterator& other) const noexcept{return it == other.it; }
		bool operator!=(const iterator& other) const noexcept{return it != other.it; }

		node_t* get_ptr() noexcept{return it; }
		const node_t* get_ptr() const noexcept{return it; }
	};

	struct const_iterator{
	private:
		const node_t* it;

	public:
		const_iterator(const node_t* it): it{it} {}
		const T& operator*() const{ return it->data; }
		const T* operator->() const noexcept{return &(it->data); }
		const_iterator& operator++() {it = it->next; return *this;}
		const_iterator operator++(int) {auto temp = const_iterator{it}; it = it->next; return temp;}
		bool operator==(const const_iterator& other) const noexcept{return it == other.it; }
		bool operator!=(const const_iterator& other) const noexcept{return it != other.it; }

		const node_t* get_ptr() const noexcept{return it; }
	};
	using iterator_t = iterator;
	using const_iterator_t = const_iterator;

private:

	node_t* head = nullptr;
	size_t length = 0;


	node_t* tail() {
		if(head == nullptr) return nullptr;
		node_t* pos = head;
		while(pos->next != nullptr) pos = pos->next;
		return pos;
	}

public:
	linked_list() {}
	linked_list(initializer_list<T> list) {
		node_t** pos = &head;
		for(const auto& i: list) {
			*pos = new node_t{i};
			pos = &((*pos)->next);
		}
		*pos = nullptr;
		length = list.size();
	}
	~linked_list() {
		clear();
	}
	linked_list(const linked_list& other) {
		//copy constructor
		node_t** pps = &head,      //point to a pointer that point to self
		       * po  = other.head; //point other 
		while(po != nullptr) {
			*pps = new node_t{po->data/* copy */, nullptr};
			pps = &((*pps)->next);
			po = po->next;
		}
		length = other.length;
	}
	linked_list(linked_list&& other) noexcept: head{other.head}, length{other.length} {
		//move constructor
		other.head = nullptr;
		other.length = 0;
	}
	linked_list& operator=(const linked_list& other) {
		//copy assignment
		if(this == &other) return *this;
		clear();
		node_t** pps = &head,      //point to a pointer that point to self
		       * po  = other.head; //point other 
		while(po != nullptr) {
			*pps = new node_t{po->data/* copy */, nullptr};
			pps = &((*pps)->next);
			po = po->next;
		}
		length = other.length;
		return *this;
	}
	linked_list& operator=(linked_list&& other) noexcept{
		//move assignment
		if(this == &other) return *this;
		clear();
		head = other.head;
		length = other.length;
		other.head = nullptr;
		other.length = 0;
		return *this;
	}

	size_t size() const noexcept{return length; }

	//no zero length check
	T& front() {return head->data; }
	const T& front() const{return head->data; }
	T& back() {return tail()->data; }
	const T& back() const{return tail()->data; }


	iterator_t begin()              noexcept{return {head};    }
	iterator_t end()                noexcept{return {nullptr}; }
	const_iterator_t begin()  const noexcept{return {head};    }
	const_iterator_t end()    const noexcept{return {nullptr}; }
	const_iterator_t cbegin() const noexcept{return {head};    }
	const_iterator_t cend()   const noexcept{return {nullptr}; }

	bool is_empty() const noexcept{return !head; }


	template <typename U> 
	requires convertible_to<U, T>
	linked_list& push(U&& val) {
		auto pos = tail();
		if(!pos) head = new node_t{forward<U>(val), nullptr};
		else {
			//where tail != nullptr
			pos->next = new node_t{forward<U>(val), nullptr};
		}
		length++;
		return *this;
	}

	template <typename U>
	requires convertible_to<U, T>
	linked_list& unshift(U&& val) {
		if(!head) {
			head = new node_t{forward<U>(val), nullptr};
		}else {
			node_t* temp = head;
			head = new node_t{forward<U>(val), temp};
		}
		length++;
		return *this;
	}

	template <typename U>
	requires convertible_to<U, T>
	linked_list& insert(size_t index, U&& val)  {
		//the param index will be param val's new index of list, 
		//the previous element of the index will be shift to the next of the new element.
		if(index > length) index = length;
		node_t** pos = &head;
		for(size_t i = 0; i < index; i++) pos = &((*pos)->next);
		*pos = new node_t{forward<U>(val), *pos};
		length++;
		return *this;
	}

	template <typename U>
	requires convertible_to<U, T>
	linked_list& insert_after(const iterator_t& it, U&& val) {
		//insert val after the data that it points
		//no before_begin() / before_cbegin() method, 
		//it means it's impossible to insert a element to the head
		//no iterator validity check
		it.get_ptr()->next = new node_t{*it, it.get_ptr()->next};
		length++;
		return *this;
	}

	T& operator[](size_t index) {
		//no boundary check
		node_t* pos = head;
		for(size_t i = 0; i < index; i++) pos = pos->next;
		return pos->data;
	}

	const T& operator[](size_t index) const{
		//no boundary check
		node_t* pos = head;
		for(size_t i = 0; i < index; i++) pos = pos->next;
		return pos->data;
	}

	T* get_ptr(size_t index) {
		if(index >= length) return nullptr;
		node_t* pos = head;
		for(size_t i = 0; i < index; i++) pos = pos->next;
		return &(pos->data);
	}

	const T* get_ptr(size_t index) const{
		if(index >= length) return nullptr;
		node_t* pos = head;
		for(size_t i = 0; i < index; i++) pos = pos->next;
		return &(pos->data);
	}

	bool erase(size_t index) {
		//returns whether erasing satisfied
		if(index >= length) return false;
		node_t** pos = &head;
		for(size_t i = 0; i < index; i++) pos = &((*pos)->next);
		node_t* temp = (*pos)->next;
		delete *pos;
		*pos = temp;
		length--;
		return true;
	}


	void erase_after(const iterator_t& it) {
		//no before_begin() / before_cbegin() method, 
		//it means it's impossible to erase a element of the head
		//no iterator validity check, therefore it's useless to return whether the operation is satisfied.
		node_t* afters = it.get_ptr()->next->next;
		delete it.get_ptr()->next;
		it.get_ptr()->next = afters;
		length--;
	}	

	T shift() {
		//no zero length check
		T temp = move(head->data);
		node_t* old_head = head;
		head = head->next;
		delete old_head;
		length--;
		return temp;
	}

	T pop() {
		//no zero length check
		node_t** pos = &head;
		while((*pos)->next != nullptr) pos = &((*pos)->next);
		T temp = move((*pos)->data);
		delete *pos;
		*pos = nullptr;
		length--;
		return temp;
	}

	void clear() {
		if(!head) return;
		node_t* pos = head;
		while(pos != nullptr) {
			node_t* temp = pos->next;
			delete pos;
			pos = temp;
		}
		head = nullptr;
		length = 0;
	}

	void reverse() noexcept{
		if(length <= 1) return;
		node_t* l = nullptr, 
		      * c = head, 
		      * r = head->next;
		while(c->next != nullptr) {
			c->next = l;
			l = c;
			c = r;
			r = r->next;	
		}
		c->next = l;
		head = c;

	}

	template <typename ListT, typename CompareT = less<T>>
	requires same_as<remove_reference_t<ListT>, linked_list> && predicate<CompareT, T, T>
	void merge(ListT&& other, const CompareT& comp = {}) {
		//if a < b in some order, then comp(a, b) should returns true, otherwise returns false.
		//merge two 'sorted' linked_list to one, by increasing order
		if(this == &other) return;
		node_t** ppnew = &head,
		       * ps = head,
		       * po = other.head;
		while(ps != nullptr && po != nullptr) {
			// if(po->data < ps->data) {
			if( comp(po->data, ps->data) ) {
				//merge po
				*ppnew = po;
				po = po->next;
			}else {
				*ppnew = ps;
				ps = ps->next;
			}
			ppnew = &((*ppnew)->next);
		}
		if(ps == nullptr) {
			*ppnew = po;
		}else {
			*ppnew = ps;
		}
		length += other.length;
		other.length = 0;
		other.head = nullptr;
	}

	// template <typename CompareT = less<T>>
	// requires predicate<CompareT, T, T>
	// void quick_sort(const CompareT& comp = {}) {
		
	// }

	//using merge sort to sort linked_list, inplace, and stable.
	template <typename CompareT = less<T>, bool overflow_check = false> 
	requires predicate<CompareT, T, T>
	void sort(const CompareT& comp = {}) {
		if(length <= 1) return;

		node_t** pos = &head;

		//first sort
		// if((*pos)->next->data < (*pos)->data) {
		if( comp( (*pos)->next->data, (*pos)->data ) ) {
			swap_nodes<false>(*pos, (*pos)->next);
		}	
		for(size_t i = 1; i < length / 2; i++) {
			pos = &((*pos)->next->next);
			// if((*pos)->next->data < (*pos)->data) {
			if( comp( (*pos)->next->data, (*pos)->data ) ) {	
				swap_nodes<false>(*pos, (*pos)->next);
			}
		}

		//sort and merge
		//O( log2(N) )
		size_t merge_size = 2, group_size = 4;
		while(group_size <= length) { 
			//O( N )
			pos = &head; //reset pos
			//complete groups merging
			for(size_t group = 0; group < (length / group_size); group++) {
				node_t** mid = next_n<false>(pos, merge_size),
				      ** to  = next_n<false>(mid, merge_size);
				//merge the group, and point to next group
				pos = merge_nodes(pos, mid, to, comp);
				//it's an error: 
				//pos = to; because where 'to' might be invalidated. 
			}
			//incomplete group merging
			if((length % group_size) > merge_size) {
				node_t** mid = next_n<false>(pos, merge_size);
				merge_nodes(pos, mid, next_n<true>(mid, merge_size)/*this might out of range*/, comp);
			}

			merge_size = group_size;
			//overflow check, it's hard to reach this situation, so check is defaultly disabled.
			if constexpr(overflow_check) {
				if(group_size > (numeric_limits<size_t>::max() / 2)) {
					group_size = numeric_limits<size_t>::max();
					break;
				}
			}
			group_size *= 2;

		}
		if(length > merge_size) {
			//last merge
			//where list contains a complete group and a incomplete group
			node_t** mid = next_n<false>(&head, merge_size);
			merge_nodes(&head, mid, next_n<true>(mid, merge_size)/*this also might out of range*/, comp);
		}
	}

	// template <typename CompareT>

	template <typename CompareT = less_equal<T>> //where CompareT should be less_equal<T> to match sort(less<T>{})
	requires predicate<CompareT, T, T>
	bool is_sorted(const CompareT& comp = {}) {
		node_t* pos = head;
		while(pos->next != nullptr) {
			if( !comp(pos->data, pos->next->data) ) return false; 
			pos = pos->next;
		}
		return true;
	}

	friend void swap(linked_list& a, linked_list& b) noexcept{
		node_t* temp = a.head;
		size_t temp_len = a.length;
		a.head = b.head;
		a.length = b.length;
		b.head = temp;
		b.length = temp_len;
	}

private:
	template <bool null_check = true>
	static node_t** next_n(node_t** pos, size_t n) noexcept(null_check) {
		//advance pos n times
		for(size_t i = 0; i < n; i++) {
			if constexpr(null_check) {
				if(*pos == nullptr) return pos;
			}
			pos = &((*pos)->next);
		}
		return pos;
	}

	template <bool null_check = true>
	static void swap_nodes(node_t*& a, node_t*& b) noexcept(null_check) {
		if constexpr(null_check) {
			if(a == nullptr) {
				if(b == nullptr) return;
				a = b;
				b = b->next;
				a->next = nullptr;
				return;
			}
			if(b = nullptr) {
				b = a;
				a = a->next;
				b->next = nullptr;
				return;	
			}
		} // null_check

		//it works well when a == b or a == b->next or etc.
		//swap two nodes
		node_t* temp = a;
		a = b;
		b = temp;
		//swap two nodes' next nodes
		temp = a->next;
		a->next = b->next;
		b->next = temp;
	}

	template <typename CompareT = less<T>>
	requires predicate<CompareT, T, T>
	static node_t** merge_nodes(node_t** from, node_t** mid, node_t** to, const CompareT& comp = {}) {
		//returns the new seq's tail ptr ptr.
		//[from, mid) is a ordered seq, [mid, to) is also a ordered seq;
		//merge these two seqs. 
		//assume that |index(from) - index(mid)| >= 2 and
		//            |index(mid)  - index(to) | >= 1
		node_t** pos = from,
		       * pa = *from, 
		       * pb = *mid;
		while(pa != *mid and pb != *to) {
			// pb->data < pa->data
			if( comp(pb->data, pa->data) ) {
				*pos = pb;
				pb = pb->next;
			}else {
				*pos = pa;
				pa = pa->next;
			}
			pos = &((*pos)->next);
		}

		if(pa == *mid) {
			*pos = pb;
			return to;  //return new seq tail
		}else {
			//pb == *to
			*mid = *to;
			*pos = pa;
			return mid; //return new seq tail
		}

	}


}; //class linked_list<T>


template <typename DataT>
std::ostream& operator<<(std::ostream& os, const linked_list<DataT>& list) {
	os << '[';
	if(list.size() != 0) {
		os << *list.cbegin();
		for(auto it = ++list.cbegin(); it != list.cend(); ++it) {
			os << ", " << *it;
		}
	}
	return os << ']';
}


} //namespace rais::study

using namespace rais::study;

int main() {
	std::ios::sync_with_stdio();
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
	list4.merge(list3, [](int a, int b){return a < b;});
	std::cout << "merged. \nlist3: " << list3 << "\nlist4: " << list4 << '\n';
	std::cout << "12. test initializer_list constructor: " << linked_list<char>{'2','3', '4', 'c', 'x', 'n'} << "\n\n";
	auto list5 = linked_list<int>{99, 7, 3, 5, 7, 2, 2, 34, 53, 53, 12, 42, 94, 53, 81, 1, 4, 9};
	std::cout << "13. test sort: \nbefore sort: " << list5 << '\n';
	list5.sort();
	std::cout << " after sort: " << list5 << '\n';
	std::cout << "14. test is_sorted: " << std::boolalpha << list5.is_sorted() << '\n';



}