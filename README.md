# 介绍

rpch-c是跨语言rpch框架的c语言实现，更多详细信息可参考[rpch-go](https://github.com/gufeijun/rpch-go)。

使用epoll + 线程池 + reactor模型实现，仅适用于支持epoll的linux或者类unix系统，勿将此库运行在windows。

除了采用cJSON库进行结构体到json的序列化之外，未使用任何其他第三方库。

# 使用

启动一个最简单的rpc服务器：

**1. 创建IDL文件**

新建math.gfj：

```protobuf
//math.gfj
service Math{
    int32 Add(int32,int32)
}
```

该IDL定义个一个Math服务，服务里提供了Add方法。传参是两个int32类型的数据，返回值int32类型。

使用[hgen](https://github.com/gufeijun/hgen)编译器对其进行编译：`hgen -dir . -lang c ./math.gfj`。

即会在当前目录下生成四个文件：`math.rpch.client.c`，`math.rpch.client.h`，`math.rpch.server.c`，`math.rpch.server.c`。客户端程序需要链接`math.rpch.client.c`，服务端需要链接`math.rpch.server.c`。

`math.rpch.server.h`中定义了服务端需要实现的函数接口。

`math.rpch.client.h`中定义了客户端调用rpc服务的api。

hgen编译器的介绍以及IDL的语法见[hgen](https://github.com/gufeijun/hgen)。

**2. 服务端实现**

```c
//引入编译器生成的头文件
#include "math.rpch.server.h"
//include 本框架中的server.h头文件，其定义了启动rpc服务端的操作
#include "server.h"	

//实现math.rpch.server.h中生成的函数接口，即Math.Add服务
int32_t Math_Add(int32_t a, int32_t b, error_t* err) { return a + b; }

int main() {
    error_t err = error_new();
    //创建服务端
    server_t* svr = server_create();
    //在实现Math_Add后给svr注册Math服务
    //该函数实现在编译器生成的math.rpch.server.c中，框架用户只需要调用即可
    register_Math_service(svr);
    //启动服务端
    server_listen(svr, "127.0.0.1:8080", NULL, &err);
    if (!err.null) {
        printf("err occurred: %s\n", err.msg);
        return -1;
    }
    server_destroy(svr);
    return 0;
}
```

服务端实现只需要在意两件是，一个是实现在`math.rpch.server.h`中定义的服务接口，另一个就是使用编译器自动生成的注册服务函数进行注册服务即可。

**3. 客户端实现**

```c
#include <assert.h>
#include <stdint.h>

//引入编译器生成的头文件
#include "math.rpch.client.h"
//include 本框架中的client.h头文件，其定义了建立rpc客户端的操作
#include "client.h"

int main() {
    error_t err = error_new();
    //建立rpc客户端
    client_t* cli = client_dial("127.0.0.1:8080", &err);
    if (!err.null) {
        printf("err occurred: %s\n", err.msg);
        return -1;
    }
    {
        //进行rpc服务调用
        uint32_t res = Math_Add(1, 1, cli);
        assert(!client_failed(cli) && res == 2);
    }
    printf("test succ\n");
    client_destroy(cli);
    return 0;
}
```

客户端调用服务端的方法也极为简单。

# 注意事项

### 服务端

+ 如果生成的服务handler中，参数存在指针类型时，这些指针参数具有有限的声明周期。如：

  ```c
  void bookMarket_shelve(struct Book* book, error_t* err) {
      //将book存到hashmap中
      hashmap_set(book_map, book->name, book);				//wrong
      hashmap_set(book_map, book->name, Book_clone(book));	//right
  }
  ```

  book的内存空间会在当前rpc请求结束后被释放，所以对该实例进行持久化是错误的，应该使用`Book_clone`进行深拷贝。这个深拷贝函数由编译器生成，用户只需调用即可。同理，对于字符串：

  ```c
  void to_upper(char* str){
      store_and_use_later(str);			//wrong
      store_and_use_lator(strdup(str))	//right
  }
  ```

  使用strdup对字符串进行深拷贝。

+ 如果生成的服务handler中，返回值为指针类型时(对应IDL中返回值为一个message或者string)，我们应该返回值的所有数据放在堆空间，框架会统一进行free，无需担心内存泄露：

  ```c
  struct Book* bookMarket_createBook(error_t* err) {
      // wrong
      struct Book book;
      book.book_name = "线性代数";
      book.price = 10;
      return &book;
      
      //right
      struct Book* book = malloc(sizeof(struct Book));
      //成员如果也存在指针，也需要把所有成员的数据放在堆区，或者让其为NULL。
      book->book_name = strdup("线性代数");
      book->price = 10;
      return book;
      
      //right
      //或者使用更简单的方法，Book_create函数由编译器生成
      //能够将该结构包括内嵌的结构体成员的所有数据,递归放在堆区
      //当然在这个例子不明显，如果Book结构还内嵌了其他struct指针的话，
      //使用Book_create可以省略掉你很多手动malloc的步骤
      struct Book* book = Book_create();
      book->book_name = strdup("线性代数");
      book->price = 10;
      return book;
  }
  ```

  同理，对于字符串返回值：

  ```c
  char* string_tolower(char* str, error_t* err) {
      unsigned long len = strlen(str);
      for (unsigned long i = 0; i < len; i++) {
          str[i] = tolower(str[i]);
      }
      return str;					//wrong
      return strdup(str);			//right
  }
  ```

+ 如果出现错误，则可以对`error_t* err`进行操作，一旦为err指定了错误，则服务端不会将函数返回值传递给客户端，相反将错误信息传递给客户端。对于error_t的操作定义在框架的`error.h`头文件中，请自行查阅。

### 客户端

+ 如果客户端调用的rpc函数返回值为指针，则客户端有义务对该堆数据进行释放。如：

  ```c
  struct Account* res = bookMarket_consume(book, &account, cli);
  //do something to res...
  Account_delete(res);		//使用完后进行释放
  ```

  不要直接对res进行`free`，因为res结构体的指针成员数据也会放在堆中，应使用编译器给你生成的释放函数，都以(类型名_delete)命名。

  同理对于字符串返回值：

  ```c
  char* res = string_toupper("hello",cli);
  //do something...
  free(res);					//使用完后进行释放
  ```

+ 如果客户端调用的rpc函数形参为指针，我们传入的参数没有必要分配在堆区上，可以分配在栈区：

  ```c
  struct Account account;		//acount可以放在栈区，不需要malloc
  account.userName = "jack";	//acount的指针成员也可以不必放在堆区
  account.balance = 100;
  struct Account* res = bookMarket_consume(book, &account, cli);
  ```

# 安装

下载本仓库，将本仓库的c源码文件编译后得到的obeject文件链接到您的项目即可。

或者使用ar命令合并所有的obeject文件到一个静态库。

可以参考本仓库每个案例的`build.sh`。

# references

+ [rpch-c](https://github.com/gufeijun/rpch-c)：rpch框架的c语言实现。
+ [rpch-go](https://github.com/gufeijun/rpch-go)：rpch框架的go语言实现。
+ [rpch-node](https://github.com/gufeijun/rpch-node)：rpch框架的nodejs实现。