# Tiny-Muduo
A tiny server.


2022/11/18 V3
将server类中的功能继续抽象为connection类以及Acceptor类
1. conncetion类负责处理已经建立的连接上的IO事件 之后有不同的处理逻辑 可以绑定不同的回调函数 connection类对象的声明周期由server类管理 需要绑定delete的回调函数
2. Acceptor类负责接受新的连接 

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
