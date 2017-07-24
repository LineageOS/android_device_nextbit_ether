ifeq ($(WITH_TWRP),true)
TARGET_RECOVERY_DEVICE_MODULES += init.recovery.qcom.rc
TW_INCLUDE_CRYPTO := true
endif
