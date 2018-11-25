#include <stdio.h>
#include <assert.h>
#include "Bootstrap.h"


static KernelTaskContext context = {0};


/* async endpoint for periodic timer */
static vka_object_t timer_aep;

/* platsupport (periodic) timer */

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

int main(void)
{

    printf("Salut, Monde!\n");

    memset(&context , 0 , sizeof(KernelTaskContext) );

    context.info = platsupport_get_bootinfo();
    

    int err = bootstrapSystem(&context);
    assert(err == 0);

    init_timers();



    printf("----END ---- \n");
    return 0;
}

