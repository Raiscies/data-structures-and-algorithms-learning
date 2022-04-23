#include <cstddef>
#include <utility>
#include <initializer_list>
#include <concepts>
#include <type_traits>
#include <string>   //for testing
#include <iostream> //for testing

using std::size_t;
using std::initializer_list;
using std::forward;
using std::move;
using std::same_as;
using std::is_convertible_v;
using std::remove_reference_t;

namespace rais::study {


template <typename T>
struct list_node {
	T data;
	list_node* next;

	template <typename U>
	list_node(U&& val, list_node* next): data(forward<U>(val)), next(next) {}
	template <typename U>
	list_node(U&& val): data(forward<U>(val)) {}
};

template <typename T>
class linked_list {
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

	bool empty() const noexcept{return !head; }


	template <typename U> 
	linked_list& push(U&& val) requires is_convertible_v<U, T> {
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
	linked_list& unshift(U&& val) requires is_convertible_v<U, T> {
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
	linked_list& insert(size_t index, U&& val) requires is_convertible_v<U, T> {
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

	template <typename U>
	void merge(U&& other) requires same_as<remove_reference_t<U>, linked_list> && requires(T a, T b){a < b;} {
		//merge two 'sorted' linked_list to one, by increasing order
		if(this == &other) return;
		node_t** ppnew = &head,
		       * ps = head,
		       * po = other.head;
		while(ps != nullptr && po != nullptr) {
			if(po->data < ps->data) {
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

	template <typename U, typename CompareT>
	void merge(U&& other, CompareT&& comp) //if a < b in some order, then comp(a, b) should returns true, otherwise returns false.
	requires same_as<remove_reference_t<U>, linked_list> && requires(const T& a, const T& b){{comp(a, b)}->same_as<bool>;} {
		//merge two 'sorted' linked_list to one, by increasing order
		if(this == &other) return;
		node_t** ppnew = &head,
		       * ps = head,
		       * po = other.head;
		while(ps != nullptr && po != nullptr) {
			// if(po->data < ps->data) {
			if( comp(static_cast<const T&>(po->data), static_cast<const T&>(ps->data)) ) {
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

	friend void swap(linked_list& a, linked_list& b) noexcept{
		node_t* temp = a.head;
		size_t temp_len = a.length;
		a.head = b.head;
		a.length = b.length;
		b.head = temp;
		b.length = temp_len;
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
	std::cout << "12. test initializer_list constructor: " << linked_list<char>{'2','3', '4', 'c', 'x', 'n'} << '\n';


}