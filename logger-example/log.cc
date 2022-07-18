#include <sst/core/sst_config.h>
#include <sst/core/simulation.h>
#include "log.h"

log::log( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {
    output.init("deadlocksim-" + getName() + "->", 1, 0, SST::Output::STDOUT);

    clock = params.find<std::string>("tickFreq", "1s");
    num_ports = params.find<int64_t>("num_nodes", 1);
    
    // Register the node as a primary component.
	// Then declare that the simulation cannot end until this 
	// primary component declares primaryComponentOKToEndSim();
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

    port = new SST::Link*[num_ports];
    for (int i = 0; i < num_ports; ++i) {
        std::string strport = "port" + std::to_string(i);
        port[i] = configureLink(strport, new SST::Event::Handler<log>(this, &log::messageHandler));
        if (!port[i]) {
            output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
        }
    }

    registerClock(clock, new SST::Clock::Handler<log>(this, &log::tick));
}

void log::setup() {
    // Array of states

    // Array of Requests

    // Array of Time idle
    output.output("Test");
}

bool log::tick( SST::Cycle_t currentCycle ) {

    output.output("Test"); 
    return (false);
}


void log::messageHandler( SST::Event *ev ) { 
    LogEvent *le = dynamic_cast<LogEvent*>(ev);
    if (le != NULL) { 
        output.output("Test");
    }
}