#pragma once
#include <memory>

namespace std {

#if defined(_MSC_VER)

#define __throw_out_of_range _Xout_of_range

template<typename _tp>
struct __identity : public unary_function<_tp, _tp>
{
    _tp& operator () (_tp& x) const
    { return x; }

    const _tp& operator () (const _tp& x) const
    { return x; }
};

template<typename _pair>
struct __select1st : public unary_function<_pair, typename _pair::first_type>
{
    typename _pair::first_type& operator () (_pair& x) const
    { return x.first; }

    const typename _pair::first_type& operator () (const _pair& x) const
    { return x.first; }
};

template<typename _input_iterator>
_input_iterator& __make_move_if_noexcept_iterator(_input_iterator& iter) { return iter; }

#else

#define __identity  _Identity
#define __select1st _Select1st

#endif

} // std

namespace xxfl {

template<typename _allocator>
struct __alloc_wrapper
{
#if defined(_MSC_VER)
    typedef std::allocator_traits<_allocator> _alloc_traits;
#else
    typedef __gnu_cxx::__alloc_traits<_allocator> _alloc_traits;
#endif

    typedef typename _alloc_traits::pointer         pointer;
    typedef typename _alloc_traits::const_pointer   const_pointer;
    typedef typename _allocator::reference          reference;
    typedef typename _allocator::const_reference    const_reference;
    typedef typename _allocator::size_type          size_type;
    typedef typename _allocator::difference_type    difference_type;

    template<typename _tp>
    struct rebind { typedef typename _allocator::template rebind<_tp>::other other; };

    _allocator _alloc;

    static constexpr bool propagate_on_container_copy_assignment()
    {
#if defined(_MSC_VER)
        return _alloc_traits::propagate_on_container_copy_assignment::value;
#else
        return _alloc_traits::_S_propagate_on_copy_assign();
#endif
    }

    static constexpr bool propagate_on_container_move_assignment()
    {
#if defined(_MSC_VER)
        return _alloc_traits::propagate_on_container_move_assignment::value;
#else
        return _alloc_traits::_S_propagate_on_move_assign();
#endif
    }

    static constexpr bool allocator_always_compares_equal()
    {
#if defined(_MSC_VER)
        return _alloc_traits::is_always_equal::value;
#else
        return _alloc_traits::_S_always_equal();
#endif
    }

    static constexpr bool is_nothrow_swap()
    {
#if defined(_MSC_VER)
        return std::_Is_nothrow_swappable<_allocator>::value;
#else
        return _alloc_traits::_S_nothrow_swap();
#endif
    }

    __alloc_wrapper() {}
    __alloc_wrapper(const _allocator& a) : _alloc(a) {}
    __alloc_wrapper(_allocator&& a) : _alloc(std::move(a)) {}
    __alloc_wrapper(const __alloc_wrapper& wrapper) : _alloc(wrapper._alloc) {}

    _allocator select_on_container_copy_construction() const
    {
#if defined(_MSC_VER)
        return _alloc_traits::select_on_container_copy_construction(_alloc);
#else
        return _alloc_traits::_S_select_on_copy(_alloc);
#endif
    }

    void copy_allocator(const _allocator& a)
    {
#if defined(_MSC_VER)
        std::_Pocca(_alloc, a);
#else
        std::__alloc_on_copy(_alloc, a);
#endif
    }

    void move_allocator(_allocator& a)
    {
#if defined(_MSC_VER)
        std::_Pocma(_alloc, a);
#else
        std::__alloc_on_move(_alloc, a);
#endif
    }

    void swap_allocator(_allocator& a)
    {
#if defined(_MSC_VER)
        std::_Pocs(_alloc, a);
#else
        std::__alloc_on_swap(_alloc, a);
#endif
    }

    pointer allocate(size_type size)
    {
#if defined(_MSC_VER)
        return _alloc.allocate(size);
#else
        return _alloc_traits::allocate(_alloc, size);
#endif
    }

    void deallocate(pointer p, size_type size)
    {
#if defined(_MSC_VER)
        _alloc.deallocate(p, size);
#else
        _alloc_traits::deallocate(_alloc, p, size);
#endif
    }

    template<typename _ptr, typename... _args>
    void construct(_ptr p, _args&&... args)
    {
#if defined(_MSC_VER)
        _alloc.construct(p, std::forward<_args>(args)...);
#else
        _alloc_traits::construct(_alloc, p, std::forward<_args>(args)...);
#endif
    }

    template<typename _ptr>
    void destroy(_ptr p)
    {
#if defined(_MSC_VER)
        _alloc.destroy(p);
#else
        _alloc_traits::destroy(_alloc, p);
#endif
    }

    template<typename _input_iterator>
    void destroy(_input_iterator first, _input_iterator last)
    {
#if defined(_MSC_VER)
        std::_Destroy_range(first, last);
#else
        std::_Destroy(first, last, _alloc);
#endif
    }

    size_type max_size() const noexcept
    {
#if defined(_MSC_VER)
        return _alloc.max_size();
#else
        return _alloc_traits::max_size(_alloc);
#endif
    }
};

} // xxfl
