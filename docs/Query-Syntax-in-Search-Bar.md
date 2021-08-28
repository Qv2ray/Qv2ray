- You can now search for connections by group, connection name, tags, latency, outbound count, inbound count, protocol, port and server address using the new searching syntax.
- The new query syntax is prefixed with a `>` character.

### e.g.
  - Searching for connections with tags, AND relation: `> tags=onetag,"another tag", anotherTag2`
  - Searching for connections with tags, OR relation: `> tags=onetag|"another tag"| anotherTag2`
    - Comma will be replaced with the default relation: `AND` for `Equals`, `OR` for `NotEquals`
  - Searching for connections without tags: `> tags!= excludedTag`
  - Searching for connections with latency no more than 50ms: `> latency<=50`
  - Searching for connections to a specific server: `> address=my.server.com`
  - and many more...
  
### Multiple searching criteria can be joined by a semicolon char (`;`)
  - Searching for connections tagged with `JP` and `Direct`, where the latency is less than 40ms, while exclude the currently-connected one: `> tags=JP,Direct; latency<40; !connected`
  
### All searching criteria
- `group`: Group Name (string)
- `Name`: Connection Name (string)
- `tags`: Connection Tags (list<string>)
- `latency`: Connection Latency (int)
- `inbounds`: Inbounds count (int)
- `outbounds`: Outbounds count (int)
- `connected`: Is current connection? (bool)
- `protocol`: First outbound protocol (string)
- `address`: First outbound address (string)
- `port`: First outbound port (int)
