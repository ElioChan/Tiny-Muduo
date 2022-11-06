# Tiny-Muduo
A tiny server.


2022/11/6 V2
将server以及client中的功能拆分到Socket InetAddress Epoll util类中 方便后续代码修改
Socket:
    包含创造socket_fd, bind, listen, get_fd等功能
    之后会添加set_fd的状态的功能 即对fcntl的封装
InetAddress:
    主要用于设置地址
Epoll:
    包含
    创建epfd, 
    addFd(对epoll_ctl(CTL_ADD)的封装), 
    poll(轮询获取活跃事件，对epoll_wait的封装 返回epoll_event数组)
    之后会添加修改事件状态以及删除事件的函数 即对epoll_ctl另外两个参数的封装
util:
    包含一个异常判断函数errif(目前异常判断比较简单 直接输出异常信息 后续需要改进保证服务器一直处于正常运作)
