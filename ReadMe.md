# EasySocketZ

## example
example.cpp 添加了 tcp和udp的服务端客户端实现
客户端（阻塞
服务端（非阻塞

## 实现原理
简单的socket封装，windows实测没问题，目前支持tcp和udp(未实现)  
简单的处理了粘包以及阻塞和非阻塞的实现
非阻塞采用select，全局fd_set，最大数量默认4096连接数