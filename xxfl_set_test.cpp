#include "xxfl_set_test.h"

std::mt19937 rand_gen;

int main()
{
    std::random_device rd;
    rand_gen.seed(rd());

    while (true)
    {
        std::printf("main menu:\n"
                    "  0. exit\n"
                    "  1. interface test\n"
                    "  2. performance test\n"
                    "  3. memory usage test\n"
                    "  4. verification test\n"
                    "  5. get max capacity\n"
                    "select: ");

        uint32_t select_idx = 0;
        std::scanf("%u", &select_idx);
        std::printf("\n");

        if (select_idx == 0)
        {
            break;
        }
        else if (select_idx == 1)
        {
            interface_test();
        }
        else if (select_idx == 2)
        {
            performance_test();
        }
        else if (select_idx == 3)
        {
            memory_usage_test();
        }
        else if (select_idx == 4)
        {
            verification_test();
        }
        else if (select_idx == 5)
        {
            get_max_capacity();
        }
        else
        {
            continue;
        }

        std::printf("\n");
    }

    return 0;
}
