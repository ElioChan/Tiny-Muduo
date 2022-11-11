# Tiny-Muduo
A tiny server.


2022/11/11 V3
添加Channel类、Eventloop类
1. Channel:<br>
    将server、client 和 Epoll之间的联系抽象为Channel类 相较于原来直接用fd可以包含更多的信息 方便对于不同类型的连接执行不同的处理逻辑
    Channel->server: 通过fd联系 每个fd抽象为一个channel类
    Channel->Epoll: 通过epoll_event.event.data.ptr联系 ptr指向Channel类 同时channel类中的fd指向client/server

2. EventLoop:<br>
    将server类中的轮询的部分封装为Eventloop类 一个Eventloop指向一个Epoll
    其中还包含一个loop函数 用于轮询Epoll中的数据(即调用epoll类中的poll函数后 得到返回的activeChannel进行轮询)
    loop函数中直接对activeChannel进行处理 通过Channel类的回调函数callback()分别执行不同的流程
    callback函数在Channel类创建时就绑定 对于server_Channel 绑定newconnection函数 即接受新的连接 
    对于client_Channel 绑定handleReadEvent函数 目前只有这两种处理逻辑 后续可以根据Https报文信息中的request绑定不同的回调函数



2022/11/6 V2
将server以及client中的功能拆分到Socket InetAddress Epoll util类中 方便后续代码修改
1. Socket:<br>
    包含创造socket_fd, bind, listen, get_fd等功能  
    之后会添加set_fd的状态的功能 即对fcntl的封装  
2. InetAddress:<br>
    主要用于设置地址  
3. Epoll:<br>
    包含
    创建epfd,   
    addFd(对epoll_ctl(CTL_ADD)的封装),   
    poll(轮询获取活跃事件，对epoll_wait的封装 返回epoll_event数组)  
    之后会添加修改事件状态以及删除事件的函数 即对epoll_ctl另外两个参数的封装  
4. util:<br>
    包含一个异常判断函数errif(目前异常判断比较简单 直接输出异常信息 后续需要改进保证服务器一直处于正常运作)  


