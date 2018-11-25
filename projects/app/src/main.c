#include <stdio.h>
#include <assert.h>
#include "Bootstrap.h"


static KernelTaskContext context = {0};


/* async endpoint for periodic timer */
static vka_object_t timer_aep;

static uint64_t getTime()
{
	uint64_t t = 0;
	ltimer_get_time(&context.timer.ltimer, &t);

	return t;
}


static void init_timers()
{
	int err = vka_alloc_notification(&context.vka, &timer_aep);
	assert(err == 0);


	err = sel4platsupport_new_io_ops(context.vspace, context.vka, &context.ops);
        assert(err == 0);


	err = sel4platsupport_new_arch_ops(&context.ops, &context.simple, &context.vka);
        assert(err == 0);


	cspacepath_t notification_path;

        vka_cspace_make_path( &context.vka, timer_aep.cptr, &notification_path);

	err = sel4platsupport_init_default_timer_ops(&context.vka, &context.vspace, &context.simple, context.ops,
                                                   notification_path.capPtr, &context.timer);


	assert(err == 0);

}


static void clearTimerInterupts()
{
	ltimer_reset(&context.timer.ltimer);
/*
	seL4_Word sender_badge = 0;
	

	seL4_MessageInfo_t ret =  seL4_Poll(timer_aep.cptr, &sender_badge);
	printf("Poll returned badge %lx\n", sender_badge);
*/	
}


static void test_interrupt()
{
	int err =  ltimer_set_timeout(&context.timer.ltimer, 2000 * NS_IN_MS, TIMEOUT_PERIODIC);//TIMEOUT_RELATIVE); //TIMEOUT_PERIODIC);
	assert(err == 0);


	uint64_t startT = getTime();


	while(1)
	{
		printf("Start waiting at %lu\n" , startT); 

		seL4_Word sender_badge = 0;


//		seL4_Recv(timer_aep.cptr, &sender_badge);
		seL4_Wait(timer_aep.cptr, &sender_badge);

		startT = getTime();
		printf("After waiting at %lu sender badge %lx\n" , startT , sender_badge); 
		sel4platsupport_handle_timer_irq(&context.timer, sender_badge);
	}
}

int main(void)
{

    printf("Salut, Monde!\n");

    memset(&context , 0 , sizeof(KernelTaskContext) );

    context.info = platsupport_get_bootinfo();
    

    int err = bootstrapSystem(&context);
    assert(err == 0);

    init_timers();

    clearTimerInterupts();
    test_interrupt();

    printf("----END ---- \n");
    return 0;
}

