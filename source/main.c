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
    printf("Testing:\n");

    for (int i = 0; i < sizeof(hostnames) / sizeof(hostnames[0]); i++)
    {
        // Print hostname in a fixed-width field, then temporary dots
        printf("  %-30s ...", hostnames[i]);
        consoleUpdate(console);

        int result = resolveHostname(hostnames[i]);

        // Move back 3 characters to overwrite dots
        printf("\x1b[3D");

        // Print the status
        switch(result) {
            case DNS_BLOCKED:
                printf(CONSOLE_GREEN "blocked" CONSOLE_RESET);
                break;
            case DNS_RESOLVED:
                printf(CONSOLE_RED "unblocked" CONSOLE_RESET);
                break;
            case DNS_UNRESOLVED:
                printf(CONSOLE_YELLOW "unresolved" CONSOLE_RESET);
                break;
        }

        // End the line cleanly
        printf("\n");
        consoleUpdate(console);
    }

printf("\nPress B to exit. Press X to retry.\n");
consoleUpdate(console);
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
