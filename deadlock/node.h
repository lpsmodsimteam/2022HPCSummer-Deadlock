#ifndef _node_H
#define _node_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include "CommunicationEvents.h"

#define IDLE 0
#define EXECUTING 1

/**
 * @brief Node Component Class. The Node generates or passes along messages in its queue
 * to connected node components. 
 * 
 */
class node : public SST::Component {

public: 
	/**
	 * @brief Construct a new node component for the simulation composition. 
	 * Occurs before the simulation starts.
	 * 
	 * @param id Component ID tracked by the simulator.
	 * @param params Parameters passed in via the Python driver file.
	 */
	node( SST::ComponentId_t id, SST::Params& params );

	/**
	 * @brief Destroy the node component. Occurs after the simulation is finished.
	 * 
	 */
	~node();

	/**
	 * @brief Setup Phase. This member runs after every component has been constructed but before the simulation has started.
	 * 
	 */
	void setup();

	/**
	 * @brief Finish Phase. This function runs before every component is deconstructed in the simulator composition.
	 * 
	 */
	void finish();

	/**
	 * @brief Contains node's behavior that is run every time it ticks.
	 * 
	 * @param currentCycle Current cycle the component has 
	 * @return true Component is finished.
	 * @return false Component is not finished.
	 */
	bool tick( SST::Cycle_t currentCycle); 

	/**
	 * @brief Handles incoming messages and status messages.
	 * Determines if a message should be consumed or added to the node's queue.
	 * 
	 * @param ev Message Event that the component received.
	 */
	void messageHandler(SST::Event *ev); 

	/**
	 * @brief Handles credit information received from connected nodes.
	 * 
	 * @param ev Credit Event that the component received.
	 */
	void creditHandler(SST::Event *ev);

	int generated; // Lock so that if a node generates a message it will not also send out a message from its queue as well in one tick.

	
	/**
	 * @brief Macro for registering a component into SST and generate info for SST-Info
	 * 
	 */
	SST_ELI_REGISTER_COMPONENT(
		node, // class
		"deadlock", // element library
		"node", // component
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ), // current element version
		"nodes that send and receive data circularly. Used to demonstrate a communication deadlock.", // description of component.
		COMPONENT_CATEGORY_UNCATEGORIZED // * Not grouped in a category. (No category to filter with via sst-info).
	)
	
	/**
	 * @brief Macro for documenting a component's parameters for SST-Info. 
	 * Layout is: parameter name, description, default value.
	 */
	SST_ELI_DOCUMENT_PARAMS(
		{"queueMaxSize", "The size of the node's queue.", "50"},
		{"tickFreq", "The frequency the component is called at.", "10s"},
		{"id", "ID for the node.", "1"},
		{"total_nodes", "Number of nodes in simulation.", "1"},
	)

	/**
	 * @brief Macro for documenting a component's ports for SST-Info.
	 * Layout is: port name, description, event
	 */
	SST_ELI_DOCUMENT_PORTS(
		{"nextPort", "Port which receives credit probe from the next node.", {"MessageEvent"}},
		{"prevPort", "Port which receives Message info from previous node.", {"CreditEvent"}}
	)

private:
	SST::Output output; // SST Output object for printing to console synchronously (?)

	std::queue<Message> msgqueue; // Queue data type that stores Message structures.
	int queueMaxSize; // Max size of node's queue.
	int queueCurrSize; // Current size of node's queue.
	int queueCredits; // Amount of space left in the next linked node's queue. 

	int64_t randSeed; // Seed for MarsagliaRNG
	SST::RNG::MarsagliaRNG *rng;	

	int node_id; // Node's id
	int total_nodes; // Total number of nodes in simulation.

	void sendMessage(); // Sends a single message across a link from one node to a connected node's queue.
	void sendCredits(); // Sends number of credits to previous node in circular list.
	void addMessage(); // Utilizes RNG to create a message and send it out from a node.

	SST::Link *nextPort; // Pointer to queue port
	SST::Link *prevPort; // Pointer to port that will send # of credits to previous node.

	std::string clock; // Node's clock which accepts unit math as a string.

	bool node_state; // Determines what state the node is in. 
	int idle_duration; // Captures the duration the time has been idle.
	int block_requests; // Amount of times the node has attempted to send a message to a node connected to it.

};

#endif
