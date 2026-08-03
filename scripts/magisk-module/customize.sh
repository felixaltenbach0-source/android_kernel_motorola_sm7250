#!/system/bin/sh
# NetHunter kernel installer for Motorola nairo (sm7250/lito).
#
# This is a Magisk module (installable from Magisk Manager or TWRP) that
# flashes the bundled kernel at install time: it unpacks the CURRENT boot
# partition with magiskboot, swaps in the bundled uncompressed `Image`
# (BOARD_KERNEL_IMAGE_NAME=Image on lineage-22.2), repacks, and writes the
# result back. Because the ramdisk is left untouched, the Magisk patch in it
# is preserved. It also flashes the bundled dtbo.img to the dtbo partition.
#
# Runs as root under Magisk's module installer; util_functions.sh has already
# been sourced (ui_print/abort/BOOTMODE/MODPATH are available).

ui_print " "
ui_print "NetHunter Kernel (nairo) - installing"

# Locate a partition block device. A/B devices (Current boot slot: _a/_b)
# name their partitions boot_a/boot_b, so honour SLOT (set by
# util_functions.sh's mount_partitions) when present, then fall back to the
# plain name, and finally do a recursive scan of /dev/block (like Magisk's own
# find_block) for OEM layouts that don't use by-name symlinks.
find_block() {
    local part="$1" n d found slot
    slot="$SLOT"
    if [ -z "$slot" ]; then
        slot=$(grep_cmdline androidboot.slot_suffix 2>/dev/null)
        [ -n "$slot" ] || slot=$(grep_cmdline androidboot.slot 2>/dev/null)
        [ -n "$slot" ] && [ "${slot#_}" = "$slot" ] && slot="_${slot}"
    fi
    for n in "${part}${slot}" "$part"; do
        for d in /dev/block/bootdevice/by-name /dev/block/mapper; do
            [ -e "$d/$n" ] && { readlink -f "$d/$n"; return 0; }
        done
    done
    found=$(find /dev/block \( -type b -o -type c -o -type l \) \( -iname "$part" -o -iname "${part}${slot}" \) 2>/dev/null | head -n1)
    [ -n "$found" ] && { readlink -f "$found"; return 0; }
    return 1
}

BOOTBLOCK=$(find_block boot) || abort "! boot partition not found"
DTBOBLOCK=$(find_block dtbo) || DTBOBLOCK=

# Prefer the device's own magiskboot (correct architecture and always present
# when Magisk is installed); fall back to the bundled ARM binary.
MB=/data/adb/magisk/magiskboot
[ -x "$MB" ] || MB="$MODPATH/magiskboot"
chmod 755 "$MB" 2>/dev/null
[ -x "$MB" ] || abort "! magiskboot not found"

[ -f "$MODPATH/Image" ] || abort "! kernel Image missing from module"

ui_print "- Repacking boot image ($BOOTBLOCK) with new kernel..."
if ! (
    cd "$MODPATH" && \
    "$MB" --unpack "$BOOTBLOCK" >&2 && \
    cp -f "$MODPATH/Image" kernel && \
    "$MB" --repack "$BOOTBLOCK" >&2 && \
    [ -f new-boot.img ]
); then
    abort "! failed to repack boot image"
fi

ui_print "- Writing new boot image..."
if ! cat "$MODPATH/new-boot.img" > "$BOOTBLOCK"; then
    abort "! failed to flash boot image"
fi
sync
rm -f "$MODPATH/new-boot.img"
rm -f "$MODPATH/kernel" "$MODPATH/ramdisk.cpio" "$MODPATH/ramdisk.cpio.lz4" \
      "$MODPATH/header" "$MODPATH/dtb" "$MODPATH/extra" "$MODPATH/second" 2>/dev/null
ui_print "- Kernel installed"

if [ -n "$DTBOBLOCK" ] && [ -f "$MODPATH/dtbo.img" ]; then
    ui_print "- Flashing dtbo.img to $DTBOBLOCK..."
    if cat "$MODPATH/dtbo.img" > "$DTBOBLOCK"; then
        sync
        ui_print "- dtbo.img installed"
    else
        ui_print "! failed to flash dtbo.img (kernel was already flashed)"
    fi
fi

ui_print " "
ui_print "Done. Reboot to apply the new kernel."
ui_print " "
