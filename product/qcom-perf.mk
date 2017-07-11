# property for vendor specific library
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.extension_library=libqti-perfd-client.so \
    ro.vendor.at_library=libqti-at.so \
    ro.vendor.gt_library=libqti-gt.so \
    sys.games.gt.prof=0 \
    ro.core_ctl_min_cpu=1 \
    ro.core_ctl_max_cpu=2
