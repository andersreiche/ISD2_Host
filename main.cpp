/* 
 * File:   controller.h
 * Author: Anders
 */

#include <iostream>
#include "TCPServer.h"
#include "daemon.h"


TCPServer tcp;
using namespace std;

void * loop(void * m) {
    pthread_detach(pthread_self());
    while (1) {
        string str = tcp.getMessage();
        if (str != "") {
            if (str == "GETTEMP") {
                tcp.Send(tostr(get_temp(read_ADC())));
            } else if (str.length() == 1) {
               heater(toint(str));
            }
            tcp.clean();
        }
        sleep(1);
    }
    tcp.detach();
}

int main(int argc, char * argv[]) {

    if (argc > 1) {
        if (string(argv[1]) == "--help" || string(argv[1]) == "-h") {
            print_help();
        }
        exit(0);
    }

    //////// Create an orphan ////////
    pid_t process_id = fork(); // Create child process
    if (process_id < 0) { // Indication of fork() failure
        printf("fork failed!\n");
        exit(1);
    }

    if (process_id > 0) { // PARENT PROCESS. Need to kill it.
        printf("process_id of child process %d \n", process_id);
        exit(0);
    }

    umask(0); //unmask the file mode granting read/write/exe rights
    pid_t sid = setsid(); //set new session
    if (sid < 0) {
        to_syslog("sid < 0, closing process");
        exit(EXIT_FAILURE);
    }

    setup_ADC(); // echo the device tree overlay to the cape manager
    pin_init(); // sets the neccesary pins

    //////// Daemonizing ////////
    chdir("/"); // Change the current working directory to root.
    // Close stdin. stdout and stderr, daemonizing the process
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    to_syslog("Started daemonized process");

    // Calls the signal handler if SIGHUP is recieved
    if (signal(SIGHUP, sig_handler) == SIG_ERR) {
        to_syslog("signal handler returned an error, closing process");
        exit(EXIT_FAILURE);
    }

    //////// TIMER SECTION ////////
    struct itimerval timer;

    // install signal_handler as the signal handler for SIGVTALRM.
    if (signal(SIGALRM, timer_handler) == SIG_ERR) {
        to_syslog("Could not install signal handler, closing process");
        exit(EXIT_FAILURE);
    }

    // Configure the timer to expire after 15 sec.
    timer.it_value.tv_sec = 5;
    timer.it_value.tv_usec = 0;

    // And every 15 seconds after that
    timer.it_interval.tv_sec = 5;
    timer.it_interval.tv_usec = 0;

    if (setitimer(ITIMER_REAL, &timer, NULL)) {
        to_syslog("Could not install timer, closing process");
        exit(EXIT_FAILURE);
    }

    pthread_t msg;
    tcp.setup(1955);
    while (1) {
        if (pthread_create(&msg, NULL, loop, (void *) 0) == 0) {
            to_syslog("Waiting for connecton...");
            tcp.receive();
        } else {
            to_syslog("Could not create thread: loop");
        }
    }
    to_syslog("main exited with return code 1");
    return 1;
}