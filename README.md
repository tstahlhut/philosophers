# Philosophers
42 school project about the dining philosophers problem

## Purpose

The main purpose of this 42 school project is to learn about multithreading.

In this project, I learned about how to create and join threads, what deadlocks are and what data races are and how to prevent the latter by applying mutexes. 

## Problem Description

*N* philosophers sit around a round table. Each philosopher has a bowl of spaghetti in front of them and between each of them lies a fork (i.e. *n* forks). Repeatively, the philosophers eat, sleep and think. Each action excludes the other two, i.e. when a philosopher is thinking, they are neither eating nor sleeping. In order to be able to eat, each philosopher needs to hold to forks: their left and their right fork (it may be easier to think of chopsticks). This means that not all philosophers can eat simultaneously. Thus, they have to take turns so that no philosopher dies of starvation.

## Rules

The philosophers are represented by threads and the forks should be protected by mutexes so that no philosopher steals the fork of their neighbor. The philsophers do not talk to each other. So, they do not know if another philosopher is going to starve soon. 

The program takes 4 to 5 arguments:

	./philo 5 800 200 200 6

1. __number_of_philosophers__ [nb]: The number of philosophers is identical to the number of forks.
2. __time_to_die__ (in ms) [t_die]: If a philosopher did not start eating time_to_die ms since the beginning ot their last meal or the beginning of the simulation, they die.
3. __time_to_eat__ (in ms) [t_eat]: The time a philosopher needs to eat.
4. __time_to_sleep__ (in ms) [t_sleep]: The time a philosopher should sleep.
5.  (Optional) __number_of_times_each_philosopher_must_eat__ [nb_meals]: When each philosopher has eaten at least _nb_meals, the simulation stops. If this parameter is not given, the simualtion runs until a philosopher has died.

The goal of this project is to not let any philosopher die.

## Theoretical Background

First, we need to clarify the terms. What is a process? A thread? Deadlock? Mutex?

### Multithreading

Basically, a process is a program which is executing. Every process has at least one thread. If it has mutliple, we speak of multithreading. The advantages of multithreading is that tasks can be split up and done concurrently and not sequentially. This can speed up the program because the threads execute independently. You may already know how to fork processes which is quite similar because the processes also execute independently. However, there is a huge difference between forking and threading a process. If you create a child process by forking, all the resources of the parent process are copied. When you start a thread, however, the resources are not copied but the thread __shares__ the resources, such as memory, with the main thread (the process). This is very important because it can lead to data races. 

Thus, "a process is a unit of resources, while a thread is a unit of scheduling and execution".([Wikipedia "Thread"](https://en.wikipedia.org/wiki/Thread_(computing)))

### Data Races

As all the threads of one process share the same resources, and thus memory and variables, we have to be careful about changing the values of variables. (An exception are of course local variables that are only used by one thread.) If we want to count, for example, the amount of meals eaten by all the philosophers, we could create the variable _int_ *meals*. Each time a philsopher has eaten, they increase the variable. 

<details>
	<summary>Example code</summary>

		#include <pthread.h>
		#include <stdio.h>
		#include <unistd.h>

		int     meals;

		void    *eating(void *arguments)
		{
				int     i;

				i = 0;
				while (i < 10)
				{
						usleep(10);
						meals++;
						i++;
				}
				return (NULL);
		}

		int     main(void)
		{
				pthread_t       philo[100];
				int             i;

				meals = 0;
				i = 0;
				while (i < 100)
				{
						if (pthread_create(&philo[i], NULL, &eating, NULL) != 0)
								return (1);
						i++;
				}
				i = 0;
				while (i < 100)
				{
						if (pthread_join(philo[i], NULL) != 0)
								return (2);
						i++;
			}
			printf("All philosophers have eaten %i meals.\n", meals);
			return (0);
	}

</details>

If we have for example 100 philosophers (threads) that eat 10 meals, we would expect the outcome of *meals* to be:

	100 * 10 = 1000;

However, the outcome varies, each time we run the program:

	vm@Ubuntu-VirtualBox:~$ gcc -g -pthread data_race.c -o data_race
	vm@Ubuntu-VirtualBox:~$ ./data_race 
	All philosophers have eaten 978 meals.
	vm@Ubuntu-VirtualBox:~$ ./data_race 
	All philosophers have eaten 983 meals.
	vm@Ubuntu-VirtualBox:~$ ./data_race 
	All philosophers have eaten 982 meals.  

It never accounts to 1000. Something seems to get lost. This is called a data race or race condition. 

In order to understand this, we need to know two things. First, that each operating system has a scheduler that typically takes care of which thread(s) are executed. If there are a lot of threads not all can be executed at the same time. The scheduler decides which threads run and which pause. The switching between the threads happens so fast, that to us it does not seem as if they pause at all (like moving pictures). Second, we need to understand how the value of a variable is changed. First, the value is read, then the new value computed (in this case increased by 1) and then the change is written.
As threads run simultaneously and may be paused at any line of code, it can happen that a thread is paused just after having read the current value of the variable *meals* (e.g. 50). It waits while another thread increases the value of *meals* and writes the change to it (meals = 51). When the paused thread continues its execution, it does not read the value of *meals* again but continues with increasing the value by 1 (50 + 1) and changes the current value of *meals* (51) to its "new" value "51". Thus, one meal got lost. The problem gets worse, the more threads there are.

#### Tools to detect data races

In order to detect data races, you can compile your program with:

	-fsanitize=thread

<details> <summary> Here is the output of our code example </summary>

ThreadSanitizer gives 1 Warning for our example and points out the line where the problem occured: line 15 where the number of meals is increased. 

	vm@Ubuntu-VirtualBox:~$ gcc -fsanitize=thread -g -pthread data_race.c -o data_race
	vm@Ubuntu-VirtualBox:~$ ./data_race 
	==================
	WARNING: ThreadSanitizer: data race (pid=7475)
	Read of size 4 at 0x55a459c27014 by thread T2:
		#0 eating /home/vm/data_race.c:15 (data_race+0x12c7)

	Previous write of size 4 at 0x55a459c27014 by thread T1:
		#0 eating /home/vm/data_race.c:15 (data_race+0x12df)

	As if synchronized via sleep:
		#0 usleep ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:359 (libtsan.so.0+0x66547)
		#1 eating /home/vm/data_race.c:14 (data_race+0x12b8)

	Location is global 'meals' of size 4 at 0x55a459c27014 (data_race+0x000000004014)

	Thread T2 (tid=7478, running) created by main thread at:
		#0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
		#1 main /home/vm/data_race.c:30 (data_race+0x1381)

	Thread T1 (tid=7477, running) created by main thread at:
		#0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
		#1 main /home/vm/data_race.c:30 (data_race+0x1381)

	SUMMARY: ThreadSanitizer: data race /home/vm/data_race.c:15 in eating
	==================
	All philosophers have eaten 996 meals.
	ThreadSanitizer: reported 1 warnings
</details>

Otherwise, when you use valgrind, you can run it with --tool=helgrind:

	valgrind --tool=helgrind ./program_name

But you cannot run fsanitize and helgrind simultaneously!

### Mutexes

To prevent data races we can protect certain areas of code. This can be done by mutexes. 

	pthread_mutex_t	mutex_meals;

	pthread_mutex_init(&mutex_meals, NULL);
	pthread_mutex_lock(&mutex_meals);
	meals++;
	pthread_mutex_unlock(&mutex_meals);

Mutexes are a special variable type that have been especially created for this purpose. They need to be initialized at the beginning and can then be used to lock a certain line or lines of code. In our example we want to lock the line of code where the number of meals is increased. If one thread has locked the mutex and another thread hits the same line of code (pthread_mutex_lock), the second thread has to wait until the mutex is unlocked again. Thus, a mutex prevents threads from accessing the same memory address (the same data) at the same time. 

If we run the example code with the introduced mutex again, the outcome is right:

 <details> <summary> Example code </summary>

	#include <pthread.h>
	#include <stdio.h>
	#include <unistd.h>

	int     meals;
	pthread_mutex_t mutex_meals;

	void    *eating(void *arguments)
	{
			int     i;

			i = 0;
			while (i < 10)
			{
					usleep(10);
					pthread_mutex_lock(&mutex_meals);
					meals++;
					pthread_mutex_unlock(&mutex_meals);
					i++;
			}
			return (NULL);
	}

	int     main(void)
	{
			pthread_t       philo[100];
			int             i;

			pthread_mutex_init(&mutex_meals, NULL);
			meals = 0;
			i = 0;
			while (i < 100)
			{
					if (pthread_create(&philo[i], NULL, &eating, NULL) != 0)
							return (1);
					i++;
			}
			i = 0;
			while (i < 100)
			{
					if (pthread_join(philo[i], NULL) != 0)
							return (2);
					i++;
			}
			printf("All philosophers have eaten %i meals.\n", meals);
			return (0);
	}
</details>

	vm@Ubuntu-VirtualBox:~$ ./data_race 
	All philosophers have eaten 1000 meals.

### Deadlock

Data races are not the only tricky part about multithreading. Deadlocks are another. The [dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem), on which this 42 project is based, is a famous example problem in computer science. It illustrates well problems that can occur with synchronization, such as deadlocks.

A deadlock is when every thread waits for another thread to provide something (a message, unlock a mutex) and is thus unable to proceed execution. This is best illustrated with the dining philosophers problem:

As all the philsophers (threads) follow the same routine (eat, sleep, think, repeat) they will all start by picking up one fork, then trying to pick up the other fork in order to eat. But when all philosophers pick up, let's say, their right fork first, all the forks will be in use and they will wait forever for the left fork to become available. Philosopher 1 waits for philosopher 2 to put down the fork, philosopher 2 for philosopher 3, ... philosopher n for philosopher 1. All threads will wait, the program will wait - forever. This is a typical deadlock situation.

#### How to Prevent Deadlock 

There are [multiple solutions](https://en.wikipedia.org/wiki/Dining_philosophers_problem) for the dining philosophers problem. I tried two quite common solutions. First, the so-called "resource hierarchy", where every philosopher except for one (the last or first) will pick up their right fork first and then their left fork, whereas one philosopher will pick up their left fork first and then their right fork. Thus, one fork will stay on the table and either the last or first philosopher can start eating.

Lateron, I applied a solution that is very common at 42 and seems to me more efficient. In this solution the philosophers with an even number pick up their right fork first and the odd numbered philsophers their left fork first. Thus, half of the philosophers (rounded down, 2 if 5 philos) can eat simultaneously. Some even let the even numbered philosophers sleep at the beginning of the routine so that they can find a good rhythm. (But that sounds to me a little bit like cheating.)


## Evaluating My Program

The project has not yet been evaluated so I do not know if it passes the three evaluators and their tests or not. 

### Status Quo

Right now, on my VM, about every fifth time or less a philosopher dies when executing:

	./philo 5 800 200 200 6

This could be due to CPU overload (I checked with command top). Therefore, I will leave it as it is and test at 42 computers. 

Right now, I do not know how to make it more efficient. 
- The mutexes only lock for a very short sequence of code
- Odd and even numbered philsophers pick up the forks in different sequence (I could insert a short usleep at the beginning of the routine for the even numbered philosopher but it feels like cheating)
- The sequence of putting down the forks is the same for odd and even numbered philsosophers (changing that did not have an effect)
- I have 1 monitoring thread (supervisor) that checks for each philosopher if they have eaten enough meals and then for all philsophers if they have eaten enough meals in total. I could give the first task to a monitoring thread for each philosopher but I doubt that it speeds things up. This would just mean that there are more threads competing for execution and more mutexes locked and unlocked. 
- When putting down the forks, I also check their status. They should be 1 (as no philo can pick up a fork that is 1) but I just do this error check for myself to be sure that no philosopher steals forks. These lines could be omitted and would maybe add to performance. 

#### Question: Can a philosopher die while eating?

I decided to let a philosopher die even if they are eating and not checking for if they are eating. This means that if t_die = 200 and t_eat = 210, the philsopher dies of starvation although they are currently holding two forks and are eating. I decided to do so, because the subject states that: 

	"If a philosopher didn’t start eating time_to_die milliseconds since the beginning of their last meal or the beginning of the simulation, they die."

It nowhere states that a philosopher cannot die while eating.

#### Tests Passed

	./philo 4 310 200 100

One philosopher should die.

### Valgrind

I ran valgrind and there are no memory leaks.

#### Tests Run

	valgrind ./philo 0 0 0 0 0
	valgrind ./philo 1 0 0 0 0
	valgrind ./philo 1 1 1 1 1
	valgrind ./philo 2147483647 0 0 0 0
	valgrind ./philo 0 2147483647 0 0 0
	valgrind ./philo 0 2147483648 0 0 0
	valgrind ./philo 5 800 200 200 6
	valgrind ./philo 1 800 200 200

Note: As valgrind slows down the program, philosophers die even if they are not supposed to. This is normal.

### Norminette

All *.c files are norminetted. In the header file the comments have to be removed for it to pass norminette. Otherwise, everything is "OK!".

## Usage

Git clone the project and cd in folder:

	cd philosophers

Build the executable by running:

	make

Run the program with the following 4 arguments:

	./philo <nb_of_philosophers> <time_to_die> <time_to_eat> <time_to_sleep>

Optionally, you may add a 5th argument:

	<nb_of_times_each_philosopher_must_eat>

All arguments must be integers between 0 and MAX_INT and represent the time in milliseconds, except for the number of philosophers, which is limited to 500 to not overload the CPU. This number may be changed in the code (check.c). For example:

	./philo 5 800 200 200 6

If nb_of_times_each_philosopher_must_eat is given, the program exits when all philosophers have eaten at least nb_of_times_each_philosopher_must_eat (or earlier if a philsosopher dies). Otherwise, the program  only exits when a philosopher has died. If no one dies, it runs infinitely. 