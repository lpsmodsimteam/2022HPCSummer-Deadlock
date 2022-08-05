# Reference: http://sst-simulator.org/SSTPages/SSTUserPythonFileFormat/

import sst  # Use SST Library

TOTAL_NODES = 3  # Total nodes in simulation. (Excluding the logger node).

# Component node from element deadlock (deadlock.node), named "node_zero"
node_zero = sst.Component("Node 0", "deadlocklog.node")

# Adding parameters to node.
node_zero.addParams(
    {
        "queueMaxSize": "120",  # Max message queue size.
        "tickFreq": "3ms",  # Frequency component updates at.
        "id": "0",  # ID of node
        "total_nodes": f"{TOTAL_NODES}",  # Total nodes in simulation
        "message_gen": "0.90",  # Probability that the node will generate a message on tick.
    }
)

node_one = sst.Component("Node 1", "deadlocklog.node")
node_one.addParams(
    {
        "queueMaxSize": "100",
        "tickFreq": "5ms",
        "id": "1",
        "total_nodes": f"{TOTAL_NODES}",
        "message_gen": "0.90",
    }
)

node_two = sst.Component("Node 2", "deadlocklog.node")
node_two.addParams(
    {
        "queueMaxSize": "80",
        "tickFreq": "2ms",
        "id": "2",
        "total_nodes": f"{TOTAL_NODES}",
        "message_gen": "0.90",
    }
)


# Create a log component from element deadlock (deadlocklog.log) named "Logger".
node_log = sst.Component("Logger", "deadlocklog.log")

# Add parameters to logging node.
node_log.addParams(
    {
        "tickFreq": "1ms",  # Frequency component updates at.
        "num_nodes": f"{TOTAL_NODES}",  # This should be equal to the total number of node components to behave properly.
        "idle_threshold": "50",  # The number of consecutive cycles idle that all monitored nodes must exceed for deadlock to be declared.
        "request_threshold": "50",  # The number of consecutive request that all monitored nodes must exceed for deadlock to be declared.
    }
)

# Connect the nodes by their ports.
sst.Link("Message_Link_Zero").connect(
    (node_zero, "nextPort", "1ms"), (node_one, "prevPort", "1ms")
)
sst.Link("Message_Link_One").connect(
    (node_one, "nextPort", "1ms"), (node_two, "prevPort", "1ms")
)
sst.Link("Message_Link_Two").connect(
    (node_two, "nextPort", "1ms"), (node_zero, "prevPort", "1ms")
)

# Connect the nodes to the logger.
sst.Link("Log_Link_Zero").connect(
    (node_zero, "logPort", "1ps"), (node_log, "port0", "1ps")
)

sst.Link("Log_Link_One").connect(
    (node_one, "logPort", "1ps"), (node_log, "port1", "1ps")
)

sst.Link("Log_Link_Two").connect(
    (node_two, "logPort", "1ps"), (node_log, "port2", "1ps")
)
