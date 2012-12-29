package symphony;

@license(LGPL);

channel LinkChannel extends ned.DelayChannel
{
    delay = 100ms;
}
channel ShortLinkChannel extends LinkChannel
{
    @display("ls=#FF0F18");
}
channel LongDistanceLinkChannel extends LinkChannel
{
    @display("ls=#0917FF");
}

