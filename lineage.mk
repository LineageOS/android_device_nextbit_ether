# Boot animation
TARGET_SCREEN_HEIGHT := 1080
TARGET_SCREEN_WIDTH := 1920

# Inherit 64-bit configs
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Inherit device configuration
$(call inherit-product, device/nextbit/ether/device.mk)

## Device identifier. This must come after all inclusions
PRODUCT_NAME := lineage_ether
BOARD_VENDOR := nextbit
TARGET_VENDOR := nextbit
PRODUCT_DEVICE := ether

PRODUCT_BRAND := Nextbit
PRODUCT_MODEL := Robin
PRODUCT_MANUFACTURER := Nextbit

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="ether-user 7.1.1 Robin_Nougat_108 00WW_Jenkins_108 release-keys"

BUILD_FINGERPRINT := Nextbit/ether/ether:7.1.1/Robin_Nougat_108/00WW_Jenkins_108:user/release-keys
