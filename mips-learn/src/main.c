#include <stdio.h>
#include <libdragon.h>
#include <signal.h>

#define RSPQ_DEBUG 1

DEFINE_RSP_UCODE(rsp_test);

uint32_t test_id;

void test_init()
{
    rspq_init();
    test_id = rspq_overlay_register(&rsp_test);
}

void test_close()
{
    rspq_overlay_unregister(test_id);
}

static inline void rspl_test(uint16_t* dest)
{
    extern uint32_t test_id;
    rspq_write(test_id, 0x0, PhysicalAddr(dest));
}

int main()
{
    console_init();
    assertf(debug_init_isviewer(), "Failed to start debugging");

    test_init();
    uint16_t* dest = malloc_uncached_aligned(16, sizeof(uint16_t));
    *dest = 0;
    printf("Before rsp %u\n", *dest);
    rspl_test(dest);
    rspq_wait();
    printf("RSP done %u\n", *dest);

    test_close();

    while (1) { }
    console_close();
    return 0;
}
