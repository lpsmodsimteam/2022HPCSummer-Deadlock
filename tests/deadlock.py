import sst

node_one = sst.Component("Node 1", "deadlock.node")
node_one.addParams({
    "queueMaxParams" : "50",
    "tickFreq" : "10s"
})

node_two = sst.Component("Node 2", "deadlock.node")
node_two.addParams({
    "queueMaxParams" : "50",
    "tickFreq" : "10s"
})

sst.Link("MessageLink").connect( (node_one, "recvPort", "1ps") , (node_two, "recvPort", "1ps") )
sst.Link("CreditLink").connect( (node_one, "creditPort", "1ps"), (node_two, "creditPort", "1ps") )