BOARD_USES_QCOM_HARDWARE := true

BOARD_USES_QC_TIME_SERVICES := true

# Init
TARGET_INIT_VENDOR_LIB := libinit_ether
TARGET_RECOVERY_DEVICE_MODULES := libinit_ether
TARGET_PLATFORM_DEVICE_BASE := /devices/soc.0/

# Added to indicate that protobuf-c is supported in this build
PROTOBUF_SUPPORTED := true

# Enable peripheral manager
TARGET_PER_MGR_ENABLED := true

TARGET_HW_DISK_ENCRYPTION := true
TARGET_KEYMASTER_WAIT_FOR_QSEE := true
