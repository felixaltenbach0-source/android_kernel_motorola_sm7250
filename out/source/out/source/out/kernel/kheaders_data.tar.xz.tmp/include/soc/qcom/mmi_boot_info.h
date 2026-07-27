

#ifndef __ASMARM_BOOTINFO_H
#define __ASMARM_BOOTINFO_H



#define PU_REASON_USB_CABLE		0x00000010 
#define PU_REASON_FACTORY_CABLE		0x00000020 
#define PU_REASON_PWR_KEY_PRESS		0x00000080 
#define PU_REASON_CHARGER		0x00000100 
#define PU_REASON_POWER_CUT		0x00000200 
#define PU_REASON_SW_AP_RESET		0x00004000 
#define PU_REASON_WDOG_AP_RESET		0x00008000 
#define PU_REASON_AP_KERNEL_PANIC	0x00020000 
#define PU_REASON_MODEM_RESET		0x80000000 
#define PU_REASON_INVALID		0xFFFFFFFF


#define BATTERY_GOOD_VOLTAGE    1
#define BATTERY_LO_VOLTAGE      2
#define BATTERY_UNKNOWN         (-1)


#define BOOTINFO_MAX_NAME_LEN    32
#define BOOTINFO_MAX_VAL_LEN    128
#define BOOTINFO_BUF_SIZE       (BOOTINFO_MAX_NAME_LEN + \
					3 + BOOTINFO_MAX_VAL_LEN)

u32  bi_powerup_reason(void);
const char *bi_bootreason(void);
u32  bi_mbm_version(void);
const char *bi_bootmode(void);
u32 bi_boot_seq(void);
u32 bi_hwrev(void);

#endif
