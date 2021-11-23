
/*
 *  ======== main_nortos.c ========
 */
#include <stdint.h>
#include <stddef.h>

#include <NoRTOS.h>

/* Example/Board Header files */
#include <ti/drivers/Board.h>

extern void *mainThread(void *arg0);

/*
 *  ======== main ========
 */
int main(void)
{
    /* Call driver init functions */
    Board_init();

    /* Start NoRTOS */
    NoRTOS_start();

    /* Call mainThread function */
    mainThread(NULL);

    return 0;
}
