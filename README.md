xxfl_set
==========


### 历史：
* 2017-04-29: 第一版 v1.0.0 完成。


### 介绍：
* 本项目 xxfl_set 的主要内容是用B+树实现了两个C++的泛型容器类：xxfl::set 和 xxfl::map。当元素类型尺寸较小时，可用来取代依赖于红黑树的 std::set 或 std::map，在保证执行效率的同时还能获得更高的内存利用率。项目中还包含了跨平台的测试工程用来检验新容器的有效性和各项性能。

* 新容器的接口尽量模仿std版本，用C++11规范编写，并同时支持GCC编译器和MSVC编译器。目前已知支持Windows和Linux系统。测试工程在Code::Blocks 16和Visual Studio 2015下编译运行通过。

* 虽然现在B+树多用于内存和文件系统之间的数据交互，但是全驻内存的B+树也是有一定优势的。B+树可以解决红黑树的一个痛点：存储较小尺寸元素时内存利用率很差。对于本项目实现的B+树，只要元素尺寸在32位下不超过20字节、64位下不超过36字节，就可以保证内存利用率高于红黑树。而且元素越小优势就越大。不过这些数字比较保守，是针对元素按顺序插入的情况来统计的。如果元素是随机插入的，总体的内存利用率还会更高。

* 尽管B+树在插入删除时经常需要整体移动元素，以及在查找时的比较次数要比红黑树多一些，不过得益于现代CPU的cache机制以及内存分配次数的减少，实际执行效率上B+树优于红黑树的情况并不少见。元素类型尺寸越小、以及比较操作耗时越少，B+树相对于红黑树的效率优势会越高。

* B+树在全树只有一个结点而且元素很少时内存利用率会差很多。特别是类似情况的容器数量很多时问题会更突出。我对此做了一些修改解决了这个问题。


### 使用方法：
* 将 /src 下的所有文件拷出来放到你自己的C++工程中。注意工程要支持C++11规范。需要用set就引用 xxfl_set.h，需要用map就引用 xxfl_map.h。

* 新容器的模板类定义比std版多了两个用于B+树的模板参数：结点bucket的最大尺寸和树的最大高度。这两个参数都有默认值。但是使用者可以根据具体需求修改这两个参数来改善性能。

* 上面两个模板参数还决定了容器的元素容量上限。容量上限有三种情况：理论容量（所有结点都恰好被完美塞满）、实际容量（针对纯插入的情况）、以及保守容量（混合插入和删除的情况）。相关信息请参考测试工程例子。


### 注意事项：
* 由于B+树中的元素位置不固定，因此指向有效元素的迭代器在容器执行插入或删除操作后可能会失效。

* 同理，所有记录容器中元素位置的指针都会在插入删除操作后变得不安全。

* 接口 erase(first, last) 我写了两个不同的实现：一个逻辑简单但是效率较低，另一个逻辑复杂但是效率较高。默认后者。但是因为测试量不够我不能保证第二个实现完全正确。请使用者自行斟酌。


### 其他：
* 本项目是个人开源项目。代码可以自由使用，但是请自担风险。

* 项目主页：<https://github.com/xxfldev/xxfl_set>。只要还没下线就会长期维护。有任何相关问题或者建议欢迎联系。