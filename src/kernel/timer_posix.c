/* timer_posix.c */

static timer_t mytimer;
static struct sigaction old_alarm_handler;

void timer_handler (int sig, siginfo_t *extra, void *cruft)
{
    int sem_value;
    int noverflow;

#if 0
    if( (noverflow=timer_getoverrun(*(timer_t *)
       extra->si_value.sival_ptr)) != 0) {
       /*timer has overflowed -- error !*/
       printf ("timer overrun!\n");
    }
#endif

    __mg_timer_counter++;

    // alert desktop
    AlertDesktopTimerEvent ();

    return;
}

BOOL mg_InitTimer (void)
{
    struct itimerspec it;
    struct timespec resolution;
    struct sigaction sa;
    struct sigevent timer_event;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_SIGINFO;         /*real-time signal*/
    sa.sa_sigaction=timer_handler;    /*pointer to action*/

    if(sigaction(SIGRTMIN, &sa, &old_alarm_handler) < 0){
        perror("sigaction error");
        return FALSE;
    }

    /* first determine whether the desired clock exists */

    if (clock_getres(CLOCK_REALTIME, &resolution) < 0){
        perror("clock_getres error");
        return FALSE;
    }

    /* create a timer based upon the CLOCK_REALTIME clock */

     it.it_interval.tv_sec=0;
    
    /* set resolution to one-tenth of the maximum allowed */

     it.it_interval.tv_nsec=resolution.tv_nsec;
     //it.it_interval.tv_nsec=10000000;
     it.it_value=it.it_interval;

     timer_event.sigev_notify=SIGEV_SIGNAL;
     timer_event.sigev_signo= SIGRTMIN;
     timer_event.sigev_value.sival_ptr = (void *)&mytimer;

     if (timer_create(CLOCK_REALTIME, &timer_event, &mytimer) < 0){
        perror("timer create error");
        return FALSE;
     }
     
     /* relative timer, go off at the end of the interval*/
    if (timer_settime(mytimer, 0 , &it, NULL) < 0){
        perror("settimer");
        return FALSE;
    }

    __mg_timer_counter = 0;

    return TRUE;
}

void mg_TerminateTimer (void)
{
    int i;

    timer_delete (mytimer);

    if (sigaction (SIGRTMIN, &old_alarm_handler, NULL) == -1) {
        fprintf (stderr, "KERNEL>Timer: sigaction call failed!\n");
        perror ("sigaction");
    	return;
    }

    for (i=0; i<MAX_TIMERS; i++) {
        if (timerstr[i] != NULL)
            free ( timerstr[i] );
        timerstr[i] = NULL;
    }
}

