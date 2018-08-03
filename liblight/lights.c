/*
 * Copyright (C) 2016 The CyanogenMod Project
 * Copyright (C) 2018 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "ether_lights"
#define LOG_NDEBUG 0
#include <cutils/log.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <hardware/lights.h>

#define WHITE_LED_FILE "/sys/class/leds/nbq_wled/brightness"
#define WHITE_BLINK_FILE "/sys/class/leds/nbq_wled/blink"
#define WHITE_DUTY_PCTS_FILE "/sys/class/leds/nbq_wled/duty_pcts"
#define WHITE_START_IDX_FILE "/sys/class/leds/nbq_wled/start_idx"
#define WHITE_PAUSE_LO_FILE "/sys/class/leds/nbq_wled/pause_lo"
#define WHITE_PAUSE_HI_FILE "/sys/class/leds/nbq_wled/pause_hi"
#define WHITE_RAMP_STEP_MS_FILE "/sys/class/leds/nbq_wled/ramp_step_ms"
#define LCD_FILE "/sys/class/leds/lcd-backlight/brightness"
#define SEGMENTED_LED_FILE "/sys/class/leds/lp5523:channel0/device/leds_on_off"

#define NUM_LED_SEGMENTS 4
#define SEGMENT_BRIGHTNESS 100

#define RAMP_SIZE 8
#define RAMP_STEP_DURATION 50

static const int BRIGHTNESS_RAMP[RAMP_SIZE]
        = { 0, 12, 25, 37, 50, 72, 85, 100 };

static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
static struct light_state_t g_attention;
static struct light_state_t g_notification;
static int g_battery_bars = -1;

static int write_int(const char *path, int value)
{
    int fd = open(path, O_WRONLY);

    if (fd >= 0) {
        char buffer[20];
        int bytes = sprintf(buffer, "%d\n", value);
        int amt = write(fd, buffer, bytes);
        close(fd);
        return amt == -1 ? -errno : 0;
    } else {
        ALOGE("write_int failed to open %s\n", path);
        return -errno;
    }
}

static int write_str(const char *path, char *value)
{
    int fd = open(path, O_WRONLY);

    if (fd >= 0) {
        char buffer[1024];
        int bytes = snprintf(buffer, sizeof(buffer), "%s\n", value);
        ssize_t amt = write(fd, buffer, (size_t)bytes);
        close(fd);
        return amt == -1 ? -errno : 0;
    } else {
        ALOGE("write_str failed to open %s\n", path);
        return -errno;
    }
}

static int is_lit(const struct light_state_t *state)
{
    return state->color & 0x00ffffff;
}

static int rgb_to_brightness(const struct light_state_t *state)
{
    return state->color & 0x000000ff;
}

static int set_light_battery(struct light_device_t *dev,
        const struct light_state_t *state)
{
    int brightness = 0;
    char buf[20];
    int err = 0;

    if (!dev)
        return -ENODEV;

    pthread_mutex_lock(&g_lock);

    // framework sends the level as the upper 8 bits of color
    int level = (state->color & 0xFF000000) >> 24;
    ALOGV("%s: color=%x level=%d", __func__, state->color, level);

    // sanity check
    if (level < 0)
        level = 0;
    else if (level > 100)
        level = 100;

    // level is a percentage, so find the number of bars which
    // should be lit, and light 'em up
    int bars = 0;
    if (is_lit(state)) {
        if (level > 90)
            bars = 4;
        else if (level > 75)
            bars = 3;
        else if (level > 50)
            bars = 2;
        else if (level > 25)
            bars = 1;
    }

    if (bars == g_battery_bars)
        goto out;

    for (int i = 1; i <= NUM_LED_SEGMENTS; i++) {
        brightness = (bars >= i) ? SEGMENT_BRIGHTNESS : 0;
        snprintf(buf, sizeof(buf), "%d %d", NUM_LED_SEGMENTS - (i - 1), brightness);
        ALOGV("%s: %d = %s (bars=%d)", __func__, i, buf, bars);
        err = write_str(SEGMENTED_LED_FILE, buf);
        if (err < 0) {
            ALOGD("%s failed to write LED segment %s err=%d", __func__, buf, err);
            break;
        }
    }
    g_battery_bars = bars;

out:
    pthread_mutex_unlock(&g_lock);

    return err;
}

static int set_light_backlight(struct light_device_t *dev,
        const struct light_state_t *state)
{
    int err = 0;
    int brightness = rgb_to_brightness(state);

    if (!dev)
        return -ENODEV;

    pthread_mutex_lock(&g_lock);

    err = write_int(LCD_FILE, brightness);

    pthread_mutex_unlock(&g_lock);

    return err;
}

static char *get_scaled_duty_pcts(int brightness)
{
    char *buf = calloc(RAMP_SIZE, 5 * sizeof(char));
    char *pad = "";
    int i = 0;

    if (!buf)
        return NULL;

    for (i = 0; i < RAMP_SIZE; i++) {
        char temp[5];
        snprintf(temp, sizeof(temp), "%s%d", pad, (BRIGHTNESS_RAMP[i] * brightness / 255));
        strcat(buf, temp);
        pad = ",";
    }
    ALOGV("%s: brightness=%d duty=%s", __func__, brightness, buf);

    return buf;
}

static int set_speaker_light_locked(struct light_device_t *dev,
        const struct light_state_t *state)
{
    int brightness, blink;
    int onMS, offMS, stepDuration, pauseHi;
    unsigned int colorRGB;
    char *duty;

    if (!dev)
        return -ENODEV;

    if (state == NULL) {
        write_int(WHITE_DUTY_PCTS_FILE, 0);
        write_int(WHITE_BLINK_FILE, 0);
        write_int(WHITE_LED_FILE, 0);
        return 0;
    }

    switch (state->flashMode) {
        case LIGHT_FLASH_TIMED:
            onMS = state->flashOnMS;
            offMS = state->flashOffMS;
            break;
        case LIGHT_FLASH_NONE:
        default:
            onMS = 0;
            offMS = 0;
            break;
    }

    ALOGV("set_speaker_light_locked mode %d, color=%08X, onMS=%d, offMS=%d\n",
            state->flashMode, state->color, onMS, offMS);

    // framework sends the brightness as the upper 8 bits of color
    brightness = (state->color & 0xFF000000) >> 24;
    blink = onMS > 0 && offMS > 0;

    if (blink) {
        stepDuration = RAMP_STEP_DURATION;
        pauseHi = onMS - (stepDuration * RAMP_SIZE * 2);
        if (stepDuration * RAMP_SIZE * 2 > onMS) {
            stepDuration = onMS / (RAMP_SIZE * 2);
            pauseHi = 0;
        }

        // white
        write_int(WHITE_START_IDX_FILE, 0);
        duty = get_scaled_duty_pcts(brightness);
        write_str(WHITE_DUTY_PCTS_FILE, duty ? duty : "0");
        write_int(WHITE_PAUSE_LO_FILE, offMS);
        // The led driver is configured to ramp up then ramp
        // down the lut. This effectively doubles the ramp duration.
        write_int(WHITE_PAUSE_HI_FILE, pauseHi);
        write_int(WHITE_RAMP_STEP_MS_FILE, stepDuration);
        free(duty);

        // start the party
        write_int(WHITE_BLINK_FILE, 1);
    } else {
        write_int(WHITE_LED_FILE, brightness);
    }

    return 0;
}

static void handle_speaker_light_locked(struct light_device_t *dev)
{
    set_speaker_light_locked(dev, NULL);
    if (is_lit(&g_attention)) {
        set_speaker_light_locked(dev, &g_attention);
    } else if (is_lit(&g_notification)) {
        set_speaker_light_locked(dev, &g_notification);
    }
}

static int set_light_notifications(struct light_device_t *dev,
        const struct light_state_t *state)
{
    pthread_mutex_lock(&g_lock);

    g_notification = *state;
    handle_speaker_light_locked(dev);

    pthread_mutex_unlock(&g_lock);

    return 0;
}

static int set_light_attention(struct light_device_t *dev,
        const struct light_state_t *state)
{
    pthread_mutex_lock(&g_lock);

    g_attention = *state;
    handle_speaker_light_locked(dev);

    pthread_mutex_unlock(&g_lock);

    return 0;
}

static int close_lights(struct hw_device_t *dev)
{
    if (dev)
        free(dev);

    return 0;
}

static int open_lights(const struct hw_module_t *module, const char *name,
        struct hw_device_t **device)
{
    int (*set_light)(struct light_device_t *dev,
            const struct light_state_t *state);

    if (!strcmp(LIGHT_ID_BACKLIGHT, name))
        set_light = set_light_backlight;
    else if (!strcmp(LIGHT_ID_BATTERY, name))
        set_light = set_light_battery;
    else if (!strcmp(LIGHT_ID_NOTIFICATIONS, name))
        set_light = set_light_notifications;
    else if (!strcmp(LIGHT_ID_ATTENTION, name))
        set_light = set_light_attention;
    else
        return -EINVAL;

    struct light_device_t *dev = calloc(1, sizeof(struct light_device_t));

    if (!dev)
        return -ENOMEM;

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*)module;
    dev->common.close = (int (*)(struct hw_device_t*))close_lights;
    dev->set_light = set_light;

    *device = (struct hw_device_t*)dev;

    return 0;
}

static struct hw_module_methods_t lights_module_methods = {
    .open =  open_lights,
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .module_api_version = LIGHTS_DEVICE_API_VERSION_1_0,
    .hal_api_version = HARDWARE_HAL_API_VERSION,
    .id = LIGHTS_HARDWARE_MODULE_ID,
    .name = "Ether Lights HAL",
    .author = "The LineageOS Project",
    .methods = &lights_module_methods,
};
