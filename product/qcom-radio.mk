PRODUCT_PROPERTY_OVERRIDES += \
    rild.libpath=/vendor/lib64/libril-qc-qmi-1.so \
    ril.subscription.types=NV,RUIM

PRODUCT_PROPERTY_OVERRIDES += \
    persist.data.mode=concurrent \
    persist.data.netmgrd.qos.enable=true \
    persist.data.tcp_rst_drop=true \
    persist.radio.apm_sim_not_pwdn=1 \
    persist.radio.custom_ecc=1 \
    persist.radio.sib16_support=1 \
    persist.radio.VT_CAM_INTERFACE=1 \
    ro.data.large_tcp_window_size=true \
    ro.use_data_netmgrd=true \
    ro.telephony.default_network=9

PRODUCT_PROPERTY_OVERRIDES += \
    log.tag.RILQ=WARN

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml
