import sst

# Component node from element deadlock (deadlock.node), named "node_one"
node_one = sst.Component("Node 1", "deadlock.node")
node_one.addParams(
    {
        "queueMaxSize": "10",  # max message queue size.
        "tickFreq": "10ns",  # simulated time node runs at.
        "randSeed": "112233",  # seed for random generation of messages for node
    }
)

node_two = sst.Component("Node 2", "deadlock.node")
node_two.addParams({"queueMaxSize": "10", "tickFreq": "5ns"})

node_three = sst.Component("Node 3", "deadlock.node")
node_three.addParams({"queueMaxSize": "10", "tickFreq": "10ns"})

# Connect the nodes by their ports.
sst.Link("Message_Link_One").connect(
    (node_one, "nextPort", "1ps"), (node_two, "prevPort", "1ps")
)
sst.Link("Message_Link_Two").connect(
    (node_two, "nextPort", "1ps"), (node_three, "prevPort", "1ps")
)
sst.Link("Message_Link_Three").connect(
    (node_three, "nextPort", "1ps"), (node_one, "prevPort", "1ps")
)
