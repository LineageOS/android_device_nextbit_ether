PRODUCT_PACKAGES += \
    a4wpservice \
    android.wipower \
    android.wipower.xml \
    com.quicinc.wbcserviceapp \
    libwipower_jni \
    wipowerservice

PRODUCT_PROPERTY_OVERRIDES += \
    ro.bluetooth.emb_wp_mode=true \
    ro.bluetooth.wipower=true
