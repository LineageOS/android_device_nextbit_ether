BOARD_KERNEL_BASE := 0x00000000
BOARD_KERNEL_PAGESIZE :=  4096
BOARD_KERNEL_TAGS_OFFSET := 0x00000100
BOARD_RAMDISK_OFFSET     := 0x01000000

TARGET_KERNEL_SOURCE := kernel/nextbit/msm8992
TARGET_KERNEL_CONFIG := lineageos_ether_defconfig
TARGET_KERNEL_ARCH := arm64
BOARD_KERNEL_CMDLINE := console=ttyHSL0,115200,n8 androidboot.hardware=qcom user_debug=31 msm_rtb.filter=0x37 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 boot_cpus=0-5
BOARD_KERNEL_IMAGE_NAME := Image.gz-dtb

ENABLE_CPUSETS := true
ENABLE_SCHED_BOOST := true

TARGET_USES_64_BIT_BINDER := true
