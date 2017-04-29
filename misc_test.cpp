#include "xxfl_set_test.h"

#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>

uint64_t get_memory_usage()
{
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    return pmc.WorkingSetSize / 1024;
}
#else
uint64_t get_memory_usage()
{
    return 0;
}
#endif

template<typename _container>
void container_test_memory_usage_sequential(uint32_t insert_count, uint32_t containers_count)
{
    uint64_t before_usage = get_memory_usage();

    std::vector<_container> containers;
    containers.reserve(containers_count);

    for (uint32_t i = 0; i < containers_count; ++i)
    {
        containers.push_back(_container());
        container_insert_sequential(containers.back(), insert_count);
    }

    std::printf("%" PRIu64 " kbytes\n", get_memory_usage() - before_usage);
}

template<typename _container>
void container_test_memory_usage_random(uint32_t insert_count, uint32_t containers_count)
{
    uint64_t before_usage = get_memory_usage();

    std::vector<_container> containers;
    containers.reserve(containers_count);

    for (uint32_t i = 0; i < containers_count; ++i)
    {
        containers.push_back(_container());
        container_insert_random_2(containers.back(), insert_count);
    }

    std::printf("%" PRIu64 " kbytes\n", get_memory_usage() - before_usage);
}

void memory_usage_test()
{
    const uint32_t total_values_count_min = 500000;

#if !defined(_WIN32)
    std::printf("not supported on this platform\n");
    return;
#endif

    std::printf("insert count: ");
    uint32_t insert_count = 0;
    std::scanf("%u", &insert_count);
    std::printf("\n");

    if (insert_count == 0)
    {
        return;
    }

    uint32_t containers_count = 1;
    if (insert_count <= total_values_count_min / 2)
    {
        containers_count = total_values_count_min / insert_count;
        std::printf("sum of %u containers\n\n", containers_count);
    }

    {
        std::printf("std_int_set: ");
        container_test_memory_usage_sequential<std_int_set>(insert_count, containers_count);

        std::printf("xxfl_int_set(sequential): ");
        container_test_memory_usage_sequential<xxfl_int_set>(insert_count, containers_count);

        std::printf("xxfl_int_set(random): ");
        container_test_memory_usage_random<xxfl_int_set>(insert_count, containers_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_set: ");
        container_test_memory_usage_sequential<std_string_set>(insert_count, containers_count);

        std::printf("xxfl_string_set(sequential): ");
        container_test_memory_usage_sequential<xxfl_string_set>(insert_count, containers_count);

        std::printf("xxfl_string_set(random): ");
        container_test_memory_usage_random<xxfl_string_set>(insert_count, containers_count);

        std::printf("\n");
    }

    {
        std::printf("std_int_map: ");
        container_test_memory_usage_sequential<std_int_map>(insert_count, containers_count);

        std::printf("xxfl_int_map(sequential): ");
        container_test_memory_usage_sequential<xxfl_int_map>(insert_count, containers_count);

        std::printf("xxfl_int_map(random): ");
        container_test_memory_usage_random<xxfl_int_map>(insert_count, containers_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_map: ");
        container_test_memory_usage_sequential<std_string_map>(insert_count, containers_count);

        std::printf("xxfl_string_map(sequential): ");
        container_test_memory_usage_sequential<xxfl_string_map>(insert_count, containers_count);

        std::printf("xxfl_string_map(random): ");
        container_test_memory_usage_random<xxfl_string_map>(insert_count, containers_count);

        std::printf("\n");
    }
}

void verification_test()
{
    const uint32_t values_count = 100000;

    std_int_set aa;
    xxfl_int_set bb;
    xxfl_thin_int_set cc;

    std::printf("testing...");

    while (aa.size() < values_count)
    {
        uint32_t r = rand_gen() % (values_count * 10);
        aa.insert(r);
        bb.insert(r);
        cc.insert(r);
    }

    for (uint32_t i = 0; i < values_count * 5; ++i)
    {
        uint32_t r = rand_gen() % (values_count * 10);
        aa.erase(r);
        bb.erase(r);
        cc.erase(r);
    }

    for (uint32_t i = 0; i < values_count * 5; ++i)
    {
        uint32_t r = rand_gen() % (values_count * 10);
        aa.insert(r);
        bb.insert(r);
        cc.insert(r);
    }

    for (uint32_t i = 0; i < values_count * 5; ++i)
    {
        uint32_t r = rand_gen() % (values_count * 10);
        aa.erase(r);
        bb.erase(r);
        cc.erase(r);
    }

    bool success = (aa.size() == bb.size() && std::equal(aa.begin(), aa.end(), bb.begin()));
    success &= (aa.size() == cc.size() && std::equal(aa.begin(), aa.end(), cc.begin()));

    std::printf("%s\n", success? "passed" : "error");
}

template<typename _container>
void container_get_max_capacity()
{
    std::printf("max_capacity_in_theory       = %" PRIu64 "\n",
                _container::_bplus_tree_type::max_capacity_in_theory());
    std::printf("max_capacity_in_practice     = %" PRIu64 "\n",
                _container::_bplus_tree_type::max_capacity_in_practice());
    std::printf("max_capacity_in_conservative = %" PRIu64 "\n\n",
                _container::_bplus_tree_type::max_capacity_in_conservative());
}

void get_max_capacity()
{
    std::printf("xxfl_int_set:\n");
    container_get_max_capacity<xxfl_int_set>();

    std::printf("xxfl_thin_int_set:\n");
    container_get_max_capacity<xxfl_thin_int_set>();

    std::printf("xxfl_string_set:\n");
    container_get_max_capacity<xxfl_string_set>();

    std::printf("xxfl_thin_string_set:\n");
    container_get_max_capacity<xxfl_thin_string_set>();

    std::printf("xxfl_int_map:\n");
    container_get_max_capacity<xxfl_int_map>();

    std::printf("xxfl_thin_int_map:\n");
    container_get_max_capacity<xxfl_thin_int_map>();

    std::printf("xxfl_string_map:\n");
    container_get_max_capacity<xxfl_string_map>();

    std::printf("xxfl_thin_string_map:\n");
    container_get_max_capacity<xxfl_thin_string_map>();
}
