PRODUCT_PACKAGES += \
    Snap

TARGET_HAS_LEGACY_CAMERA_HAL1 := true

# Camera2 API
PRODUCT_PROPERTY_OVERRIDES += \
    media.camera.ts.monotonic=0 \
    persist.camera.HAL3.enabled=1

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.full.xml:system/etc/permissions/android.hardware.camera.full.xml \
    frameworks/native/data/etc/android.hardware.camera.raw.xml:system/etc/permissions/android.hardware.camera.raw.xml
