#pragma once
#include "xxfl_bplus_tree.h"

namespace xxfl {

template<typename _key_type,
         typename _compare = std::less<_key_type>,
         typename _allocator = std::allocator<_key_type>,
         uint32_t _bucket_bysize_max = XXFL_BPLUS_TREE_BUCKET_BYSIZE_MAX_DEFAULT,
         uint32_t _tree_height_max = XXFL_BPLUS_TREE_HEIGHT_MAX_DEFAULT>
class set
{
public:
    typedef _key_type  key_type;
    typedef _key_type  value_type;
    typedef _compare   key_compare;
    typedef _compare   value_compare;
    typedef _allocator allocator_type;

    typedef _bplus_tree<key_type, value_type, std::_Identity<value_type>, key_compare, allocator_type,
                        _bucket_bysize_max, _tree_height_max> _bplus_tree_type;

protected:
    _bplus_tree_type _tree;

    typedef typename __alloc_wrapper<allocator_type>::template rebind<key_type>::other _key_alloc_type;
    typedef __alloc_wrapper<_key_alloc_type> _alloc_wrapper;

public:
    typedef typename _alloc_wrapper::pointer                   pointer;
    typedef typename _alloc_wrapper::const_pointer             const_pointer;
    typedef typename _alloc_wrapper::reference                 reference;
    typedef typename _alloc_wrapper::const_reference           const_reference;
    typedef typename _bplus_tree_type::_const_iterator         iterator;
    typedef typename _bplus_tree_type::_const_iterator         const_iterator;
    typedef typename _bplus_tree_type::_const_reverse_iterator reverse_iterator;
    typedef typename _bplus_tree_type::_const_reverse_iterator const_reverse_iterator;
    typedef size_t                                             size_type;
    typedef ptrdiff_t                                          difference_type;

    set() {}

    explicit set(const key_compare& comp, const allocator_type& alloc = allocator_type())
    : _tree(comp, _key_alloc_type(alloc)) {}

    explicit set(const allocator_type& alloc) : _tree(key_compare(), _key_alloc_type(alloc)) {}

    template<typename _input_iterator>
    set(_input_iterator first, _input_iterator last)
    { _tree.insert_range(first, last); }

    template<typename _input_iterator>
    set(_input_iterator first, _input_iterator last,
        const key_compare& comp,
        const allocator_type& alloc = allocator_type())
    : _tree(comp, _key_alloc_type(alloc))
    { _tree.insert_range(first, last); }

    template<typename _input_iterator>
    set(_input_iterator first, _input_iterator last,
        const allocator_type& alloc)
    : _tree(key_compare(), _key_alloc_type(alloc))
    { _tree.insert_range(first, last); }

    set(const set& x) : _tree(x._tree) {}
    set(const set& x, const allocator_type& alloc) : _tree(x._tree, _key_alloc_type(alloc)) {}

    set(set&& x)
    noexcept(std::is_nothrow_copy_constructible<key_compare>::value)
    : _tree(std::move(x._tree)) {}

    set(set&& x, const allocator_type& alloc)
    noexcept(std::is_nothrow_copy_constructible<key_compare>::value &&
             _alloc_wrapper::allocator_always_compares_equal())
    : _tree(std::move(x._tree), _key_alloc_type(alloc)) {}

    set(std::initializer_list<value_type> il,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type())
    : _tree(comp, _key_alloc_type(alloc))
    { _tree.insert_range(il.begin(), il.end()); }

    set(std::initializer_list<value_type> il, const allocator_type& alloc)
    : _tree(key_compare(), _key_alloc_type(alloc))
    { _tree.insert_range(il.begin(), il.end()); }

    set& operator = (const set& x)
    {
        _tree = x._tree;
        return *this;
    }

    set& operator = (set&& x)
    noexcept(_alloc_wrapper::propagate_on_container_move_assignment() ||
             _alloc_wrapper::allocator_always_compares_equal())
    {
        _tree.move_assign(x._tree);
        return *this;
    }

    set& operator = (std::initializer_list<value_type> il)
    {
        _tree.clear();
        _tree.insert_range(il.begin(), il.end());
        return *this;
    }

    key_compare key_comp() const { return _tree._comp; }
    value_compare value_comp() const { return _tree._comp; }
    allocator_type get_allocator() const noexcept { return _tree._awrapper._alloc; }

    iterator begin() const noexcept { return _tree.cbegin(); }
    iterator end() const noexcept { return _tree.cend(); }

    reverse_iterator rbegin() const noexcept { return _tree.crbegin(); }
    reverse_iterator rend() const noexcept { return _tree.crend(); }

    const_iterator cbegin() const noexcept { return _tree.cbegin();}
    const_iterator cend() const noexcept { return _tree.cend(); }

    const_reverse_iterator crbegin() const noexcept { return _tree.crbegin(); }
    const_reverse_iterator crend() const noexcept { return _tree.crend(); }

    bool empty() const noexcept { return _tree._values_count == 0; }

    size_type size() const noexcept { return _tree._values_count; }
    size_type max_size() const noexcept { return _tree.max_size(); }

    void swap(set& x) noexcept(_alloc_wrapper::is_nothrow_swap()) { _tree.swap(x._tree); }

    template<typename... _args>
    std::pair<iterator, bool> emplace(_args&&... args)
    { return _tree.template insert<iterator>(value_type(std::forward<_args>(args)...)); }

    template<typename... _args>
    iterator emplace_hint(const const_iterator& position, _args&&... args)
    { return _tree.template insert<iterator>(position, value_type(std::forward<_args>(args)...)); }

    std::pair<iterator, bool> insert(const value_type& x)
    { return _tree.template insert<iterator>(x); }

    std::pair<iterator, bool> insert(value_type&& x)
    { return _tree.template insert<iterator>(std::move(x)); }

    iterator insert(const const_iterator& position, const value_type& x)
    { return _tree.template insert<iterator>(position, x); }

    iterator insert(const const_iterator& position, value_type&& x)
    { return _tree.template insert<iterator>(position, std::move(x)); }

    template<typename _input_iterator>
    void insert(_input_iterator first, _input_iterator last)
    { _tree.insert_range(first, last); }

    void insert(std::initializer_list<value_type> il)
    { _tree.insert_range(il.begin(), il.end()); }

    iterator erase(const const_iterator& position)
    { return _tree.template erase<iterator>(position); }

    size_type erase(const key_type& key)
    { return _tree.erase(key); }

    iterator erase(const const_iterator& first, const const_iterator& last)
    { return _tree.template erase_range<iterator>(first, last); }

    void clear() noexcept { _tree.clear(); }

    size_type count(const key_type& key) const
    { return _tree.template find<const_iterator>(key)._value_ptr != nullptr; }

    iterator find(const key_type& key)
    { return _tree.template find<iterator>(key); }

    const_iterator find(const key_type& key) const
    { return _tree.template find<const_iterator>(key); }

    iterator lower_bound(const key_type& key)
    { return _tree.template lower_bound<iterator>(key); }

    const_iterator lower_bound(const key_type& key) const
    { return _tree.template lower_bound<const_iterator>(key); }

    iterator upper_bound(const key_type& key)
    { return _tree.template upper_bound<iterator>(key); }

    const_iterator upper_bound(const key_type& key) const
    { return _tree.template upper_bound<const_iterator>(key); }

    std::pair<iterator, iterator> equal_range(const key_type& key)
    { return _tree.template equal_range<iterator>(key); }

    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
    { return _tree.template equal_range<const_iterator>(key); }

    template<typename _a, typename _b, typename _c, uint32_t _d, uint32_t _e>
    friend bool xxfl::operator == (const xxfl::set<_a, _b, _c, _d, _e>&,
                                   const xxfl::set<_a, _b, _c, _d, _e>&);

    template<typename _a, typename _b, typename _c, uint32_t _d, uint32_t _e>
    friend bool xxfl::operator < (const xxfl::set<_a, _b, _c, _d, _e>&,
                                  const xxfl::set<_a, _b, _c, _d, _e>&);
};

template<typename _a, typename _b, typename _c, uint32_t _d, uint32_t _e>
inline bool operator == (const xxfl::set<_a, _b, _c, _d, _e>& x,
                         const xxfl::set<_a, _b, _c, _d, _e>& y)
{ return x._tree == y._tree; }

template<typename _a, typename _b, typename _c, uint32_t _d, uint32_t _e>
inline bool operator < (const xxfl::set<_a, _b, _c, _d, _e>& x,
                        const xxfl::set<_a, _b, _c, _d, _e>& y)
{ return x._tree < y._tree; }

template<typename _a, typename _b, typename _c, uint32_t _d, uint32_t _e>
inline bool operator != (const xxfl::set<_a, _b, _c, _d, _e>& x,
                         const xxfl::set<_a, _b, _c, _d, _e>& y)
{ return !(x == y); }

template<typename _a, typename _b, typename _c, uint32_t _d, uint32_t _e>
inline bool operator > (const xxfl::set<_a, _b, _c, _d, _e>& x,
                        const xxfl::set<_a, _b, _c, _d, _e>& y)
{ return y < x; }

template<typename _a, typename _b, typename _c, uint32_t _d, uint32_t _e>
inline bool operator <= (const xxfl::set<_a, _b, _c, _d, _e>& x,
                         const xxfl::set<_a, _b, _c, _d, _e>& y)
{ return !(y < x); }

template<typename _a, typename _b, typename _c, uint32_t _d, uint32_t _e>
inline bool operator >= (const xxfl::set<_a, _b, _c, _d, _e>& x,
                         const xxfl::set<_a, _b, _c, _d, _e>& y)
{ return !(x < y); }

template<typename _a, typename _b, typename _c, uint32_t _d, uint32_t _e>
inline void swap(xxfl::set<_a, _b, _c, _d, _e>& x,
                 xxfl::set<_a, _b, _c, _d, _e>& y)
{ x.swap(y); }

} // xxfl
