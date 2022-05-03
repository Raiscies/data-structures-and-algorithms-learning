#pragma once

//双向链表实现

#include <cstddef>
#include <utility>
#include <concept>

namespace rais::study {

using std::size_t;
using std::forward;

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
	~double_list() {
		//TODO
	}

	size_t length() const noexcept{ return len; } 
	size_t size() const noexcept{ return len; }

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

	}

	template <typename U>
	requires convertible_to<U, const T&>
	double_list& insert(iterator_t it, U&& val) {
		it.get_ptr()->priv->next = new node_t{forward<U>(val), it.get_ptr()->priv, it.get_ptr()};
	}

	T& operator[](size_t index) {
		return const_cast<T&>(static_cast<const double_list&>(*this)[index]);
	}
	const T& operator[](size_t index) const{
		//no zero length and boundary check
		const node_t* pos = head;
		if(index <= len / 2) {
			//indexing from head
			for(size_t i = 0; i < index; i++) pos = pos->next;
		}else {
			//indexing from tail
			for(size_t i = 0; i < (len - index); i++) pos = pos->priv;
		}
		return pos->data;
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

protected:
	node_t* tail() noexcept{
		if(head == nullptr) return nullptr;
		return head->priv;
	}
	const node_t* tail() const noexcept{
		if(head == nullptr) return nullptr;
		return head->priv;	
	} 
	// node_t*& tail() noexcept{
	// 	if(head == nullptr) return head;
	// 	node_t** pos = &head;
	// 	while((*pos)->next != nullptr) pos = &((*pos)->next);
	// 	return *pos;
	// }
	// const node_t* const & tail() const noexcept{
	// 	if(head == nullptr) return head;
	// 	const node_t* const * pos = &head;
	// 	while((*pos)->next != nullptr) pos = &((*pos)->next);
	// 	return *pos;
	// }


}; //class double_list

} //namespace rais::study