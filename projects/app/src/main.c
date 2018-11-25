#include <stdio.h>
#include <assert.h>
#include <platsupport/local_time_manager.h>
#include "Bootstrap.h"

#define MAX_TIMERS 64
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

static int onPeriodic1(uintptr_t token)
{
	printf("on Periodic1 %lx\n" , token);
}

static int onPeriodic2(uintptr_t token)
{
        printf("on Periodic2 %lx\n" , token);
}


static int TimerAllocAndRegister(time_manager_t *tm , uint64_t period_ns, uint64_t start, uint32_t id, timeout_cb_fn_t callback, uintptr_t token)
{
	int err = tm_alloc_id_at(tm , id);
	if( !err)
	{
		return tm_register_periodic_cb(tm , period_ns ,start,id , callback, token);
	}
	return err;
} 

static void test_interrupt()
{
	int err = 0;
	//err =  ltimer_set_timeout(&context.timer.ltimer, 2000 * NS_IN_MS, TIMEOUT_PERIODIC);//TIMEOUT_RELATIVE); //TIMEOUT_PERIODIC);
	//assert(err == 0);

	err = TimerAllocAndRegister(&context.tm , 2000*NS_IN_MS , 0/*start */ , 10 /*id*/ , onPeriodic1 , 10 /* token */);
	assert(err == 0);

        err = TimerAllocAndRegister(&context.tm , 500*NS_IN_MS , 0/*start */ , 1 /*id*/ , onPeriodic2 , 2 /* token */);
        assert(err == 0);

	while(1)
	{
		printf("Start waiting\n"); 

		seL4_Word sender_badge = 0;

		seL4_Wait(timer_aep.cptr, &sender_badge);

		printf("After waiting\n");
		sel4platsupport_handle_timer_irq(&context.timer, sender_badge);
		err = tm_update(&context.tm);
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


    err = tm_init(&context.tm ,&context.timer.ltimer ,&context.ops , MAX_TIMERS);
    assert(err == 0); 




    test_interrupt();

    printf("----END ---- \n");
    return 0;
}

