#ifndef _node_H
#define _node_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>

#define QUEUE_NOT_FULL 0
#define QUEUE_FULL 1

class node : public SST::Component {

public: 
	node( SST::ComponentId_t id, SST::Params& params ); // Constructor
	~node(); // Deconstructor

	bool tick( SST::Cycle_t currentCycle); // 

	void recvEvent(SST::Event *ev);
	void creditEvent(SST::Event *ev);
	
	// Register the component for lookup via sst-info
	SST_ELI_REGISTER_COMPONENT(
		node, // class
		"deadlock", // element library
		"node", // component
		SST_ELI_ELEMENT_VERSION( 1, 0, 0 ), // current element version
		"nodes that send and receive data circularly. Used to demonstrate a node.", // description of component.
		COMPONENT_CATEGORY_UNCATEGORIZED // * Not grouped in a category. (No category to filter with via sst-info).
	)
	
	// Parameters, description, default value
	SST_ELI_DOCUMENT_PARAMS(
		{"queueMaxSize", "The size of the node's queue.", "50"},
	)

	// Port name, description, event type
	SST_ELI_DOCUMENT_PORTS(
		{"recvPort", "Port which outputs a message into the next nodes queue", {"sst.Interfaces.StringEvent"}},
		{"creditPort", "Port which sends credit info to previous node.", {"sst.Interfaces.StringEvent"}}
	)

private:
	SST::Output output; // SST Output object for printing to console synchronously (?)
	int queueMaxSize; // Max size of nodes queue.
	int queueCurrSize; // Current size of nodes queue.
	int queueCredits; // Amount of space left in the next linked nodes queue. 
	int64_t randSeed; // Seed for MarsagliaRNG
	SST::RNG::MarsagliaRNG *rng; //

	void sendMessage(); // Sends a single message across a link from one node to a connected nodes queue.
	void sendCredits(); // Sends number of credits to previous node in circular list.
	void addMessage(); // Utilizes RNG to add messages to each node to simulate messages added from external sources.
	SST::Link *recvPort; // Pointer to queue port
	SST::Link *creditPort; // Pointer to port that will send # of credits to previous node.

	std::string clock; // Defining a clock which can be described via unit math as a string (?).
};

#endif
