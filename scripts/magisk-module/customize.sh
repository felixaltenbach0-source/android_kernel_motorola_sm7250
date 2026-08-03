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

find_block() {
    local part="$1" d
    [ -e "/dev/block/bootdevice/by-name/$part" ] && { echo "/dev/block/bootdevice/by-name/$part"; return 0; }
    [ -e "/dev/block/mapper/$part" ] && { echo "/dev/block/mapper/$part"; return 0; }
    for d in /dev/block/platform/*/*/by-name /dev/block/platform/*/by-name; do
        [ -e "$d/$part" ] && { echo "$d/$part"; return 0; }
    done
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
