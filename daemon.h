
#ifndef DAEMON_H
#define DAEMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <sstream>
#include <pthread.h>
#include <stdlib.h>
#include <fstream>

#define AIN0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define USAGE_MSG "Usage: set <temperature> (takes temperatures between 20 and 50) \n"

using namespace std;

void setup_ADC(void);
float get_temp(int value);
int read_ADC(void);
void to_syslog(string str);
void sig_handler(int signo);
void timer_handler (int signo);
int toint(std::string s);
void heater(float temperature, int target);

template <typename T> string tostr(const T& t) {
    ostringstream os;
    os << t;
    return os.str();
}

//void get_input(void); // maybe unneccesary

#endif /* DAEMON_H */