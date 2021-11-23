/*
 *  ======== uartecho.c ========
 */
#include <stdint.h>
#include <stddef.h>
#include <ti/drivers/GPIO.h>        /* For Driver Header files */
#include <ti/drivers/UART.h>        /* For Driver Header files */
#include <unistd.h>                 /* For usleep() */

#include "Board.h"                  /* Example/Board Header files */

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    char        input;
    const char  echoPrompt[] = "Echoing characters:\r\n";
    UART_Handle uart;
    UART_Params uartParams;
    useconds_t time = 50000;

    /* Call driver init functions */
    GPIO_init();
    UART_init();

    /* Configure the LED pin */
    GPIO_setConfig(Board_GPIO_LED1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Turn off user LED */
    GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_OFF);

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 38400;

    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        /* UART_open() failed */
        while (1);
    }

    UART_write(uart, echoPrompt, sizeof(echoPrompt));

    /* Loop forever echoing */
    while (1) {
        UART_read(uart, &input, 1);
        GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_ON);
        usleep(time);
        GPIO_write(Board_GPIO_LED1, Board_GPIO_LED_OFF);
        UART_write(uart, &input, 1);
    }
}