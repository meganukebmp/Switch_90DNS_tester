#include <stdio.h>
#include <switch.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "main.h"

PrintConsole *console;

int main(int argc, char **argv)
{
    // Initialize default console
    console = consoleGetDefault();
    consoleInit(console);

    // Configure pad for player 1 accepting input from any controller
    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadFullCtrl);
    padInitializeAny(&pad);

    // Initialize sockets
    socketInitializeDefault();
    // Initialize nifm
    nifmInitialize(NifmServiceType_User);

    checkHostnames();

    // Main loop
    while(appletMainLoop())
    {
        // Update the pad inputs
        padUpdate(&pad);

        // Get key pressed on the pad
        u64 kDown = padGetButtonsDown(&pad);

        // Exit on B
        if (kDown & HidNpadButton_B) break;
        // Retry on X
        if (kDown & HidNpadButton_X)
        {
            consoleClear();
            checkHostnames();
        }
    }

    // stop sockets before quit
    socketExit();
    // Quit nifm
    nifmExit();
    consoleExit(console);
    return 0;
}

void checkHostnames() 
{
    printf("90DNS Testing Utility v1.1.0\n\n");

    // Check if system says we're connected to a network (wifi or ethernet)
    Result net_rc = nifmGetInternetConnectionStatus(NULL, NULL, NULL);
    if (R_FAILED(net_rc)) {
        printf(CONSOLE_RED "WARNING, NOT CONNECTED TO ANY NETWORK! AIRPLANE MODE?\n" CONSOLE_RESET);
    }
    int unblocked = 0;
    const int domainCount  = sizeof(domains) / sizeof(domains[0]);
    const int serviceCount = sizeof(serviceHosts) / sizeof(serviceHosts[0]);

    // Top-level domains, two per row to fit the 45-row console
    printf("Domains:\n");
    for (int i = 0; i < domainCount; i += 2)
    {
        if (testHostname(domains[i], DOMAIN_FIELD_WIDTH) == DNS_RESOLVED) unblocked++;
        if (i + 1 < domainCount)
        {
            if (testHostname(domains[i + 1], DOMAIN_FIELD_WIDTH) == DNS_RESOLVED) unblocked++;
        }
        printf("\n");
        consoleUpdate(console);
    }

    // Specific update/download/auth/telemetry hosts, one per row (names are long)
    printf("\nUpdate, download & telemetry hosts:\n");
    for (int i = 0; i < serviceCount; i++)
    {
        if (testHostname(serviceHosts[i], SERVICE_FIELD_WIDTH) == DNS_RESOLVED) unblocked++;
        printf("\n");
        consoleUpdate(console);
    }

    // Overall verdict so a single unblocked service host can't be missed in the list
    if (unblocked == 0)
        printf("\n" CONSOLE_GREEN "All tested hosts are blocked." CONSOLE_RESET "\n");
    else
        printf("\n" CONSOLE_RED "%d host(s) UNBLOCKED - console can reach Nintendo!" CONSOLE_RESET "\n", unblocked);

    printf("\nPress B to exit. Press X to retry.\n");
    consoleUpdate(console);
}

// Resolve one hostname and print it with a padded, colored status.
// Does not print a newline so callers can place two results on one row.
RESOLVER_STATUS testHostname(const char* hostname, int width)
{
    // Print hostname in a fixed-width field, then temporary dots
    printf("  %-*s ...", width, hostname);
    consoleUpdate(console);

    RESOLVER_STATUS result = resolveHostname(hostname);

    // Move back 3 characters to overwrite dots
    printf("\x1b[3D");

    // Print the status padded to 10 chars so the next column lines up
    switch(result) {
        case DNS_BLOCKED:
            printf(CONSOLE_GREEN "%-10s" CONSOLE_RESET, "blocked");
            break;
        case DNS_RESOLVED:
            printf(CONSOLE_RED "%-10s" CONSOLE_RESET, "unblocked");
            break;
        case DNS_UNRESOLVED:
            printf(CONSOLE_YELLOW "%-10s" CONSOLE_RESET, "unresolved");
            break;
    }

    return result;
}

RESOLVER_STATUS resolveHostname(const char* hostname)
{
    struct hostent *he;
    struct in_addr a;
    // use gethostbyname to attempt hostname connection
    he = gethostbyname(hostname);
    if (he)
    {
        // go over all returned addresses for hostname
        while (*he->h_addr_list)
        {
            bcopy(*he->h_addr_list++, (char *) &a, sizeof(a));
            // succeed if any of them redirect to localhost
            if (strcmp(inet_ntoa(a), "127.0.0.1") == 0 || strcmp(inet_ntoa(a), "0.0.0.0") == 0)
            {
                return DNS_BLOCKED;
            }
        }
        return DNS_RESOLVED;
    }
    return DNS_UNRESOLVED;
}
