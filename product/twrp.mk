# TWRP
ifeq ($(WITH_TWRP),true)
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/etc/twrp.fstab:recovery/root/etc/twrp.fstab
endif
