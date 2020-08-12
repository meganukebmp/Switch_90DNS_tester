#ifndef HOSTNAMEITEM_H
#define HOSTNAMEITEM_H

enum HostnameState {
    UNKNOWN,
    BLOCKED,
    UNBLOCKED
};

// TODO: add thumbnails
// TODO: implement destructor to cleanup
class HostnameItem {
    public:
        HostnameItem(char* hostname);
        //~HostnameItem();
        brls::ListItem* getListItem();
        char *getHostname();
        void setState(HostnameState state);

    private:
        char _hostname[512];
        HostnameState _state;
        brls::ListItem* _listItem;
};

#endif // HOSTNAMEITEM_H