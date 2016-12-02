#ifndef __factors_h
#define __factors_h

#include <system/config.h>

__BEGIN_SYS

enum DMA_Operation
{
    READ,
    WRITE
};

enum DMA_Transaction_Size
{
    // 4 B
    WRITE_DMA_TLP_SIZE_4_B = 0x1,
    WRITE_DMA_TLP_COUNT_4_B = 0x1,
    DMA_BUFFER_SIZE_4_B = 4,

    // 128 B
    WRITE_DMA_TLP_SIZE_128_B = 0x20,
    WRITE_DMA_TLP_COUNT_128_B = 0x1,
    DMA_BUFFER_SIZE_128_B = 128,

    // 4 KiB
    WRITE_DMA_TLP_SIZE_4_KiB = 0x20,
    WRITE_DMA_TLP_COUNT_4_KiB = 0x20,
    DMA_BUFFER_SIZE_4_KiB = 4 * 1024,

    // 4 MiB (v1)
    WRITE_DMA_TLP_SIZE_4_MiB = 0x20,
    WRITE_DMA_TLP_COUNT_4_MiB = 0x8000,
    DMA_BUFFER_SIZE_4_MiB = 4 * 1024 * 1024,

    // Others: to be used with WRITE_TLP_COUNT = 0x1
    MAX_WRITE_DMA_TLP_SIZE = 0x1fff, /* Using WRITE_TLP_COUNT = 0x1 and this will
                                      * give a DMA transaction of size:
                                      * 32764 B (32 KiB - 4 B)
                                      *  */
    WRITE_DMA_TLP_SIZE_4_KiB_v2 = 0x400
};

enum Worker_Access_Pattern
{
    SEQ_R75_W25,
    RAND_R75_W25
};

struct Factors
{
    /* DMA - related */
    static const bool fpga_enabled = true; /*! @F */
    static const bool fpga_monitor_enabled = true;
    static const unsigned int dma_operation = WRITE; /*! @F */
    static const unsigned long long delay_between_dma_transactions = 0; /*! @F */
    static const unsigned int dma_buffer_size = 32 * 1024;
    static const unsigned int write_dma_tlp_size = WRITE_DMA_TLP_SIZE_4_KiB; /*! @F */
    static const unsigned int write_dma_tlp_count = 0x1; /*! @F */
    static const unsigned int dma_runs = 100;
    static const bool fpga_uses_interrupts = false;
    static const bool auto_restart_dma_transactions = true;

    /* Worker - related */
    static const unsigned int worker_access_pattern = SEQ_R75_W25; /*! @F */
    static const unsigned int work_set_size = 16 * 1024 * 1024; /*! @F */
    static const unsigned long worker_iteractions = 30;

};

__END_SYS

#endif
