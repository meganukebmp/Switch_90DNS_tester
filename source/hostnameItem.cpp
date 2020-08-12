#include <borealis.hpp>
#include <string.h>
#include "hostnameItem.h"

HostnameItem::HostnameItem(char* hostname) {
    // Drop last char when copying string (space for terminator)
    strncpy(_hostname, hostname, (sizeof(_hostname)/sizeof(char))-1);
    // TODO: Allocating memory here. Is it getting cleared?
    _listItem = new brls::ListItem(_hostname);
    // Set initial state
    setState(UNKNOWN);
}

brls::ListItem* HostnameItem::getListItem() {
    return _listItem;
}

char *HostnameItem::getHostname() {
    return _hostname;
}

void HostnameItem::setState(HostnameState state) {
    _state = state;
    switch(_state) {
        case UNKNOWN:
            _listItem->setValue("Untested", true);
            break;
        case BLOCKED:
            _listItem->setValue("Blocked", false);
            break;
        case UNBLOCKED:
            _listItem->setValue("Unblocked", true);
            break;
    }
}

