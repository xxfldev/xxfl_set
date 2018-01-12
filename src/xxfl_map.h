#pragma once
#include "xxfl_bplus_tree.h"

namespace xxfl {

template<typename _key_type,
         typename _mapped_type,
         typename _compare = std::less<_key_type>,
         typename _allocator = std::allocator<std::pair<const _key_type, _mapped_type> >,
         uint32_t _bucket_bysize_max = XXFL_BPLUS_TREE_BUCKET_BYSIZE_MAX_DEFAULT,
         uint32_t _tree_height_max = XXFL_BPLUS_TREE_HEIGHT_MAX_DEFAULT>
class map
{
public:
    typedef _key_type                                key_type;
    typedef _mapped_type                             mapped_type;
    typedef std::pair<const _key_type, _mapped_type> value_type;
    typedef std::pair<_key_type, _mapped_type>       moveable_value_type;
    typedef _compare                                 key_compare;
    typedef _allocator                               allocator_type;

    typedef _bplus_tree<key_type, value_type, moveable_value_type,
                        std::__select1st<value_type>, key_compare, allocator_type,
                        _bucket_bysize_max, _tree_height_max> _bplus_tree_type;

    struct value_compare
    {
        key_compare _comp;

        value_compare(key_compare comp) : _comp(comp) {}

        bool operator () (const value_type& x, const value_type& y) const
        { return _comp(x.first, y.first); }
    };

    _bplus_tree_type _tree;

protected:
    typedef typename __alloc_wrapper<allocator_type>::template rebind<key_type>::other _pair_alloc_type;
    typedef __alloc_wrapper<_pair_alloc_type> _alloc_wrapper;

public:
    typedef typename _alloc_wrapper::pointer                   pointer;
    typedef typename _alloc_wrapper::const_pointer             const_pointer;
    typedef typename _alloc_wrapper::reference                 reference;
    typedef typename _alloc_wrapper::const_reference           const_reference;
    typedef typename _bplus_tree_type::_iterator               iterator;
    typedef typename _bplus_tree_type::_const_iterator         const_iterator;
    typedef typename _bplus_tree_type::_reverse_iterator       reverse_iterator;
    typedef typename _bplus_tree_type::_const_reverse_iterator const_reverse_iterator;
    typedef size_t                                             size_type;
    typedef ptrdiff_t                                          difference_type;

    map() {}

    explicit map(const key_compare& comp, const allocator_type& alloc = allocator_type())
    : _tree(comp, _pair_alloc_type(alloc)) {}

    explicit map(const allocator_type& alloc) : _tree(key_compare(), _pair_alloc_type(alloc)) {}

    template<typename _input_iterator>
    map(_input_iterator first, _input_iterator last)
    { _tree.insert_range(first, last); }

    template<typename _input_iterator>
    map(_input_iterator first, _input_iterator last,
        const key_compare& comp,
        const allocator_type& alloc = allocator_type())
    : _tree(comp, _pair_alloc_type(alloc))
    { _tree.insert_range(first, last); }

    template<typename _input_iterator>
    map(_input_iterator first, _input_iterator last,
        const allocator_type& alloc)
    : _tree(key_compare(), _pair_alloc_type(alloc))
    { _tree.insert_range(first, last); }

    map(const map& x) : _tree(x._tree) {}
    map(const map& x, const allocator_type& alloc) : _tree(x._tree, _pair_alloc_type(alloc)) {}

    map(map&& x)
    noexcept(std::is_nothrow_copy_constructible<key_compare>::value)
    : _tree(std::move(x._tree)) {}

    map(map&& x, const allocator_type& alloc)
    noexcept(std::is_nothrow_copy_constructible<key_compare>::value &&
             _alloc_wrapper::allocator_always_compares_equal())
    : _tree(std::move(x._tree), _pair_alloc_type(alloc)) {}

    map(std::initializer_list<value_type> il,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type())
    : _tree(comp, _pair_alloc_type(alloc))
    { _tree.insert_range(il.begin(), il.end()); }

    map(std::initializer_list<value_type> il, const allocator_type& alloc)
    : _tree(key_compare(), _pair_alloc_type(alloc))
    { _tree.insert_range(il.begin(), il.end()); }

    map& operator = (const map& x)
    {
        _tree = x._tree;
        return *this;
    }

    map& operator = (map&& x)
    noexcept(_alloc_wrapper::propagate_on_container_move_assignment() ||
             _alloc_wrapper::allocator_always_compares_equal())
    {
        _tree.move_assign(x._tree);
        return *this;
    }

    map& operator = (std::initializer_list<value_type> il)
    {
        _tree.clear();
        _tree.insert_range(il.begin(), il.end());
        return *this;
    }

    key_compare key_comp() const { return _tree._comp; }
    value_compare value_comp() const { return value_compare(_tree._comp); }
    allocator_type get_allocator() const noexcept { return _tree._awrapper._alloc; }

    iterator begin() noexcept { return _tree.begin(); }
    const_iterator begin() const noexcept { return _tree.cbegin(); }
    iterator end() noexcept { return _tree.end(); }
    const_iterator end() const noexcept { return _tree.cend(); }

    reverse_iterator rbegin() noexcept { return _tree.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return _tree.crbegin(); }
    reverse_iterator rend() noexcept { return _tree.rend(); }
    const_reverse_iterator rend() const noexcept { return _tree.crend(); }

    const_iterator cbegin() const noexcept { return _tree.cbegin(); }
    const_iterator cend() const noexcept { return _tree.cend(); }

    const_reverse_iterator crbegin() const noexcept { return _tree.crbegin(); }
    const_reverse_iterator crend() const noexcept { return _tree.crend(); }

    bool empty() const noexcept { return _tree._values_count == 0; }

    size_type size() const noexcept { return _tree._values_count; }
    size_type max_size() const noexcept { return _tree.max_size(); }

    void swap(map& x) noexcept(_alloc_wrapper::is_nothrow_swap()) { _tree.swap(x._tree); }

    template<typename... _args>
    std::pair<iterator, bool> emplace(_args&&... args)
    { return _tree.template insert<iterator>(value_type(std::forward<_args>(args)...)); }

    template<typename... _args>
    iterator emplace_hint(const const_iterator& position, _args&&... args)
    { return _tree.template insert<iterator>(position, value_type(std::forward<_args>(args)...)); }

    std::pair<iterator, bool> insert(const value_type& x)
    { return _tree.template insert<iterator>(x); }

    template<typename _pair, typename = typename std::enable_if<std::is_constructible<value_type, _pair&&>::value>::type>
    std::pair<iterator, bool> insert(_pair&& x)
    { return _tree.template insert<iterator>(std::forward<_pair>(x)); }

    iterator insert(const const_iterator& position, const value_type& x)
    { return _tree.template insert<iterator>(position, x); }

    template<typename _pair, typename = typename std::enable_if<std::is_constructible<value_type, _pair&&>::value>::type>
    iterator insert(const const_iterator& position, _pair&& x)
    { return _tree.template insert<iterator>(position, std::forward<_pair>(x)); }

    template<typename _input_iterator>
    void insert(_input_iterator first, _input_iterator last)
    { _tree.insert_range(first, last); }

    void insert(std::initializer_list<value_type> il)
    { _tree.insert_range(il.begin(), il.end()); }

    iterator erase(const iterator& position)
    { return _tree.template erase<iterator>(position); }

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

    mapped_type& operator [] (const key_type& key)
    {
        iterator it(&_tree);

        if (_tree._values_count > 0)
        {
            _bplus_tree_node<value_type>* cur_node;
            it._value_ptr = _tree.lower_bound_core(key, it._stack, cur_node);

            if (it._value_ptr == cur_node->values_end() || _tree.key_less(key, *it))
            {
                _tree.insert_core(it,
                                  std::piecewise_construct,
                                  std::tuple<const key_type&>(key),
                                  std::tuple<>());
            }
        }
        else
        {
            _tree.insert_first_value(it,
                                     std::piecewise_construct,
                                     std::tuple<const key_type&>(key),
                                     std::tuple<>());
        }

        return (*it).second;
    }

    mapped_type& operator [] (key_type&& key)
    {
        iterator it(&_tree);

        if (_tree._values_count > 0)
        {
            _bplus_tree_node<value_type>* cur_node;
            it._value_ptr = _tree.lower_bound_core(key, it._stack, cur_node);

            if (it._value_ptr == cur_node->values_end() || _tree.key_less(key, *it))
            {
                _tree.insert_core(it,
                                  std::piecewise_construct,
                                  std::forward_as_tuple<const key_type&>(std::move(key)),
                                  std::tuple<>());
            }
        }
        else
        {
            _tree.insert_first_value(it,
                                     std::piecewise_construct,
                                     std::forward_as_tuple<const key_type&>(std::move(key)),
                                     std::tuple<>());
        }

        return (*it).second;
    }

    mapped_type& at(const key_type& key)
    {
        iterator it = _tree.template find<iterator>(key);
        if (it._value_ptr == nullptr)
        {
            std::__throw_out_of_range("xxfl::map::at");
        }
        return (*it).second;
    }

    const mapped_type& at(const key_type& key) const
     {
        const_iterator it = _tree.template find<const_iterator>(key);
        if (it._value_ptr == nullptr)
        {
            std::__throw_out_of_range("xxfl::map::at");
        }
        return (*it).second;
    }
};

template<typename _a, typename _b, typename _c, typename _d, uint32_t _e, uint32_t _f>
inline bool operator == (const xxfl::map<_a, _b, _c, _d, _e, _f>& x,
                         const xxfl::map<_a, _b, _c, _d, _e, _f>& y)
{ return x._tree == y._tree; }

template<typename _a, typename _b, typename _c, typename _d, uint32_t _e, uint32_t _f>
inline bool operator < (const xxfl::map<_a, _b, _c, _d, _e, _f>& x,
                        const xxfl::map<_a, _b, _c, _d, _e, _f>& y)
{ return x._tree < y._tree; }

template<typename _a, typename _b, typename _c, typename _d, uint32_t _e, uint32_t _f>
inline bool operator != (const xxfl::map<_a, _b, _c, _d, _e, _f>& x,
                         const xxfl::map<_a, _b, _c, _d, _e, _f>& y)
{ return !(x == y); }

template<typename _a, typename _b, typename _c, typename _d, uint32_t _e, uint32_t _f>
inline bool operator > (const xxfl::map<_a, _b, _c, _d, _e, _f>& x,
                        const xxfl::map<_a, _b, _c, _d, _e, _f>& y)
{ return y < x; }

template<typename _a, typename _b, typename _c, typename _d, uint32_t _e, uint32_t _f>
inline bool operator <= (const xxfl::map<_a, _b, _c, _d, _e, _f>& x,
                         const xxfl::map<_a, _b, _c, _d, _e, _f>& y)
{ return !(y < x); }

template<typename _a, typename _b, typename _c, typename _d, uint32_t _e, uint32_t _f>
inline bool operator >= (const xxfl::map<_a, _b, _c, _d, _e, _f>& x,
                         const xxfl::map<_a, _b, _c, _d, _e, _f>& y)
{ return !(x < y); }

template<typename _a, typename _b, typename _c, typename _d, uint32_t _e, uint32_t _f>
inline void swap(xxfl::map<_a, _b, _c, _d, _e, _f>& x,
                 xxfl::map<_a, _b, _c, _d, _e, _f>& y)
{ x.swap(y); }

} // xxfl
