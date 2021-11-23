/**
 * Practical work: Developing RTOS programs
 * Exercise 1 : Task and Priorities with TI - POSIX APIs
 * File : main.c
 * Description :
 *  - Main function initializes threads , UART channel and BIOS (Kernel scheduler)
 *  - Task 1 : Switch on green led
 *  - Task 2 : Switch on red led
 *  - ISR    : Send UART message
 * @Author : Tiziano NARDONE - Undergraduate student in Computer Science - University of Reims Champagne Ardenne France
 * */

/* Includes */
#include <xdc/runtime/System.h>         /* For System abort */
#include <pthread.h>                    /* For POSIX Thread header files */
#include <ti/sysbios/knl/Clock.h>       /* For Clock tick period */
#include <ti/drivers/GPIO.h>            /* For GPIO manager */
#include <ti/drivers/UART.h>            /* For UART manager */
#include <ti/sysbios/BIOS.h>            /* For RTOS header files */
#include <ti/drivers/Board.h>           /* For Example/Board Header files */
#include <unistd.h>                     /* For sleep */
#include "board.h"                      /* For the board configuration */

/* Defines */
#define THREAD_STACK_SIZE 1024                 /* Size of the stack */
#define LED_RED Board_GPIO_LED0         /* Led 0 */
#define LED_GREEN Board_GPIO_LED1       /* Led 1 */
#define LED_ON Board_GPIO_LED_ON        /* Switch Led on */
#define LED_OFF Board_GPIO_LED_OFF      /* Switch Led off */
#define BUTTON_0 Board_GPIO_BUTTON0     /* Button 0 */

/* Global */
UART_Handle uart;                                       /* UART Handle */
const char  UARTprompt[] = "Button pressed\r\n";        /* MSG interrupt */

/* Thread 1 */
void *thread1_fxn(void * arg0){
    while(1)
        GPIO_write(LED_GREEN, LED_ON);
}

/* Thread 2 */
void *thread2_fxn(void * arg0){
    int i;
    while(1){
        sleep(3);
        GPIO_write(LED_GREEN, LED_OFF);
        GPIO_write(LED_RED, LED_ON);
        for(i=0; i < 2000000 ; i++);
        GPIO_write(LED_RED, LED_OFF);
    }
}

/* ISR */
void InterruptServiceRoutine(uint_least8_t index){
    if( (UART_write(uart, UARTprompt, sizeof(UARTprompt))) == UART_STATUS_ERROR ){
        System_abort("Error writing UART\n");
    }
}

/* main */
int main(void){
    UART_Params uartParams;
    pthread_t           thread;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;

    /* Initialization drivers */
    Board_init();
    GPIO_init();
    UART_init();

    /* Initialization Interrupt */
    GPIO_setConfig(BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setCallback(BUTTON_0, InterruptServiceRoutine);
    GPIO_enableInt(BUTTON_0);

    /* Initialization UART channel */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;
    if( (uart = UART_open(Board_UART0, &uartParams)) == NULL ){
        System_abort("Error opening UART channel\n");
    }

    /* Initialize the attributes structure with default values */
    pthread_attr_init(&attrs);

    /* Thread 1 : Set priority, detach state, and stack size attributes */
    priParam.sched_priority = 1;
    retc = pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    if( (retc |= pthread_attr_setstacksize(&attrs, THREAD_STACK_SIZE)) != 0 ){
        System_abort("Error setting stack size thread 1\n");
    }
    /* Thread 1 : creating thread */
    if( (retc = pthread_create(&thread, &attrs, thread1_fxn, NULL)) != 0 ){
        System_abort("Error creating thread 1\n");
    }

    /* Thread 2 : Set priority, detach state, and stack size attributes */
    priParam.sched_priority = 2;
    retc = pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    if( (retc |= pthread_attr_setstacksize(&attrs, THREAD_STACK_SIZE)) != 0 ){
        System_abort("Error setting stack size thread 1\n");
    }
    /* Thread 2 : creating thread */
    if( (retc = pthread_create(&thread, &attrs, thread2_fxn, NULL)) != 0 ){
        System_abort("Error creating thread 2\n");
    }

    /* Start the TI-RTOS scheduler */
    BIOS_start();

    return (0);
}
