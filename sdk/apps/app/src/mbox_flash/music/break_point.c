
#pragma bss_seg(".break_point.data.bss")
#pragma data_seg(".break_point.data")
#pragma const_seg(".break_point.text.const")
#pragma code_seg(".break_point.text")
#pragma str_literal_override(".break_point.text.const")

#include "break_point.h"
#include "music_play.h"
#include "device_app.h"
#include "vm.h"
/* #include "mbox_memory.h" */
/* #include "device_memory.h" */
/* #include "memory.h" */

/* #define LOG_TAG_CONST       APP */
#define LOG_TAG_CONST       OFF
#define LOG_TAG             "[music]"
#include "log.h"

#if MBOX_USE_VM_MEMORY
static const u8 playpoint_table[MAX_DEVICE][2] = {
    /* findex                   breakpoint */
    {VM_INDEX_UDISK_INDEX,   VM_INDEX_UDISK_BP},
    {VM_INDEX_SDMMCA_INDEX,  VM_INDEX_SDMMCA_BP},
    {VM_INDEX_INRFLSH_INDEX, VM_INDEX_INRFLSH_BP},
};

void clear_music_break_point(u8 dindex)
{
    u32 ret;
    dp_buff tmp_dp;

    vm_read(playpoint_table[dindex][1], \
            (u8 *)&tmp_dp, \
            sizeof(tmp_dp));
    if (0 != check_dp(&tmp_dp)) {
        log_info("clear dp!\n");
        clear_dp(&tmp_dp);
        vm_write(playpoint_table[dindex][1], \
                 (u8 *)&tmp_dp, \
                 sizeof(tmp_dp));
    }
}

bool save_music_break_point(u8 dindex, u8 mode)
{
    u32 res = 0;
    if (0 == mode) {
        log_info("Save idx\n");
        res = vm_write(playpoint_table[dindex][0], \
                       (u8 *)(&pctl[0].findex), \
                       sizeof(pctl[0].findex));
        /* log_info("vm_write:%d %d", playpoint_table[dindex][0], sizeof(pctl[0].findex)); */
        /* log_info_hexdump((u8 *)&pctl[0].findex, sizeof(pctl[0].findex)); */
        if (res != sizeof(pctl[0].findex)) {
            log_info("Save idx fail\n");
            return false;
        }
    } else {
        log_info("Save bp\n");
        res = vm_write(playpoint_table[dindex][1], \
                       (u8 *)(pctl[0].pdp), \
                       sizeof(dp_buff));
        if (res != sizeof(dp_buff)) {
            log_info("Save bp fail\n");
            return false;
        }
        /* log_info_hexdump((u8 *)(pctl[0].pdp), sizeof(dp_buff)); */
    }
    return true;
}

bool load_music_break_point(u8 dindex, u8 mode)
{
    u32 res;
    if (0 == mode) {
        log_info("Load idx\n");
        u32 findex = 0;
        res = vm_read(playpoint_table[dindex][0], \
                      (u8 *)(&findex), \
                      sizeof(findex));
        /* log_info("vm_read:%d %d\n", playpoint_table[dindex][0], sizeof(findex)); */
        /* log_info_hexdump((u8 *)&findex, sizeof(findex)); */
        if (res != sizeof(findex)) {
            log_info("Load idx fail\n");
            return false;
        } else {
            pctl[0].findex = findex;
        }
    } else {
        log_info("Load bp\n");
        dp_buff dp = {0};
        res = vm_read(playpoint_table[dindex][1], \
                      (u8 *)(&dp), \
                      sizeof(dp_buff));
        if (res != sizeof(dp_buff)) {
            log_info("Load bp fail\n");
            return false;
        } else {
            memcpy((void *)pctl[0].pdp, (void *)&dp, sizeof(dp_buff));
            /* log_info_hexdump((u8 *)(pctl[0].pdp), sizeof(dp_buff)); */
        }
    }

    return true;
}
#endif
