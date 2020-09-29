#include <stdio.h>
#include <switch.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int resolveHostname(const char* hostname);
void checkHostnames();

// TODO: Handle these
// conntest.nintendowifi.net -> 95.216.149.205
// ctest.cdn.nintendo.net -> 95.216.149.205
// 90dns.test -> 95.216.149.205
const char *hostnames[] = {
	"nintendo.com",
	"nintendo.net",
	"nintendo.jp",
	"nintendo.co.jp",
	"nintendo.co.uk",
	"nintendo-europe.com",
	"nintendowifi.net",
	"nintendo.es",
	"nintendo.co.kr",
	"nintendo.tw",
	"nintendo.com.hk",
	"nintendo.com.au",
	"nintendo.co.nz",
	"nintendo.at",
	"nintendo.be",
	"nintendods.cz",
	"nintendo.dk",
	"nintendo.de",
	"nintendo.fi",
	"nintendo.fr",
	"nintendo.gr",
	"nintendo.hu",
	"nintendo.it",
	"nintendo.nl",
	"nintendo.no",
	"nintendo.pt",
	"nintendo.ru",
	"nintendo.co.za",
	"nintendo.se",
	"nintendo.ch",
	"potato.nintendo.com",
	"nintendoswitch.com",
	"nintendoswitch.com.cn",
	"nintendoswitch.cn"
};

int main(int argc, char **argv)
{
    consoleInit(NULL);
	socketInitializeDefault();

	printf("90DNS Testing Utility\n\n");

	checkHostnames();
	
    // Main loop
    while(appletMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
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

void checkHostnames() 
{
	// iterate through hostnames array
	for (int i = 0; i < sizeof(hostnames)/sizeof(hostnames[0]); ++i)
	{
		printf("Testing %s ...", hostnames[i]);
		// update console here so we get a "live" output
		consoleUpdate(NULL);
		if (resolveHostname(hostnames[i]) == 0) {
			printf("\033[%dC\033[1;32mblocked!\033[0m\n", 40-((int)strlen(hostnames[i])+12));
		}
		else 
		{
			printf("\033[%dC\033[1;31munblocked!\033[0m\n", 40-((int)strlen(hostnames[i])+12));
		}
	}
	printf("\nPress B to exit. Press X to retry.");
}

int resolveHostname(const char* hostname)
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
				return 0;
			}
		}
	}
	return 1;
}
