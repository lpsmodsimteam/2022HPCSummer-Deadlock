/**
 * Simple node simulation
 *
 * ~~Very Simple example of a circular wait deadlock that will be expanded on.
 * W.I.P.
 * */

#include <sst/core/sst_config.h> 
#include <sst/core/interfaces/stringEvent.h> // Include stringEvent event type.
#include <sst/core/simulation.h>
#include <sst/core/stopAction.h>
#include "node.h" // Element header file.

using SST::Interfaces::StringEvent; 

// Constructor definition
node::node( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
	output.init("deadlocksim-" + getName() + "->", 1, 0, SST::Output::STDOUT); // Formatting output for console.

	// Get parameters
	queueMaxSize = params.find<int64_t>("queueMaxSize", 50);
	clock = params.find<std::string>("tickFreq", "10s");
	randSeed = params.find<int64_t>("randseed", 112233);

	// Initialize Variables
	//queueCredits = queueMaxSize;
	queueCurrSize = 0;
	queueCredits = 1; // Arbitrary non-zero number since this will be overwritten after the first tick.

	// Initialize Random
	rng = new SST::RNG::MarsagliaRNG(10, randSeed); // Create a Marsaglia RNG with a default value and a random seed.

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
	nextPort = configureLink("nextPort", new SST::Event::Handler<node>(this, &node::testMsgEvent));
	// Check if port exist. Error out if not
	if ( !nextPort ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'nextPort'\n");
	}

	// Configure our port for returning credit information to a node.
	prevPort = configureLink("prevPort", new SST::Event::Handler<node>(this, &node::testMsgEvent));
	// Check if port exist. Error out if not
	if ( !prevPort ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'prevPort'\n");
	}
}

// Deconstructor definition
node::~node() {

}

bool node::tick( SST::Cycle_t currentCycle ) {
	// Replace with output
	output.output(CALL_INFO, "Size of queue: %d\n", queueCurrSize);
	output.output(CALL_INFO, "Amount of credits: %d\n", queueCredits);
	std::cout << "Sim-Time: " << getCurrentSimTimeNano() << std::endl;


	// Checking if no credits are available.
	if ( queueCredits <= 0 ) {
		// This will need to be edited, it will end when its full 
		// but not necessarily when a node exists.
		output.output(CALL_INFO, "DEADLOCKED (kinda)\n");

		MessageTypes type = STATUS;
		StatusTypes status = WAITING;
		int numCredits = queueMaxSize - queueCurrSize;
		struct Message msg = { getName(), type, status, numCredits };
		nextPort->send(new BaseMessageEvent(msg));
		
		//primaryComponentOKToEndSim();
		//return(true);
	}
	

	// Rng and collect messages and add to queue size.
	if (queueCurrSize < queueMaxSize) {
		addMessage();
	}

	// Send a message out every tick if the next nodes queue is not full,
	// and if the node has messages in its queue to send.
	if (queueCredits > 0 && queueCurrSize > 0) {

		output.output(CALL_INFO, "Sending Message\n");
		sendMessage();
	}

	// Send credits back to previous node.
	sendCreditsMsg();
	return(false);
}

/**
// Receive message from node.
void node::recvEvent(SST::Event *ev) {
	// StringEvent is unnecessary 
	queueCurrSize++; // Increment queue size.
	delete ev;

}

// Receive # of credits from node.
void node::creditEvent(SST::Event *ev) {
	StringEvent *se = dynamic_cast<StringEvent*>(ev);
	if ( se != NULL ) {
		queueCredits = atoi(&(se->getString().c_str()[0]));
	}
	delete ev;
}
**/

void node::handleEvent(SST::Event *ev) {
	StringEvent *se = dynamic_cast<StringEvent*>(ev);
	if ( se != NULL ) {
		// Check if the event is from a node sending a message.
		if (se->getString().compare("Sending") == 0) {
			queueCurrSize++; // Increment queue size.
		} else {
			// Otherwise the node is receiving credit information from the next node it's connected to.
			queueCredits = atoi(&(se->getString().c_str()[0]));
		}
	}

	delete ev;
}

void node::testMsgEvent(SST::Event *ev) {
	BaseMessageEvent *me = dynamic_cast<BaseMessageEvent*>(ev);
	if ( me != NULL ) {
		switch (me->msg.type)
		{
			case MESSAGE:
				std::cout << getName() << " is receiving a message from " << me->msg.source_node << std::endl;
				queueCurrSize++;
				break;
			case CREDIT:
				std::cout << getName() << " is receiving # of credits from " << me->msg.source_node << std::endl;
				queueCredits = me->msg.credits;
				break;
			case STATUS:
				// Check which node the message originated from:

				// If the message originated from the same node, the status message has looped through
				// the ring of nodes back to its original sender.
				if (me->msg.source_node == getName()) {
					std::cout << getName() << " received a message from itself!" << std::endl;
					// If the status is sending, a node in the ring can still send messages so a deadlock
					// has not occured.
					if (me->msg.status == SENDING) {
						//Can remove this ?
					}

					// All nodes in the ring have status Waiting, a deadlock has occured.
					if (me->msg.status == WAITING) {
						SST::StopAction exit;
						exit.execute();
					}
				} else { 
					std::cout << getName() << " is pinged with a status from " << me->msg.source_node << std::endl;
					// The node receives a status message from another node.
					// Two situations can happen here:

					// 1. The node receives the status SENDING. A node in the ring can still send messages.
					// 	  In this case the current node sends along the status SENDING.
						if (me->msg.status == SENDING) {
							MessageTypes type = STATUS;
							StatusTypes status = SENDING;
							int numCredits = me->msg.credits;
							struct Message msg = { me->msg.source_node, type, status, numCredits };
							nextPort->send(new BaseMessageEvent(msg));
						}

					// 2. The node receives the status WAITING. In this case the previous node(s) is waiting.
					//	  The current node determines if it can send or if its waiting as well and updates the status before passing the message along.
						else if (me->msg.status == WAITING) {
							if (queueCredits <= 0) {
								// The message needs to be reconstructed before sending out.
								MessageTypes type = STATUS;
								StatusTypes status = WAITING;
								int numCredits = me->msg.credits;
								struct Message msg = { me->msg.source_node, type, status, numCredits };
								nextPort->send(new BaseMessageEvent(msg));
							} else {
								MessageTypes type = STATUS;
								StatusTypes status = SENDING;
								int numCredits = me->msg.credits;
								struct Message msg = { me->msg.source_node, type, status, numCredits };
								nextPort->send(new BaseMessageEvent(msg));
							}
						}

					// If the current node can still send messages, update the message with status 'SENDING' and pass the message along.
					// Else the current node can not send, update the message with status 'WAITING' and pass the message along.
				}
				break;
		}

		
	}
}
/**
// Simulate sending a single message out to linked component in composition.
void node::sendMessage() {
	queueCurrSize--; 
	nextPort->send(new StringEvent("Sending"));
} */

void node::sendMessage() {
	queueCurrSize--; 
	MessageTypes type = MESSAGE;
	StatusTypes status = SENDING;
	int numCredits = queueMaxSize - queueCurrSize;
	struct Message msg = { getName(), type, status, numCredits };

	nextPort->send(new BaseMessageEvent(msg));
}

// Send # of slots in queue to previous connected node.
void node::sendCredits() {
	prevPort->send(new StringEvent(std::to_string(queueMaxSize - queueCurrSize)));
}

void node::sendCreditsMsg() {
	MessageTypes type = CREDIT;
	StatusTypes status = SENDING;
	int numCredits = queueMaxSize - queueCurrSize;
	struct Message msg = { getName(), type, status, numCredits };
	prevPort->send(new BaseMessageEvent(msg));
}

// Simulation purposes, add messages randomly to a nodes queue.
void node::addMessage() {
	int rndNumber;
	rndNumber = (int)(rng->generateNextInt32()); // Generate a random 32-bit integer
	//rndNumber = (rndNumber & 0x0000FFFF) ^ ((rndNumber & 0xFFFF0000) >> 16); // XOR the upper 16 bits with the lower 16 bits.
	rndNumber = abs((int)(rndNumber % 3)); // Generate a integer 0-2.

	queueCurrSize += 1; // Add messages to queue.
}


