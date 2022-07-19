/// \file
#ifndef communication_H
#define communication_H
#include <sst/core/event.h>

/**
 * @brief Enum for the type of messages in the simulation. 
 * 
 */
enum MessageTypes {
	MESSAGE,	/**< Type for messages which are stored in a node's queue or consumed. */
	STATUS,		/**< Type for messages which node's instantly pass along to determine if a system-deadlock has occured. */
};

/**
 * @brief Enum for the status type of a status message that a node sends out. 
 * 
 */
enum StatusTypes {
	SENDING,	/**< Type for nodes that are able to send messages. */
	WAITING,	/**< Type for nodes that are unable to send messages. */
};

/**
 * @brief Message structure. Contains information regarding message source/destination, status of sending node, and type of message.
 * 
 */
struct Message {
	int source_id;	/**< ID for node that the message originates from. */
	int dest_id;	/**< ID for node that the message is destined to. */
	StatusTypes status;		/**< Status of node that passes the message along. Only used when the message type is STATUS.*/
	MessageTypes type;		/**< Type of message. */
};

/**
 * @brief CreditProbe structure. Contains information containing how much space is left in a node's queue.
 * 
 */
struct CreditProbe {
	int credits;	/**< Amount of free space in the node's queue. */
};

/**
 * @brief Log Structure. Contains logging information that is sent to central logging node. 
 * 
 */
struct Log {
	int idle_time; /**< Amount of cycle the node has been idle. */
	int node_status; /**< Status of node (Idle/Executing). */
	int num_requests; /**< Number of requests for queue resource a node has made since it last sent a message. */
	int node_id; /**< ID of node that sent the log data */
};

/**
 * @brief Custom event type that handles Message structures. 
 * 
 */
class MessageEvent : public SST::Event {

public:
		
	/**
	 * @brief Serialize members of the Message struct. 
	 * 
	 * @param ser Wrapper class for objects to declare the order in which their members are serialized/deserialized.
	 */
	void serialize_order(SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & msg.source_id;  
		ser & msg.dest_id;
		ser & msg.status;
		ser & msg.type;
	}

	
	MessageEvent(Message msg) :
		Event(),
		msg(msg)
	{}

	
	MessageEvent() {} // For serialization

	Message msg; // Data type handled by event.

	ImplementSerializable(MessageEvent); // For serialization.
};

/**
 * @brief Custom event type that handles CreditProbe structures. 
 * 
 */
class CreditEvent : public SST::Event {

public:
	
	/**
	 * @brief Serialize members of the Credit Probe struct. 
	 * 
	 * @param ser Wrapper class for objects to declare the order in which their members are serialized/deserialized.
	 */
	void serialize_order(SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & probe.credits;
	}

	CreditEvent(CreditProbe probe) :
		Event(),
		probe(probe)
	{}

	CreditEvent() {} // For serialization

	CreditProbe probe; // Data type handled by event.

	ImplementSerializable(CreditEvent); // For serialization.

};

// Custom event type that handles logging info meant for the log node.
class LogEvent : public SST::Event {

public:

	/**
	 * @brief Serialize members of the Log struct. 
	 * 
	 * @param ser Wrapper class for objects to declare the order in which their members are serialized/deserialized.
	 */
	void serialize_order(SST::Core::Serialization::serializer &ser) override {
		Event::serialize_order(ser);
		ser & log.idle_time;
		ser & log.node_status;
		ser & log.num_requests;
	}

	LogEvent(Log log) :
		Event(),
		log(log)
	{}

	LogEvent() {} // For serialization

	Log log; // Data type handled by event.

	ImplementSerializable(LogEvent); // For serialization.
};

#endif