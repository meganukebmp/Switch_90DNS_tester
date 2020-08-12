#include <vector>
#include <atomic>
#include <netdb.h>
#include <thread>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <borealis.hpp>
#include "hostnameItem.h"
#include "netWorker.h"

int testHostname(const char* hostname) {
    // attempt to resolve hostname
    struct addrinfo *result;
    if (getaddrinfo(hostname, NULL, NULL, &result)) {
        return 2;
    }

    // allocate enough characters for an ipv4 address
    char ipAddress[16];
    // extract only ipv4 chunk of the addrinfo struct (casting sockaddr to sockaddr_in)
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)result->ai_addr;
    // convert to textual representation
    inet_ntop(AF_INET, &(ipv4->sin_addr), ipAddress, 16);

    // Check if blocked
    if (strcmp(ipAddress, "127.0.0.1") == 0 || strcmp(ipAddress, "0.0.0.0") == 0) {
        // free up memory
        freeaddrinfo(result);
        return 0;
    }
    // free up memory
    freeaddrinfo(result);
    return 1;
}

// The function called by the worker thread
void workerFunction(std::atomic<bool> &runningFlag, std::atomic<bool> &terminate,
    std::vector<HostnameItem> &v) {

    // set the thread running flag
    runningFlag = true;
    // iterate through input hostnames and test them
    for (unsigned long i=0; i < v.size(); i++) {
        // stop iterator on termination request
        if (terminate) break;

        // TODO: shrink/optimize this (make test return enums)
        int res = testHostname(v[i].getHostname());
        switch(res) {
            case 0:
                v[i].setState(BLOCKED);
                break;
            case 1:
                v[i].setState(UNBLOCKED);
                break;
            case 2:
                v[i].setState(UNKNOWN);
                break;
        }
    }
    // unset the thread running flag
    runningFlag = false;
}