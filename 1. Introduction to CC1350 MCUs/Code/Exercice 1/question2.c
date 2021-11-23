/*
 *  ======== empty.c ========
 */

/* For usleep() */
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
/* Board Header file */
#include "Board.h"

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    /* 1s   = 1 000 000 microseconds
     * 0.5s =   500 000 microseconds */
    useconds_t time = 500000;
    int i;

    /* Call driver init functions */
    GPIO_init();

    /* Configure the LED pin */
    GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(Board_GPIO_LED1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Turn off user LED */
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_OFF);
    GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_OFF);

    while (1) {

        for( i=0 ; i < 4 ; i++){
            usleep(time);
            GPIO_toggle(Board_GPIO_LED0);
        }
        for( i=0 ; i < 4 ; i++){
            usleep(time);
            GPIO_toggle(Board_GPIO_LED1);
        }

    }
}
