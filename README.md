# network_programming
基于WinSock编写的计算机网络课程设计，包含网络聊天程序、网络代理服务器、基于 IP 多播的网络会议程序、电子邮件客户端程序、Ping 程序和TELNET 终端。

**本仓库内的文件夹不全是能够导入IDE的项目，使用前应注意辨别。如果无法编译，请按照下面的提示配置好链接的库。**

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

### 联系

发送电子邮件:soruku@foxmail.com（不接受以应付作业为来意的询问）

