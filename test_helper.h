#pragma once

#define __STDC_FORMAT_MACROS

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <string>
#include <set>
#include <map>
#include <vector>
#include "src/xxfl_set.h"
#include "src/xxfl_map.h"

typedef uint32_t test_int; // uint32_t or uint64_t

typedef std::pair<const test_int, test_int>       int_pair;
typedef std::pair<const std::string, std::string> string_pair;

typedef std::less<test_int>    def_int_compare;
typedef std::less<std::string> def_string_compare;

typedef std::allocator<uint8_t> def_allocator;

typedef xxfl::set<test_int>    xxfl_int_set;
typedef xxfl::set<std::string> xxfl_string_set;

typedef xxfl::set<test_int, def_int_compare, def_allocator, sizeof(test_int) * 8, 24>          xxfl_thin_int_set;
typedef xxfl::set<std::string, def_string_compare, def_allocator, sizeof(std::string) * 8, 24> xxfl_thin_string_set;

typedef xxfl::map<test_int, test_int>       xxfl_int_map;
typedef xxfl::map<std::string, std::string> xxfl_string_map;

typedef xxfl::map<test_int, test_int, def_int_compare, def_allocator, sizeof(int_pair) * 8, 12>             xxfl_thin_int_map;
typedef xxfl::map<std::string, std::string, def_string_compare, def_allocator, sizeof(string_pair) * 8, 12> xxfl_thin_string_map;

typedef std::set<test_int>    std_int_set;
typedef std::set<std::string> std_string_set;

typedef std::map<test_int, test_int>       std_int_map;
typedef std::map<std::string, std::string> std_string_map;

typedef std::vector<test_int>    std_int_vector;
typedef std::vector<std::string> std_string_vector;
typedef std::vector<int_pair>    std_int_pair_vector;
typedef std::vector<string_pair> std_string_pair_vector;

extern std::mt19937 rand_gen;

std::string number_to_string(uint32_t n);
uint32_t string_to_number(const std::string& str);

void int_vector_append_sequential(std_int_vector& x, uint32_t count);
void string_vector_append_sequential(std_string_vector& x, uint32_t count);
void int_pair_vector_append_sequential(std_int_pair_vector& x, uint32_t count);
void string_pair_vector_append_sequential(std_string_pair_vector& x, uint32_t count);

template<typename _container, typename _value_type = typename _container::value_type>
struct container_op
{
    static void insert(_container& x, uint32_t n) {}
    static void erase(_container& x, uint32_t n) {}
    static typename _container::iterator find(_container& x, uint32_t n) {}
};

template<typename _container>
struct container_op<_container, test_int>
{
    static void insert(_container& x, uint32_t n) { x.insert(n); }
    static void erase(_container& x, uint32_t n) { x.erase(n); }
    static typename _container::iterator find(_container& x, uint32_t n) { return x.find(n); }
};

template<typename _container>
struct container_op<_container, std::string>
{
    static void insert(_container& x, uint32_t n) { x.insert(number_to_string(n)); }
    static void erase(_container& x, uint32_t n) { x.erase(number_to_string(n)); }
    static typename _container::iterator find(_container& x, uint32_t n) { return x.find(number_to_string(n)); }
};

template<typename _container>
struct container_op<_container, int_pair>
{
    static void insert(_container& x, uint32_t n) { x.insert(int_pair(n, n)); }
    static void erase(_container& x, uint32_t n) { x.erase(n); }
    static typename _container::iterator find(_container& x, uint32_t n) { return x.find(n); }
};

template<typename _container>
struct container_op<_container, string_pair>
{
    static void insert(_container& x, uint32_t n)
    {
        std::string str(number_to_string(n));
        x.insert(string_pair(str, str));
    }

    static void erase(_container& x, uint32_t n) { x.erase(number_to_string(n)); }
    static typename _container::iterator find(_container& x, uint32_t n) { return x.find(number_to_string(n)); }
};

template<typename _container>
void container_insert_sequential(_container& x, uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i)
    {
        container_op<_container>::insert(x, i);
    }
}

template<typename _container>
void container_insert_random_1(_container& x, uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i)
    {
        container_op<_container>::insert(x, rand_gen());
    }
}

template<typename _container>
void container_insert_random_2(_container& x, uint32_t count)
{
    size_t size_limit = x.size() + count;
    while (x.size() < size_limit)
    {
        container_op<_container>::insert(x, rand_gen());
    }
}
