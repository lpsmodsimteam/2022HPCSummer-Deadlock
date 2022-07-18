import sst
import random

NUM_NODES = 8
QUEUE_MIN_SIZE = 80
QUEUE_MAX_SIZE = 120
TICK_MIN_FREQ = 2
TICK_MAX_FREQ = 5
random.seed

nodes = dict()
for x in range(NUM_NODES):
    nodes[f"node_{x}"] = sst.Component(f"Node {x}", "deadlocklog.node")
    nodes[f"node_{x}"].addParams(
        {
            "queueMaxSize": f"{random.randint(QUEUE_MIN_SIZE, QUEUE_MAX_SIZE)}",
            "tickFreq": f"{random.randint(TICK_MIN_FREQ, TICK_MAX_FREQ)}ms",
            "id": f"{x}",
            "total_nodes": f"{NUM_NODES}",
        }
    )

node_log = sst.Component("Logger", "deadlocklog.log")
node_log.addParams(
    {
        "tickFreq": "1ms",
        "num_nodes": f"{NUM_NODES}",
    }
)

# "queueMaxSize": f"{random.randint(QUEUE_MIN_SIZE, QUEUE_MAX_SIZE)}",
#            "tickFreq": f"{random.randint(TICK_MIN_FREQ, TICK_MAX_FREQ)}s",

for x in range(NUM_NODES - 1):
    sst.Link(f"Link_{x}").connect(
        (nodes[f"node_{x}"], "nextPort", "1ms"),
        (nodes[f"node_{x + 1}"], "prevPort", "1ms"),
    )

sst.Link(f"Link_{NUM_NODES - 1}").connect(
    (nodes[f"node_{NUM_NODES - 1}"], "nextPort", "1ms"),
    (nodes["node_0"], "prevPort", "1ms"),
)

for x in range(NUM_NODES):
    sst.Link(f"Log_Link_{x}").connect(
        (node_log, f"port{x}", "1ps"), (nodes[f"node_{x}"], "logPort", "1ps")
    )
