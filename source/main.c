#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <switch.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int resolveHostname(const char* hostname, short shouldValid);
void checkHostnames();

const char* blockhostnames[] = {"nintendo.com",       "nintendo.net",
                                "nintendo.jp",        "nintendo.co.jp",
                                "nintendo.co.uk",     "nintendo-europe.com",
                                "nintendowifi.net",   "nintendo.es",
                                "nintendo.co.kr",     "nintendo.tw",
                                "nintendo.com.hk",    "nintendo.com.au",
                                "nintendo.co.nz",     "nintendo.at",
                                "nintendo.be",        "nintendods.cz",
                                "nintendo.dk",        "nintendo.de",
                                "nintendo.fi",        "nintendo.fr",
                                "nintendo.gr",        "nintendo.hu",
                                "nintendo.it",        "nintendo.nl",
                                "nintendo.no",        "nintendo.pt",
                                "nintendo.ru",        "nintendo.co.za",
                                "nintendo.se",        "nintendo.ch",
                                "nintendoswitch.cn",  "nintendoswitch.com.cn",
                                "nintendoswitch.com", "sun.hac.lp1.d4c.nintendo.net",
                                "potato.nintendo.com"};

const char* validhostnames[] = {"90dns.test", "ctest.cdn.nintendo.net",
                                "conntest.nintendowifi.net"};

int main(int argc, char** argv)
{
    consoleInit(NULL);
    socketInitializeDefault();

    checkHostnames();

    // Main loop
    while (appletMainLoop())
    {
        // Scan all the inputs. This should be done once for each frame
        hidScanInput();

        // get key pressed for player 1
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        // exit on B
        if (kDown & KEY_B) break;
        // retry on X
        if (kDown & KEY_X)
        {
            consoleClear();
            checkHostnames();
        }

        consoleUpdate(NULL);
    }

    // stop sockets before quit
    socketExit();
    consoleExit(NULL);
    return 0;
}

int precheck()
{
    printf("precheck for url 90dns.test ... \n");
    struct hostent* he = gethostbyname("90dns.test");
    if (!he)
    {
        printf("\033[1;31mNot connect to Internet?\033[0m\n");
        return 1;
    }

    if (resolveHostname("90dns.test", 1) == 0)
    {
        printf("\033[1;32mCorrect 90DNS for url 90dns.test!\033[0m\n");
        return 0;
    }

    printf("\033[1;31munPrecheck 90DNS for url 90dns.test failed!\033[0m\n");
    return 1;
}

void checkHostnames()
{
    printf("90DNS Testing Utility\n\n");

    if (precheck())
    {
        // 90dns check failed, block next nintendo url check
        printf("\nPress B to exit. Press X to retry.");
        return;
    }

    printf("\nStart Nintendo url checking ...\n\n");
    size_t incorrect = 0;

    // iterate through validhostname array to see whether dns is correct
    for (int i = 0; i < sizeof(validhostnames) / sizeof(validhostnames[0]); ++i)
    {
        printf("Testing %s ...", validhostnames[i]);
        // update console here so we get a "live" output
        consoleUpdate(NULL);
        if (resolveHostname(validhostnames[i], 1) == 0)
        {
            printf("\033[%dC\033[1;32mCorrect!\033[0m\n",
                   45 - ((int)strlen(validhostnames[i]) + 12));
        }
        else
        {
            printf("\033[%dC\033[1;31mIncorrect IP, Check your dns!\033[0m\n",
                   45 - ((int)strlen(validhostnames[i]) + 12));
            incorrect++;
        }
    }

    // iterate through blockhostnames array
    for (int i = 0; i < sizeof(blockhostnames) / sizeof(blockhostnames[0]); ++i)
    {
        printf("Testing %s ...", blockhostnames[i]);
        // update console here so we get a "live" output
        consoleUpdate(NULL);
        if (resolveHostname(blockhostnames[i], 0) == 0)
        {
            printf("\033[%dC\033[1;32mBlocked!\033[0m\n",
                   45 - ((int)strlen(blockhostnames[i]) + 12));
        }
        else
        {
            printf("\033[%dC\033[1;31mUnblocked!\033[0m\n",
                   45 - ((int)strlen(blockhostnames[i]) + 12));
            incorrect++;
        }
    }

    if (incorrect == 0)
    {
        printf("\033[1;32mEnjoy 90DNS!\033[0m\n");
    }
    else
    {
        printf("\033[1;31mPay Attention with your dns configuration!\033[0m\n");
    }

    printf("\nPress B to exit. Press X to retry.");
}

int resolveHostname(const char* hostname, short shouldValid)
{
    struct hostent* he;
    struct in_addr a;
    // use gethostbyname to attempt hostname connection
    he = gethostbyname(hostname);
    if (he)
    {
        // go over all returned addresses for hostname
        while (*he->h_addr_list)
        {
            bcopy(*he->h_addr_list++, (char*)&a, sizeof(a));
            if (shouldValid)
            {
                // succeed if any of them redirect to these two ip
                if (strcmp(inet_ntoa(a), "207.246.121.77") == 0 ||
                    strcmp(inet_ntoa(a), "95.216.149.205") == 0)
                {
                    return 0;
                }
            }
            else
            {
                // succeed if any of them redirect to localhost
                if (strcmp(inet_ntoa(a), "127.0.0.1") == 0 || strcmp(inet_ntoa(a), "0.0.0.0") == 0)
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}
