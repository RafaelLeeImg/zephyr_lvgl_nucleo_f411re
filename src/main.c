/*
 *  Copyright (C) 2022 Rafael Lee <rafaellee.img@gmail.com>
 */

#include <device.h>
#include <display/cfb.h>
#include <drivers/display.h>
#include <drivers/gpio.h>
#include <lvgl.h>
#include <src/lv_core/lv_obj.h>
#include <src/lv_widgets/lv_canvas.h>

#include <stdio.h>
#include <string.h>
#include <zephyr.h>

#if defined(CONFIG_SSD16XX)
#define DISPLAY_DRIVER "SSD16XX"
#endif

#if defined(CONFIG_SSD1306)
#define DISPLAY_DRIVER "SSD1306"
#endif

#if defined(CONFIG_ILI9340)
#define DISPLAY_DRIVER "ILI9340"
#endif

#ifndef DISPLAY_DRIVER
#define DISPLAY_DRIVER "DISPLAY"
#endif

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

#define LED0_NODE DT_ALIAS(led0)
#define LED0 DT_GPIO_LABEL(LED0_NODE, gpios)
#define LED0_PIN DT_GPIO_PIN(LED0_NODE, gpios)
#define LED0_FLAGS DT_GPIO_FLAGS(LED0_NODE, gpios)

#define LED1_NODE DT_ALIAS(led1)
#define LED1 DT_GPIO_LABEL(LED1_NODE, gpios)
#define LED1_PIN DT_GPIO_PIN(LED1_NODE, gpios)
#define LED1_FLAGS DT_GPIO_FLAGS(LED1_NODE, gpios)

#define LED2_NODE DT_ALIAS(led2)
#define LED2 DT_GPIO_LABEL(LED2_NODE, gpios)
#define LED2_PIN DT_GPIO_PIN(LED2_NODE, gpios)
#define LED2_FLAGS DT_GPIO_FLAGS(LED2_NODE, gpios)

#define LED3_NODE DT_ALIAS(led3)
#define LED3 DT_GPIO_LABEL(LED3_NODE, gpios)
#define LED3_PIN DT_GPIO_PIN(LED3_NODE, gpios)
#define LED3_FLAGS DT_GPIO_FLAGS(LED3_NODE, gpios)

#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

const struct device *dev_led0 = { 0 };

struct printk_data_t {
	void *fifo_reserved; /* 1st word reserved for use by fifo */
	uint32_t led;
	uint32_t cnt;
};

K_FIFO_DEFINE(printk_fifo);

lv_obj_t *screen0_slider_obj;
static short screen0_slider_value = 15;

typedef struct {
	lv_obj_t **object;
	short *value;
	short step;
	short max;
	short min;
} param_t;

typedef struct {
	lv_obj_t *screen;
	int count;
	param_t *params;
} screens_t;

static param_t screen0_elements[] = {
	{ .object = &screen0_slider_obj,
	  .value = &screen0_slider_value,
	  .step = 5,
	  .max = 100,
	  .min = 0 },
};

static screens_t screens[] = {
	{ .screen = NULL, .count = 1, .params = screen0_elements },
};

void main(void)
{
	// LED initialize
	dev_led0 = device_get_binding(LED0);

	int ret = 0;
	ret = gpio_pin_configure(dev_led0, LED0_PIN, GPIO_OUTPUT_ACTIVE | LED0_FLAGS);
	if (ret < 0) {
		return;
	}

	gpio_pin_set(dev_led0, LED0_PIN, 0);

	// LVGL
	const struct device *display_dev;

	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

	if (display_dev == NULL) {
		printk("device not found.  Aborting test.");
		return;
	}

	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return;
	}

	lv_obj_t *screen = lv_scr_act();

	lv_obj_t *hello_world_label;

	hello_world_label = lv_label_create(lv_scr_act(), NULL);

	lv_label_set_text(hello_world_label, "Hello world!");
	lv_obj_align(hello_world_label, NULL, LV_ALIGN_CENTER, 0, 0);

	char count_str[11] = { 0 };
	uint32_t count = 0U;

	while (1) {
		if ((count % 10) == 0U) {
			sprintf(count_str, "%d", count / 10U);

			// gpio_pin_toggle (dev_led3, LED3_PIN);
			gpio_pin_toggle(dev_led0, LED0_PIN);
			lv_label_set_text(hello_world_label, count_str);

			lv_task_handler();
		}
		k_busy_wait(100000); // 0.1s
		++count;
	}
}

void blink(const struct device *port, gpio_pin_t pin, uint32_t sleep_ms)
{
	while (1) {
		gpio_pin_toggle(port, pin);
		k_busy_wait(1000 * sleep_ms);
	}
}

