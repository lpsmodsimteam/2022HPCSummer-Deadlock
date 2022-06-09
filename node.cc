/**
 * Simple node simulation
 *
 * ~~Very Simple example of a circular wait deadlock that will be expanded on.
 * W.I.P.
 * */

#include <sst/core/sst_config.h> 
#include <sst/core/interfaces/stringEvent.h> // Include stringEvent event type.
#include "node.h" // Element header file.

using SST::Interfaces::StringEvent; 

// Constructor definition
node::node( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
	output.init("deadlocksim-" + getName() + "->", 1, 0, SST::Output::STDOUT); // Formatting output for console.

	// Get parameters
	queueMaxSize = params.find<int64_t>("queueMaxSize", 50);
	clock = params.find<std::string>("tickFreq", "10s");

	// Initialize Variables
	queueCredits = 0;
	queueCurrSize = 0;

	// Register the node as a primary component.
	// Then declare that the simulation cannot end until this 
	// primary component declares primaryComponentOKToEndSim();
	registerAsPrimaryComponent(); 
	primaryComponentDoNotEndSim();

	// Set Main Clock
	// Handler object is created with a reference to this object and a pointer to
	// a function that is called on every clock tick event (?).
	registerClock(clock, new SST::Clock::Handler<node>(this, &node::tick));
	
	// Configure the port for receiving a message from a node.
	recvPort = configureLink("recvPort", new SST::Event::Handler<node>(this, &node::recvEvent));
	// Check if port exist. Error out if not
	if (!recvPort) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'output'\n");
	}

	// Configure our port for returning credit information to a node.
	creditPort = configureLink("creditPort", new SST::Event::Handler<node>(this, &node::creditEvent));
	// Check if port exist. Error out if not
	if ( !creditPort ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'creditPort'\n");
	}
}

// Deconstructor definition
node::~node() {

}

bool node::tick( SST::Cycle_t currentCycle ) {
	//Testing simulation, just trying to get some output.
	output.output("Sending 10 times...");
	
	// Replace with output
	std::cout << "Size of queue: " << queueCurrSize << std::endl;

	// Checking if queue size is full if linked nodes queue size is full.
	if (queueCurrSize == 10) {
		// This will need to be edited, it will end when its full 
		// but not necessarily when a node exists.
		primaryComponentOKToEndSim();
		return(true);
	}

	// Rng and collect messages and add to queue size.

	// Send a message out every tick if the next nodes queue is not full.
	if (queueCredits > queueMaxSize) {
		sendMessage();
	}

	// Send credits back to previous node.
	sendCredits();
	return(false);
}

// Receive payload from node.
void node::recvEvent(SST::Event *ev) {
	// StringEvent is unnecessary 
	queueCurrSize++; // Increment queue size.

}

// Receive # of credits from node.
void node::creditEvent(SST::Event *ev) {
	StringEvent *se = dynamic_cast<StringEvent*>(ev);
	if ( se != NULL ) {
		queueCredits = atoi(&(se->getString().c_str()[0]));
	}
	delete ev;
}

// Simulate sending a single message out to linked component in composition.
void node::sendMessage() {
	queueCurrSize--; 
	recvPort->send(new StringEvent(std::to_string(5)));
}

// Send # of slots in queue to previous connected node.
void node::sendCredits() {
	creditPort->send(new StringEvent(std::to_string(queueCurrSize)));
}

// Simulation purposes, add messages random to a nodes queue.
void node::addMessage() {

}


