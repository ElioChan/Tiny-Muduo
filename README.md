# Tiny-Muduo
A tiny server.



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

2022/11/18 V4
将server类中的功能继续抽象为connection类以及Acceptor类
1. conncetion类负责处理已经建立的连接上的IO事件 之后有不同的处理逻辑 可以绑定不同的回调函数 connection类对象的声明周期由server类管理 需要绑定delete的回调函数
2. Acceptor类负责接受新的连接 

2023/2/6 V5
加入tcp缓冲区 用于流量控制 (tcp实现流量控制主要是通过滑动窗口：接收方在回复的ACK中会带上自己的接收窗口的大小 接收端需要根据这个值条调整自己的发送策略)
实现：增加一个buffer类 每一个tcp连接都需要增加一个buffer对象(实际上应该是两个一个用于发送缓冲区 一个用于接收缓冲区 但是这里实现的是回射服务器 所以这两个可以用同一个)
每个buffer类中用string str来存储缓冲区中的数据
成员函数：
- 用于输入缓冲区的函数 append
- 用于清空缓冲区的函数 clear
- 用于得到其中一行的数据的函数 getline
- 用于返回缓冲区大小的函数 size 注意这里的返回值类型为ssize_t
- 用于将string 转化为 const char*的函数 c_str
成员变量：
- 发送缓冲区 writeBuffer
- 接收缓冲区 readBuffer
在这个代码中 由于是回射 所以read和write Buffer可以设成一个

同时本次还将整合代码 将所有的依赖项放入./src文件夹内

2022/2/9 V6
加入线程池 之前的代码中 由eventloop线程负责处理全部的任务 每次有新的客户端进来 都是由loop线程进行轮询并处理的 这样当客户端访问量增大时 主线程的性能会严重下降 而且现在的电脑大多支持多线程 这样就没有最大限度的利用资源 所以增设线程池 每当channel中有新的handleEvent进来 就将这个任务task 加入线程池 由线程池中的线程执行 而eventloop线程 就只负责连接客户端 
在引入线程池时 最重要的一点就是互斥锁 即两个任务若需要访问相同的资源 我们需要利用锁 对访问的资源 的读写操作 上锁
还有一点是当任务队列为空时 CPU不应该不断轮询耗费CPU(???其实这一点不是很明白) 
为了解决上面两个问题 前者需要用到mutex 后者需要用到条件变量condition_variable

todo: 目前线程只可以绑定function<void()> 无法返回返回值 也无法传参 后续利用智能指针进行改进
以及acceptor 和 connection 的ET/LT 触发模式  和 连接的阻塞与否 需要思考什么组合最合适 效率最高 为什么

2022/2/11 V7
改进线程池 由于之前的版本中线程只能绑定function<void()>类型的函数 因此利用模板对add函数进行改进 为了减少开销 利用智能指针的完美转发 将add函数的参数列表可以完美转发至funcion的参数中 
由于cpp不支持模板的分离式编译 因此需要在.h文件中编写add函数 
同时 研究LT/ET 以及 阻塞IO和阻塞IO后 我们发现acceptor更适合用LT模式的非阻塞 connection更适合用ET模式的非阻塞，具体原因分析如下：
首先我们需要搞清楚LT/ET 以及阻塞/非阻塞IO的区别 和特性
- ET：水平触发 当有可读事件达到时 epoll_wait会通知程序去处理。如果这次没有处理完，那么他会一直通知你 直到处理完；
- LT：边沿触发 当有可读事件到达时，epoll_wait会通知程序去处理。如果这次没有处理完 那么他就不管你了 直到下次出现可读写事件 也就是锁只会通知你一次
- 可以将有数据到达 或者 有链接到达 看作高电平 没有数据或者没有待连接的客户端 看作低电平 那么水平触发就是只要在高电平 epoll_wait就会通知；而边沿触发就是当从低电平变为高电平 或者低电平变为高电平时 epoll_wait才会通知
- 所以边沿触发 会比 水平触发 更加高效 因为它只用通知一次
- 阻塞IO：当你去读一个阻塞的文件描述符，若没有数据可以读，那么会一直阻塞在那里，直到有数据可以读(这里的读数据不仅仅指读数据 还指连接客户端)
- 非阻塞IO：当你去读一个非阻塞的文件描述符，若没有数据可读，那么会直接返回，若返回数据 则说明读成功了，若失败 则返回相应的errno 可以根据错误码进行进一步的处理
- 有关组合选择：
	- 对于监听的sockfd，使用水平触发模式，因为边缘触发会导致在高并发的情况下，有的客户端连接不上(因为高并发的情况下，连接fd可以看作是一起到的 所以需要用while去连接全部到达的连接符)
	- 对于读写的connfd，水平触发模式下，阻塞和非阻塞的效果都一样，不过最好使用非阻塞
	- 对于读写的connfd，边沿触发模式下，必须使用非阻塞，并且一次性全部读完(即使用while循环读取数据 当返回的错位代码为EAGAIN/EWOULDBLOCK时，要再次读取)。否则会一直堵塞在读取数据处，就无法进行其他的操作了
