#ifndef __ASM_L2X0_H__
#define __ASM_L2X0_H__

#define L2X0_CACHE_ID                   0x000
#define L2X0_CACHE_TYPE                 0x004
#define L2X0_CTRL                       0x100
#define L2X0_AUX_CTRL                   0x104
#define L2X0_EVENT_CNT_CTRL             0x200
#define L2X0_EVENT_CNT1_CFG             0x204
#define L2X0_EVENT_CNT0_CFG             0x208
#define L2X0_EVENT_CNT1_VAL             0x20C
#define L2X0_EVENT_CNT0_VAL             0x210
#define L2X0_INTR_MASK                  0x214
#define L2X0_MASKED_INTR_STAT           0x218
#define L2X0_RAW_INTR_STAT              0x21C
#define L2X0_INTR_CLEAR                 0x220
#define L2X0_CACHE_SYNC                 0x730
#define L2X0_INV_LINE_PA                0x770
#define L2X0_INV_WAY                    0x77C
#define L2X0_CLEAN_LINE_PA              0x7B0
#define L2X0_CLEAN_LINE_IDX             0x7B8
#define L2X0_CLEAN_WAY                  0x7BC
#define L2X0_CLEAN_INV_LINE_PA          0x7F0
#define L2X0_CLEAN_INV_LINE_IDX         0x7F8
#define L2X0_CLEAN_INV_WAY              0x7FC
#define L2X0_LOCKDOWN_WAY_D             0x900
#define L2X0_LOCKDOWN_WAY_I             0x904
#define L2X0_TEST_OPERATION             0xF00
#define L2X0_LINE_DATA                  0xF10
#define L2X0_LINE_TAG                   0xF30
#define L2X0_DEBUG_CTRL                 0xF40

#endif

