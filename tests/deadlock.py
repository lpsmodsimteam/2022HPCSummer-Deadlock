import sst

# Component node from element deadlock (deadlock.node), named "node_one"
node_one = sst.Component("Node 1", "deadlock.node")
node_one.addParams({
    "queueMaxSize" : "50", # max message queue size of 50
    "tickFreq" : "12s"  # process node every 12s
})

node_two = sst.Component("Node 2", "deadlock.node")
node_two.addParams({
    "queueMaxSize" : "50",
    "tickFreq" : "6s"
})

node_three = sst.Component("Node 3", "deadlock.node")
node_three.addParams({
    "queueMaxSize" : "50",
    "tickFreq" : "17s"
})

# Connect the nodes by their ports.
sst.Link("Message_Link_One").connect( (node_one, "nextPort", "1ps") , (node_two, "prevPort", "1ps") )
sst.Link("Message_Link_Two").connect( (node_two, "nextPort", "1ps") , (node_three, "prevPort", "1ps") )
sst.Link("Message_Link_Three").connect( (node_three, "nextPort", "1ps") , (node_one, "prevPort", "1ps") )
