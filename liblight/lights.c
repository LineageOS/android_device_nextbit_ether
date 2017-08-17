/*
 * Copyright (C) 2016 The CyanogenMod Project
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


/*
 *
 * Lights HAL for the Nextbit Robin
 *
 * Supports the qpnp-leds PWM interface of the white notification
 * LED (with delay/speed control) as well as the segmented LEDs
 * driven by the LP5523.  The segmented LEDs are used as a charge
 * meter while the device is plugged in.
 */


//#define LOG_NDEBUG 0
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

/******************************************************************************/

static pthread_once_t g_init = PTHREAD_ONCE_INIT;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
static struct light_state_t g_attention;
static struct light_state_t g_notification;
static int g_battery_bars = -1;

char const*const WHITE_LED_FILE
        = "/sys/class/leds/nbq_wled/brightness";

char const*const WHITE_BLINK_FILE
        = "/sys/class/leds/nbq_wled/blink";

char const*const WHITE_DUTY_PCTS_FILE
        = "/sys/class/leds/nbq_wled/duty_pcts";

char const*const WHITE_START_IDX_FILE
        = "/sys/class/leds/nbq_wled/start_idx";

char const*const WHITE_PAUSE_LO_FILE
        = "/sys/class/leds/nbq_wled/pause_lo";

char const*const WHITE_PAUSE_HI_FILE
        = "/sys/class/leds/nbq_wled/pause_hi";

char const*const WHITE_RAMP_STEP_MS_FILE
        = "/sys/class/leds/nbq_wled/ramp_step_ms";

char const*const LCD_FILE
        = "/sys/class/leds/lcd-backlight/brightness";

char const*const SEGMENTED_LED_FILE
        = "/sys/class/leds/lp5523:channel0/device/leds_on_off";

#define NUM_LED_SEGMENTS 4
#define SEGMENT_BRIGHTNESS 100

#define RAMP_SIZE 8
static int BRIGHTNESS_RAMP[RAMP_SIZE]
        = { 0, 12, 25, 37, 50, 72, 85, 100 };
#define RAMP_STEP_DURATION 50

/**
 * device methods
 */

void init_globals(void)
{
    // init the mutex
    pthread_mutex_init(&g_lock, NULL);
}

static int
write_int(char const* path, int value)
{
    int fd;
    static int already_warned = 0;

    fd = open(path, O_RDWR);
    if (fd >= 0) {
        char buffer[20];
        int bytes = sprintf(buffer, "%d\n", value);
        int amt = write(fd, buffer, bytes);
        close(fd);
        return amt == -1 ? -errno : 0;
    } else {
        if (already_warned == 0) {
            ALOGE("write_int failed to open %s\n", path);
            already_warned = 1;
        }
        return -errno;
    }
}

static int
write_str(char const* path, char* value)
{
    int fd;
    static int already_warned = 0;

    fd = open(path, O_RDWR);
    if (fd >= 0) {
        char buffer[1024];
        int bytes = snprintf(buffer, sizeof(buffer), "%s\n", value);
        ssize_t amt = write(fd, buffer, (size_t)bytes);
        close(fd);
        return amt == -1 ? -errno : 0;
    }
    if (already_warned == 0) {
        ALOGE("write_int failed to open %s\n", path);
        already_warned = 1;
    }
    return -errno;
}

static int
is_lit(struct light_state_t const* state)
{
    return state->color & 0x00ffffff;
}

static int
rgb_to_brightness(struct light_state_t const* state)
{
    int color = state->color & 0x00ffffff;
    return ((77*((color>>16)&0x00ff))
            + (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
}

static int
set_light_battery(struct light_device_t* dev,
        struct light_state_t const* state)
{
    int brightness = 0;
    char buf[20];
    int err = 0;

    if (!dev)
        return -1;

    pthread_mutex_lock(&g_lock);

    // framework sends the level as the lower 8 bits of color
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
        else if (level > 60)
            bars = 3;
        else if (level > 30)
            bars = 2;
        else if (level > 5)
            bars = 1;
    }

    if (bars == g_battery_bars)
        goto out;

    for (int i = 1; i <= NUM_LED_SEGMENTS; i++) {
        brightness = (bars >= i) ? SEGMENT_BRIGHTNESS : 0;
        snprintf(buf, sizeof(buf), "%d %d", i, brightness);
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


static int
set_light_backlight(struct light_device_t* dev,
        struct light_state_t const* state)
{
    int err = 0;
    int brightness = rgb_to_brightness(state);
    if(!dev) {
        return -1;
    }
    pthread_mutex_lock(&g_lock);
    err = write_int(LCD_FILE, brightness);
    pthread_mutex_unlock(&g_lock);
    return err;
}

static char*
get_scaled_duty_pcts(int brightness)
{
    char *buf = malloc(5 * RAMP_SIZE * sizeof(char));
    char *pad = "";
    int i = 0;

    memset(buf, 0, 5 * RAMP_SIZE * sizeof(char));

    for (i = 0; i < RAMP_SIZE; i++) {
        char temp[5] = "";
        snprintf(temp, sizeof(temp), "%s%d", pad, (BRIGHTNESS_RAMP[i] * brightness / 255));
        strcat(buf, temp);
        pad = ",";
    }
    ALOGV("%s: brightness=%d duty=%s", __func__, brightness, buf);
    return buf;
}

static int
set_speaker_light_locked(struct light_device_t* dev,
        struct light_state_t const* state)
{
    int white, blink;
    int onMS, offMS, stepDuration, pauseHi;
    unsigned int colorRGB;
    char *duty;

    if(!dev) {
        return -1;
    }

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

    // take the alpha value since we support custom brightness
    white = (state->color & 0xFF000000) >> 24;
    blink = onMS > 0 && offMS > 0;

    // disable all blinking to start
    write_int(WHITE_BLINK_FILE, 0);

    if (blink) {
        stepDuration = RAMP_STEP_DURATION;
        pauseHi = onMS - (stepDuration * RAMP_SIZE * 2);
        if (stepDuration * RAMP_SIZE * 2 > onMS) {
            stepDuration = onMS / (RAMP_SIZE * 2);
            pauseHi = 0;
        }

        // white
        write_int(WHITE_START_IDX_FILE, 0);
        duty = get_scaled_duty_pcts(white);
        write_str(WHITE_DUTY_PCTS_FILE, duty);
        write_int(WHITE_PAUSE_LO_FILE, offMS);
        // The led driver is configured to ramp up then ramp
        // down the lut. This effectively doubles the ramp duration.
        write_int(WHITE_PAUSE_HI_FILE, pauseHi);
        write_int(WHITE_RAMP_STEP_MS_FILE, stepDuration);
        free(duty);

        // start the party
        write_int(WHITE_BLINK_FILE, 1);

    } else {

        write_int(WHITE_LED_FILE, white);
    }

    return 0;
}

static void
handle_speaker_light_locked(struct light_device_t* dev)
{
    set_speaker_light_locked(dev, NULL);
    if (is_lit(&g_attention)) {
        set_speaker_light_locked(dev, &g_attention);
    } else if (is_lit(&g_notification)) {
        set_speaker_light_locked(dev, &g_notification);
    }
}

static int
set_light_notifications(struct light_device_t* dev,
        struct light_state_t const* state)
{
    pthread_mutex_lock(&g_lock);
    g_notification = *state;
    handle_speaker_light_locked(dev);
    pthread_mutex_unlock(&g_lock);
    return 0;
}

static int
set_light_attention(struct light_device_t* dev,
        struct light_state_t const* state)
{
    pthread_mutex_lock(&g_lock);
    g_attention = *state;
    handle_speaker_light_locked(dev);
    pthread_mutex_unlock(&g_lock);
    return 0;
}

/** Close the lights device */
static int
close_lights(struct light_device_t *dev)
{
    if (dev) {
        free(dev);
    }
    return 0;
}


/******************************************************************************/

/**
 * module methods
 */

/** Open a new instance of a lights device using name */
static int open_lights(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
{
    int (*set_light)(struct light_device_t* dev,
            struct light_state_t const* state);

    if (0 == strcmp(LIGHT_ID_BACKLIGHT, name))
        set_light = set_light_backlight;
    else if (0 == strcmp(LIGHT_ID_BATTERY, name))
        set_light = set_light_battery;
    else if (0 == strcmp(LIGHT_ID_NOTIFICATIONS, name))
        set_light = set_light_notifications;
    else if (0 == strcmp(LIGHT_ID_ATTENTION, name))
        set_light = set_light_attention;
    else
        return -EINVAL;

    pthread_once(&g_init, init_globals);

    struct light_device_t *dev = malloc(sizeof(struct light_device_t));

    if(!dev)
        return -ENOMEM;

    memset(dev, 0, sizeof(*dev));

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

/*
 * The lights Module
 */
struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = LIGHTS_HARDWARE_MODULE_ID,
    .name = "Ether Lights Module",
    .author = "The CyanogenMod Project",
    .methods = &lights_module_methods,
};
