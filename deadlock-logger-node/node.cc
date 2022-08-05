/// \file
/**
   Simulation of a communication deadlock occuring in a ring topology network of nodes.
 */

#include <sst/core/sst_config.h> 
#include <sst/core/simulation.h>
#include <sst/core/stopAction.h>
#include "node.h" 

// Constructor definition
node::node( SST::ComponentId_t id, SST::Params& params) : SST::Component(id) {
	output.init("deadlocksim-" + getName() + "->", 1, 0, SST::Output::STDOUT); // Formatting output for console.

	// Get parameters
	queueMaxSize = params.find<int64_t>("queueMaxSize", 50);
	clock = params.find<std::string>("tickFreq", "10s");
	randSeed = params.find<int64_t>("randseed", 121212);
	node_id = params.find<int64_t>("id", 1);
	total_nodes = params.find<int64_t>("total_nodes", 5);
	message_gen = params.find<float>("message_gen", 0.5);

	// Initialize Variables
	queueCurrSize = 0;
	queueCredits = 0;
	generated = 0;
	rndNumber = 0;

	// Initialize Random
	rng = new SST::RNG::MarsagliaRNG(10, randSeed); // Create a Marsaglia RNG with a default value and a random seed.

	// Set Main Clock
	// Handler object is created with a reference to this object and a pointer to
	// a function that is called on every clock tick event (?).
	registerClock(clock, new SST::Clock::Handler<node>(this, &node::tick));
	
	// Configure the port for receiving a message from a node.
	nextPort = configureLink("nextPort", new SST::Event::Handler<node>(this, &node::creditHandler));
	// Check if port exist. Error out if not.
	if ( !nextPort ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'nextPort'\n");
	}

	// Configure the port for returning credit information to a node.
	prevPort = configureLink("prevPort", new SST::Event::Handler<node>(this, &node::messageHandler));
	// Check if port exist. Error out if not.
	if ( !prevPort ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'prevPort'\n");
	}

	// Configure the port for sending log info to logger node.
	logPort = configureLink("logPort", new SST::Event::Handler<node>(this, &node::logHandler));
	if ( !logPort ) {
		output.fatal(CALL_INFO, -1, "Failed to configure port 'logPort'\n");
	}
}

// Deconstructor definition
node::~node() {

}

// SST Setup Phase, called for each node after all nodes have been constructed.
void node::setup() {
	output.verbose(CALL_INFO, 1, 0, "id %d initialized\n", node_id);

	struct CreditProbe creds = { queueMaxSize - (int)msgqueue.size() }; // Send initial credits to all nodes during setup.
	prevPort->send(new CreditEvent(creds));
}

// SST Finish Phase, called for each node when the simulation ends and before all nodes are cleaned up.
void node::finish() {
	output.verbose(CALL_INFO, 1, 0, "Final queue size is %ld | Max queue size is %d | Final credit size is %d\n", msgqueue.size(), queueMaxSize, queueCredits);
	struct Message top = msgqueue.front();
	output.verbose(CALL_INFO, 1, 0, "Top of queue: Dest_ID-%d\n", top.dest_id);
}

// Runs every clock tick
bool node::tick( SST::Cycle_t currentCycle ) {
	// Replace with output
	if (node_id == 0) {
		//output.verbose(CALL_INFO, 1, 0, "\n--------------------------Sim-Time: %lu--------------------------\n", getCurrentSimTime());
		std::cout << "\n Sim-Time: " << getCurrentSimTime() << std::endl;
	}	
	output.verbose(CALL_INFO, 1, 0, "Status: %d | Credits: %d | Time Idle: %d | Requests %d\n", node_state, queueCredits, idle_duration, block_requests);
	output.verbose(CALL_INFO, 2, 0, "Size of queue: %ld\n", msgqueue.size());
	output.verbose(CALL_INFO, 2, 0, "Amount of credits: %d\n", queueCredits);

	if (node_state == IDLE) {
		idle_duration++;
	} else {
		idle_duration = 0;
		block_requests = 0;
	}

	node_state = IDLE;

	// Node is blocked from sending.
	if (queueCredits <= 0) {
		block_requests++;
	}

	// Rng and generate message to send out.
	if (queueCredits > 0) {	
		addMessage();
	}

	// Send a message out every tick if the next nodes queue is not full,
	// AND if the node has messages in its queue to send.
	if ((generated != 1 && (queueCredits > 0 && msgqueue.size() > 0))) {
		sendMessage();
		sendCredits();
	} else if (generated != 1 && !msgqueue.empty()) {
		// Peek at the top message to see if it needs to be delivered to the next node.
		struct Message top = msgqueue.front();
		if (top.dest_id == (node_id + 1) % total_nodes) {
			sendMessage();
			sendCredits();
		}
	}

	generated = 0;

	sendLog();

	return(false);
}

void node::messageHandler(SST::Event *ev) {
	MessageEvent *me = dynamic_cast<MessageEvent*>(ev);
	if ( me != NULL ) {
		switch (me->msg.type)
		{
			case MESSAGE:
				output.verbose(CALL_INFO, 2, 0, "is receiving a message from node %d.\n", me->msg.source_id);
				output.verbose(CALL_INFO, 2, 0, "Message Details: SourceID %d | DestID %d\n", me->msg.source_id, me->msg.dest_id);

				// Check if the message is meant for the node and that the node has correct space.	
				if (me->msg.dest_id != node_id && msgqueue.size() < queueMaxSize) {
					//output.verbose(CALL_INFO, 2, 0, "Sending a message. Queue size is now %ld\n", msgqueue.size());
					output.verbose(CALL_INFO, 2, 0, "Message was added to the queue\n");
					msgqueue.push(me->msg);
					sendCredits(); 
				} else if (me->msg.dest_id == node_id) {
					output.verbose(CALL_INFO, 2, 0, "Consumed a message\n");
				} else if (msgqueue.size() >= queueMaxSize) {
					output.verbose(CALL_INFO, 2, 0, "Message was dropped\n");
				}
				break;
		}
	}
	delete ev; // Clean up event to prevent memory leaks.
}

void node::logHandler( SST::Event *ev ) {
	output.fatal(CALL_INFO, -1, "Node should not be receiving logging info from logger node. Error!");
	delete ev;
}

void node::creditHandler(SST::Event *ev) {
	CreditEvent *ce = dynamic_cast<CreditEvent*>(ev);
	if ( ce != NULL ) {
		queueCredits = ce->probe.credits;
	}
}

// Simulate sending a single message out to linked component in composition.
void node::sendMessage() {
	node_state = EXECUTING;
	struct Message msg = msgqueue.front();
	msgqueue.pop();
	nextPort->send(new MessageEvent(msg));
}

// Send number of credits left to the previous node.
void node::sendCredits() {
	// Construct credit message to send.
	output.verbose(CALL_INFO, 2, 0, "Sending credits\n");
	struct CreditProbe creds = { queueMaxSize - (int)msgqueue.size() };
	prevPort->send(new CreditEvent(creds));
}

void node::sendLog() {
	output.verbose(CALL_INFO, 2, 0, "Sending log data\n");
	struct Log log = { idle_duration, node_state, block_requests, node_id };
	logPort->send(new LogEvent(log));
}

// Simulation purposes, generate messages randomly and send to next node.
void node::addMessage() {
	node_state = EXECUTING;	
	rndNumber = (rng->nextUniform()); // Generate a random 32-bit integer
	
	if (rndNumber <= message_gen) {
		// Construct and send a message
		generated = 1;

		// Generate a random destination node that exist in the simulation.
		int rndNode = (int)(rng->generateNextInt32());
		rndNode = abs((int)(rndNode % total_nodes)); // Generate a integer 0-(Total Nodes - 1)
		output.verbose(CALL_INFO, 2, 0, "Generating a message.\n");
		struct Message newMsg = { node_id, rndNode, SENDING, MESSAGE};
		nextPort->send(new MessageEvent(newMsg));
	}
}


