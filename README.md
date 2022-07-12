# network_programming

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

#### Dev-C++

编译器 -> 编译器配置集 -> 编译时加入下列选项

```c++
-lwsock32
```


