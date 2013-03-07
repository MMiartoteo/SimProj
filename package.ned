package symphony;

@license(LGPL);

channel LinkChannel extends ned.DatarateChannel
{
    delay = 100ms;
    datarate = 10Mbps;
}
channel ShortLinkChannel extends LinkChannel
{
    @display("ls=#FF0F18");
}
channel LongDistanceLinkChannel extends LinkChannel
{
    @display("ls=#0917FF");
}

