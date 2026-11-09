# Socket layer: client/server interaction

`socket.c` is shared by both binaries (it is in meson's `common` sources) and provides
the whole wire abstraction: `socket_send` (write-all loop, `MSG_NOSIGNAL`) and
`socket_read_str` (reads until the `'\0'` terminator, resyncs on overflow). Neither
side sees raw `send`/`read` calls.

The two interaction loops are inverse: the client writes then reads, the server reads
then writes.

```mermaid
sequenceDiagram
  autonumber
  participant C as awale_client.c<br/>(client loop: write → read)
  participant CS as socket.c<br/>(client side)
  participant SS as socket.c<br/>(server side)
  participant S as awale_server.c<br/>(server_interact: read → write)

  Note over C: loop: prompt, send command, print response
  C->>CS: socket_send(fd, "move 3\0", 7)
  CS-->>SS: TCP bytes (write-all, MSG_NOSIGNAL)
  SS->>S: socket_read_str(fd, buf, MAX_INPUT_LEN)<br/>reads until '\0', strips it
  S->>S: exec_cmd("move 3") → handler → board text
  S->>SS: socket_send(fd, "board...\0", n)
  SS-->>CS: TCP bytes
  CS->>C: socket_read_str(fd, resp, MAX_RESPONSE_LEN)
  C-->>C: printf response

  Note over C,S: on disconnect: read returns 0 → session ends,<br/>server frees the slot and ends the match
```

```mermaid
flowchart LR
  subgraph BINARIES
    CL[awale_client] --> SOCK
    SV[awale_server] --> SOCK
  end
  SOCK["socket.c (shared)<br/>socket_send: write-all + MSG_NOSIGNAL<br/>socket_read_str: read until '\0', overflow resync<br/>server_socket/accept/client_connect, socket_close"]
  SOCK --> TCP[(TCP stream)]
```
