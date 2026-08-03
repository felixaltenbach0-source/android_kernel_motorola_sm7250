### AnyKernel3 Ramdisk Mod Script
## osm0sis @ xda-developers
##
## NetHunter kernel installer for Motorola nairo (sm7250/lito, A-only).
## Flashable in TWRP or Magisk Manager. The kernel in this zip is the
## uncompressed `Image` (BOARD_KERNEL_IMAGE_NAME=Image on lineage-22.2).
## Boot parameters mirror device/motorola/sm7250-common/BoardConfigCommon.mk:
## header v2, BOARD_INCLUDE_DTB_IN_BOOTIMG=true, separated dtbo partition.

### AnyKernel setup
# global properties
properties() { '
kernel.string=NetHunter Kernel for Motorola nairo (sm7250/lito)
do.devicecheck=0
do.modules=1
do.systemless=1
do.cleanup=1
do.cleanuponabort=0
device.name1=nairo
supported.versions=
supported.patchlevels=
supported.vendorpatchlevels=
'; } # end properties


### AnyKernel install
## boot files attributes
boot_attributes() {
set_perm_recursive 0 0 755 644 $RAMDISK/*;
set_perm_recursive 0 0 750 750 $RAMDISK/init* $RAMDISK/sbin;
} # end attributes

# boot shell variables
BLOCK=/dev/block/bootdevice/by-name/boot;
IS_SLOT_DEVICE=0;
RAMDISK_COMPRESSION=auto;
PATCH_VBMETA_FLAG=auto;

# import functions/variables and setup patching - see for reference (DO NOT REMOVE)
. tools/ak3-core.sh;

# boot install
dump_boot; # unpack the current boot image and preserve its ramdisk + dtb

write_boot; # repack with the new kernel (Magisk patch is retained)
## end boot install
