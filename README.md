# network_programming
基于WinSock编写的计算机网络课程设计，包含网络聊天程序、网络代理服务器、基于 IP 多播的网络会议程序、电子邮件客户端程序、Ping 程序和TELNET 终端。

### 使用前

#### QT Creator

在untitled.pro文件中任意空白行插入

```c++
LIBS += -lWs2_32
```

#### CLion 

在CMakeList中添加

```c++
link_libraries(ws2_32 wsock32)
```

#### Dev-C++等

编译器 -> 编译器配置集 -> 编译时加入下列选项

```c++
-lwsock32
```


