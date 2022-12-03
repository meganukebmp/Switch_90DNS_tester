#ifndef __DNSTEST_MAIN_H
#define __DNSTEST_MAIN_H

typedef enum RESOLVER_STATUS {
    DNS_BLOCKED,
    DNS_RESOLVED,
    DNS_UNRESOLVED
} RESOLVER_STATUS;

RESOLVER_STATUS resolveHostname(const char* hostname);
void checkHostnames();

// List of hostnames to test
const char *hostnames[] = {
    "nintendo.com",
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
    "nintendoswitch.cn",
    "nintendoswitch.com.cn",
    "nintendoswitch.com",
    "sun.hac.lp1.d4c.nintendo.net",
    "google.com",
    "cloudflare.com",
    "ntp.org",
    "nist.gov",
    "fortheusers.org",
    "github.com",
    "example.com"
};

#endif
