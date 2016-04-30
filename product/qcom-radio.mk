PRODUCT_PROPERTY_OVERRIDES += \
    rild.libpath=/vendor/lib64/libril-qc-qmi-1.so \
    rild.libargs=-d /dev/smd0 \
    ril.subscription.types=NV,RUIM

PRODUCT_PROPERTY_OVERRIDES += \
    persist.cne.feature=1 \
    persist.data.mode=concurrent \
    persist.data.netmgrd.qos.enable=true \
    persist.data.tcp_rst_drop=true \
    persist.env.fastdorm.enabled=true \
    persist.radio.apm_sim_not_pwdn=1 \
    persist.radio.app_hw_mbn_path=/firmware/image/modem_pr/mcfg/configs/mcfg_hw/generic/common/MSM8994/LA \
    persist.radio.custom_ecc=1 \
    persist.radio.sib16_support=1 \
    ro.use_data_netmgrd=true \
    ro.data.large_tcp_window_size=true \
    ro.telephony.default_network=9 \
    telephony.lteOnCdmaDevice=1

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.telephony.cdma.xml:system/etc/permissions/android.hardware.telephony.cdma.xml \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml

