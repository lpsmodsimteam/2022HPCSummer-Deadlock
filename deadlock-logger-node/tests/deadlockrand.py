# Reference: http://sst-simulator.org/SSTPages/SSTUserPythonFileFormat/

import sst  # Use SST Library
import random

NUM_NODES = 3  # Number of nodes (Excluding the logger node).
SEED = 1234  # Seed for randomized parameters.

# Node parameters are randomly generated between the two ranges for queue size and tick frequency.
QUEUE_MIN_SIZE = 80  # Minimum possible queue size
QUEUE_MAX_SIZE = 120  # Maximum possible queue size.
TICK_MIN_FREQ = 2  # Minimum tick frequency of nodes.
TICK_MAX_FREQ = 5  # Maximum tick frequency of nodes.

random.seed  # Create rng seed.

nodes = dict()

# Create all nodes and assign them a random maximum queue size and tick frequency.
for x in range(NUM_NODES):

    # Creating a node from element deadlocklog (deadlocklog.node) named "Node {x}".
    nodes[f"node_{x}"] = sst.Component(f"Node {x}", "deadlocklog.node")

    # Adding parameters to sender node.
    nodes[f"node_{x}"].addParams(
        {
            "queueMaxSize": f"{random.randint(QUEUE_MIN_SIZE, QUEUE_MAX_SIZE)}",  # Max message queue size.
            "tickFreq": f"{random.randint(TICK_MIN_FREQ, TICK_MAX_FREQ)}ms",  # Frequency component ticks at.
            "id": f"{x}",  # ID of node.
            "total_nodes": f"{NUM_NODES}",  # Number of nodes in the simulation (excluding the logger node).
            "message_gen": "0.90",  # Probability that the node will generate a message on tick.
        }
    )

# Create a log component from element deadlock (deadlocklog.log) named "Logger".
node_log = sst.Component("Logger", "deadlocklog.log")

# Add parameters to node_log.
node_log.addParams(
    {
        "tickFreq": "1ms",  # Frequency component updates at.
        "num_nodes": f"{NUM_NODES}",  # This should be equal to the total number of node components to behave properly.
    }
)

# Connect nodes in a chain
for x in range(NUM_NODES - 1):
    sst.Link(f"Link_{x}").connect(
        (nodes[f"node_{x}"], "nextPort", "1ms"),
        (nodes[f"node_{x + 1}"], "prevPort", "1ms"),
    )

# Connect the last node in the chain with the first node creating a ring topology.
sst.Link(f"Link_{NUM_NODES - 1}").connect(
    (nodes[f"node_{NUM_NODES - 1}"], "nextPort", "1ms"),
    (nodes["node_0"], "prevPort", "1ms"),
)

# Connect all nodes in the ring to the logger node.
for x in range(NUM_NODES):
    sst.Link(f"Log_Link_{x}").connect(
        (node_log, f"port{x}", "1ps"), (nodes[f"node_{x}"], "logPort", "1ps")
    )
