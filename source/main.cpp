#include <stdio.h>
#include <vector>
#include <atomic>
#include <thread>
#include <switch.h>
#include <string.h>
#include <borealis.hpp>
#include "hostnameItem.h"
#include "netWorker.h"

std::atomic<bool> workerRunning(false);
std::atomic<bool> workerTerminate(false);

std::thread workerThread;

int main(int argc, char **argv) {

    // Set Borealis log level
    brls::Logger::setLogLevel(brls::LogLevel::DEBUG);

    // Attempt to initialize Borealis
    if (!brls::Application::init("Borealis example"))
    {
        brls::Logger::error("Unable to init Borealis application");
        return EXIT_FAILURE;
    }

    // Attempt to open hostnames file
    FILE* hostnamesFile;
    if (!(hostnamesFile = fopen("romfs:/hostnames.list", "r"))) {
        brls::Logger::error("Failed to load hostnames list");
        return EXIT_FAILURE;   
    }

    // Create vector for storing hostname state items
    std::vector<HostnameItem> hostnamesVector;

    // Create all hostname items and push them on to the vector
    while (!feof(hostnamesFile)) {
        char hostname[512];
        // drop last char (space for terminator)
        fgets(hostname, (sizeof(hostname)/sizeof(char))-1, hostnamesFile);
        // strip trailing newline if it exists
        int len = strlen(hostname);
        if (len) {
            if (hostname[len-1] == '\n') {
                hostname[len-1] = '\0';
            }
        }
        HostnameItem item(hostname);
        hostnamesVector.push_back(item);
    }
    // Close open file handle
    fclose(hostnamesFile);

    // Create root frame
    brls::AppletFrame* rootFrame = new brls::AppletFrame(true, true);
    rootFrame->setTitle("90DNS Testing Utlity");
    rootFrame->setFooterText(APP_VERSION);
    rootFrame->setIcon("romfs:/icon.png");

    // Box Layout for the split layout. Controls on top, list on the bottom
    brls::BoxLayout* containerLayout = new brls::BoxLayout(brls::BoxLayoutOrientation::VERTICAL);

    // The controls layout (top)
    brls::BoxLayout* controlLayout = new brls::BoxLayout(brls::BoxLayoutOrientation::HORIZONTAL);
    controlLayout->setHeight(100);
    controlLayout->setSpacing(30);
    controlLayout->setMargins(16, 60, 16, 60);

    // Test all button
    brls::Button* testAllButton = new brls::Button();
    testAllButton->setLabel("Test All");
    testAllButton->setHeight(72);
    testAllButton->setWidth(200);

    // Progress spinner for when the thread is running
    brls::ProgressSpinner* spinner = new brls::ProgressSpinner();
    spinner->setHeight(72);
    spinner->setWidth(72);
    // Start off hidden. Skip animating
    spinner->hide([](){}, false);

    // Handle button click event
    testAllButton->getClickEvent()->subscribe(
        [testAllButton, &hostnamesVector, &spinner](brls::View* view) {
        // spawn a worker thread 
        workerThread = std::thread(workerFunction,
                                   std::ref(workerRunning),
                                   std::ref(workerTerminate),
                                   std::ref(hostnamesVector));
        // disable the button and show the spinner
        testAllButton->setState(brls::ButtonState::DISABLED);
        spinner->show([](){});
    });

    // Information label
    brls::Label* singleTestLabel = new brls::Label(brls::LabelStyle::REGULAR,
                                                   "Press any hostname to test just that one");

    // Hostnames list (bottom)
    brls::List* hostnamesList = new brls::List();

    // Push all hostnames onto the list
    for (unsigned long i=0; i<hostnamesVector.size(); i++) {
        hostnamesList->addView(hostnamesVector[i].getListItem());
    }

    // Put all elements onto the control layout (top)
    controlLayout->addView(testAllButton);
    controlLayout->addView(singleTestLabel);
    controlLayout->addView(spinner);

    // Put the list and control layouts onto the split container
    containerLayout->addView(controlLayout);
    containerLayout->addView(hostnamesList, true);
    // Put the split as root content
    rootFrame->setContentView(containerLayout);

    // Push root frame on to display stack
    brls::Application::pushView(rootFrame);

    // Main loop
    while (brls::Application::mainLoop()) {
        // If there is a running worker thread AND it's flag is set to false that thread must have
        // signalled that it is finished. Attempt to join it.
        // Since this effectively fires only once, this is a good place to put other events that
        // we might need to do once a thread finishes, such as starting and stopping animations.
        if (workerThread.joinable()) {
            if (!workerRunning) {
                workerThread.join();
                // make the button clickable again
                testAllButton->setState(brls::ButtonState::ENABLED);
                // hide the spinner
                spinner->hide([](){});
            }
        }
    }

    // Tell the worker thread to terminate
    workerTerminate = true;

    // always wait for the thread to finish before exiting
    if (workerThread.joinable()) workerThread.join();

    // Exit
    return EXIT_SUCCESS;
}
