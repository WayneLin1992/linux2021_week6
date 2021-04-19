#include "fiber.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <sys/time.h>
#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                           } while (0)
uint32_t futex_addr = 1;
uint32_t futex_addr_1 = 1;
futex(uint32_t *uaddr, int futex_op, uint32_t val, const struct timespec *timeout, uint32_t *uaddr2, uint32_t val3)
{
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

static void fwait(uint32_t *futexp)
{
    long s;

           /* atomic_compare_exchange_strong(ptr, oldval, newval)
              atomically performs the equivalent of:

                  if (*ptr == *oldval)
                      *ptr = newval;

              It returns true if the test yielded true and *ptr was updated. */

    while (1) {

               /* Is the futex available? */
               const uint32_t one = 1;
         if (atomic_compare_exchange_strong(futexp, &one, 0))
             break;      /* Yes */

               /* Futex is not available; wait. */

         s = futex(futexp, FUTEX_WAIT, 0, NULL, NULL, 0);
         if (s == -1 && errno != EAGAIN)
             errExit("futex-FUTEX_WAIT");
    }
}

       /* Release the futex pointed to by 'futexp': if the futex currently
          has the value 0, set its value to 1 and the wake any futex waiters,
          so that if the peer is blocked in fwait(), it can proceed. */

static void fpost(uint32_t *futexp)
{
    long s;
    /* atomic_compare_exchange_strong() was described
         in comments above. */
    const uint32_t zero = 0;
    if (atomic_compare_exchange_strong(futexp, &zero, 1)) {
        s = futex(futexp, FUTEX_WAKE, 1, NULL, NULL, 0);
        if (s  == -1)
            errExit("futex-FUTEX_WAKE");
    }
}

static void fibonacci() {
    int fib[2] = {0, 1};
    printf("Fib(0) = 0\nFib(1) = 1\n");
    for (int i = 2; i < 15; ++i) {
        int nextFib = fib[0] + fib[1];
//        fwait(&futex_addr);
	futex(&futex_addr, FUTEX_WAIT, 0, NULL, NULL, 0);
        printf("Fib(%d) = %d\n", i, nextFib);;
        fib[0] = fib[1];
        fib[1] = nextFib;
        futex(&futex_addr, FUTEX_WAKE, 1, NULL, NULL, 0);
//        fpost(&futex_addr);
        fiber_yield();
    }
}

static void squares() {
    for (int i = 1; i < 10; ++i) {
        futex(&futex_addr_1, FUTEX_WAIT, 0, NULL, NULL, 0);
        printf("%d * %d = %d\n", i, i, i * i);
	futex(&futex_addr_1, FUTEX_WAKE, 1, NULL, NULL, 0);
//        fpost(&futex_addr_1);
//        futex_addr = 0;
//        futex_wait(&futex_addr,0);
        fiber_yield();
    }
}

int main() {
    fiber_init();

    fiber_spawn(&fibonacci);
    sleep(1);
    fiber_spawn(&squares);

    /* Since fibers are non-preemptive, we must allow them to run */
//    sleep(1);
    fiber_wait_all();

    return 0;
}
