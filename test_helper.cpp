#include "xxfl_set_test.h"

std::string number_to_string(uint32_t n)
{
    const uint32_t def_string_len = 16;

    char string_buf[def_string_len + 1];

    std::memset(string_buf, '0', def_string_len - 10);
    std::snprintf(string_buf + (def_string_len - 10), 11, "%.10u", n);

    return std::string(string_buf, def_string_len);
}

uint32_t string_to_number(const std::string& str)
{
    return (uint32_t)std::atoi(str.c_str());
}

void int_vector_append_sequential(std_int_vector& x, uint32_t count)
{
    x.reserve(x.size() + count);

    for (uint32_t i = 0; i < count; ++i)
    {
        x.push_back(i);
    }
}

void string_vector_append_sequential(std_string_vector& x, uint32_t count)
{
    x.reserve(x.size() + count);

    for (uint32_t i = 0; i < count; ++i)
    {
        x.push_back(number_to_string(i));
    }
}

void int_pair_vector_append_sequential(std_int_pair_vector& x, uint32_t count)
{
    x.reserve(x.size() + count);

    for (uint32_t i = 0; i < count; ++i)
    {
        x.push_back(int_pair(i, i));
    }
}

void string_pair_vector_append_sequential(std_string_pair_vector& x, uint32_t count)
{
    x.reserve(x.size() + count);

    for (uint32_t i = 0; i < count; ++i)
    {
        std::string str(number_to_string(i));
        x.push_back(string_pair(str, str));
    }
}
