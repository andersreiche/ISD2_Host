/* 
 * File:   controller.h
 * Author: Anders
 */

#include "daemon.h"

using namespace std;

void print_help(void) {
    cout << endl << endl << endl << endl << endl << endl << endl << endl << endl;
    cout << "######################################################################" << endl;
    cout << "                          USAGE INFORMATION" << endl;
    cout << "######################################################################" << endl;
    cout << "Launch the process:        sudo ./isd2_host " << endl;
    cout << "listen to process output:  tail -f /var/log/syslog | grep Daemon" << endl;
    cout << "connect directly:          telnet <host_ip = localhost> <port = 1955>" << endl;
    cout << endl;
    cout << "This process uses client side input filtering." << endl;
    cout << "Bypassing with telnet is at your own risk!" << endl;
    cout << endl;
    cout << "######################################################################" << endl;
    cout << "######################################################################" << endl;
    cout << endl;
}

void setup_ADC(void) {
    system("echo BB-ADC > /sys/devices/platform/bone_capemgr/slots");
}

/* TODO: if expansion is needed, consider making this function general*/
void pin_init(void) {
    /* configure p9.12 as digital output and set pin low*/
    system("echo 60 > /sys/class/gpio/export");
    system("echo out > /sys/class/gpio/gpio60/direction");
    system("echo 0 > /sys/class/gpio/gpio60/value");
    

}

void heaterstate(string state) {
    if (state == "ON"){
        system("echo 1 > /sys/class/gpio/gpio60/value");
        to_syslog("Heater on");
    } else if (state == "OFF") {
        system("echo 0 > /sys/class/gpio/gpio60/value");
        to_syslog("Heater off");
    } else {
        to_syslog("heaterstate was called with faulty parameter: " + state);
    }
}

float get_temp(int value) {
    float temp = value * (1.80f / 4096.0f);
    return (100 * temp);
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

void heater(int state) {
    if (state) {
        heaterstate("ON");
    } else {
        heaterstate("OFF");
    }
}