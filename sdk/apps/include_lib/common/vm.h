#ifndef _VM_H_
#define _VM_H_

#include "ioctl.h"
#include "dev_mg/device.h"

//#define IOCTL_SET_VM_INFO               _IOW('V', 1, 1)
//#define IOCTL_GET_VM_INFO               _IOW('V', 2, 1)
// enum {
//     #<{(|
//      *  用户自定义配置项 (0-64)
//      |)}>#
//     #<{(| ... |)}>#
// }

// VM define and api
typedef u16 vm_hdl;

struct vm_table {
    u16  index;
    u16 value_byte;
    int value;      //cache value which value_byte <= 4
};

typedef enum _vm_err {
    VM_ERR_NONE = 0,
    VM_INDEX_ERR = -0x100,
    VM_INDEX_EXIST,     //0xFF
    VM_DATA_LEN_ERR,    //0xFE
    VM_READ_NO_INDEX,   //0xFD
    VM_READ_DATA_ERR,   //0xFC
    VM_WRITE_OVERFLOW,  //0xFB
    VM_NOT_INIT,
    VM_INIT_ALREADY,
    VM_DEFRAG_ERR,
    VM_ERR_INIT,
    VM_ERR_PROTECT
} VM_ERR;

typedef union {
    u8 buff[20];//reserve;
    u8 index_song[68];//VM_INDEX_SONG 数据长度
    u8 index_eng[68];//VM_INDEX_ENG 数据长度
    u8 index_poetry[68];//VM_INDEX_POETRY 数据长度
    u8 index_story[68];//VM_INDEX_STORY 数据长度
    u8 index_ext_song[68];//VM_INDEX_EXT_SONG 数据长度
    u8 index_vol[1];
} VM_INDEX_BUFF;

typedef enum _FLASH_ERASER {
    CHIP_ERASER = 0,
    BLOCK_ERASER,
    SECTOR_ERASER,
    PAGE_ERASER,
} FLASH_ERASER;

typedef u32(*flash_code_protect_cb_t)(u32 offset, u32 len);
u32 flash_code_protect_callback(u32 offset, u32 len);
extern volatile u8 vm_busy;
//
void spi_cache_way_switch(u8 way_num);

// vm api
VM_ERR vm_eraser(void);
VM_ERR vm_init(void *dev_hdl, u32 vm_addr, u32 vm_len, u8 vm_mode);
void vm_check_all(u8 level);    //level : default 0
void vm_defrag_line_set(u8 defrag_line);//设置vm空间整理的阀值,按照百分比计算，取值范围0~100
int vm_get_area_using_info(u32 *vm_area_len, u32 *vm_used_len);
// io api
s32 vm_write_phy(vm_hdl hdl, u8 *data_buf, u16 len);
s32 vm_read_phy(vm_hdl hdl, u8 *data_buf, u16 len);

bool sfc_erase(FLASH_ERASER cmd, u32 addr);
u32 sfc_write(const u8 *buf, u32 addr, u32 len);
u32 sfc_read(u8 *buf, u32 addr, u32 len);


void spi_port_hd(u8 level);

bool sfc_erase_zone(u32 addr, u32 len);

void vm_api_write_mult(u16 start_id, u16 end_id, void *buf, u16 len, u32 delay);
int vm_api_read_mult(u16 start_id, u16 end_id, void *buf, u16 len);
VM_ERR syscfg_vm_init_phy(u32 eeprom_saddr, u32 eeprom_size);
u16 vm_buff_alloc(u8 **buf);

// vm擦写时可放出多个中断
void vm_isr_response_index_register(u8 index);
void vm_isr_response_index_unregister(u8 index);
u32 get_vm_isr_response_index_h(void);//获取放出中断的高32位(index 32-63)
u32 get_vm_isr_response_index_l(void);//获取放出中断的低32位(index 0-31)


#endif  //_VM_H_

