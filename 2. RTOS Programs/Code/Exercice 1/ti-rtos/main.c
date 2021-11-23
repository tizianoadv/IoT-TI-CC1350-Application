/**
 * Practical work: Developing RTOS programs
 * Exercise 1 : Task and Priorities with TI - RTOS APIs
 * File : main.c
 * Description :
 *  - Main function which initializes tasks (Threads), UART channel and BIOS (RTOS)
 *  - Task 1 : Switch on green led
 *  - Task 2 : Switch on red led
 *  - ISR    : Send UART message
 * @Author : Tiziano NARDONE - Undergraduate student in Computer Science - University of Reims Champagne Ardenne France
 * */

/* Includes */
#include <xdc/runtime/System.h>         /* For System abort */
#include <ti/sysbios/knl/Task.h>        /* For Module task manager*/
#include <ti/sysbios/knl/Clock.h>       /* For Clock tick period */
#include <ti/drivers/GPIO.h>            /* For GPIO manager */
#include <ti/drivers/UART.h>            /* For UART manager */
#include <ti/sysbios/BIOS.h>            /* For RTOS header files */
#include <ti/drivers/Board.h>           /* For Example/Board Header files */
#include "board.h"                      /* For the board configuration */

/* Defines */
#define STACK_SIZE 1024                 /* Size of the stack */
#define LED_RED Board_GPIO_LED0         /* Led 0 */
#define LED_GREEN Board_GPIO_LED1       /* Led 1 */
#define LED_ON Board_GPIO_LED_ON        /* Switch Led on */
#define LED_OFF Board_GPIO_LED_OFF      /* Switch Led off */
#define BUTTON_0 Board_GPIO_BUTTON0    /* Button 0 */

/* Global */
UART_Handle uart;                                       /* UART Handle */
const char  UARTprompt[] = "Button pressed\r\n";        /* MSG interrupt */

/* Task 1 */
void task1_fxn(){
    while(1)
        GPIO_write(LED_GREEN, LED_ON);
}

/* Task 2 */
void task2_fxn(){
    int i;
    while(1){
        Task_sleep(3000000 / Clock_tickPeriod);
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
    Task_Params taskParams;
    UART_Params uartParams;

    /* Initialization drivers */
    Board_init();
    GPIO_init();
    UART_init();

    /* Initialization LEDs */
    GPIO_setConfig(LED_RED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(LED_GREEN, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Initialization Interrupt */
    GPIO_setConfig(BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
    GPIO_setCallback(BUTTON_0, InterruptServiceRoutine);
    GPIO_enableInt(BUTTON_0);

    /* Initialization Task 1 */
    Task_Params_init(&taskParams);
    taskParams.stackSize = STACK_SIZE;
    taskParams.priority = 1;
    if( (Task_create((Task_FuncPtr) task1_fxn, &taskParams, NULL)) == NULL ){
        System_abort("Error creating task1\n");
    }

    /* Initialization Task 2 */
    Task_Params_init(&taskParams);
    taskParams.stackSize = STACK_SIZE;
    taskParams.priority = 2;
    if( (Task_create((Task_FuncPtr) task2_fxn, &taskParams, NULL)) == NULL ){
        System_abort("Error creating task2\n");
    }

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

    /* Start the TI-RTOS scheduler */
    BIOS_start();

    return (0);
}
