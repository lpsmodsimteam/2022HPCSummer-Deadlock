import sst

node_one = sst.Component("Node 1", "deadlock.node")
node_one.addParams({
    "queueMaxSize" : "50",
    "tickFreq" : "10s"
})

node_two = sst.Component("Node 2", "deadlock.node")
node_two.addParams({
    "queueMaxSize" : "50",
    "tickFreq" : "10s"
})

#node_three = sst.Component("Node 3", "deadlock.node")
#node_three.addParams({
#    "queueMaxSize" : "50",
#    "tickFreq" : "10s"
#})

sst.Link("Message_Link_One").connect( (node_one, "recvPort", "1ps") , (node_two, "recvPort", "1ps") )
sst.Link("Credit_Link_One").connect( (node_one, "creditPort", "1ps"), (node_two, "creditPort", "1ps") )

#sst.Link("Message_Link_Two").connect( (node_two, "recvPort", "1ps") , (node_three, "recvPort", "1ps") )
#sst.Link("Credit_Link_Two").connect( (node_two, "creditPort", "1ps"), (node_three, "creditPort", "1ps") )

#sst.Link("Message_Link_Three").connect( (node_three, "recvPort", "1ps") , (node_one, "recvPort", "1ps") )
#sst.Link("Credit_Link_Three").connect( (node_three, "creditPort", "1ps"), (node_one, "creditPort", "1ps") )