#pragma once
#include "xxfl_set_platform_helper.h"

namespace xxfl {

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4200)
#endif

template<typename _value_type>
struct _bplus_tree_node
{
    uint32_t _count;
    union
    {
        _value_type* _ref_value;
        uint32_t _bucket_bysize; // this field won't available except for root node
    };

    uint8_t _bucket[];

    _bplus_tree_node** nodes() const noexcept { return (_bplus_tree_node**)_bucket; }
    _value_type* values() const noexcept { return (_value_type*)_bucket; }

    _bplus_tree_node** nodes_end() const noexcept { return (_bplus_tree_node**)_bucket + _count; }
    _value_type* values_end() const noexcept { return (_value_type*)_bucket + _count; }
};

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

template<typename _value_type, uint32_t _tree_height_max>
struct _bplus_tree_base;

template<typename _value_type, uint32_t _tree_height_max>
struct _bplus_tree_iterator_base
{
    typedef _value_type  value_type;

    typedef std::bidirectional_iterator_tag iterator_category;
    typedef std::ptrdiff_t                  difference_type;

    typedef _bplus_tree_base<_value_type, _tree_height_max> _bplus_tree_type;
    typedef _bplus_tree_node<_value_type>                   _node_type;

    _bplus_tree_type* _tree;
    _value_type* _value_ptr;
    _node_type** _stack[_tree_height_max];

    _bplus_tree_iterator_base() noexcept {}

    _bplus_tree_iterator_base(_bplus_tree_type* tree) noexcept : _tree(tree) {}

    _bplus_tree_iterator_base(_bplus_tree_type* tree, _value_type* value_ptr) noexcept
    : _tree(tree), _value_ptr(value_ptr) {}

    void set_first() noexcept
    {
        if (_tree->_values_count == 0)
        {
            _value_ptr = nullptr;
        }
        else
        {
            _node_type* cur_node = _tree->_root_node;

            for (uint32_t depth = _tree->_tree_height - 1; depth != (uint32_t)-1; --depth)
            {
                _node_type** node_ptr = cur_node->nodes();
                _stack[depth] = node_ptr;
                cur_node = *node_ptr;
            }

            _value_ptr = cur_node->values();
        }
    }

    void set_last() noexcept
    {
        if (_tree->_values_count == 0)
        {
            _value_ptr = nullptr;
        }
        else
        {
            _node_type* cur_node = _tree->_root_node;

            for (uint32_t depth = _tree->_tree_height - 1; depth != (uint32_t)-1; --depth)
            {
                _node_type** node_ptr = cur_node->nodes() + (cur_node->_count - 1);
                _stack[depth] = node_ptr;
                cur_node = *node_ptr;
            }

            _value_ptr = cur_node->values() + (cur_node->_count - 1);
        }
    }

    void cross_node_increment() noexcept
    {
        for (uint32_t depth = 0; depth < _tree->_tree_height; ++depth)
        {
            _node_type* parent_node = (depth + 1 < _tree->_tree_height)? *_stack[depth + 1] :
                                                                         _tree->_root_node;
            uint32_t node_pos = (uint32_t)(_stack[depth] - parent_node->nodes());

            if (node_pos + 1 < parent_node->_count)
            {
                ++_stack[depth];
                _node_type* cur_node = *_stack[depth];

                for (depth = depth - 1; depth != (uint32_t)-1; --depth)
                {
                    _node_type** node_ptr = cur_node->nodes();
                    _stack[depth] = node_ptr;
                    cur_node = *node_ptr;
                }

                _value_ptr = cur_node->values();
                return;
            }
        }

        _value_ptr = nullptr;
    }

    void cross_node_decrement() noexcept
    {
        for (uint32_t depth = 0; depth < _tree->_tree_height; ++depth)
        {
            _node_type* parent_node = (depth + 1 < _tree->_tree_height)? *_stack[depth + 1] :
                                                                         _tree->_root_node;

            if (_stack[depth] > parent_node->nodes())
            {
                --_stack[depth];
                _node_type* cur_node = *_stack[depth];

                for (depth = depth - 1; depth != (uint32_t)-1; --depth)
                {
                    _node_type** node_ptr = cur_node->nodes() + (cur_node->_count - 1);
                    _stack[depth] = node_ptr;
                    cur_node = *node_ptr;
                }

                _value_ptr = cur_node->values() + (cur_node->_count - 1);
                return;
            }
        }

        _value_ptr = nullptr;
    }

    void increment() noexcept
    {
        if (_value_ptr == nullptr)
        {
            return;
        }

        _node_type* cur_node = (_tree->_tree_height > 0)? *_stack[0] :
                                                          _tree->_root_node;
        uint32_t value_pos = (uint32_t)(_value_ptr - cur_node->values());

        if (value_pos + 1 < cur_node->_count)
        {
            ++_value_ptr;
        }
        else
        {
            cross_node_increment();
        }
    }

    void decrement() noexcept
    {
        if (_value_ptr == nullptr)
        {
            set_last();
            return;
        }

        _node_type* cur_node = (_tree->_tree_height > 0)? *_stack[0] :
                                                          _tree->_root_node;

        if (_value_ptr > cur_node->values())
        {
            --_value_ptr;
        }
        else
        {
            cross_node_decrement();
        }
    }
};

template<typename _value_type, uint32_t _tree_height_max>
struct _bplus_tree_iterator : _bplus_tree_iterator_base<_value_type, _tree_height_max>
{
    typedef _bplus_tree_iterator_base<_value_type, _tree_height_max> _base;

    typedef _value_type& reference;
    typedef _value_type* pointer;

    using typename _base::_bplus_tree_type;

    _bplus_tree_iterator() noexcept {}

    _bplus_tree_iterator(_bplus_tree_type* tree) noexcept : _base::_bplus_tree_iterator_base(tree) {}

    _bplus_tree_iterator(_bplus_tree_type* tree, _value_type* value_ptr) noexcept
    : _base::_bplus_tree_iterator_base(tree, value_ptr) {}

    _bplus_tree_iterator(const _bplus_tree_iterator& it) noexcept : _base::_bplus_tree_iterator_base(it) {}

    _bplus_tree_iterator& _const_cast() const noexcept { return const_cast<_bplus_tree_iterator&>(*this); }

    reference operator * () const noexcept { return *_base::_value_ptr; }
    pointer operator -> () const noexcept { return _base::_value_ptr; }

    _bplus_tree_iterator& operator ++ () noexcept
    {
        _base::increment();
        return *this;
    }

    _bplus_tree_iterator operator ++ (int) noexcept
    {
        _bplus_tree_iterator tmp(*this);
        _base::increment();
        return tmp;
    }

    _bplus_tree_iterator& operator -- () noexcept
    {
        _base::decrement();
        return *this;
    }

    _bplus_tree_iterator operator -- (int) noexcept
    {
        _bplus_tree_iterator tmp(*this);
        _base::decrement();
        return tmp;
    }

    bool operator == (const _bplus_tree_iterator& x) const noexcept
    { return _base::_value_ptr == x._value_ptr; }

    bool operator != (const _bplus_tree_iterator& x) const noexcept
    { return _base::_value_ptr != x._value_ptr; }
};

template<typename _value_type, uint32_t _tree_height_max>
struct _bplus_tree_const_iterator : _bplus_tree_iterator_base<_value_type, _tree_height_max>
{
    typedef _bplus_tree_iterator_base<_value_type, _tree_height_max> _base;

    typedef const _value_type& reference;
    typedef const _value_type* pointer;

    typedef _bplus_tree_iterator<_value_type, _tree_height_max> iterator;

    using typename _base::_bplus_tree_type;

#if defined(_MSC_VER)
    typedef _base _Unchecked_type;
#endif

    _bplus_tree_const_iterator() noexcept {}

    _bplus_tree_const_iterator(_bplus_tree_type* tree) noexcept : _base::_bplus_tree_iterator_base(tree) {}

    _bplus_tree_const_iterator(_bplus_tree_type* tree, _value_type* value_ptr) noexcept
    : _base::_bplus_tree_iterator_base(tree, value_ptr) {}

    _bplus_tree_const_iterator(const _base& it) noexcept : _base::_bplus_tree_iterator_base(it) {}

    iterator& _const_cast() const noexcept { return (iterator&)(const_cast<_bplus_tree_const_iterator&>(*this)); }

    reference operator * () const noexcept { return *_base::_value_ptr; }
    pointer operator -> () const noexcept { return _base::_value_ptr; }

    _bplus_tree_const_iterator& operator ++ () noexcept
    {
        _base::increment();
        return *this;
    }

    _bplus_tree_const_iterator operator ++ (int) noexcept
    {
        _bplus_tree_const_iterator tmp(*this);
        _base::increment();
        return tmp;
    }

    _bplus_tree_const_iterator& operator -- () noexcept
    {
        _base::decrement();
        return *this;
    }

    _bplus_tree_const_iterator operator -- (int) noexcept
    {
        _bplus_tree_const_iterator tmp(*this);
        _base::decrement();
        return tmp;
    }

    bool operator == (const _bplus_tree_const_iterator& x) const noexcept
    { return _base::_value_ptr == x._value_ptr; }

    bool operator != (const _bplus_tree_const_iterator& x) const noexcept
    { return _base::_value_ptr != x._value_ptr; }
};

} // xxfl

namespace std {

template<typename _value_type, uint32_t _tree_height_max>
inline bool
operator == (const xxfl::_bplus_tree_iterator<_value_type, _tree_height_max>& x,
             const xxfl::_bplus_tree_const_iterator<_value_type, _tree_height_max>& y) noexcept
{ return x._value_ptr == y._value_ptr; }

template<typename _value_type, uint32_t _tree_height_max>
inline bool
operator != (const xxfl::_bplus_tree_iterator<_value_type, _tree_height_max>& x,
             const xxfl::_bplus_tree_const_iterator<_value_type, _tree_height_max>& y) noexcept
{ return x._value_ptr != y._value_ptr; }

} // std
