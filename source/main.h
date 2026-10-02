#ifndef __DNSTEST_MAIN_H
#define __DNSTEST_MAIN_H

typedef enum RESOLVER_STATUS {
    DNS_BLOCKED,
    DNS_RESOLVED,
    DNS_UNRESOLVED
} RESOLVER_STATUS;

RESOLVER_STATUS resolveHostname(const char* hostname);
RESOLVER_STATUS testHostname(const char* hostname, int width);
void checkHostnames();

// Column widths used when printing hostnames.
// Domains are short and shown two per row; service hosts are long and get one row each.
// Default console is 80x45, so both layouts must stay within 80 columns.
#define DOMAIN_FIELD_WIDTH  22
#define SERVICE_FIELD_WIDTH 40

// Top-level Nintendo domains. Passing these only proves the bare domain is redirected;
// a hosts file without '*' wildcards can pass all of them while subdomains still resolve.
const char *domains[] = {
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
    "nintendo.pl",
    "nintendoswitch.com",
    "nintendoswitch.com.cn",
    "nintendoswitch.cn",
};

// Specific hosts the console actually contacts for updates, downloads, auth and telemetry.
// These are what must be blocked to stop system/game updates from downloading.
const char *serviceHosts[] = {
    "sun.hac.lp1.d4c.nintendo.net",       // system update metadata
    "superfly.hac.lp1.d4c.nintendo.net",  // title version list
    "tagaya.hac.lp1.eshop.nintendo.net",  // title version list (legacy)
    "aqua.hac.lp1.d4c.nintendo.net",      // title metadata
    "atum.hac.lp1.d4c.nintendo.net",      // content download
    "atumn.hac.lp1.d4c.nintendo.net",     // content download (CDN)
    "dauth-lp1.ndas.srv.nintendo.net",    // device authentication
    "aauth-lp1.ndas.srv.nintendo.net",    // application authentication
    "receive-lp1.dg.srv.nintendo.net",    // telemetry
    "receive-lp1.er.srv.nintendo.net",    // error reports
};

#endif
