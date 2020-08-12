#ifndef NETWORKER_H
#define NETWORKER_H

int testHostname(const char* hostname);
void workerFunction(std::atomic<bool> &runningFlag, std::atomic<bool> &terminate, std::vector<HostnameItem> &v);

#endif // NETWORKING_H