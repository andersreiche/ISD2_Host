
#include "daemon.h"

using namespace std;

void setup_ADC(void) {
    system("echo BB-ADC > /sys/devices/platform/bone_capemgr/slots");
}

void pin_init(void) {
    system("config-pin p8.41 out");
    system(" echo \"out\" > /sys/class/gpio/gpio20/direction");
}

void heaterstate(string state) {
    if (state == "ON"){
        system("echo 1 > /sys/class/gpio/gpio20/value");
        to_syslog("Heater on");
    } else if (state == "OFF") {
        system("echo 0 > /sys/class/gpio/gpio20/value");
        to_syslog("Heater off");
    } else {
        to_syslog("heaterstate was called with faulty parameter: " + state);
    }
}

float get_temp(int value) {
    float milivolts = (value / 4096) * 1800;
    return (milivolts - 500) / 10;
}

int read_ADC(void) {
    int value = 0;
    stringstream ss;
    ss << AIN0;
    fstream fs;
    fs.open(ss.str().c_str(), fstream::in);
    if (fs.is_open()) {
        fs >> value;
        fs.close();
    } else {
        value = -1;
    }
    return value;
}

void to_syslog(string str) {
    openlog("Daemon ", LOG_PID, LOG_USER);
    syslog(LOG_INFO, str.c_str());
    closelog();
}

void sig_handler(int signo) {
    openlog("Daemon ", LOG_PID, LOG_USER);
    if (signo == SIGHUP)
        syslog(LOG_INFO, "SIGHUP recieved, exiting");
    closelog();
    exit(0);
}

void timer_handler(int signo) {
    if (signo == SIGALRM) {
        int ADC_val = read_ADC();
        if (ADC_val == -1) {
            to_syslog("Could not read ADC, try running the program with sudo");
        } else {
            to_syslog("Temperature: " + tostr(get_temp(ADC_val)) + " degC");
        }
    }
}

int toint(std::string s) //The conversion function
{
    return atoi(s.c_str());
}

void heater(float temperature, int target) {
    if (temperature < target) {
        heaterstate("ON");
    } else {
        heaterstate("OFF");
    }
}