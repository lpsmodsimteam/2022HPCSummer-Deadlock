# Reference: http://sst-simulator.org/SSTPages/SSTUserPythonFileFormat/

import sst  # Use SST library

TOTAL_NODES = 3  # Total nodes in the simulation.

# Creating a node component from element deadlock (deadlock.node) named "Node 0".
node_zero = sst.Component("Node 0", "deadlock.node")

# Add parameters to node_zero
node_zero.addParams(
    {
        "queueMaxSize": "120",  # max message queue size.
        "tickFreq": "3ms",  # frequency node updates at.
        "id": "0",  # id of node.
        "total_nodes": f"{TOTAL_NODES}",  # total nodes in simulation.
        "message_gen": "0.90",
    }
)

node_one = sst.Component("Node 1", "deadlock.node")
node_one.addParams(
    {
        "queueMaxSize": "100",
        "tickFreq": "5ms",
        "id": "1",
        "total_nodes": f"{TOTAL_NODES}",
        "message_gen": "0.90",
    }
)

node_two = sst.Component("Node 2", "deadlock.node")
node_two.addParams(
    {
        "queueMaxSize": "80",
        "tickFreq": "2ms",
        "id": "2",
        "total_nodes": f"{TOTAL_NODES}",
        "message_gen": "0.90",
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
