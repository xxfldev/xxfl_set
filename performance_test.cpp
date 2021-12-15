#include <chrono>
#include "xxfl_set_test.h"

typedef std::chrono::steady_clock::time_point timestamp_t;

timestamp_t get_cur_time()
{
    return std::chrono::steady_clock::now();
}

double get_elapsed_time(const timestamp_t& start)
{
    using namespace std::chrono;
    return duration_cast<duration<double>>(steady_clock::now() - start).count();
}

template<typename _container>
void container_test_insert_performance_sequential(uint32_t insert_count, uint32_t loops_count)
{
    timestamp_t start_time = get_cur_time();

    for (uint32_t i = 0; i < loops_count; ++i)
    {
        _container aa;
        container_insert_sequential(aa, insert_count);
    }

    std::printf("%f sec\n", get_elapsed_time(start_time));
}

template<typename _container>
void container_test_insert_performance_random(uint32_t insert_count, uint32_t loops_count)
{
    timestamp_t start_time = get_cur_time();

    for (uint32_t i = 0; i < loops_count; ++i)
    {
        _container aa;
        container_insert_random_1(aa, insert_count);
    }

    std::printf("%f sec\n", get_elapsed_time(start_time));
}

void test_insert_performance()
{
    const uint32_t total_insert_count_min = 500000;

    std::printf("insert count: ");
    uint32_t insert_count = 0;
    int ns = std::scanf("%u", &insert_count);
    std::printf("\n");

    if (insert_count == 0 || ns < 1)
    {
        return;
    }

    uint32_t loops_count = 1;
    if (insert_count <= total_insert_count_min / 2)
    {
        loops_count = total_insert_count_min / insert_count;
        std::printf("loop %u times\n\n", loops_count);
    }

    {
        std::printf("std_int_set(sequential): ");
        container_test_insert_performance_sequential<std_int_set>(insert_count, loops_count);

        std::printf("xxfl_int_set(sequential): ");
        container_test_insert_performance_sequential<xxfl_int_set>(insert_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_int_set(random): ");
        container_test_insert_performance_random<std_int_set>(insert_count, loops_count);

        std::printf("xxfl_int_set(random): ");
        container_test_insert_performance_random<xxfl_int_set>(insert_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_set(sequential): ");
        container_test_insert_performance_sequential<std_string_set>(insert_count, loops_count);

        std::printf("xxfl_string_set(sequential): ");
        container_test_insert_performance_sequential<xxfl_string_set>(insert_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_set(random): ");
        container_test_insert_performance_random<std_string_set>(insert_count, loops_count);

        std::printf("xxfl_string_set(random): ");
        container_test_insert_performance_random<xxfl_string_set>(insert_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_int_map(sequential): ");
        container_test_insert_performance_sequential<std_int_map>(insert_count, loops_count);

        std::printf("xxfl_int_map(sequential): ");
        container_test_insert_performance_sequential<xxfl_int_map>(insert_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_int_map(random): ");
        container_test_insert_performance_random<std_int_map>(insert_count, loops_count);

        std::printf("xxfl_int_map(random): ");
        container_test_insert_performance_random<xxfl_int_map>(insert_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_map(sequential): ");
        container_test_insert_performance_sequential<std_string_map>(insert_count, loops_count);

        std::printf("xxfl_string_map(sequential): ");
        container_test_insert_performance_sequential<xxfl_string_map>(insert_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_map(random): ");
        container_test_insert_performance_random<std_string_map>(insert_count, loops_count);

        std::printf("xxfl_string_map(random): ");
        container_test_insert_performance_random<xxfl_string_map>(insert_count, loops_count);

        std::printf("\n");
    }
}

template<typename _container>
void container_test_erase_performance_sequential(uint32_t erase_count, uint32_t loops_count)
{
    _container aa;
    container_insert_sequential(aa, erase_count);

    timestamp_t start_time = get_cur_time();

    for (uint32_t i = 0; i < loops_count; ++i)
    {
        _container bb(aa);
        for (uint32_t j = 0; j < erase_count; ++j)
        {
            container_op<_container>::erase(bb, j);
        }
    }

    std::printf("%f sec\n", get_elapsed_time(start_time));
}

template<typename _container>
void container_test_erase_performance_random(uint32_t erase_count, uint32_t loops_count)
{
    _container aa;
    container_insert_sequential(aa, erase_count);

    timestamp_t start_time = get_cur_time();

    for (uint32_t i = 0; i < loops_count; ++i)
    {
        _container bb(aa);
        for (uint32_t j = 0; j < erase_count; ++j)
        {
            container_op<_container>::erase(bb, rand_gen() % erase_count);
        }
    }

    std::printf("%f sec\n", get_elapsed_time(start_time));
}

void test_erase_performance()
{
    const uint32_t total_erase_count_min = 500000;

    std::printf("erase count: ");
    uint32_t erase_count = 0;
    int ns = std::scanf("%u", &erase_count);
    std::printf("\n");

    if (erase_count == 0 || ns < 1)
    {
        return;
    }

    uint32_t loops_count = 1;
    if (erase_count <= total_erase_count_min / 2)
    {
        loops_count = total_erase_count_min / erase_count;
        std::printf("loop %u times\n\n", loops_count);
    }

    {
        std::printf("std_int_set(sequential): ");
        container_test_erase_performance_sequential<std_int_set>(erase_count, loops_count);

        std::printf("xxfl_int_set(sequential): ");
        container_test_erase_performance_sequential<xxfl_int_set>(erase_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_int_set(random): ");
        container_test_erase_performance_random<std_int_set>(erase_count, loops_count);

        std::printf("xxfl_int_set(random): ");
        container_test_erase_performance_random<xxfl_int_set>(erase_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_set(sequential): ");
        container_test_erase_performance_sequential<std_string_set>(erase_count, loops_count);

        std::printf("xxfl_string_set(sequential): ");
        container_test_erase_performance_sequential<xxfl_string_set>(erase_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_set(random): ");
        container_test_erase_performance_random<std_string_set>(erase_count, loops_count);

        std::printf("xxfl_string_set(random): ");
        container_test_erase_performance_random<xxfl_string_set>(erase_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_int_map(sequential): ");
        container_test_erase_performance_sequential<std_int_map>(erase_count, loops_count);

        std::printf("xxfl_int_map(sequential): ");
        container_test_erase_performance_sequential<xxfl_int_map>(erase_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_int_map(random): ");
        container_test_erase_performance_random<std_int_map>(erase_count, loops_count);

        std::printf("xxfl_int_map(random): ");
        container_test_erase_performance_random<xxfl_int_map>(erase_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_map(sequential): ");
        container_test_erase_performance_sequential<std_string_map>(erase_count, loops_count);

        std::printf("xxfl_string_map(sequential): ");
        container_test_erase_performance_sequential<xxfl_string_map>(erase_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_map(random): ");
        container_test_erase_performance_random<std_string_map>(erase_count, loops_count);

        std::printf("xxfl_string_map(random): ");
        container_test_erase_performance_random<xxfl_string_map>(erase_count, loops_count);

        std::printf("\n");
    }
}

template<typename _container>
void container_test_find_performance(uint32_t values_count, uint32_t find_count)
{
    _container aa;
    container_insert_sequential(aa, values_count);

    timestamp_t start_time = get_cur_time();

    volatile uint64_t tmp = 0;
    for (uint32_t i = 0; i < find_count; ++i)
    {
        auto it = container_op<_container>::find(aa, rand_gen() % values_count);
        tmp += (uint64_t)&it;
    }

    std::printf("%f sec\n", get_elapsed_time(start_time));
}

void test_find_performance()
{
    const uint32_t find_count_def = 500000;

    std::printf("values count: ");
    uint32_t values_count = 0;
    int ns = std::scanf("%u", &values_count);
    std::printf("\n");

    if (values_count == 0 || ns < 1)
    {
        return;
    }

    std::printf("find %u times\n\n", find_count_def);

    {
        std::printf("std_int_set: ");
        container_test_find_performance<std_int_set>(values_count, find_count_def);

        std::printf("xxfl_int_set: ");
        container_test_find_performance<xxfl_int_set>(values_count, find_count_def);

        std::printf("\n");
    }

    {
        std::printf("std_string_set: ");
        container_test_find_performance<std_string_set>(values_count, find_count_def);

        std::printf("xxfl_string_set: ");
        container_test_find_performance<xxfl_string_set>(values_count, find_count_def);

        std::printf("\n");
    }

    {
        std::printf("std_int_map: ");
        container_test_find_performance<std_int_map>(values_count, find_count_def);

        std::printf("xxfl_int_map: ");
        container_test_find_performance<xxfl_int_map>(values_count, find_count_def);

        std::printf("\n");
    }

    {
        std::printf("std_string_map: ");
        container_test_find_performance<std_string_map>(values_count, find_count_def);

        std::printf("xxfl_string_map: ");
        container_test_find_performance<xxfl_string_map>(values_count, find_count_def);

        std::printf("\n");
    }
}

template<typename _container>
void container_test_traversing_performance(uint32_t values_count, uint32_t loops_count)
{
    _container aa;
    container_insert_sequential(aa, values_count);

    timestamp_t start_time = get_cur_time();

    volatile uint64_t tmp = 0;
    for (uint32_t i = 0; i < loops_count; ++i)
    {
        for (auto value : aa)
        {
            tmp += (uint64_t)&value;
        }
    }

    std::printf("%f sec\n", get_elapsed_time(start_time));
}

void test_traversing_performance()
{
    const uint32_t total_increment_count_min = 50000000;

    std::printf("values count: ");
    uint32_t values_count = 0;
    int ns = std::scanf("%u", &values_count);
    std::printf("\n");

    if (values_count == 0 || ns < 1)
    {
        return;
    }

    uint32_t loops_count = 1;
    if (values_count <= total_increment_count_min / 2)
    {
        loops_count = total_increment_count_min / values_count;
        std::printf("loop %u times\n\n", loops_count);
    }

    {
        std::printf("std_int_set: ");
        container_test_traversing_performance<std_int_set>(values_count, loops_count);

        std::printf("xxfl_int_set: ");
        container_test_traversing_performance<xxfl_int_set>(values_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_set: ");
        container_test_traversing_performance<std_string_set>(values_count, loops_count);

        std::printf("xxfl_string_set: ");
        container_test_traversing_performance<xxfl_string_set>(values_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_int_map: ");
        container_test_traversing_performance<std_int_map>(values_count, loops_count);

        std::printf("xxfl_int_map: ");
        container_test_traversing_performance<xxfl_int_map>(values_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_map: ");
        container_test_traversing_performance<std_string_map>(values_count, loops_count);

        std::printf("xxfl_string_map: ");
        container_test_traversing_performance<xxfl_string_map>(values_count, loops_count);

        std::printf("\n");
    }
}

template<typename _container>
void container_test_combined_performance(uint32_t values_count, uint32_t loops_count)
{
    timestamp_t start_time = get_cur_time();

    volatile uint64_t tmp = 0;

    for (uint32_t i = 0; i < loops_count; ++i)
    {
        _container aa;
        container_insert_sequential(aa, values_count);

        for (uint32_t j = 0; j < values_count; ++j)
        {
            auto it = container_op<_container>::find(aa, rand_gen() % values_count);
            tmp += (uint64_t)&it;
        }

        for (auto value : aa)
        {
            tmp += (uint64_t)&value;
        }

        for (uint32_t j = 0; j < values_count; ++j)
        {
            container_op<_container>::erase(aa, rand_gen() % values_count);
        }
    }

    std::printf("%f sec\n", get_elapsed_time(start_time));
}

void test_combined_performance()
{
    const uint32_t total_ops_count_min = 500000;

    std::printf("values count: ");
    uint32_t values_count = 0;
    int ns = std::scanf("%u", &values_count);
    std::printf("\n");

    if (values_count == 0 || ns < 1)
    {
        return;
    }

    uint32_t loops_count = 1;
    if (values_count <= total_ops_count_min / 2)
    {
        loops_count = total_ops_count_min / values_count;
        std::printf("loop %u times\n\n", loops_count);
    }

    {
        std::printf("std_int_set: ");
        container_test_combined_performance<std_int_set>(values_count, loops_count);

        std::printf("xxfl_int_set: ");
        container_test_combined_performance<xxfl_int_set>(values_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_set: ");
        container_test_combined_performance<std_string_set>(values_count, loops_count);

        std::printf("xxfl_string_set: ");
        container_test_combined_performance<xxfl_string_set>(values_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_int_map: ");
        container_test_combined_performance<std_int_map>(values_count, loops_count);

        std::printf("xxfl_int_map: ");
        container_test_combined_performance<xxfl_int_map>(values_count, loops_count);

        std::printf("\n");
    }

    {
        std::printf("std_string_map: ");
        container_test_combined_performance<std_string_map>(values_count, loops_count);

        std::printf("xxfl_string_map: ");
        container_test_combined_performance<xxfl_string_map>(values_count, loops_count);

        std::printf("\n");
    }
}

void performance_test()
{
    while (true)
    {
        std::printf("performance test:\n"
                    "  0. return\n"
                    "  1. insert performance\n"
                    "  2. erase performance\n"
                    "  3. find performance\n"
                    "  4. traversing performance\n"
                    "  5. combined performance\n"
                    "select: ");

        uint32_t select_idx = 0;
        int ns = std::scanf("%u", &select_idx);
        std::printf("\n");

        if (select_idx == 0 || ns < 1)
        {
            return;
        }
        else if (select_idx == 1)
        {
            test_insert_performance();
        }
        else if (select_idx == 2)
        {
            test_erase_performance();
        }
        else if (select_idx == 3)
        {
            test_find_performance();
        }
        else if (select_idx == 4)
        {
            test_traversing_performance();
        }
        else if (select_idx == 5)
        {
            test_combined_performance();
        }

        std::printf("\n");
    }
}
