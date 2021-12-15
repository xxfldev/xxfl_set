#include <thread>
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

#elif defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>

uint64_t get_memory_usage()
{
    mach_task_basic_info mtbi;
    mach_msg_type_number_t info_cnt = MACH_TASK_BASIC_INFO_COUNT;

    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&mtbi, &info_cnt) != KERN_SUCCESS )
    {
        return 0;
    }
    return mtbi.resident_size / 1024;
}

#elif defined(__linux__)
#include <cstdio>
#include <unistd.h>

uint64_t get_memory_usage()
{
    FILE* fp = fopen("/proc/self/statm", "r");
    if (fp == nullptr)
    {
        return 0;
    }

    uint64_t resident = 0;
    int ns = std::fscanf(fp, "%*s%llu", &resident);

    fclose(fp);

    return (ns == 1)? resident * sysconf(_SC_PAGESIZE) / 1024: 0;
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
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    uint64_t before_usage = get_memory_usage();

    std::vector<_container> containers;
    containers.reserve(containers_count);

    for (uint32_t i = 0; i < containers_count; ++i)
    {
        containers.push_back(_container());
        container_insert_sequential(containers.back(), insert_count);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::printf("%" PRIu64 " kbytes\n", get_memory_usage() - before_usage);
}

template<typename _container>
void container_test_memory_usage_random(uint32_t insert_count, uint32_t containers_count)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    uint64_t before_usage = get_memory_usage();

    std::vector<_container> containers;
    containers.reserve(containers_count);

    for (uint32_t i = 0; i < containers_count; ++i)
    {
        containers.push_back(_container());
        container_insert_random_2(containers.back(), insert_count);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::printf("%" PRIu64 " kbytes\n", get_memory_usage() - before_usage);
}

void memory_usage_test()
{
    const uint32_t total_values_count_min = 500000;

    std::printf("insert count: ");
    uint32_t insert_count = 0;
    int ns = std::scanf("%u", &insert_count);
    std::printf("\n");

    if (insert_count == 0 || ns < 1)
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

    std::printf("testing...");

    while (aa.size() < values_count)
    {
        uint32_t r = rand_gen() % (values_count * 10);
        aa.insert(r);
        bb.insert(r);
    }

    for (uint32_t i = 0; i < values_count * 5; ++i)
    {
        uint32_t r = rand_gen() % (values_count * 10);
        aa.erase(r);
        bb.erase(r);
    }

    for (uint32_t i = 0; i < values_count * 5; ++i)
    {
        uint32_t r = rand_gen() % (values_count * 10);
        aa.insert(r);
        bb.insert(r);
    }

    for (uint32_t i = 0; i < values_count * 5; ++i)
    {
        uint32_t r = rand_gen() % (values_count * 10);
        aa.erase(r);
        bb.erase(r);
    }

    bool success = (aa.size() == bb.size() && std::equal(aa.begin(), aa.end(), bb.begin()));

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

    std::printf("xxfl_string_set:\n");
    container_get_max_capacity<xxfl_string_set>();

    std::printf("xxfl_int_map:\n");
    container_get_max_capacity<xxfl_int_map>();

    std::printf("xxfl_string_map:\n");
    container_get_max_capacity<xxfl_string_map>();
}
