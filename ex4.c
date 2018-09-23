#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gpio.h>
#include <power_clocks_lib.h>
#include <sysclk.h>
#include <compiler.h>
#include <board.h>
#include <stdio_usb.h>


#define CONFIG_USART_IF (AVR32_USART2)

#include "FreeRTOS.h"
#include "task.h"

#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00

//Task A
struct taskArgs{
	uint32_t pin;
	int period;
};

//Task B
/*
struct responseTaskArgs {
	struct {
		uint32_t test;
		uint32_t response;
	} pin;
	bool delayMode;
};
*/

//Task C
struct responseTaskArgs {
	struct {
		uint32_t test;
		uint32_t response;
	} pin;
	bool doTask;
};

void busy_delay_ms(int delay){
    for(; delay != 0; delay--){
        for(int i = 0; i < 2108; i++){
            asm volatile ("" ::: "memory");
        }
    }
}

void busy_delay_short(void){
    for(int i = 0; i < 10; i++){
        asm volatile ("" ::: "memory");
    }
}

void init(){
	board_init();
	
    gpio_configure_pin(TEST_A, GPIO_DIR_INPUT);
    gpio_configure_pin(TEST_B, GPIO_DIR_INPUT);
    gpio_configure_pin(TEST_C, GPIO_DIR_INPUT);
    gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);

	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
	
	stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
	    setbuf(stdout, NULL);
	    setbuf(stdin,  NULL);
    #endif
}

//Task A
static void taskFn(void* args){
	struct taskArgs a = *(struct taskArgs*)args;
	const portTickType delay = a.period / portTICK_RATE_MS;
	
    int iter = 0;

	while(1){
		gpio_toggle_pin(a.pin);
		printf("tick %d\n", iter++);
		
		vTaskDelay(delay);
	}
}

//Task B 
/*
static void responseTask(void* args){
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	while(1){
		if(gpio_pin_is_low(a.pin.test)){
			gpio_set_pin_low(a.pin.response);
			if(a.delayMode){
				vTaskDelay(1);
			} else {
				busy_delay_short();
			}
			gpio_set_pin_high(a.pin.response);
		}
	}
}
*/

//Task C
/*
static void responseTask(void* args){
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	while(1){
		
		if(gpio_pin_is_low(a.pin.test)){
			
			if(a.doTask)
				busy_delay_ms(3);
			
			gpio_set_pin_low(a.pin.response);
			vTaskDelay(1);
			gpio_set_pin_high(a.pin.response);
		}
	}
}*/


//Task D
static void responseTask(void* args){
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	while(1){
		if(gpio_pin_is_low(a.pin.test)){
			
			if(a.doTask)
				busy_delay_ms(3);
			
			gpio_set_pin_low(a.pin.response);
			vTaskDelay(1);
			gpio_set_pin_high(a.pin.response);
		} else {
			vTaskDelay(1);
		}
	}
}


int main(){
	init();
    
	/*
	//Task A    
	xTaskCreate(taskFn, "", 1024, (&(struct taskArgs){LED0_GPIO, 200}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(taskFn, "", 1024, (&(struct taskArgs){LED1_GPIO, 500}), tskIDLE_PRIORITY + 1, NULL);
	*/
	
	/*
	//Task B
	xTaskCreate(responseTask, "", 1024, (&(struct responseTaskArgs){{TEST_A, RESPONSE_A}, 1}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTask, "", 1024, (&(struct responseTaskArgs){{TEST_B, RESPONSE_B}, 1}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTask, "", 1024, (&(struct responseTaskArgs){{TEST_C, RESPONSE_C}, 1}), tskIDLE_PRIORITY + 1, NULL);
	*/	
	
	
	//Task C and D
	/*
	xTaskCreate(responseTask, "", 1024, (&(struct responseTaskArgs){{TEST_A, RESPONSE_A}, 0}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTask, "", 1024, (&(struct responseTaskArgs){{TEST_B, RESPONSE_B}, 0}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTask, "", 1024, (&(struct responseTaskArgs){{TEST_C, RESPONSE_C}, 1}), tskIDLE_PRIORITY + 1, NULL);
	*/
	
	//Task E 
	
	xTaskCreate(responseTask, "", 1024, (&(struct responseTaskArgs){{TEST_A, RESPONSE_A}, 0}), tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(responseTask, "", 1024, (&(struct responseTaskArgs){{TEST_B, RESPONSE_B}, 0}), tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(responseTask, "", 1024, (&(struct responseTaskArgs){{TEST_C, RESPONSE_C}, 1}), tskIDLE_PRIORITY + 1, NULL);	
	
	
	// Start the scheduler, anything after this will not run.
	vTaskStartScheduler();
    
}

