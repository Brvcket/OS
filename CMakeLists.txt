cmake_minimum_required(VERSION 3.26)
project(OS C)

set(CMAKE_C_STANDARD 11)

add_executable(OS
        week01/main.c
        week02/ex1.c
        week02/ex2.c
        week02/ex3.c
        week02/ex4.c
        week02/ex5.c
        week03/ex1.c
        week03/ex2.c
        week03/ex3.c
        week03/ex4.c
        week04/ex1.c
        week04/ex2.c
        week04/ex3.c
        week04/ex4.c
        week05/channel.c
        week05/ex2.c
        week05/ex3.c
        week05/ex4.c
        week05/publisher.c
        week05/subscriber.c
        week06/agent.c
        week06/controller.c
        week06/scheduler.c
        week06/scheduler_rr.c
        week06/scheduler_sjf.c
        week06/worker.c
        week07/allocator.c
        week07/ex2.c
        week08/ex1.c
        week08/ex3.c
        week08/mmu.c
        week08/pager.c
        week09/mmu.c
        week09/pager.c)
