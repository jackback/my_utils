## 说明
本地socket。这里面有一个很关键的东西，socket进程通信命名方式有两种。
一是普通的命名，socket会根据此命名创建一个同名的socket文件，客户端连接的时候通过读取该socket文件连接到socket服务端。这种方式的弊端是服务端必须对socket文件的路径具备写权限，客户端必须知道socket文件路径，且必须对该路径有读权限。
另外一种命名方式是抽象命名空间，这种方式不需要创建socket文件，只需要命名一个全局名字，即可让客户端根据此名字进行连接。后者的实现过程与前者的差别是，后者在对地址结构成员sun_path数组赋值的时候，必须把第一个字节置0，即sun_path[0] = 0。





