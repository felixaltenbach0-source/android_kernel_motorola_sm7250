#!/usr/bin/bash
export ANDROID_NDK=/opt/android-ndk-r16b
export NDK_HOME=$ANDROID_NDK
export PATH=$ANDROID_NDK/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64/bin:$PATH

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
MODULE_DIRTREE="modules/system_root"

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
make -j${JOBS} O=out ARCH=arm64 Image.gz
make -j${JOBS} O=out ARCH=arm64 modules
make headers_install \
     O=out \
     ARCH=arm64 \
     INSTALL_HDR_PATH=$PWD/out/kernel-headers
make -j${JOBS} O=out ARCH=arm64 dtbo

echo "=== Installing modules ==="
make O=out ARCH=arm64 INSTALL_MOD_PATH=$PWD/out/modules_install modules_install

# modules_install creates lib/modules/<ver>/build and .../source symlinks that
# point back into the kernel source tree. `zip` follows symlinks by default, so
# leaving them in place makes zip recurse nethunter/ -> build/source ->
# nethunter/ -> ... forever, hanging the machine. Remove them now so neither
# this script's zip nor the CI `zip -r out.zip out` step can recurse.
find out/modules_install/lib/modules -maxdepth 2 -type l \( -name build -o -name source \) -delete

echo "=== Packaging NetHunter kernel zip ==="
rm -rf "${NHKERNEL_DIR}"
mkdir -p "${NHKERNEL_DIR}"
mkdir -p "${UPLOAD_DIR}"

cp out/arch/arm64/boot/Image.gz "${NHKERNEL_DIR}/"
cp out/arch/arm64/boot/dtbo.img "${NHKERNEL_DIR}/"

if [ -d out/modules_install/lib ]; then
    mkdir -p "${NHKERNEL_DIR}/${MODULE_DIRTREE}"
    cp -r out/modules_install/lib "${NHKERNEL_DIR}/${MODULE_DIRTREE}/"
fi

cd "${NHKERNEL_DIR}"
zip -r "${UPLOAD_DIR}/${NH_ARCHIVE}" *
cd "${KERNELDIR}"

rm -rf "${NHKERNEL_DIR}"

echo "=== Done ==="
echo "Output: ${UPLOAD_DIR}/${NH_ARCHIVE}"
