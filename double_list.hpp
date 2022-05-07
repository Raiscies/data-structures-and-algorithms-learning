#pragma once

//双向链表实现

#include <cstddef>
#include <utility>
#include <concept>
#include <initializer_list>

namespace rais::study {

using std::size_t;
using std::forward;
using std::initializer_list;

//concepts
using std::convertible_to;

template <typename T>
struct double_node {

	T data;
	double_node* priv, //point to privious node
	           * next; //point to next node

	template <typename U>
	requires convertible_to<U, const T&>
	double_node(U&& val): data(forward<U>(val)) {}

	template <typename U>
	requires convertible_to<U, const T&>
	double_node(U&& val, double_node* priv): data(forward<U>(val)), priv{priv} {}

	template <typename U>
	requires convertible_to<U, const T&>
	double_node(U&& val, double_node* priv, double_node* next): data(forward<U>(val)), priv{priv}, next{next} {}
};


/* 
 * 双链表实现.
 * - 链表为空时头指针head == nullptr
 * - 链表不为空时head->priv == tail, 即头节点的前继指针指向尾节点, 
 *   但尾节点的后继指针指向nullptr, 即tail->next == nullptr
 * 
 *
 */
template <typename T>
class double_list {

public:

	using element_t = T;
	using node_t = double_node<T>;

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
		iterator& operator--() {it = it->priv; return *this;}
		iterator operator--(int) {auto temp = iterator{it}; it = it->priv; return temp;}
		bool operator==(const iterator& other) const noexcept{return it == other.it; }
		bool operator!=(const iterator& other) const noexcept{return it != other.it; }

		node_t* get_ptr() noexcept{return it; }
		const node_t* get_ptr() const noexcept{return it; }
	};

	struct const_iterator {
	private:
		const node_t* it;

	public:
		const_iterator(const node_t* it): it{it} {}
		const T& operator*() const{ return it->data; }
		const T* operator->() const noexcept{return &(it->data); }
		const_iterator& operator++() {it = it->next; return *this;}
		const_iterator operator++(int) {auto temp = const_iterator{it}; it = it->next; return temp;}
		const_iterator& operator--() {it = it->priv; return *this;}
		const_iterator operator--(int) {auto temp = const_iterator{it}; it = it->priv; return temp;}
		bool operator==(const const_iterator& other) const noexcept{return it == other.it; }
		bool operator!=(const const_iterator& other) const noexcept{return it != other.it; }

		const node_t* get_ptr() const noexcept{return it; }
	};
	using iterator_t = iterator;
	using const_iterator_t = const_iterator;


	node_t* head = nullptr;
	size_t len = 0;


	double_list() {}
	double_list(initializer_list<T> list) {
		const auto it = list.begin();
		head = new node_t{*it};
		node_t* pos = head;
		++it;
		for(; it != list.end(); ++it) {
			pos->next = new node_t{i, pos};
			pos = pos->next;
		}
		pos->next = nullptr;
		head->priv = pos;
		len = list.size();
	}
	double_list(const double_list& other) {
		const auto it = other.begin();
		head = new node_t{*it};
		node_t* pos = head;
		++it;
		for(; it != other.end(); ++it) {
			pos->next = new node_t{i, pos};
			pos = pos->next;
		}
		pos->next = nullptr;
		head->priv = pos;
		len = other.len;
	}
	double_list(double_list&& other) noexcept: head{other.head}, len{other.len} {
		other.head = nullptr;
		other.len = 0;
	}
	double_list& operator=(const double_list& other) {
		if(this == &other) return *this;
		auto temp = other;
		return *this = move(temp);	
	}
	double_list& operator=(double_list&& other) noexcept{
		if(this == &other) return *this;
		clear();
		head = other.head;
		len = other.len;
		other.head = nullptr;
		other.len = 0;
		return *this;
	}

	~double_list() {
		clear();
	}

	size_t length() const noexcept{ return len; } 
	size_t size() const noexcept{ return len; }

	//no zero length check
	T& front() {return head->data; }
	const T& front() const{return head->data; }
	T& back() {return head->priv->data; }
	const T& back() const{return head->priv->data; }


	iterator_t begin()              noexcept{return {head};    }
	iterator_t end()                noexcept{return {nullptr}; }
	const_iterator_t begin()  const noexcept{return {head};    }
	const_iterator_t end()    const noexcept{return {nullptr}; }
	const_iterator_t cbegin() const noexcept{return {head};    }
	const_iterator_t cend()   const noexcept{return {nullptr}; }

	bool is_empty() const noexcept{return !head; }


	template <typename U>
	requires convertible_to<U, const T&>
	double_list& push(U&& val) {
		if(!head) {
			head = new node_t{forward<U>(val), nullptr, nullptr};
			head->priv = head;
			// head<--head-->nullptr
		}else {
			node_t* tail = head->priv;
			head->priv = tail->next = new node_t{forward<U>(val), tail_node, nullptr};
		}
		len++;
		return *this;
	}

	template <typename U>
	requires convertible_to<U, const T&>
	double_list& unshift(U&& val) {
		head = new node_t{forward<U>(val), head->priv, head};
		len++;
		return *this;
	}

	template <typename U>
	requires convertible_to<U, const T&>
	double_list& insert(size_t index, U&& val) {
		if(head == nullptr) {
			//len == 0
			head = new node_t{forward<U>(val), nullptr, nullptr};
			head->priv = head;
		}else if(index > len) {
			//insert to the tail
			head->priv = head->priv->next = new node_t{forward<U>(val), head->priv, nullptr};
		}else {
			node_t* pos = get_node(index);
			pos->priv = pos->priv->next = new node_t{forward<U>(val), pos->priv, pos};
		}
	
		len++;
		return *this;
	}

	template <typename U>
	requires convertible_to<U, const T&>
	double_list& insert(iterator_t it, U&& val) {
		if(head == nullptr) {
			//len == 0
			head = new node_t{forward<U>(val), nullptr, nullptr};
			head->priv = head;
		}else if(it.get_ptr() == nullptr) {
			//insert to the tail
			head->priv = head->priv->next = new node_t{forward<U>(val), head->priv, nullptr};
		}else {
			it.get_ptr()->priv = it.get_ptr()->priv->next = new node_t{forward<U>(val), it.get_ptr()->priv, it.get_ptr()};
		}
		len++;
		return *this;
	}

	T& operator[](size_t index) {
		return const_cast<T&>(static_cast<const double_list&>(*this)[index]);
	}
	const T& operator[](size_t index) const{
		//no zero length and boundary check
		const node_t* pos = get_node(index);
		return pos->data;
	}

	T* get_ptr(size_t index) {
		return const_cast<T*>(static_cast<const double_list*>(this)->get_ptr(index));
	}
	const T* get_ptr(size_t index) const{
		if(index >= len) return nullptr;
		return &get_node(index)->data;
	}

	T shift() {
		//no zero length check
		T temp = move(head->data);
		node_t* old_head = head;
		head = head->next;
		if(head != nullptr) {
			//len != 1
			head->priv = old_head->priv;
		}
		delete old_head;
		len--;
		return temp;
	}

	T pop() {
		//no zero length check
		node_t* tail_node = head->priv;
		T temp = move(tail_node->data);
		if(head->next != nullptr) {
			//len != 1
			head->priv = tail_node->priv;
			tail_node->priv->next = nullptr;
		}else {
			//len == 1
			head = nullptr;
		}
		delete tail_node;
		len--;
		return temp;
	}

	bool erase(size_t index) {
		if(index >= len) return false;
		if(index == 0) {
			//erase head node
			node_t* old_head = head;
			head = head->next;
			if(len != 1) head->priv = old_head->priv;
			delete old_head;
		}else if(index == len - 1){
			//erase tail node
			node_t* old_tail = head->priv;
			old_tail->priv->next = nullptr;
			head->priv = old_tail->priv;
			delete old_tail;
		}else {
			node_t* pos = get_node(index);
			pos->priv->next = pos->next;
			pos->next->priv = pos->priv;
			delete pos;
		}
		len--;
		return true;
	}

	void clear() {
		if(head == nullptr) return;
		node_t* pos = head;
		for(size_t i = 0; i < len - 1; i++) {
			pos = pos->next;
			delete pos->priv;
		}
		delete pos;
		head = nullptr;
		len = 0;
	}

	friend void swap(double_list& a, double_list& b) noexcept{
		node_t* temp_head = a.head;
		size_t temp_len = a.len;
		a.head = b.head;
		a.len = b.len;
		b.head = temp_head;
		b.len = temp_len;
	}

	template <typename OStreamT>
	requires requires(OutputStreamT& os, const T& val, char c, const char* s) {
		{os << val}->same_as<OutputStreamT&>;
		{os << c}->same_as<OutputStreamT&>;
		{os << s}->same_as<OutputStreamT&>;
	}
	friend OStreamT& operator<<(OStreamT& os, const double_list& list) {
		os << '[';
		if(list.size() != 0) {
			os << *list.cbegin();
			for(auto it = ++list.cbegin(); it != list.cend(); ++it) {
				os << ", " << *it;
			}
		}
		return os << ']';
	}

protected:

	node_t* get_node(size_t index) {
		return const_cast<node_t*>(static_cast<const double_list*>(this)->get_node(index)); 
	}
	const node_t* get_node(size_t index) const{
		//no boundary and zero length check
		const node_t* pos = head;
			if(index <= len / 2) {
			//indexing from head
			for(size_t i = 0; i < index; i++) pos = pos->next;
		}else {
			//indexing from tail
			for(size_t i = 0; i < (len - index); i++) pos = pos->priv;
		}
		return pos;
	}


}; //class double_list

} //namespace rais::study