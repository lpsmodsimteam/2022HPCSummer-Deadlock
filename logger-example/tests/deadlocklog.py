import sst

TOTAL_NODES = 3

# Component node from element deadlock (deadlock.node), named "node_one"
node_zero = sst.Component("Node 0", "deadlocklog.node")
node_zero.addParams(
    {
        "queueMaxSize": "120",  # max message queue size.
        "tickFreq": "3ms",  # simulated time node runs at.
        "id": "0",  # id of node
        "total_nodes": f"{TOTAL_NODES}",  # total nodes in simulation
    }
)

node_one = sst.Component("Node 1", "deadlocklog.node")
node_one.addParams(
    {
        "queueMaxSize": "100",
        "tickFreq": "5ms",
        "id": "1",
        "total_nodes": f"{TOTAL_NODES}",
    }
)

node_two = sst.Component("Node 2", "deadlocklog.node")
node_two.addParams(
    {
        "queueMaxSize": "80",
        "tickFreq": "2ms",
        "id": "2",
        "total_nodes": f"{TOTAL_NODES}",
    }
)


node_log = sst.Component("Logger", "deadlocklog.log")
node_log.addParams(
    {
        "tickFreq": "1ms",
        "num_nodes": f"{TOTAL_NODES}",
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
