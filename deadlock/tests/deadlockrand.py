import sst
import random

NUM_NODES = 5  # Number of nodes

# Node parameters are randomly generated between the two ranges for queue size and tick frequency.
QUEUE_MIN_SIZE = 80  # Minimum possible queue size
QUEUE_MAX_SIZE = 120  # Maximum possible queue size.
TICK_MIN_FREQ = 2  # Minimum tick frequency of nodes.
TICK_MAX_FREQ = 5  # Maximum tick frequency of nodes.

random.seed  # Create rng seed.

nodes = dict()  # Create a dictionary of nodes for

# Create all nodes and assign them a random maximum queue size and tick frequency.
for x in range(NUM_NODES):
    nodes[f"node_{x}"] = sst.Component(f"Node {x}", "deadlock.node")
    nodes[f"node_{x}"].addParams(
        {
            "queueMaxSize": f"{random.randint(QUEUE_MIN_SIZE, QUEUE_MAX_SIZE)}",
            "tickFreq": f"{random.randint(TICK_MIN_FREQ, TICK_MAX_FREQ)}ms",
            "id": f"{x}",
            "total_nodes": f"{NUM_NODES}",
        }
    )

# Connect nodes in a chain.
for x in range(NUM_NODES - 1):
    sst.Link(f"Link_{x}").connect(
        (nodes[f"node_{x}"], "nextPort", "1ms"),
        (nodes[f"node_{x + 1}"], "prevPort", "1ms"),
    )

# Connect the last node in the chain with the first node creating a ring topology of connected nodes.
sst.Link(f"Link_{NUM_NODES - 1}").connect(
    (nodes[f"node_{NUM_NODES - 1}"], "nextPort", "1ms"),
    (nodes["node_0"], "prevPort", "1ms"),
)
