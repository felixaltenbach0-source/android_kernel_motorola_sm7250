#!/usr/bin/bash
export ANDROID_NDK=/opt/android-ndk-r16b
export NDK_HOME=$ANDROID_NDK
export PATH=$ANDROID_NDK/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64/bin:$PATH

# merge_config.sh runs `make alldefconfig` internally without an explicit ARCH
# (it inherits the environment), so ARCH MUST be exported here. Otherwise it
# configures for the build host (x86_64 on CI) and silently drops every
# ARM64-only symbol (ARCH_QCOM, MMC_SDHCI_MSM, MSM_BT_POWER, SCSI_UFS_QCOM,
# QCOM_QMI_HELPERS, ...), which later fails the vmlinux link with undefined
# qcom_ice_*/sdhci_msm_pm_qos_*/qmi_* symbols.
export ARCH=arm64
export CROSS_COMPILE='aarch64-linux-android21-'
export HOSTLD=ld
export HOSTCC='clang'
export CC='clang'
export CPP='clang++'
export CXX='clang++'
export LD=ld

export LLVM=1
export LLVM_IAS=1
# NetHunter zip settings
KERNELDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
NHKERNEL_DIR="${KERNELDIR}/nethunter"
UPLOAD_DIR="${KERNELDIR}/output"
NH_ARCHIVE="nethunter-kernel-NetHunter.zip"

JOBS="$(nproc)"

echo "=== Cleaning build tree ==="
make -j${JOBS} O=out ARCH=arm64 mrproper

echo "=== Configuring kernel ==="
make O=out ARCH=arm64 vendor/nairo_defconfig
./scripts/kconfig/merge_config.sh -O out out/.config arch/arm64/configs/vendor/nairo_defconfig arch/arm64/configs/vendor/ext_config/nethunter-nairo.config arch/arm64/configs/vendor/ext_config/debug-lito-nairo.config
make O=out ARCH=arm64 olddefconfig

echo "=== Preparing build ==="
make O=out ARCH=arm64 prepare
make O=out ARCH=arm64 modules_prepare

echo "=== Building kernel ==="
make -j${JOBS} O=out ARCH=arm64 Image.xz
make -j${JOBS} O=out ARCH=arm64 modules
make headers_install \
     O=out \
     ARCH=arm64 \
     INSTALL_HDR_PATH=$PWD/out/kernel-headers

# There is no `dtbo` make target in this tree (that's a QTI build-system
# leftover, and `cmd_mkdtimg` in scripts/Makefile.lib is never wired up), so
# `make dtbo` fails silently and dtbo.img is never produced. Build the .dtbo
# overlays via `dtbs` (they are also pulled in by `all:`) and pack them into
# the Android DTBO table image with scripts/mkdtboimg.py.
echo "=== Building dtbo.img ==="
make -j${JOBS} O=out ARCH=arm64 dtbs
DTBO_OBJS=$(find out/arch/arm64/boot/dts -name '*.dtbo' | sort)
if [ -n "${DTBO_OBJS}" ]; then
    python3 scripts/mkdtboimg.py create out/arch/arm64/boot/dtbo.img --page_size=4096 ${DTBO_OBJS}
    echo "[+] dtbo.img: $(stat -c %s out/arch/arm64/boot/dtbo.img 2>/dev/null) bytes ($(echo "${DTBO_OBJS}" | wc -w) overlays)"
else
    echo "[-] ERROR: no .dtbo overlays were built; dtbo.img will be missing" >&2
fi

# Build boot.img (Android boot image header v2) from the uncompressed `Image`
# plus the concatenated base DTBs plus the kernel's built-in initramfs. The
# parameters mirror device/motorola/sm7250-common/BoardConfigCommon.mk on
# lineage-22.2: BOARD_BOOT_HEADER_VERSION=2, BOARD_INCLUDE_DTB_IN_BOOTIMG=true,
# BOARD_KERNEL_BASE=0x00000000, BOARD_KERNEL_PAGESIZE=4096,
# BOARD_KERNEL_IMAGE_NAME=Image, BOARD_KERNEL_SEPARATED_DTBO=true (dtbo.img is
# flashed to the separate dtbo partition), and the BOARD_KERNEL_CMDLINE below.
# The real device ramdisk (from a stock/LineageOS boot.img) is not available in
# this kernel-only tree, so the kernel's own initramfs is used instead. The
# result is a structurally valid boot.img that must be repacked with the device
# ramdisk (e.g. magiskboot unpack/repack) before flashing.
echo "=== Building boot.img ==="
make -j${JOBS} O=out ARCH=arm64 Image
DTB_IMG="out/arch/arm64/boot/dtb.img"
DTB_FILES=$(find out/arch/arm64/boot/dts -name '*.dtb' | sort)
if [ -z "${DTB_FILES}" ]; then
    echo "[-] ERROR: no base .dtb files built; boot.img will be missing" >&2
else
    cat ${DTB_FILES} > "${DTB_IMG}"
    RAMDISK=""
    INITRAMFS="$(ls out/usr/initramfs_data.cpio* 2>/dev/null | head -n1)"
    [ -n "${INITRAMFS}" ] && RAMDISK="--ramdisk ${INITRAMFS}"
    python3 scripts/mkbootimg.py \
        --kernel out/arch/arm64/boot/Image \
        ${RAMDISK} \
        --dtb "${DTB_IMG}" \
        --base 0x00000000 \
        --pagesize 4096 \
        --header_version 2 \
        --os_version 15.0.0 \
        --os_patch_level "$(date +%Y-%m)" \
        --cmdline "androidboot.console=ttyMSM0 androidboot.hardware=qcom androidboot.memcg=1 androidboot.usbcontroller=a600000.dwc3 cgroup.memory=nokmem,nosocket loop.max_part=7 lpm_levels.sleep_disabled=1 service_locator.enable=1 swiotlb=2048" \
        -o out/arch/arm64/boot/boot.img
    echo "[+] boot.img: $(stat -c %s out/arch/arm64/boot/boot.img 2>/dev/null) bytes (header v2, $(echo "${DTB_FILES}" | wc -w) dtb files)"
fi

echo "=== Installing modules ==="
make O=out ARCH=arm64 INSTALL_MOD_PATH=$PWD/out/modules_install modules_install

# modules_install creates lib/modules/<ver>/build and .../source symlinks that
# point back into the kernel source tree. `zip` follows symlinks by default, so
# leaving them in place makes zip recurse nethunter/ -> build/source ->
# nethunter/ -> ... forever, hanging the machine. Remove them now so neither
# this script's zip nor the CI `zip -r out.zip out` step can recurse.
find out/modules_install/lib/modules -maxdepth 2 -type l \( -name build -o -name source \) -delete

# Build a Debian linux-headers-<ver> package that installs the kernel build
# headers to /usr/src/linux-headers-<ver> (plus /lib/modules/<ver>/{build,source}
# symlinks) so DKMS in an arm64 Kali chroot can compile out-of-tree modules.
# The package intentionally has no Depends on any linux-image package.
build_headers_deb() {
    local KVER PKG STAGING DEST SRCTREE OBJTREE
    KVER="$(cat out/include/config/kernel.release 2>/dev/null)"
    [ -n "${KVER}" ] || KVER="$(make -s O=out ARCH=arm64 kernelrelease)"
    PKG="linux-headers-${KVER}"
    STAGING="$(mktemp -d)"
    SRCTREE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    OBJTREE="${SRCTREE}/out"
    DEST="${STAGING}/usr/src/linux-headers-${KVER}"

    echo "=== Building ${PKG} deb (DKMS headers) ==="
    mkdir -p "${DEST}" "${STAGING}/DEBIAN" "${STAGING}/lib/modules/${KVER}"
    mkdir -p "${UPLOAD_DIR}"

    # Source files needed to build out-of-tree modules. Same file set as the
    # upstream kernel's scripts/package/builddeb linux-headers package. Use
    # `git ls-files` so build/tooling artifacts in the tree can't leak in.
    # scripts/anykernel is excluded: it's the (prebuilt, device-side) AnyKernel3
    # installer, not needed to build modules.
    ( cd "${SRCTREE}" && git ls-files \
        ':(glob)**/Makefile*' ':(glob)**/Kconfig*' ':(glob)**/*.pl' \
        ':(glob)arch/*/include/**' ':(glob)include/**' ':(glob)scripts/**' \
        ':(glob)arch/arm64/module.lds' ':(glob)arch/arm64/Kbuild.platforms' \
        ':(glob)arch/arm64/Platform' ':(glob)arch/arm64/include/**' \
        ':(glob)arch/arm64/scripts/**' ':(exclude)scripts/anykernel/**' ) \
      | sort -u | tar --no-recursion -cf - -T - | ( cd "${DEST}" && tar -xf - )

    # Generated build artifacts from the O= build tree (scripts binaries,
    # generated headers, Module.symvers). Copied second so they win over the
    # plain source copies above. `find` emits paths relative to OBJTREE, so tar
    # must extract them with `-C OBJTREE` (it writes them straight into DEST
    # relative to the current dir at the other end of the pipe).
    ( cd "${OBJTREE}" && find arch/arm64/include Module.symvers include scripts -type f ) \
      | tar -C "${OBJTREE}" --no-recursion -cf - -T - | ( cd "${DEST}" && tar -xf - )

    cp "${OBJTREE}/.config" "${DEST}/.config"
    cp "${OBJTREE}/Module.symvers" "${DEST}/Module.symvers"

    # DKMS/build symlinks under /lib/modules/<ver>
    ln -s "/usr/src/linux-headers-${KVER}" "${STAGING}/lib/modules/${KVER}/build"
    ln -s "/usr/src/linux-headers-${KVER}" "${STAGING}/lib/modules/${KVER}/source"

    cat > "${STAGING}/DEBIAN/control" <<EOF
Package: ${PKG}
Version: ${KVER}-1
Section: kernel
Priority: optional
Architecture: arm64
Maintainer: NetHunter Kernel Build <nethunter@kalilinux.invalid>
Description: Linux kernel headers for ${KVER}
 Kernel build headers for version ${KVER} of the NetHunter kernel.
 Needed to compile out-of-tree kernel modules (e.g. with DKMS) in an
 arm64 Kali chroot.
EOF

    dpkg-deb --build --root-owner-group "${STAGING}" "${UPLOAD_DIR}/${PKG}-arm64.deb" >/dev/null
    echo "[+] $(ls -lh "${UPLOAD_DIR}/${PKG}-arm64.deb" | awk '{print $5}') ${PKG}-arm64.deb"
    rm -rf "${STAGING}"
}

build_headers_deb

# Package the kernel as an AnyKernel3 zip: flashable in TWRP OR installable in
# Magisk Manager (it preserves the device ramdisk + Magisk patch and repacks the
# current boot partition with the new `Image`). The .ko modules are shipped in
# modules/system/lib/modules so do.modules=1 + do.systemless=1 turn them into
# the "ak3-helper" Magisk module that overlays /system/lib/modules.
echo "=== Packaging NetHunter kernel zip (AnyKernel3) ==="
rm -rf "${NHKERNEL_DIR}"
mkdir -p "${NHKERNEL_DIR}"
mkdir -p "${UPLOAD_DIR}"

cp -r scripts/anykernel/META-INF "${NHKERNEL_DIR}/"
cp -r scripts/anykernel/tools "${NHKERNEL_DIR}/"
cp scripts/anykernel/anykernel.sh "${NHKERNEL_DIR}/anykernel.sh"
cp scripts/anykernel/LICENSE "${NHKERNEL_DIR}/"

# Kernel image for AnyKernel3 to install: uncompressed `Image` (the device's
# BOARD_KERNEL_IMAGE_NAME). Image.xz/dtbo.img/boot.img are carried along as
# reference artifacts.
cp out/arch/arm64/boot/Image "${NHKERNEL_DIR}/Image"
cp out/arch/arm64/boot/Image.xz "${NHKERNEL_DIR}/"
cp out/arch/arm64/boot/dtbo.img "${NHKERNEL_DIR}/"
[ -f out/arch/arm64/boot/boot.img ] && cp out/arch/arm64/boot/boot.img "${NHKERNEL_DIR}/"

if [ -d out/modules_install/lib/modules ]; then
    KVER="$(cat out/include/config/kernel.release)"
    mkdir -p "${NHKERNEL_DIR}/modules/system/lib/modules/${KVER}"
    cp -r out/modules_install/lib/modules/${KVER}/. "${NHKERNEL_DIR}/modules/system/lib/modules/${KVER}/"
fi

cd "${NHKERNEL_DIR}"
zip -r "${UPLOAD_DIR}/${NH_ARCHIVE}" *
cd "${KERNELDIR}"

rm -rf "${NHKERNEL_DIR}"

# Build the Magisk-module variant of the kernel: a proper Magisk module
# (module.prop + Magisk's module installer) so it installs from Magisk Manager
# as well as TWRP. At install time customize.sh repacks the CURRENT boot
# partition with the bundled uncompressed `Image` via magiskboot, preserving
# the device ramdisk (i.e. the Magisk patch), and flashes the bundled dtbo.img.
# Kernel modules are shipped under system/lib/modules for systemless loading.
build_magisk_module() {
    local MODDIR KVER
    KVER="$(cat out/include/config/kernel.release 2>/dev/null)"
    [ -n "${KVER}" ] || KVER="$(make -s O=out ARCH=arm64 kernelrelease)"
    MODDIR="$(mktemp -d)"

    echo "=== Packaging Magisk module zip (nethunter-kernel-nairo-magisk.zip) ==="
    mkdir -p "${MODDIR}/META-INF/com/google/android" "${MODDIR}/system/lib/modules/${KVER}"

    sed -e "s/@VERSION@/4.19-$(date +%Y%m%d%H%M%S)/" \
        -e "s/@VERSIONCODE@/$(date +%Y%m%d)/" \
        scripts/magisk-module/module.prop.in > "${MODDIR}/module.prop"
    cp scripts/magisk-module/update-binary "${MODDIR}/META-INF/com/google/android/update-binary"
    cp scripts/magisk-module/updater-script "${MODDIR}/META-INF/com/google/android/updater-script"
    cp scripts/magisk-module/customize.sh "${MODDIR}/customize.sh"
    cp scripts/anykernel/tools/magiskboot "${MODDIR}/magiskboot"
    cp out/arch/arm64/boot/Image "${MODDIR}/Image"
    [ -f out/arch/arm64/boot/dtbo.img ] && cp out/arch/arm64/boot/dtbo.img "${MODDIR}/dtbo.img"
    if [ -d "out/modules_install/lib/modules/${KVER}" ]; then
        cp -r "out/modules_install/lib/modules/${KVER}/." "${MODDIR}/system/lib/modules/${KVER}/"
    fi

    ( cd "${MODDIR}" && zip -rq "${UPLOAD_DIR}/nethunter-kernel-nairo-magisk.zip" . )
    echo "[+] nethunter-kernel-nairo-magisk.zip: $(stat -c %s "${UPLOAD_DIR}/nethunter-kernel-nairo-magisk.zip" 2>/dev/null) bytes"
    rm -rf "${MODDIR}"
}

build_magisk_module

echo "=== Done ==="
echo "Output: ${UPLOAD_DIR}/${NH_ARCHIVE}"
echo "        ${UPLOAD_DIR}/nethunter-kernel-nairo-magisk.zip"
