#pragma once

#include <cstddef>
#include <algorithm>
#include <cstring>
#include "xxfl_bplus_tree_iterator.h"

#if !defined(XXFL_BPLUS_TREE_BUCKET_BYSIZE_MAX_DEFAULT)
#define XXFL_BPLUS_TREE_BUCKET_BYSIZE_MAX_DEFAULT 2048
#endif

#if !defined(XXFL_BPLUS_TREE_HEIGHT_MAX_DEFAULT)
#define XXFL_BPLUS_TREE_HEIGHT_MAX_DEFAULT 4
#endif

namespace xxfl {

template<typename _value_type, uint32_t _tree_height_max>
struct _bplus_tree_base
{
    typedef _bplus_tree_iterator<_value_type, _tree_height_max>       _iterator;
    typedef _bplus_tree_const_iterator<_value_type, _tree_height_max> _const_iterator;
    typedef std::reverse_iterator<_iterator>                          _reverse_iterator;
    typedef std::reverse_iterator<_const_iterator>                    _const_reverse_iterator;
    typedef _bplus_tree_node<_value_type>                             _node_type;

    _node_type* _root_node;
    size_t _values_count;
    uint32_t _tree_height;

    _bplus_tree_base() noexcept : _root_node(nullptr), _values_count(0), _tree_height(0) {}

    _iterator begin() noexcept
    {
        _iterator it(this);
        it.set_first();
        return it;
    }

    _const_iterator cbegin() const noexcept
    {
        _const_iterator it(const_cast<_bplus_tree_base*>(this));
        it.set_first();
        return it;
    }

    _iterator end() noexcept { return _iterator(this, nullptr); }
    _const_iterator cend() const noexcept { return _const_iterator(const_cast<_bplus_tree_base*>(this), nullptr); }

    _reverse_iterator rbegin() noexcept { return _reverse_iterator(end()); }
    _const_reverse_iterator crbegin() const noexcept { return _const_reverse_iterator(cend()); }

    _reverse_iterator rend() noexcept { return _reverse_iterator(begin()); }
    _const_reverse_iterator crend() const noexcept { return _const_reverse_iterator(cbegin()); }
};

template<typename _key_type, typename _value_type, typename _moveable_value_type,
         typename _key_of_value, typename _compare, typename _allocator,
         uint32_t _bucket_bysize_max, uint32_t _tree_height_max>
struct _bplus_tree : _bplus_tree_base<_value_type, _tree_height_max>
{
    typedef _bplus_tree_base<_value_type, _tree_height_max> _base;

    using typename _base::_iterator;
    using typename _base::_const_iterator;
    using typename _base::_reverse_iterator;
    using typename _base::_const_reverse_iterator;
    using typename _base::_node_type;

    using _base::_root_node;
    using _base::_values_count;
    using _base::_tree_height;

    typedef typename __alloc_wrapper<_allocator>::template rebind<uint8_t>::other _node_allocator;
    typedef __alloc_wrapper<_node_allocator> _alloc_wrapper;

    _compare _comp;
    _alloc_wrapper _awrapper;

    static const uint32_t __node_bysize_max = sizeof(_node_type) + _bucket_bysize_max;

    static const uint32_t __bucket_values_capacity_max = _bucket_bysize_max / sizeof(_value_type);
    static const uint32_t __bucket_nodes_capacity_max  = _bucket_bysize_max / sizeof(_node_type*);

    static uint64_t max_capacity_in_theory()
    {
        uint64_t max_capacity = __bucket_values_capacity_max;

        for (uint32_t i = 0; i < _tree_height_max; ++i)
        {
            uint64_t temp = max_capacity * __bucket_nodes_capacity_max;
            if (temp < max_capacity) // overflow
            {
                return (uint64_t)-1;
            }

            max_capacity = temp;
        }

        return max_capacity;
    }

    static uint64_t max_capacity_in_practice()
    {
        if (_tree_height_max > 0)
        {
            uint64_t max_capacity = (__bucket_values_capacity_max / 2) * __bucket_nodes_capacity_max;

            for (uint32_t i = 0; i < _tree_height_max - 1; ++i)
            {
                uint64_t temp = max_capacity * (__bucket_nodes_capacity_max / 2);
                if (temp < max_capacity) // overflow
                {
                    return (uint64_t)-1;
                }

                max_capacity = temp;
            }

            return max_capacity;
        }
        else
        {
            return __bucket_values_capacity_max;
        }
    }

    static uint64_t max_capacity_in_conservative()
    {
        if (_tree_height_max > 0)
        {
            uint64_t max_capacity = (__bucket_values_capacity_max / 4) * __bucket_nodes_capacity_max;

            for (uint32_t i = 0; i < _tree_height_max - 1; ++i)
            {
                uint64_t temp = max_capacity * (__bucket_nodes_capacity_max / 4);
                if (temp < max_capacity) // overflow
                {
                    return (uint64_t)-1;
                }

                max_capacity = temp;
            }

            return max_capacity;
        }
        else
        {
            return __bucket_values_capacity_max;
        }
    }

    _bplus_tree() {}

    _bplus_tree(const _compare& comp, const _allocator& alloc)
    : _comp(comp), _awrapper(_node_allocator(alloc)) {}

    _bplus_tree(const _bplus_tree& tree)
    : _comp(tree._comp), _awrapper(tree._awrapper.select_on_container_copy_construction())
    {
        if (tree._values_count > 0)
        {
            clone_root_node(tree._root_node, tree._tree_height);
            _values_count = tree._values_count;
            _tree_height = tree._tree_height;
        }
    }

    _bplus_tree(const _bplus_tree& tree, const _allocator& alloc)
    : _comp(tree._comp), _awrapper(_node_allocator(alloc))
    {
        if (tree._values_count > 0)
        {
            clone_root_node(tree._root_node, tree._tree_height);
            _values_count = tree._values_count;
            _tree_height = tree._tree_height;
        }
    }

    _bplus_tree(_bplus_tree&& tree)
    : _comp(tree._comp), _awrapper(tree._awrapper)
    { move_data(tree, std::true_type()); }

    _bplus_tree(_bplus_tree&& tree, const _allocator& alloc)
    : _comp(tree._comp), _awrapper(_node_allocator(alloc))
    {
        using alloc_eq = std::integral_constant<bool, _alloc_wrapper::allocator_always_compares_equal()>;
        if (tree._values_count > 0)
        {
            move_data(tree, alloc_eq());
        }
    }

    ~_bplus_tree() noexcept
    {
        if (_root_node != nullptr)
        {
            clear_node(_root_node, _tree_height);
            deallocate_root_node();
        }
    }

    _node_type* allocate_node()
    {
        return (_node_type*)_awrapper.allocate(__node_bysize_max);
    }

    _node_type* allocate_root_node(uint32_t bucket_bysize)
    {
        _node_type* root_node = (_node_type*)_awrapper.allocate(sizeof(_node_type) + bucket_bysize);
        root_node->_bucket_bysize = bucket_bysize;
        return root_node;
    }

    void deallocate_node(_node_type* node)
    {
        _awrapper.deallocate((uint8_t*)node, __node_bysize_max);
    }

    void deallocate_root_node()
    {
        _awrapper.deallocate((uint8_t*)_root_node, sizeof(_node_type) + _root_node->_bucket_bysize);
    }

    size_t max_size() const noexcept
    {
        uint64_t alloc_value_max_size = _awrapper.max_size() * (__bucket_values_capacity_max / 2);
        return (size_t)std::min(alloc_value_max_size, max_capacity_in_practice());
    }

    void clear_node(_node_type* node, uint32_t depth)
    {
        if (depth > 0)
        {
            uint32_t child_depth = depth - 1;
            for (uint32_t i = 0; i < node->_count; ++i)
            {
                _node_type* child_node = node->nodes()[i];
                clear_node(child_node, child_depth);
                deallocate_node(child_node);
            }
        }
        else
        {
            _awrapper.destroy(node->values(), node->values_end());
            _values_count -= node->_count;
        }
    }

    void clear() noexcept
    {
        if (_values_count > 0)
        {
            clear_node(_root_node, _tree_height);
            deallocate_root_node();
            _root_node = nullptr;

            _values_count = 0;
            _tree_height = 0;
        }
    }

    void clone_node(_node_type** dst_node_ptr, const _node_type* src_node, uint32_t depth) noexcept
    {
        _node_type* dst_node = allocate_node();

        if (depth > 0)
        {
            uint32_t child_depth = depth - 1;
            for (uint32_t i = 0; i < src_node->_count; ++i)
            {
                clone_node(dst_node->nodes() + i, src_node->nodes()[i], child_depth);
            }

            dst_node->_ref_value = (*dst_node->nodes())->_ref_value;
        }
        else
        {
            for (uint32_t i = 0; i < src_node->_count; ++i)
            {
                _awrapper.construct(dst_node->values() + i, src_node->values()[i]);
            }

            dst_node->_ref_value = dst_node->values();
        }

        dst_node->_count = src_node->_count;
        *dst_node_ptr = dst_node;
    }

    void clone_root_node(const _node_type* root_node, uint32_t tree_height) noexcept
    {
        _root_node = allocate_root_node(root_node->_bucket_bysize);

        if (tree_height > 0)
        {
            uint32_t child_depth = tree_height - 1;
            for (uint32_t i = 0; i < root_node->_count; ++i)
            {
                clone_node(_root_node->nodes() + i, root_node->nodes()[i], child_depth);
            }
        }
        else
        {
            for (uint32_t i = 0; i < root_node->_count; ++i)
            {
                _awrapper.construct(_root_node->values() + i, root_node->values()[i]);
            }
        }

        _root_node->_count = root_node->_count;
    }

    void move_data(_bplus_tree& tree, std::true_type)
    {
        _root_node = tree._root_node;
        _values_count = tree._values_count;
        _tree_height = tree._tree_height;

        tree._root_node = nullptr;
        tree._values_count = 0;
        tree._tree_height = 0;
    }

    void move_data(_bplus_tree& tree, std::false_type)
    {
        if (_awrapper._alloc == tree._awrapper._alloc)
        {
            move_data(tree, std::true_type());
        }
        else if (tree._values_count > 0)
        {
            clone_root_node(tree._root_node, tree._tree_height);
            _values_count = tree._values_count;
            _tree_height = tree._tree_height;
        }
    }

    _bplus_tree& operator = (const _bplus_tree& tree)
    {
        if (this != &tree)
        {
            clear();
            _comp = tree._comp;

            if (_alloc_wrapper::propagate_on_container_copy_assignment())
            {
                if (!_alloc_wrapper::allocator_always_compares_equal() &&
                    _awrapper._alloc != tree._awrapper._alloc)
                {
                    _awrapper.copy_allocator(tree._awrapper._alloc);
                }
            }

            if (tree._values_count > 0)
            {
                clone_root_node(tree._root_node, tree._tree_height);
                _values_count = tree._values_count;
                _tree_height = tree._tree_height;
            }
        }

        return *this;
    }

    void move_assign(_bplus_tree& tree)
    {
        clear();
        _comp = tree._comp;

        if (_alloc_wrapper::propagate_on_container_move_assignment() ||
            _alloc_wrapper::allocator_always_compares_equal() ||
            _awrapper._alloc == tree._awrapper._alloc)
        {
            move_data(tree, std::true_type());
            _awrapper.move_allocator(tree._awrapper._alloc);
        }
        else
        {
            insert_range(std::__make_move_if_noexcept_iterator(tree.begin()),
                         std::__make_move_if_noexcept_iterator(tree.end()));
            tree.clear();
        }
    }

    void swap(_bplus_tree& tree) noexcept(_alloc_wrapper::is_nothrow_swap())
    {
        if (_root_node == nullptr)
        {
            if (tree._root_node != nullptr)
            {
                _root_node = tree._root_node;
                _values_count = tree._values_count;
                _tree_height = tree._tree_height;

                tree._root_node = nullptr;
                tree._values_count = 0;
                tree._tree_height = 0;
            }
        }
        else if (tree._root_node == nullptr)
        {
            tree._root_node = _root_node;
            tree._values_count = _values_count;
            tree._tree_height = _tree_height;

            _root_node = nullptr;
            _values_count = 0;
            _tree_height = 0;
        }
        else
        {
            std::swap(_root_node, tree._root_node);
            std::swap(_values_count, tree._values_count);
            std::swap(_tree_height, tree._tree_height);
        }

        std::swap(_comp, tree._comp);
        _awrapper.swap_allocator(tree._awrapper._alloc);
    }

    bool value_compare(const _value_type& x, const _value_type& y) const
    { return _comp(_key_of_value()(x), _key_of_value()(y)); }

    bool key_less(const _key_type& key, const _value_type& x) const
    { return _comp(key, _key_of_value()(x)); }

    bool key_larger(const _key_type& key, const _value_type& x) const
    { return _comp(_key_of_value()(x), key); }

    _value_type* lower_bound_core(const _key_type& key,
                                  _node_type*** stack,
                                  _node_type*& cur_node) const
    {
        cur_node = _root_node;

        for (uint32_t depth = _tree_height - 1; depth != (uint32_t)-1; --depth)
        {
            uint32_t check_nodes_count = cur_node->_count;
            _node_type** node_ptr = cur_node->nodes();

            do
            {
                uint32_t step = check_nodes_count >> 1;

                bool key_not_less_than = !key_less(key, *(node_ptr[step]->_ref_value));

                check_nodes_count = (check_nodes_count + key_not_less_than) >> 1;
                node_ptr += step * key_not_less_than;
            }
            while (check_nodes_count > 1);

            stack[depth] = node_ptr;
            cur_node = *node_ptr;
        }

        uint32_t check_values_count = cur_node->_count;
        _value_type* value_ptr = cur_node->values();

        do
        {
            uint32_t step = check_values_count >> 1;

            bool key_larger_than = key_larger(key, value_ptr[step]);

            check_values_count = (check_values_count - key_larger_than) >> 1;
            value_ptr += (step + 1) * key_larger_than;
        }
        while (check_values_count > 0);

        return value_ptr;
    }

    template<typename _output_iterator>
    _output_iterator find(const _key_type& key) const
    {
        _output_iterator it(const_cast<_bplus_tree*>(this));

        if (_values_count > 0)
        {
            _node_type* cur_node;
            it._value_ptr = lower_bound_core(key, it._stack, cur_node);

            if (it._value_ptr != cur_node->values_end() && !key_less(key, *it))
            {
                return it;
            }
        }

        it._value_ptr = nullptr;
        return it;
    }

    template<typename _output_iterator>
    _output_iterator lower_bound(const _key_type& key) const
    {
        _output_iterator it(const_cast<_bplus_tree*>(this));

        if (_values_count > 0)
        {
            _node_type* cur_node;
            it._value_ptr = lower_bound_core(key, it._stack, cur_node);

            if (it._value_ptr == cur_node->values_end())
            {
                it.cross_node_increment();
            }
        }
        else
        {
            it._value_ptr = nullptr;
        }

        return it;
    }

    template<typename _output_iterator>
    _output_iterator upper_bound(const _key_type& key) const
    {
        _output_iterator it(const_cast<_bplus_tree*>(this));

        if (_values_count > 0)
        {
            _node_type* cur_node;
            it._value_ptr = lower_bound_core(key, it._stack, cur_node);

            if (it._value_ptr == cur_node->values_end())
            {
                it.cross_node_increment();
            }
            else if (!key_less(key, *it))
            {
                it.increment();
            }
        }
        else
        {
            it._value_ptr = nullptr;
        }

        return it;
    }

    template<typename _output_iterator>
    std::pair<_output_iterator, _output_iterator> equal_range(const _key_type& key) const
    {
        _output_iterator it1 = lower_bound<_output_iterator>(key);
        _output_iterator it2(it1);

        if (it2._value_ptr != nullptr && !key_less(key, *it2))
        {
            it2.increment();
        }

        return std::pair<_output_iterator, _output_iterator>(it1, it2);
    }

    template<typename _output_iterator, typename... _args>
    void insert_core(_output_iterator& it, _args&&... args)
    {
        ++_values_count;

        _node_type* cur_node;

        if (_tree_height == 0)
        {
            if (_root_node->_bucket_bysize < (_root_node->_count + 1) * sizeof(_value_type) &&
                _root_node->_bucket_bysize < _bucket_bysize_max)
            {
                uint32_t root_bucket_bysize;
                if (_root_node->_bucket_bysize << 2 > _bucket_bysize_max)
                {
                    root_bucket_bysize = _bucket_bysize_max;
                }
                else if (_root_node->_bucket_bysize << 1 > _bucket_bysize_max)
                {
                    root_bucket_bysize = _bucket_bysize_max;
                }
                else
                {
                    root_bucket_bysize = _root_node->_bucket_bysize << 1;
                }

                _node_type* new_root_node = allocate_root_node(root_bucket_bysize);
                new_root_node->_count = _root_node->_count;

                for (uint32_t i = 0; i < _root_node->_count; ++i)
                {
                    _awrapper.construct(new_root_node->values() + i, std::move(_root_node->values()[i]));
                    _awrapper.destroy(_root_node->values() + i);
                }

                it._value_ptr = new_root_node->values() + (it._value_ptr - _root_node->values());

                deallocate_root_node();
                _root_node = new_root_node;
            }

            cur_node = _root_node;
        }
        else
        {
            cur_node = *it._stack[0];
        }

        if (cur_node->_count < __bucket_values_capacity_max)
        {
            if (it._value_ptr == cur_node->values_end())
            {
                _awrapper.construct(it._value_ptr, std::forward<_args>(args)...);
            }
            else
            {
                _awrapper.construct(cur_node->values_end(), std::move(cur_node->values()[cur_node->_count - 1]));

                std::move_backward(it._value_ptr,
                                   cur_node->values_end() - 1,
                                   (_moveable_value_type*)cur_node->values_end());

                *(_moveable_value_type*)it._value_ptr = _moveable_value_type(std::forward<_args>(args)...);
            }

            ++cur_node->_count;

            return;
        }

        _node_type *new_node = allocate_node();
        new_node->_ref_value = new_node->values();

        bool x_in_new_node;
        uint32_t insert_pos = (uint32_t)(it._value_ptr - cur_node->values());

        if (insert_pos < __bucket_values_capacity_max - __bucket_values_capacity_max / 2)
        {
            _awrapper.construct(new_node->values(), std::move(cur_node->values()[__bucket_values_capacity_max / 2]));

            for (uint32_t i = 1; i < __bucket_values_capacity_max - __bucket_values_capacity_max / 2; ++i)
            {
                _awrapper.construct(new_node->values() + i, std::move(cur_node->values()[__bucket_values_capacity_max / 2 + i]));
                _awrapper.destroy(cur_node->values() + __bucket_values_capacity_max / 2 + i);
            }

            std::move_backward(it._value_ptr,
                               cur_node->values() + __bucket_values_capacity_max / 2,
                               (_moveable_value_type*)cur_node->values() + __bucket_values_capacity_max / 2 + 1);

            new_node->_count = __bucket_values_capacity_max - __bucket_values_capacity_max / 2;
            cur_node->_count = __bucket_values_capacity_max / 2 + 1;

            x_in_new_node = false;
        }
        else
        {
            const uint32_t move_pos = __bucket_values_capacity_max - __bucket_values_capacity_max / 2;

            for (uint32_t i = move_pos; i < insert_pos; ++i)
            {
                _awrapper.construct(new_node->values() + i - move_pos, std::move(cur_node->values()[i]));
                _awrapper.destroy(cur_node->values() + i);
            }
            for (uint32_t i = insert_pos; i < __bucket_values_capacity_max; ++i)
            {
                _awrapper.construct(new_node->values() + i - move_pos + 1, std::move(cur_node->values()[i]));
                _awrapper.destroy(cur_node->values() + i);
            }

            new_node->_count = __bucket_values_capacity_max / 2 + 1;
            cur_node->_count = __bucket_values_capacity_max - __bucket_values_capacity_max / 2;

            it._value_ptr = new_node->values() + insert_pos - move_pos;
            x_in_new_node = true;
        }

        _awrapper.construct(it._value_ptr, std::forward<_args>(args)...);

        for (uint32_t depth = 0; depth < _tree_height; ++depth)
        {
            _node_type* parent_node = (depth + 1 < _tree_height)? *(it._stack[depth + 1]) :
                                                                  _root_node;
            insert_pos = (uint32_t)(it._stack[depth] - parent_node->nodes() + 1);

            if (parent_node->_count < __bucket_nodes_capacity_max)
            {
                std::memmove(parent_node->nodes() + insert_pos + 1,
                             parent_node->nodes() + insert_pos,
                             (parent_node->_count - insert_pos) * sizeof(_node_type*));

                parent_node->nodes()[insert_pos] = new_node;

                ++parent_node->_count;
                it._stack[depth] += x_in_new_node;

                return;
            }

            _node_type *new_parent_node = allocate_node();

            if (insert_pos < __bucket_nodes_capacity_max - __bucket_nodes_capacity_max / 2)
            {
                std::memcpy(new_parent_node->nodes(),
                            parent_node->nodes() + __bucket_nodes_capacity_max / 2,
                            (__bucket_nodes_capacity_max - __bucket_nodes_capacity_max / 2) * sizeof(_node_type*));

                std::memmove(parent_node->nodes() + insert_pos + 1,
                             parent_node->nodes() + insert_pos,
                             (__bucket_nodes_capacity_max / 2 - insert_pos) * sizeof(_node_type*));

                parent_node->nodes()[insert_pos] = new_node;

                new_parent_node->_count = __bucket_nodes_capacity_max - __bucket_nodes_capacity_max / 2;
                parent_node->_count = __bucket_nodes_capacity_max / 2 + 1;

                it._stack[depth] += x_in_new_node;
                x_in_new_node = false;
            }
            else
            {
                uint32_t new_insert_pos = insert_pos - (__bucket_nodes_capacity_max - __bucket_nodes_capacity_max / 2);

                std::memcpy(new_parent_node->nodes(),
                            parent_node->nodes() + (__bucket_nodes_capacity_max - __bucket_nodes_capacity_max / 2),
                            new_insert_pos * sizeof(_node_type*));

                std::memcpy(new_parent_node->nodes() + (new_insert_pos + 1),
                            parent_node->nodes() + insert_pos,
                            (__bucket_nodes_capacity_max / 2 - new_insert_pos) * sizeof(_node_type*));

                new_parent_node->nodes()[new_insert_pos] = new_node;

                new_parent_node->_count = __bucket_nodes_capacity_max / 2 + 1;
                parent_node->_count = __bucket_nodes_capacity_max - __bucket_nodes_capacity_max / 2;

                if (new_insert_pos > 0 || x_in_new_node)
                {
                    it._stack[depth] = new_parent_node->nodes() + (new_insert_pos - !x_in_new_node);
                    x_in_new_node = true;
                }
            }

            new_parent_node->_ref_value = (*new_parent_node->nodes())->_ref_value;

            new_node = new_parent_node;
            cur_node = parent_node;
        }

        _root_node->_ref_value = (_tree_height == 0)? _root_node->values() : (*_root_node->nodes())->_ref_value;

        _root_node = allocate_root_node(_bucket_bysize_max);
        _root_node->_count = 2;
        _root_node->nodes()[0] = cur_node;
        _root_node->nodes()[1] = new_node;

        it._stack[_tree_height] = _root_node->nodes() + x_in_new_node;
        ++_tree_height;
    }

    template<typename _output_iterator, typename... _args>
    void insert_first_value(_output_iterator& it, _args&&... args)
    {
        if (_root_node == nullptr)
        {
            _root_node = allocate_root_node(2 * sizeof(_value_type));
        }

        it._value_ptr = _root_node->values();
        _awrapper.construct(it._value_ptr, std::forward<_args>(args)...);

        _root_node->_count = 1;
        _values_count = 1;
    }

    template<typename _output_iterator, typename _arg>
    std::pair<_output_iterator, bool> insert(_arg&& x)
    {
        _output_iterator it(this);

        if (_values_count == 0)
        {
            insert_first_value(it, std::forward<_arg>(x));
            return std::pair<_output_iterator, bool>(it, true);
        }

        _node_type* cur_node;
        it._value_ptr = lower_bound_core(_key_of_value()(x), it._stack, cur_node);

        if (it._value_ptr == cur_node->values_end() || value_compare(x, *it))
        {
            insert_core(it, std::forward<_arg>(x));
            return std::pair<_output_iterator, bool>(it, true);
        }
        else
        {
            return std::pair<_output_iterator, bool>(it, false);
        }
    }

    template<typename _output_iterator, typename _arg>
    _output_iterator insert(const _const_iterator& position, _arg&& x)
    {
        _output_iterator it(position._const_cast());

        if (_values_count == 0)
        {
            insert_first_value(it, std::forward<_arg>(x));
            return it;
        }

        if (it._value_ptr == nullptr)
        {
            it.set_last();

            if (value_compare(*it, x))
            {
                ++it._value_ptr;
                insert_core(it, std::forward<_arg>(x));
                return it;
            }
        }
        else
        {
            _output_iterator before(it);
            before.decrement();

            if (before._value_ptr == nullptr || value_compare(*before, x))
            {
                if (value_compare(x, *it))
                {
                    insert_core(it, std::forward<_arg>(x));
                    return it;
                }
            }
        }

        _node_type* cur_node;
        it._value_ptr = lower_bound_core(_key_of_value()(x), it._stack, cur_node);

        if (it._value_ptr == cur_node->values_end() || value_compare(x, *it))
        {
            insert_core(it, std::forward<_arg>(x));
        }

        return it;
    }

    template<typename _input_iterator>
    void insert_range(_input_iterator&& first, _input_iterator&& last)
    {
        _const_iterator it_end(this, nullptr);

        for (_input_iterator pos = first; pos != last; ++pos)
        {
            insert<_iterator>(it_end, *pos);
        }
    }

    void update_ref_value(_node_type*** stack, uint32_t depth, _value_type* ref_value)
    {
        for (; depth + 1 < _tree_height; ++depth)
        {
            _node_type* parent_node = *stack[depth + 1];

            if (stack[depth] != parent_node->nodes())
            {
                return;
            }

            parent_node->_ref_value = ref_value;
        }
    }

    template<typename _output_iterator, typename _input_iterator>
    _output_iterator erase_core(const _input_iterator& it)
    {
        _output_iterator out(it._const_cast());

        _node_type* cur_node = (_tree_height > 0)? *it._stack[0] : _root_node;
        uint32_t erase_pos = (uint32_t)(it._value_ptr - cur_node->values());
        bool value_at_end = erase_pos + 1 >= cur_node->_count;

        std::move(it._value_ptr + 1,
                  cur_node->values_end(),
                  (_moveable_value_type*)it._value_ptr);

        _awrapper.destroy(cur_node->values_end() - 1);

        --_values_count;
        --cur_node->_count;

        if (_tree_height == 0)
        {
            out._value_ptr = (_value_type*)((uintptr_t)it._value_ptr * !value_at_end);

            if (_root_node->_count * sizeof(_value_type) < _root_node->_bucket_bysize >> 1)
            {
                _node_type* new_root_node = allocate_root_node(_root_node->_bucket_bysize >> 1);
                new_root_node->_count = _root_node->_count;

                for (uint32_t i = 0; i < _root_node->_count; ++i)
                {
                    _awrapper.construct(new_root_node->values() + i, std::move(_root_node->values()[i]));
                    _awrapper.destroy(_root_node->values() + i);
                }

                if (out._value_ptr != nullptr)
                {
                    out._value_ptr = new_root_node->values() + (out._value_ptr - _root_node->values());
                }

                deallocate_root_node();
                _root_node = new_root_node;
            }

            return out;
        }

        _node_type* parent_node = (_tree_height > 1)? *it._stack[1] : _root_node;
        uint32_t cur_node_pos = (uint32_t)(it._stack[0] - parent_node->nodes());
        bool node_at_end = cur_node_pos + 1 >= parent_node->_count;

        if (cur_node->_count == 0)
        {
            out.cross_node_increment();

            if (!node_at_end)
            {
                --out._stack[0];

                if (cur_node_pos == 0 && parent_node != _root_node)
                {
                    parent_node->_ref_value = parent_node->nodes()[1]->_ref_value;
                    update_ref_value(const_cast<_node_type***>(it._stack), 1, parent_node->_ref_value);
                }

                std::memmove(it._stack[0],
                             it._stack[0] + 1,
                             (parent_node->_count - cur_node_pos - 1) * sizeof(_node_type*));
            }

            deallocate_node(cur_node);
        }
        else if (cur_node_pos > 0 &&
                 cur_node->_count + parent_node->nodes()[cur_node_pos - 1]->_count <= __bucket_values_capacity_max / 2)
        {
            _node_type* prev_node = parent_node->nodes()[cur_node_pos - 1];

            for (uint32_t i = 0; i < cur_node->_count; ++i)
            {
                _awrapper.construct(prev_node->values_end() + i, std::move(cur_node->values()[i]));
                _awrapper.destroy(cur_node->values() + i);
            }

            if (!value_at_end)
            {
                out._value_ptr = prev_node->values_end() + erase_pos;
                --out._stack[0];
            }
            else
            {
                out.cross_node_increment();
                out._stack[0] -= !node_at_end;
            }

            prev_node->_count += cur_node->_count;

            std::memmove(it._stack[0],
                         it._stack[0] + 1,
                         (parent_node->_count - cur_node_pos - 1) * sizeof(_node_type*));

            deallocate_node(cur_node);
        }
        else if (!node_at_end &&
                 cur_node->_count + parent_node->nodes()[cur_node_pos + 1]->_count <= __bucket_values_capacity_max / 2)
        {
            _node_type* next_node = parent_node->nodes()[cur_node_pos + 1];

            for (uint32_t i = 0; i < next_node->_count; ++i)
            {
                _awrapper.construct(cur_node->values_end() + i, std::move(next_node->values()[i]));
                _awrapper.destroy(next_node->values() + i);
            }

            cur_node->_count += next_node->_count;

            std::memmove(it._stack[0] + 1,
                         it._stack[0] + 2,
                         (parent_node->_count - cur_node_pos - 2) * sizeof(_node_type*));

            deallocate_node(next_node);
        }
        else
        {
            if (value_at_end)
            {
                out.cross_node_increment();
            }
            return out;
        }

        --parent_node->_count;

        for (uint32_t depth = 1; depth < _tree_height; ++depth)
        {
            cur_node = parent_node;

            parent_node = (depth + 1 < _tree_height)? *(it._stack[depth + 1]) : _root_node;
            cur_node_pos = (uint32_t)(it._stack[depth] - parent_node->nodes());

            node_at_end = cur_node_pos + 1 >= parent_node->_count;

            if (cur_node->_count == 0)
            {
                if (!node_at_end)
                {
                    --out._stack[depth];

                    if (cur_node_pos == 0 && parent_node != _root_node)
                    {
                        parent_node->_ref_value = parent_node->nodes()[1]->_ref_value;
                        update_ref_value(const_cast<_node_type***>(it._stack), depth + 1, parent_node->_ref_value);
                    }

                    std::memmove(it._stack[depth],
                                 it._stack[depth] + 1,
                                 (parent_node->_count - cur_node_pos - 1) * sizeof(_node_type*));
                }

                deallocate_node(cur_node);
            }
            else if (cur_node_pos > 0 &&
                     cur_node->_count + parent_node->nodes()[cur_node_pos - 1]->_count <= __bucket_nodes_capacity_max / 2)
            {
                _node_type* prev_node = parent_node->nodes()[cur_node_pos - 1];

                std::memcpy(prev_node->nodes_end(),
                            cur_node->nodes(),
                            cur_node->_count * sizeof(_node_type*));

                if (out._stack[depth] == it._stack[depth])
                {
                    out._stack[depth - 1] = prev_node->nodes_end() + (out._stack[depth - 1] - cur_node->nodes());
                    --out._stack[depth];
                }
                else
                {
                    out._stack[depth] -= !node_at_end;
                }

                prev_node->_count += cur_node->_count;

                std::memmove(it._stack[depth],
                             it._stack[depth] + 1,
                             (parent_node->_count - cur_node_pos - 1) * sizeof(_node_type*));

                deallocate_node(cur_node);
            }
            else if (!node_at_end &&
                     cur_node->_count + parent_node->nodes()[cur_node_pos + 1]->_count <= __bucket_nodes_capacity_max / 2)
            {
                _node_type* next_node = parent_node->nodes()[cur_node_pos + 1];

                std::memcpy(cur_node->nodes_end(),
                            next_node->nodes(),
                            next_node->_count * sizeof(_node_type*));

                if (out._stack[depth] == it._stack[depth] + 1)
                {
                    out._stack[depth - 1] = cur_node->nodes_end();
                    --out._stack[depth];
                }

                cur_node->_count += next_node->_count;

                std::memmove(it._stack[depth] + 1,
                             it._stack[depth] + 2,
                             (parent_node->_count - cur_node_pos - 2) * sizeof(_node_type*));

                deallocate_node(next_node);
            }
            else
            {
                return out;
            }

            --parent_node->_count;
        }

        if (_root_node->_count == 1)
        {
            _node_type* new_root_node = *_root_node->nodes();
            deallocate_root_node();
            _root_node = new_root_node;
            _root_node->_bucket_bysize = _bucket_bysize_max;

            --_tree_height;
        }

        return out;
    }

    template<typename _output_iterator, typename _input_iterator>
    _output_iterator erase(const _input_iterator& position)
    {
        if (position._value_ptr != nullptr)
        {
            return erase_core<_output_iterator>(position);
        }
        else
        {
            return _output_iterator(this, nullptr);
        }
    }

    size_t erase(const _key_type& key)
    {
        if (_values_count > 0)
        {
            _const_iterator it(this);
            _node_type* cur_node;
            it._value_ptr = lower_bound_core(key, it._stack, cur_node);

            if (it._value_ptr != cur_node->values_end() && !key_less(key, *it))
            {
                erase_core<_const_iterator>(it);
                return 1;
            }
        }

        return 0;
    }

    void erase_range_core(const _const_iterator& first)
    {
        _node_type* cur_node = (_tree_height > 0)? *first._stack[0] : _root_node;
        uint32_t erase_count = (uint32_t)(cur_node->values_end() - first._value_ptr);

        _awrapper.destroy(first._value_ptr, cur_node->values_end());

        cur_node->_count -= erase_count;
        _values_count -= erase_count;

        if (_tree_height == 0)
        {
            return;
        }

        _node_type* parent_node = (_tree_height > 1)? *first._stack[1] : _root_node;
        uint32_t cur_node_pos = (uint32_t)(first._stack[0] - parent_node->nodes());
        bool cur_node_is_empty = cur_node->_count == 0;

        if (cur_node_is_empty)
        {
            deallocate_node(cur_node);
        }
        else if (cur_node_pos > 0 &&
                 cur_node->_count + parent_node->nodes()[cur_node_pos - 1]->_count <= __bucket_values_capacity_max / 2)
        {
            _node_type* prev_node = parent_node->nodes()[cur_node_pos - 1];

            for (uint32_t i = 0; i < cur_node->_count; ++i)
            {
                _awrapper.construct(prev_node->values_end() + i, std::move(cur_node->values()[i]));
                _awrapper.destroy(cur_node->values() + i);
            }

            prev_node->_count += cur_node->_count;
            deallocate_node(cur_node);
            cur_node_is_empty = true;
        }

        _node_type** erase_nodes_end = parent_node->nodes_end();
        for (_node_type** node_ptr = first._stack[0] + 1; node_ptr < erase_nodes_end; ++node_ptr)
        {
            clear_node(*node_ptr, 0);
            deallocate_node(*node_ptr);
        }

        parent_node->_count = cur_node_pos + !cur_node_is_empty;

        for (uint32_t depth = 1; depth < _tree_height; ++depth)
        {
            cur_node = parent_node;

            parent_node = (depth + 1 < _tree_height)? *first._stack[depth + 1] : _root_node;
            cur_node_pos = (uint32_t)(first._stack[depth] - parent_node->nodes());
            cur_node_is_empty = cur_node->_count == 0;

            if (cur_node_is_empty)
            {
                deallocate_node(cur_node);
            }
            else if (cur_node_pos > 0 &&
                     cur_node->_count + parent_node->nodes()[cur_node_pos - 1]->_count <= __bucket_nodes_capacity_max / 2)
            {
                _node_type* prev_node = parent_node->nodes()[cur_node_pos - 1];

                std::memcpy(prev_node->nodes_end(),
                            cur_node->nodes(),
                            cur_node->_count * sizeof(_node_type*));

                prev_node->_count += cur_node->_count;
                deallocate_node(cur_node);
                cur_node_is_empty = true;
            }

            erase_nodes_end = parent_node->nodes_end();
            for (_node_type** node_ptr = first._stack[depth] + 1; node_ptr < erase_nodes_end; ++node_ptr)
            {
                clear_node(*node_ptr, depth);
                deallocate_node(*node_ptr);
            }

            parent_node->_count = cur_node_pos + !cur_node_is_empty;
        }
    }

    template<typename _output_iterator>
    _output_iterator erase_range_core(const _const_iterator& first,
                                      const _const_iterator& last)
    {
        _output_iterator out(last._const_cast());

        bool same_parent;
        _node_type* first_cur_node;
        _node_type* last_cur_node;

        if (_tree_height > 0)
        {
            first_cur_node = *first._stack[0];
            last_cur_node = *last._stack[0];
            same_parent = first_cur_node == last_cur_node;
        }
        else
        {
            same_parent = true;
            first_cur_node = last_cur_node = _root_node;
        }

        uint32_t first_erase_count, last_erase_count;

        if (same_parent)
        {
            first_erase_count = (uint32_t)(last._value_ptr - first._value_ptr);

            std::move(last._value_ptr, first_cur_node->values_end(), (_moveable_value_type*)first._value_ptr);
            _awrapper.destroy(first_cur_node->values_end() - first_erase_count, first_cur_node->values_end());

            first_cur_node->_count -= first_erase_count;
            _values_count -= first_erase_count;

            out._value_ptr = first._value_ptr;
            if (_tree_height == 0)
            {
                return out;
            }
        }
        else
        {
            first_erase_count = (uint32_t)(first_cur_node->values_end() - first._value_ptr);

            _awrapper.destroy(first._value_ptr, first_cur_node->values_end());

            first_cur_node->_count -= first_erase_count;
            _values_count -= first_erase_count;

            last_erase_count = (uint32_t)(last._value_ptr - last_cur_node->values());

            std::move(last._value_ptr, last_cur_node->values_end(), (_moveable_value_type*)last_cur_node->values());
            _awrapper.destroy(last_cur_node->values_end() - last_erase_count, last_cur_node->values_end());

            last_cur_node->_count -= last_erase_count;
            _values_count -= last_erase_count;

            out._value_ptr = last_cur_node->values();
        }

        _node_type* first_parent_node;
        _node_type* last_parent_node;
        bool parent_same_parent;

        if (_tree_height > 1)
        {
            first_parent_node = *first._stack[1];
            last_parent_node = *last._stack[1];
            parent_same_parent = first_parent_node == last_parent_node;
        }
        else
        {
            parent_same_parent = true;
            first_parent_node = last_parent_node = _root_node;
        }

        _node_type* new_first_cur_node = first_cur_node;
        uint32_t first_cur_node_pos = (uint32_t)(first._stack[0] - first_parent_node->nodes());
        bool first_cur_node_is_empty = first_cur_node->_count == 0;

        if (first_cur_node_is_empty)
        {
            deallocate_node(first_cur_node);
            new_first_cur_node = (first_cur_node_pos > 0)? first_parent_node->nodes()[first_cur_node_pos - 1] : nullptr;
        }
        else if (first_cur_node_pos > 0 &&
                 first_cur_node->_count + first_parent_node->nodes()[first_cur_node_pos - 1]->_count <= __bucket_values_capacity_max / 2)
        {
            new_first_cur_node = first_parent_node->nodes()[first_cur_node_pos - 1];

            for (uint32_t i = 0; i < first_cur_node->_count; ++i)
            {
                _awrapper.construct(new_first_cur_node->values_end() + i, std::move(first_cur_node->values()[i]));
                _awrapper.destroy(first_cur_node->values() + i);
            }

            if (same_parent)
            {
                out._value_ptr = new_first_cur_node->values_end() + (out._value_ptr - first_cur_node->values());
                --out._stack[0];
            }

            new_first_cur_node->_count += first_cur_node->_count;
            deallocate_node(first_cur_node);
            first_cur_node_is_empty = true;
        }

        uint32_t last_cur_node_pos = (uint32_t)(last._stack[0] - last_parent_node->nodes());
        bool last_next_node_is_empty = false;

        if (!same_parent &&
            last_cur_node_pos + 1 < last_parent_node->_count &&
            last_cur_node->_count + last_parent_node->nodes()[last_cur_node_pos + 1]->_count <= __bucket_values_capacity_max / 2)
        {
            _node_type* last_next_node = last_parent_node->nodes()[last_cur_node_pos + 1];

            for (uint32_t i = 0; i < last_next_node->_count; ++i)
            {
                _awrapper.construct(last_cur_node->values_end() + i, std::move(last_next_node->values()[i]));
                _awrapper.destroy(last_next_node->values() + i);
            }

            last_cur_node->_count += last_next_node->_count;
            deallocate_node(last_next_node);
            last_next_node_is_empty = true;
        }

        uint32_t old_first_parent_node_count = first_parent_node->_count;
        uint32_t old_last_parent_node_count = last_parent_node->_count;

        if (parent_same_parent)
        {
            if (same_parent)
            {
                bool first_next_node_is_empty = false;

                if (first_cur_node_pos + 1 < first_parent_node->_count &&
                    new_first_cur_node->_count + first_parent_node->nodes()[first_cur_node_pos + 1]->_count <= __bucket_values_capacity_max / 2)
                {
                    _node_type* first_next_node = first_parent_node->nodes()[first_cur_node_pos + 1];

                    for (uint32_t i = 0; i < first_next_node->_count; ++i)
                    {
                        _awrapper.construct(new_first_cur_node->values_end() + i, std::move(first_next_node->values()[i]));
                        _awrapper.destroy(first_next_node->values() + i);
                    }

                    new_first_cur_node->_count += first_next_node->_count;
                    deallocate_node(first_next_node);
                    first_next_node_is_empty = true;
                }

                if (first_cur_node_is_empty || first_next_node_is_empty)
                {
                    std::memmove(first_parent_node->nodes() + (first_cur_node_pos + !first_cur_node_is_empty),
                                 first_parent_node->nodes() + (first_cur_node_pos + 1 + first_next_node_is_empty),
                                 (first_parent_node->_count - first_cur_node_pos - 1 - first_next_node_is_empty) * sizeof(_node_type*));

                    first_parent_node->_count -= first_cur_node_is_empty + first_next_node_is_empty;
                }
            }
            else // (!same_parent)
            {
                _node_type** erase_nodes_end = last._stack[0];
                for (_node_type** node_ptr = first._stack[0] + 1; node_ptr < erase_nodes_end; ++node_ptr)
                {
                    clear_node(*node_ptr, 0);
                    deallocate_node(*node_ptr);
                }

                uint32_t count_1 = first_cur_node_pos + !first_cur_node_is_empty;
                uint32_t count_2 = first_parent_node->_count - last_cur_node_pos;
                bool last_cur_node_is_empty;

                if (new_first_cur_node != nullptr &&
                    new_first_cur_node->_count + last_cur_node->_count <= __bucket_values_capacity_max / 2)
                {
                    for (uint32_t i = 0; i < last_cur_node->_count; ++i)
                    {
                        _awrapper.construct(new_first_cur_node->values_end() + i, std::move(last_cur_node->values()[i]));
                        _awrapper.destroy(last_cur_node->values() + i);
                    }

                    out._value_ptr = new_first_cur_node->values_end();
                    out._stack[0] = first._stack[0] - first_cur_node_is_empty;

                    new_first_cur_node->_count += last_cur_node->_count;
                    deallocate_node(last_cur_node);
                    last_cur_node_is_empty = true;
                }
                else
                {
                    out._stack[0] = first_parent_node->nodes() + count_1;
                    last_cur_node_is_empty = false;
                }

                if (last_next_node_is_empty)
                {
                    if (last_cur_node_is_empty)
                    {
                        std::memmove(first_parent_node->nodes() + count_1,
                                     erase_nodes_end + 2,
                                     (count_2 - 2) * sizeof(_node_type*));
                    }
                    else
                    {
                        first_parent_node->nodes()[count_1] = last_cur_node;

                        std::memmove(first_parent_node->nodes() + (count_1 + 1),
                                     erase_nodes_end + 2,
                                     (count_2 - 2) * sizeof(_node_type*));
                    }
                }
                else
                {
                    std::memmove(first_parent_node->nodes() + count_1,
                                 erase_nodes_end + last_cur_node_is_empty,
                                 (count_2 - last_cur_node_is_empty) * sizeof(_node_type*));
                }

                first_parent_node->_count = count_1 + count_2 - last_cur_node_is_empty - last_next_node_is_empty;
            }
        }
        else // (!parent_same_parent)
        {
            _node_type** erase_nodes_end = first_parent_node->nodes_end();
            for (_node_type** node_ptr = first._stack[0] + 1; node_ptr < erase_nodes_end; ++node_ptr)
            {
                clear_node(*node_ptr, 0);
                deallocate_node(*node_ptr);
            }

            first_parent_node->_count = first_cur_node_pos + !first_cur_node_is_empty;

            erase_nodes_end = last._stack[0];
            for (_node_type** node_ptr = last_parent_node->nodes(); node_ptr < erase_nodes_end; ++node_ptr)
            {
                clear_node(*node_ptr, 0);
                deallocate_node(*node_ptr);
            }

            if (last_cur_node_pos + last_next_node_is_empty > 0)
            {
                std::memmove(last_parent_node->nodes() + 1,
                             last_parent_node->nodes() + (last_cur_node_pos + last_next_node_is_empty + 1),
                             (last_parent_node->_count - last_cur_node_pos - last_next_node_is_empty - 1) * sizeof(_node_type*));

                last_parent_node->_count -= last_cur_node_pos + last_next_node_is_empty;
            }

            if (last_cur_node_pos > 0)
            {
                *last_parent_node->nodes() = last_cur_node;
                out._stack[0] = last_parent_node->nodes();
            }
        }

        if (first_parent_node->_count == old_first_parent_node_count &&
            last_parent_node->_count == old_last_parent_node_count)
        {
            return out;
        }

        if (_tree_height > 1 &&
            last_parent_node->_ref_value != (*last_parent_node->nodes())->_ref_value)
        {
            last_parent_node->_ref_value = (*last_parent_node->nodes())->_ref_value;
            update_ref_value(const_cast<_node_type***>(last._stack), 1, last_parent_node->_ref_value);
        }

        for (uint32_t depth = 1; depth < _tree_height; ++depth)
        {
            same_parent = parent_same_parent;
            first_cur_node = first_parent_node;
            last_cur_node = last_parent_node;

            if (_tree_height > depth + 1)
            {
                first_parent_node = *first._stack[depth + 1];
                last_parent_node = *last._stack[depth + 1];
                parent_same_parent = first_parent_node == last_parent_node;
            }
            else
            {
                parent_same_parent = true;
                first_parent_node = last_parent_node = _root_node;
            }

            new_first_cur_node = first_cur_node;
            first_cur_node_pos = (uint32_t)(first._stack[depth] - first_parent_node->nodes());
            first_cur_node_is_empty = first_cur_node->_count == 0;

            if (first_cur_node_is_empty)
            {
                deallocate_node(first_cur_node);
                new_first_cur_node = (first_cur_node_pos > 0)? first_parent_node->nodes()[first_cur_node_pos - 1] : nullptr;
            }
            else if (first_cur_node_pos > 0 &&
                     first_cur_node->_count + first_parent_node->nodes()[first_cur_node_pos - 1]->_count <= __bucket_nodes_capacity_max / 2)
            {
                new_first_cur_node = first_parent_node->nodes()[first_cur_node_pos - 1];

                std::memcpy(new_first_cur_node->nodes_end(),
                            first_cur_node->nodes(),
                            first_cur_node->_count * sizeof(_node_type*));

                if (same_parent)
                {
                    out._stack[depth - 1] = new_first_cur_node->nodes_end() + (out._stack[depth - 1] - first_cur_node->nodes());
                    --out._stack[depth];
                }

                new_first_cur_node->_count += first_cur_node->_count;
                deallocate_node(first_cur_node);
                first_cur_node_is_empty = true;
            }

            last_cur_node_pos = (uint32_t)(last._stack[depth] - last_parent_node->nodes());
            last_next_node_is_empty = false;

            if (!same_parent &&
                last_cur_node_pos + 1 < last_parent_node->_count &&
                last_cur_node->_count + last_parent_node->nodes()[last_cur_node_pos + 1]->_count <= __bucket_nodes_capacity_max / 2)
            {
                _node_type* last_next_node = last_parent_node->nodes()[last_cur_node_pos + 1];

                std::memcpy(last_cur_node->nodes_end(),
                            last_next_node->nodes(),
                            last_next_node->_count * sizeof(_node_type*));

                last_cur_node->_count += last_next_node->_count;
                deallocate_node(last_next_node);
                last_next_node_is_empty = true;
            }

            old_first_parent_node_count = first_parent_node->_count;
            old_last_parent_node_count = last_parent_node->_count;

            if (parent_same_parent)
            {
                if (same_parent)
                {
                    bool first_next_node_is_empty = false;

                    if (first_cur_node_pos + 1 < first_parent_node->_count &&
                        new_first_cur_node->_count + first_parent_node->nodes()[first_cur_node_pos + 1]->_count <= __bucket_nodes_capacity_max / 2)
                    {
                        _node_type* first_next_node = first_parent_node->nodes()[first_cur_node_pos + 1];

                        std::memcpy(new_first_cur_node->nodes_end(),
                                    first_next_node->nodes(),
                                    first_next_node->_count * sizeof(_node_type*));

                        new_first_cur_node->_count += first_next_node->_count;
                        deallocate_node(first_next_node);
                        first_next_node_is_empty = true;
                    }

                    if (first_cur_node_is_empty || first_next_node_is_empty)
                    {
                        std::memmove(first_parent_node->nodes() + (first_cur_node_pos + !first_cur_node_is_empty),
                                     first_parent_node->nodes() + (first_cur_node_pos + 1 + first_next_node_is_empty),
                                     (first_parent_node->_count - first_cur_node_pos - 1 - first_next_node_is_empty) * sizeof(_node_type*));

                        first_parent_node->_count -= first_cur_node_is_empty + first_next_node_is_empty;
                    }
                }
                else // (!same_parent)
                {
                    _node_type** erase_nodes_end = last._stack[depth];
                    for (_node_type** node_ptr = first._stack[depth] + 1; node_ptr < erase_nodes_end; ++node_ptr)
                    {
                        clear_node(*node_ptr, depth);
                        deallocate_node(*node_ptr);
                    }

                    uint32_t count_1 = first_cur_node_pos + !first_cur_node_is_empty;
                    uint32_t count_2 = first_parent_node->_count - last_cur_node_pos;
                    bool last_cur_node_is_empty;

                    if (new_first_cur_node != nullptr &&
                        new_first_cur_node->_count + last_cur_node->_count <= __bucket_nodes_capacity_max / 2)
                    {
                        std::memcpy(new_first_cur_node->nodes_end(),
                                    last_cur_node->nodes(),
                                    last_cur_node->_count * sizeof(_node_type*));

                        out._stack[depth - 1] = new_first_cur_node->nodes_end();
                        out._stack[depth] = first._stack[depth] - first_cur_node_is_empty;

                        new_first_cur_node->_count += last_cur_node->_count;
                        deallocate_node(last_cur_node);
                        last_cur_node_is_empty = true;
                    }
                    else
                    {
                        out._stack[depth] = first_parent_node->nodes() + count_1;
                        last_cur_node_is_empty = false;
                    }

                    if (last_next_node_is_empty)
                    {
                        if (last_cur_node_is_empty)
                        {
                            std::memmove(first_parent_node->nodes() + count_1,
                                         erase_nodes_end + 2,
                                         (count_2 - 2) * sizeof(_node_type*));
                        }
                        else
                        {
                            first_parent_node->nodes()[count_1] = last_cur_node;

                            std::memmove(first_parent_node->nodes() + (count_1 + 1),
                                         erase_nodes_end + 2,
                                         (count_2 - 2) * sizeof(_node_type*));
                        }
                    }
                    else
                    {
                        std::memmove(first_parent_node->nodes() + count_1,
                                     erase_nodes_end + last_cur_node_is_empty,
                                     (count_2 - last_cur_node_is_empty) * sizeof(_node_type*));
                    }

                    first_parent_node->_count = count_1 + count_2 - last_cur_node_is_empty - last_next_node_is_empty;
                }
            }
            else // (!parent_same_parent)
            {
                _node_type** erase_nodes_end = first_parent_node->nodes_end();
                for (_node_type** node_ptr = first._stack[depth] + 1; node_ptr < erase_nodes_end; ++node_ptr)
                {
                    clear_node(*node_ptr, depth);
                    deallocate_node(*node_ptr);
                }

                first_parent_node->_count = first_cur_node_pos + !first_cur_node_is_empty;

                erase_nodes_end = last._stack[depth];
                for (_node_type** node_ptr = last_parent_node->nodes(); node_ptr < erase_nodes_end; ++node_ptr)
                {
                    clear_node(*node_ptr, depth);
                    deallocate_node(*node_ptr);
                }

                if (last_cur_node_pos + last_next_node_is_empty > 0)
                {
                    std::memmove(last_parent_node->nodes() + 1,
                                 last_parent_node->nodes() + (last_cur_node_pos + last_next_node_is_empty + 1),
                                 (last_parent_node->_count - last_cur_node_pos - last_next_node_is_empty - 1) * sizeof(_node_type*));

                    last_parent_node->_count -= last_cur_node_pos + last_next_node_is_empty;
                }

                if (last_cur_node_pos > 0)
                {
                    *last_parent_node->nodes() = last_cur_node;
                    out._stack[depth] = last_parent_node->nodes();
                }
            }

            if (first_parent_node->_count == old_first_parent_node_count &&
                last_parent_node->_count == old_last_parent_node_count)
            {
                break;
            }

            if (_tree_height > depth + 1 &&
                last_parent_node->_ref_value != (*last_parent_node->nodes())->_ref_value)
            {
                last_parent_node->_ref_value = (*last_parent_node->nodes())->_ref_value;
                update_ref_value(const_cast<_node_type***>(last._stack), depth + 1, last_parent_node->_ref_value);
            }
        }

        return out;
    }

    template<typename _output_iterator>
    _output_iterator erase_range(const _const_iterator& first,
                                 const _const_iterator& last)
    {
        _output_iterator out(this, nullptr);

        if (first._value_ptr == nullptr)
        {
            return out;
        }
        else if (last._value_ptr == nullptr)
        {
            if (first == _base::cbegin())
            {
                clear();
                return out;
            }

            erase_range_core(first);
        }
        else
        {
            out = erase_range_core<_output_iterator>(first, last);
        }

        while (_root_node->_count == 1 && _tree_height > 0)
        {
            _node_type* new_root_node = *_root_node->nodes();
            deallocate_root_node();
            _root_node = new_root_node;
            _root_node->_bucket_bysize = _bucket_bysize_max;

            --_tree_height;
        }

        if (_tree_height == 0)
        {
            uint32_t root_bucket_bysize = _root_node->_bucket_bysize;
            uint32_t values_bysize = _root_node->_count * sizeof(_value_type);
            while (values_bysize < root_bucket_bysize >> 1)
            {
                root_bucket_bysize >>= 1;
            }

            if (root_bucket_bysize < _root_node->_bucket_bysize)
            {
                _node_type* new_root_node = allocate_root_node(root_bucket_bysize);
                new_root_node->_count = _root_node->_count;

                for (uint32_t i = 0; i < _root_node->_count; ++i)
                {
                    _awrapper.construct(new_root_node->values() + i, std::move(_root_node->values()[i]));
                    _awrapper.destroy(_root_node->values() + i);
                }

                if (out._value_ptr != nullptr)
                {
                    out._value_ptr = new_root_node->values() + (out._value_ptr - _root_node->values());
                }

                deallocate_root_node();
                _root_node = new_root_node;
            }
        }

        return out;
    }

}; // _bplus_tree

template<typename _a, typename _b, typename _c, typename _d, typename _e, typename _f, uint32_t _g, uint32_t _h>
inline bool operator == (const xxfl::_bplus_tree<_a, _b, _c, _d, _e, _f, _g, _h>& x,
                         const xxfl::_bplus_tree<_a, _b, _c, _d, _e, _f, _g, _h>& y)
{
    return x._values_count == y._values_count && std::equal(x.cbegin(), x.cend(), y.cbegin());
}

template<typename _a, typename _b, typename _c, typename _d, typename _e, typename _f, uint32_t _g, uint32_t _h>
inline bool operator < (const xxfl::_bplus_tree<_a, _b, _c, _d, _e, _f, _g, _h>& x,
                        const xxfl::_bplus_tree<_a, _b, _c, _d, _e, _f, _g, _h>& y)
{
    return std::lexicographical_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
}

} // xxfl
