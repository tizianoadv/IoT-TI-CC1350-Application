#include <stdint.h>             /* For gpiointerrupt.c */
#include <stddef.h>             /* For gpiointerrupt.c */
#include <unistd.h>             /* For usleep() */
#include <ti/drivers/GPIO.h>    /* For Driver Header files */

#include "Board.h"              /* For Example/Board Header files */


short int num_led=-1;
useconds_t time = 500000;

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on Board_GPIO_BUTTON0.
 */
void gpioButtonFxn0(uint_least8_t index){
    num_led = Board_GPIO_LED0;
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on Board_GPIO_BUTTON1.
 *  This may not be used for all boards.
 */
void gpioButtonFxn1(uint_least8_t index){
    num_led = Board_GPIO_LED1;
}

/*
 *  ======== mainThread ========
 *  LAUNCHPAD XL CC1350F128 OWNS 2 BUTTONS
 *  Others development boards may have only 1 button
 */
void *mainThread(void *arg0)
{
    /* Call driver init functions */
    GPIO_init();

    /* Configure the LEDs and buttons pins */
    GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_GPIO_LED1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_GPIO_BUTTON0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setConfig(Board_GPIO_BUTTON1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    /* Turn off user LEDs */
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_OFF);

    /* install Buttons callback */
    GPIO_setCallback(Board_GPIO_BUTTON0, gpioButtonFxn0);
    GPIO_setCallback(Board_GPIO_BUTTON1, gpioButtonFxn1);

    /* Enable interrupts */
    GPIO_enableInt(Board_GPIO_BUTTON0);
    GPIO_enableInt(Board_GPIO_BUTTON1);

    while(1){

        if(num_led >= 0){
            GPIO_toggle(num_led);
            usleep(time);
            GPIO_toggle(num_led);
            num_led = -1;
        }

    }
}
