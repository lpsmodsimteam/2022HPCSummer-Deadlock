/// \file
#include <sst/core/sst_config.h>
#include <sst/core/simulation.h>
#include "log.h"

log::log( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {
    // Configure console output and data output to a csv file.
    output.init("deadlocksim-" + getName() + "->", 1, 0, SST::Output::STDOUT);
    csvout.init("CSVOUT", 1, 0, SST::Output::FILE, "output/log_data.csv");
    csvout.output("Time,Node,Node State Changes,Idle Time,Resource Requests\n");

    // Parameters
    clock = params.find<std::string>("tickFreq", "1s");
    num_ports = params.find<int64_t>("num_nodes", 1);
    idle_threshold = params.find<int64_t>("idle_threshold", 50);
    request_threshold = params.find<int64_t>("idle_threshold", 50);
    
    // Arrays
    idleArray = (int*) malloc(num_ports * sizeof(int));
    stateArray = (int*) malloc(num_ports * sizeof(int));
    requestArray = (int*) malloc(num_ports * sizeof(int));

    // Register the node as a primary component.
	// Then declare that the simulation cannot end until this 
	// primary component declares primaryComponentOKToEndSim();
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

    // Configure a variable number of ports.
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

log::~log() {
    free(idleArray);
    free(stateArray);
    free(stateChanges);
    free(requestArray);
}

void log::setup() {
    deadlocked = false;
    idleArray = (int*) calloc(num_ports, sizeof(int)); 
    stateArray = (int*) calloc(num_ports, sizeof(int)); 
    stateChanges = (int*) calloc(num_ports, sizeof(int));
    requestArray = (int*) calloc(num_ports, sizeof(int)); 
}

bool log::tick( SST::Cycle_t currentCycle ) { 

    // Console output.
    for(int i = 0; i < num_ports; ++i) {
        output.output(CALL_INFO, "Node %d: Current State: %d, Consecutive Cycles Idle: %d, Consecutive Queue Request: %d\n", i, stateArray[i], idleArray[i], requestArray[i]);
        csvout.output("%ld,Node_%d,%d,%d,%d\n", getCurrentSimTime(), i, stateChanges[i], idleArray[i], requestArray[i]);
    }
    output.output("\n");

    // Check if all monitored nodes exceed the conditions to declare deadlock.
    for(int i = 0; i < num_ports; ++i) {
        if (stateArray[i] == 0 && idleArray[i] > idle_threshold && requestArray[i] > request_threshold) {
            deadlocked = true;
        } else {
            deadlocked = false;
            break;
        }
    }

    // If deadlocked, end simulation.
    if (deadlocked) {
        output.output(CALL_INFO, "Detected Deadlock. Ending Simulation.\n");
        primaryComponentOKToEndSim();
    }

    // For situations in which deadlock does not, end early and collect data.
    /**if (currentCycle == 1000) {
        output.output(CALL_INFO, "Ending at %ld\n", currentCycle);
        primaryComponentOKToEndSim();
    }*/
    

    return (false);
}


void log::messageHandler( SST::Event *ev ) { 
    LogEvent *le = dynamic_cast<LogEvent*>(ev);
    if (le != NULL) { 
        output.output("Receiving log from node %d\n", le->log.node_id); 
        idleArray[le->log.node_id] = le->log.idle_time;
        requestArray[le->log.node_id] = le->log.num_requests;
        if(stateArray[le->log.node_id] != le->log.node_status) {
            stateChanges[le->log.node_id] += 1;
        }
    }
}