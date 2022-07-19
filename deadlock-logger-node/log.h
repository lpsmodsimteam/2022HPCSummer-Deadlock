/// \file
#ifndef _log_H
#define _log_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include "CommunicationEvents.h"

/**
 * @brief Log Component Class. The log node collects information regarding all other nodes to determine 
 * if a system-level deadlock is occuring.
 */
class log : public SST::Component {

public:
    /**
     * @brief Construct a new log component for the simulation composition.
     * Occurs before the simulation starts.
     * 
     * @param id Component ID tracked by the simulator. 
     * @param params Parameters passed in via the Python driver file.
     */
    log( SST::ComponentId_t id, SST::Params& params );

    /**
     * @brief Deconstruct the log component. Occurs after the simulation is finished.
     * 
     */
    ~log();

    /**
     * @brief Setup phase. This member runs after every component has been constructed. 
     * 
     */
    void setup(); 

    /**
     * @brief Contains logging node's behavior that is run every time it ticks. 
     * 
     * @param currentCycle Current cycle of the component.
     * @return true Component is finished running.
     * @return false Component is not finished running.
     */
    bool tick( SST::Cycle_t currentCycle );
    
    /**
     * @brief Handles log information received from a node. 
     * 
     * @param ev LogEvent that the component received. 
     */
    void messageHandler(SST::Event *ev);

    /**
     * Currently ignoring SST_ELI Macros as they break doxygen. 
     * \cond
     */
    /**
	 * @brief Macro for registering a component into SST and generate info for SST-Info
	 * 
	 */
    SST_ELI_REGISTER_COMPONENT(
        log, // class
        "deadlocklog", // element library
        "log", // component
        SST_ELI_ELEMENT_VERSION( 1, 0, 0 ), // current element version
        "logging node that keeps track of nodes communicating in a one directional ring topology.", // description of component.
        COMPONENT_CATEGORY_UNCATEGORIZED // * Not grouped in a category. (No category to filter with via sst-info).
    )

    /**
	 * @brief Macro for documenting a component's parameters for SST-Info. Layout is: parameter name, description, default value.
	 * 
	 */
    SST_ELI_DOCUMENT_PARAMS(
        {"tickFreq", "The frequency the component is called at.", "1s"},
        {"num_nodes", "The number of nodes that the logger is logging.", "1"},
    )

    /**
	 * @brief Macro for documenting a component's ports for SST-Info. Layout is: port name, description, event.
	 * 
	 */
    SST_ELI_DOCUMENT_PORTS(
        {"port%d", "Receives logging info from connected nodes", { "LogEvent" }},
    )

private:
    SST::Output output; //!< SST Output object for printing to the console.
    SST::Output csvout; //!< SST Output object for printing to a csv file.

    SST::Link **port; //!< Pointer to an array of port pointers. Allows for variable number of ports to be dynamically allocated.

    std::string clock; //!< Logger Node's clock which accepts unit math as a string. (i.e. "1ms").
    int num_ports; //!< Number of ports that the logger node has.
    
    int *stateArray; //!< Pointer to data for each node's current state.
    int *idleArray; //!< Pointer to data for each node's time idle.
    int *requestArray; //!< Pointer to data for each node's number of requests to send a message.

    bool deadlocked; //!< Declares if system is in deadlock.
};

#endif