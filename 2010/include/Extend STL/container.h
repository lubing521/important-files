#ifndef I8_CONTAINER_H
#define I8_CONTAINER_H


#include <algorithm>
#include <functional>

#include <deque>
#include <list>
#include <map>
#include <assert.h>

#include "sync.h"

namespace i8desk {
	
	template<
		typename T,
		typename C = std::deque<T>,
		typename L = ZSync
	> 
	class ZSequenceContainer
	{
	public:
		typedef typename C::value_type	value_type;
		typedef typename C::iterator	iterator;
		typedef typename C::const_iterator const_iterator;

		ZSequenceContainer()
		{
			//m_l.Create();
		}
		~ZSequenceContainer() 
		{
		}

		iterator begin()
		{
			I8_GUARD(ZGuard, L, m_l);
			return m_q.begin();
		}
		iterator end()
		{
			I8_GUARD(ZGuard, L, m_l);
			return m_q.end();
		}

		const_iterator begin() const
		{
			I8_GUARD(ZGuard, L, m_l);
			return m_q.begin();
		}
		const_iterator end() const
		{
			I8_GUARD(ZGuard, L, m_l);
			return m_q.end();
		}

		bool empty(void) const 
		{
			I8_GUARD(ZGuard, L, m_l);
			return m_q.empty(); 
		}
		size_t size(void) const 
		{
			I8_GUARD(ZGuard, L, m_l);
			return m_q.size(); 
		}
	
		void swap(C &c)
		{
			m_q.swap(c);
		}


		template<typename V>
		bool contained(const V& val)
		{
			I8_GUARD(ZGuard, L, m_l);
			return std::find(m_q.begin(), m_q.end(), val) != m_q.end();
		}		
		template<typename Op>
		bool contained_if(Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			return std::find_if(m_q.begin(), m_q.end(), op) != m_q.end();
		}
		void put_head(const T& elem)
		{
			I8_GUARD(ZGuard, L, m_l);
			m_q.push_front(elem);
		}		
		void put_tail(const T& elem)
		{
			I8_GUARD(ZGuard, L, m_l);
			m_q.push_back(elem);
		}
		bool peek_head(T& elem) 
		{
			I8_GUARD(ZGuard, L, m_l);
			if (m_q.empty())
				return false;
			elem = m_q.front();
			return true;
		}
		bool peek_tail(T& elem) 
		{
			I8_GUARD(ZGuard, L, m_l);
			if (m_q.empty())
				return false;
			elem = m_q.back();
			return true;
		}
		template<typename V>
		bool peek(const V& val, T& elem) 
		{
			I8_GUARD(ZGuard, L, m_l);
			C::iterator it = std::find(m_q.begin(), m_q.end(), val);
			if (it == m_q.end())
				return false;
			elem = *it;
			return true;
		}
		bool get_head(T& elem)
		{
			I8_GUARD(ZGuard, L, m_l);
			if (m_q.empty())
				return false;
			elem = m_q.front();
			m_q.pop_front();
			return true;
		}
		bool get_tail(T& elem)
		{
			I8_GUARD(ZGuard, L, m_l);
			if (m_q.empty())
				return false;
			elem = m_q.back();
			m_q.pop_back();
			return true;
		}
		template<typename V>
		bool findI(const V& val, T& elem)
		{
			I8_GUARD(ZGuard, L, m_l);
			C::iterator it = std::find(m_q.begin(), m_q.end(), val);
			if (it == m_q.end())
				return false;
			elem = *it;
			return true;
		}
		template<typename V>
		bool get(const V& val, T& elem)
		{
			I8_GUARD(ZGuard, L, m_l);
			C::iterator it = std::find(m_q.begin(), m_q.end(), val);
			if (it == m_q.end())
				return false;
			elem = *it;
			m_q.erase(it);
			return true;
		}
		template<typename Pred>
		bool get_if(const Pred &op, T& elem)
		{
			I8_GUARD(ZGuard, L, m_l);
			C::iterator it = std::find_if(m_q.begin(), m_q.end(), op);
			if (it == m_q.end())
				return false;
			elem = *it;
			m_q.erase(it);
			return true;
		}

		template<typename V>
		void remove(const V& val)
		{
			I8_GUARD(ZGuard, L, m_l);
			C::iterator it = std::find(m_q.begin(), m_q.end(), val);
			if (it != m_q.end()) 
				m_q.erase(it);
		}	
		template<typename Op>
		void remove_if(Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			m_q.erase(std::remove_if(m_q.begin(), m_q.end(), op), m_q.end());
		}
		template<typename V>
		void move_to_tail(const V& val)
		{
			I8_GUARD(ZGuard, L, m_l);
			C::iterator it = std::find(m_q.begin(), m_q.end(), val);
			if (it != m_q.end()) {
				T e = *it;
				m_q.erase(it);
				m_q.push_back(e);
			}
		}
		template<typename V>
		void move_to_head(const V& val)
		{
			I8_GUARD(ZGuard, L, m_l);
			C::iterator it = std::find(m_q.begin(), m_q.end(), val);
			if (it != m_q.end()) {
				T e = *it;
				m_q.erase(it);
				m_q.push_front(e);
			}
		}
		void clear(void)
		{
			I8_GUARD(ZGuard, L, m_l);
			m_q.clear();
		}
		template<typename Pred>
		void sort(Pred pred)
		{
			I8_GUARD(ZGuard, L, m_l);
			std::sort(m_q.begin(), m_q.end(), pred);
		}		
		template<typename Pred>
		void stable_sort(Pred pred)
		{
			I8_GUARD(ZGuard, L, m_l);
			std::stable_sort(m_q.begin(), m_q.end(), pred);
		}

		//重要提示：
		//所有的传入的op及pred中不能调用本对象的任何函数, 否则将产生死锁
		//除非使用空锁或线程递归锁来实例化
		template<typename Op>
		Op foreach(Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			return std::for_each(m_q.begin(), m_q.end(), op);
		}
		template<typename Op>
		Op rforeach(Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			return std::for_each(m_q.rbegin(), m_q.rend(), op);
		}	
		template<typename Op, typename Pred>
		Op op_if(Op op, Pred pred)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ++it) 
			{
				if (pred(*it)) 
					op(*it);
			}
			return op;
		}
		template<typename Op, typename Pred>
		Op op_if_break(Op op, Pred pred)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ++it) 
			{
				if (pred(*it)) 
				{
					op(*it);
					break;
				}
			}
			return op;
		}	
	private:
		mutable L m_l; //锁
		C m_q; //容器
	};

	template<
		typename K, 
		typename T, 
		typename C = std::map<K, T>, 
		typename L = ZSync
	>
	class ZAssociatedContainer
	{
	public:
		typedef typename C::value_type element_type;
		typedef typename C::value_type::first_type key_type;
		typedef typename C::value_type::second_type value_type;
		typedef typename C::iterator iterator;
		typedef typename C::const_iterator const_iterator;


		ZAssociatedContainer()
		{
			
		}
		~ZAssociatedContainer() 
		{
		}

		iterator begin()
		{
			I8_GUARD(ZGuard, L, m_l);
			return m_q.begin();
		}
		const_iterator begin() const
		{
			I8_GUARD(ZGuard, L, m_l);
			return m_q.begin();
		}

		iterator end()
		{
			I8_GUARD(ZGuard, L, m_l);
			return m_q.end();
		}
		const_iterator end() const
		{
			I8_GUARD(ZGuard, L, m_l);
			return m_q.end();
		}
		bool empty(void) const 
		{
			I8_GUARD(ZGuard, L, m_l);
			return m_q.empty(); 
		}
		size_t size(void) const 
		{
			I8_GUARD(ZGuard, L, m_l);
			return m_q.size(); 
		}
		template<typename Key>
		bool contained(const Key& key)
		{
			I8_GUARD(ZGuard, L, m_l);
			return m_q.find(key) != m_q.end();
		}
		template<typename Op>
		bool contained_if_key(Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			using std::tr1::placeholders::_1;
			return std::find_if(m_q.begin(), m_q.end(),
						std::tr1::bind(op, 
							std::tr1::bind(&C::value_type::first, _1))
							) != m_q.end();
		}
		template<typename Op>
		bool contained_if_value(Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			using std::tr1::placeholders::_1;
			return std::find_if(m_q.begin(), m_q.end(),
						std::tr1::bind(op, 
							std::tr1::bind(&C::value_type::second, _1))
							) != m_q.end();
		}
		template<typename Key>
		void put(const Key& key, const T& elem)
		{
			assert(!contained(key));
			I8_GUARD(ZGuard, L, m_l);
			m_q.insert(std::make_pair<K, T>(key, elem));
		}
		template<typename Key>
		void set(const Key& key, const T& elem)
		{
			I8_GUARD(ZGuard, L, m_l);
			m_q[key] = elem;
		}
		template<typename Key>
		bool peek(const Key& key, T& elem)
		{
			I8_GUARD(ZGuard, L, m_l);
			C::iterator it = m_q.find(key);
			if (it == m_q.end())
				return false;
			elem = it->second;
			return true;
		}		
		bool peek_head(T& elem) 
		{
			I8_GUARD(ZGuard, L, m_l);
			C::iterator it = m_q.begin();
			if (it == m_q.end())
				return false;
			elem = it->second;
			return true;
		}
		bool peek_tail(T& elem) 
		{
			I8_GUARD(ZGuard, L, m_l);
			C::reverse_iterator rit = m_q.rbegin();
			if (rit == m_q.rend())
				return false;
			C::iterator it = rit.base();
			elem = it->second;
			return true;
		}	
		template<typename Key>
		bool get(const Key& key, T& elem)
		{
			I8_GUARD(ZGuard, L, m_l);
			C::iterator it = m_q.find(key);
			if (it == m_q.end())
				return false;
			elem = it->second;
			m_q.erase(it);
			return true;
		}
		bool get_head(T& elem)
		{
			I8_GUARD(ZGuard, L, m_l);
			C::iterator it = m_q.begin();
			if (it == m_q.end())
				return false;
			elem = it->second;
			m_q.erase(it);
			return true;
		}
		bool get_tail(T& elem)
		{
			I8_GUARD(ZGuard, L, m_l);
			C::reverse_iterator rit = m_q.rbegin();
			if (rit == m_q.rend())
				return false;
			C::iterator it = rit.base();
			elem = it->second;
			m_q.erase(it);
			return true;
		}
		template<typename Key>
		void remove(const Key& key)
		{
			I8_GUARD(ZGuard, L, m_l);
			C::iterator it = m_q.find(key);
			if (it != m_q.end()) 
				m_q.erase(it);
		}
		template<typename Op>
		void remove_if(Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ) {
				if (op(*it)) {
					it = m_q.erase(it);
					//it = m_q.begin();
				} else {
					++it;
				}
			}
		}
		template<typename Op>
		void remove_if_key(Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ) {
				if (op(it->first)) {
					it = m_q.erase(it);
					//it = m_q.begin();
				} else {
					++it;
				}
			}
		}
		template<typename Op>
		void remove_if_value(Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ) {
				if (op(it->second)) {
					it = m_q.erase(it);
					//it = m_q.begin();
				} else {
					++it;
				}
			}
		}
		void clear(void)
		{
			I8_GUARD(ZGuard, L, m_l);
			m_q.clear();
		}

		//重要提示：
		//所有的传入的op及pred中不能调用本对象的任何函数, 否则将产生死锁
		//除非使用空锁或线程递归锁来实例化
		template<typename Op>
		Op foreach(Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			return std::for_each(m_q.begin(), m_q.end(), op);
		}
		template<typename Op>
		Op foreach_key(Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ++it) {
				op(it->first);
			}
			return op;
		}
		template<typename Op>
		Op foreach_value(Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ++it) {
				op(it->second);
			}
			return op;
		}
		template<typename Op>
		Op rforeach_value(Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::reverse_iterator it = m_q.rbegin(); it != m_q.rend(); ++it) {
				op(it->second);
			}
			return op;
		}		
		template<typename Op, typename Pred>
		Op op_key_if(Op op, Pred pred)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ++it) {
				if (pred(it->first)) {
					op(it->first);
				}
			}
			return op;
		}
		template<typename Op, typename Pred>
		Op op_key_if_break(Op op, Pred pred)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ++it) {
				if (pred(it->first)) {
					op(it->first);
					break;
				}
			}
			return op;
		}	
		template<typename Op, typename Pred>
		Op op_value_if(Op op, Pred pred)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ++it) {
				if (pred(it->second)) {
					op(it->second);
				}
			}
			return op;
		}
		template<typename Op, typename Pred>
		Op op_value_if_break(Op op, Pred pred)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ++it) {
				if (pred(it->second)) {
					op(it->second);
					break;
				}
			}
			return op;
		}	
		template<typename Op, typename Pred>
		Op op_value_if_key(Op op, Pred pred)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ++it) {
				if (pred(it->first)) {
					op(it->second);
				}
			}
			return op;
		}
		template<typename Op, typename Pred>
		Op op_value_if_key_break(Op op, Pred pred)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ++it) {
				if (pred(it->first)) {
					op(it->second);
					break;
				}
			}
			return op;
		}	
		template<typename Op, typename Pred>
		Op op_key_if_value(Op op, Pred pred)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ++it) {
				if (pred(it->second)) {
					op(it->first);
				}
			}
			return op;
		}
		template<typename Op, typename Pred>
		Op op_key_if_value_break(Op op, Pred pred)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.begin(); it != m_q.end(); ++it) {
				if (pred(it->second)) {
					op(it->first);
					break;
				}
			}
			return op;
		}	
	
		template<typename Key, typename Op>
		Op op_value_lowerboundkey_if_value_break(Key key, Op op)
		{
			I8_GUARD(ZGuard, L, m_l);
			for (C::iterator it = m_q.lower_bound(key); it != m_q.end(); ++it) {
				if (op(it->second)) {
					break;
				}
			}
			return op;
		}		
	private:
		mutable L m_l; //锁
		C m_q; //容器
	};

} //namespace i8desk

#endif 