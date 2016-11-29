TARGET_HAS_LEGACY_CAMERA_HAL1 := true

PRODUCT_PROPERTY_OVERRIDES += \
    media.stagefright.legacyencoder=true \
    media.stagefright.less-secure=true \
    persist.camera.cpp.duplication=false \
    ro.qc.sdk.camera.facialproc=false \
    ro.qc.sdk.gestures.camera=false
