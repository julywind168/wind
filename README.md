# Welcome to Wind 🔥
Wind 是一种静态类型，通用编译型语言，它旨在提供一流的 jit engine (增量编译，热更新)。来取代当前（尤其是游戏开发领域）的 Lua 等脚本语言。

# Hello World
```lisp
(extern (printf fmt''u8 ...))

(func (main)'i32 {
    (let a = [1, 2, 3])
    (loop (var i 0) i < 3 {
        (printf "hello world %d\n" a[i])
    })
})
```
Wiki [Wind入门指南](https://github.com/julywind168/wind/wiki/Guides)
, 欢迎到 [Discussions](https://github.com/julywind168/wind/discussions) 讨论

# Build (Macos)
其他系统的构建是类似的，但是未测试
```bash
brew install llvm
mkdir build && cd build
cmake -G Ninja ..
ninja
mv wind ../wind && cd ..
./wind run main.wd
```

# Why?
编程语言已经有很多了，为什么要再写一个呢？它有什么独特优势呢？这是大部分人第一个疑问。

答案：进入编程的世界后, 我一直是一个游戏开发者, 游戏开发中大部分的时候都是与 c 和 lua 打交道，比如客户端的开发(unity + lua) 服务端的 [skynet](https://github.com/cloudwu/skynet) 以前的我非常喜欢 lua, 它非常灵活, 简单, 还方便(不用编译)。

但是久而久之, lua（或者说脚本语言） 的缺点还是显露出来了，比如

1. 在线上产品的 bug 率高 （由于缺乏编译器的检查, 甚至能出现简单的 typo 引入致命的 bug）这导致在更新已运营的产品时，我都十分紧张，压力山大！ 这种感觉太糟糕了。

2. 由于 lua 过于灵活（既是优点也是它的缺点）, 导致在多人合作的项目中，项目的质量和维护成本很高。缺乏类型导致可读性差等等

3. 性能，lua 和编译型语言有着几个数量级的差距, 在用 lua 开发诸如 mmo 的大型游戏时，性能需要开发者重点关注, 有些人可能会说了, 性能热点可以用 c/cpp 开发啊，关于这点看 第4点。

4. 项目的复杂度，用 lua 开发，大部分都不是纯 lua, 还会穿插使用 c/cpp。不仅我要会几种语言，还要知道 它们的互相调用 (比如 给 lua 编写 clib)，项目的构建也比较复杂


游戏开发中使用lua之类的脚本, 其实主要需求在于它可以方便的热更新, 那么有没有一种编译型语言可以做到类似的事情呢, 答案是肯定的。就是 jit 技术。但是在网上我没找到非常成熟的 rust/swift/go 的 jit engine，于是我开始了 wind 这个项目。

## wind 语言的特性：

1. 语法简单, wind 中 一切都是表达式。比如 (if ok 1 0) 等价 c 中的  ok? : 1 : 0

2. 静态类型, 这让 wind 更安全, 面向接口(protocol) (类似 rust go)

3. 没有全局变量, 没有继承, 没有函数重载 (这也是优点？好吧, 这让 wind 更简单不是吗)

4. 没有头文件, 函数, 类型等无需前置声明。因为编译器足够智能

5. 元编程, 一流的范型(单态化，无运行时性能损失。共享 ast, 编译期也不会内存爆炸), 加上
元方法, 让 wind 的容器, 比如 array table 都是用 wind 编写的。

6. 值类型语义, 不需要垃圾回收，不需要内存管理。(可以手动在堆上分配内存, 但需要手动释放。 wind 不对指针的操作提供安全保证)

7. 一流的 jit (增量编译 & 热更新), 该功能以库的形式提供。比如：
```lisp
(func (main)'i32 {
    (let jit = (jit:new))
    (jit.load "(func (foo) (printf \"fxx\n\"))")
    (jit.load "(func (main)'i32 (foo))")
    (jit.eval) // 输出: fxx
    (jit.load "(func (foo) (printf \"foo\n\"))")
    (jit.eval) // 输出: foo
})
```
7. tuple, closure 等更多

8. 与 C 的互操作 十分方便（可以查看 项目中的 echo.wd, 用 wind 实现的 echo 服务器）

## Wind 1.0 Roadmap
- [x] 基本语法, struct, protocol
- [x] 范型, tuple, array
- [] 更完善的 parse (比如 字符串中的 \\", 十六进制的数字等)
- [] 完整的元方法，比如 `__add__`
- [] string table(hashmap)
- [] module/package
- [] closure
- [] jit (wind run ... 其实就是在jit engine 中执行的, 但是热更新还没完成)
- [] stdlib

## Wind 1.x Roadmap
- [] 模式匹配 (`=` 是一个模式匹配符, 但现在它只能用于赋值)
- [] 协程 (类似 golang 的并发模式)

## Other
- [] LSP (lsp-server)
- [] Game Server Framework
- [] Game Engine

## 赞助
```
Wind 是一款个人独立开源产品，如果你喜欢这个项目，欢迎PR 或者赞助一杯咖啡鼓励一下作者 ^.^
```
<img src="https://github.com/julywind168/wind/wiki/img/wechatpay.png" align="left" height="400" width="300">
<img src="https://github.com/julywind168/wind/wiki/img/alipay.png" height="400" width="300">