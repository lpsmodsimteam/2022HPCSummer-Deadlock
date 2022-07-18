#ifndef _log_H
#define _log_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include "CommunicationEvents.h"

#define EXECUTING
#define LOGGING

class log : public SST::Component {

public:
    log( SST::ComponentId_t id, SST::Params& params );
    ~log();

    void setup(); 

    bool tick( SST::Cycle_t currentCycle );

    void messageHandler(SST::Event *ev);

    SST_ELI_REGISTER_COMPONENT(
        log,
        "deadlocklog",
        "log",
        SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
        "logging node that keeps track of nodes communicating in a one directional ring topology.",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        {"tickFreq", "The frequency the component is called at.", "1s"},
        {"num_nodes", "The number of nodes that the logger is logging.", "1"},
    )

    SST_ELI_DOCUMENT_PORTS(
        {"port%d", "Receives logging info from connected nodes", { "LogEvent" }},
    )

private:
    SST::Output output;
    SST::Output csvout;

    SST::Link **port;

    std::string clock;
    int num_ports;
    
    int *stateArray;
    int *idleArray;
    int *requestArray;

    uint safe_to_end;
};

#endif