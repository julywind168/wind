## 语法说明
Wind 是一种纯表达式的语言, 拥有极简但严格的语法规则。
需要说明的是，Wind 中函数调用 和 关键字表达式 是基于S表达式的, 比如 `(foo 1 2)` 等价于 C 中 `foo(1, 2)`, `(foo)` 等价于 C 中 `foo()`。
除此之外, 其他一切你都会感觉到非常熟悉。

1. 基本表达式 （算术，逻辑）
```
Tip: Wind 是空格敏感的, 中缀表达式, 比如 a + b, `+` 的左右建议留一个空格。
```
```lisp
1
(1 + 2) * 3
a && b  // 逻辑与
a || b  // 逻辑或
!a      // 逻辑非
```

2. S表达式
```lisp
(if true 1 0) // 关键字 If 表达式，等价于 C 的 true ? 1 : 0
```

3. Block表达式, Block 它包含一组表达式，它会返回其中最后一个表达式的值, 通常用于函数体, if 分支, 循环体。
```lisp
{
    (var x = 1)
    x + 1
}   // 该 Block 的值为 `x + 1`, 即 2 
```

4. 类型标注
```
'(单引号) 是类型标注符,可以出现在表达式后面(无空格), 每个表达式的类型都必须明确且唯一。比如: 10'u8
```

## 类型
wind 语言是静态类型的, 基本类型有 bool, i8, i16, i32, i64, u8, u16, u32, u64, f32, f64

以及对应的引用类型,只需在前面加一个 `'` (单引号), 比如 `'i32` 表示 i32 的引用类型(类似于 C 中的 int* 指针), 关于引用目前并没有完成开发。
详见 [对引用的讨论](https://github.com/julywind168/wind/discussions/2)


## 定义变量
字面表达式可以缺省标注，比如 1 不标注的话默认为 `i32`, 1.0 为 `f64`
```
// 变量定义
(var x = 1)
(var x = 1'i8)

// 定义常量, let 表示该变量无法被更改
(let x = 1)

// 定义一个 bool 变量
(var b = (!true && true || false))
```

## 赋值? 匹配？
wind 也许会借鉴 [Elixir 模式匹配](https://hexdocs.pm/elixir/pattern-matching.html)
```
(var x = 1)
(x = 2)
```

## 函数 & extern
```
// 声明外部函数 (C/Wind 函数)
(extern (printf fmt'ptr))


// 定义一个函数
(func (foo a'i32 b'i32)'i32 -> {
    (let x = (a + b))
    (let y = (a - b))
    (x + y)
})
```

## loop 表达式
语法: (loop [init] test body)

test 中禁止定义变量

类似 C 语言的 while 循环, init 表达式会在循环开始时执行一次，且是可选的，init 中定义的变量也只能在 body 中使用

注意: loop 表达式是 void 类型
```
(loop (var x 0) (x < 10) (x = x + 1))
```

## if 表达式
语法: (if test then [else])

else 是可选的，缺少 else 的话, if 表达式则是 void 类型

注意: if 表达式是有返回值的。它相当于 C 的 (ok ? a : b)
```
(if (x + y > 0)
    {
        (print "x + y > 0")
    }
    {
        (print "x + y <= 0")
    }
)
```

## cond 表达式
语法: (cond
    test1 `=>` body1
    test2 `=>` body2
    ...
)

test 表达式必须是 bool 类型

注意: cond 表达式 是有返回值的。相当于其他语言中的 if elseif else end 语句，最后一个case 必须是 true => ...
```
(var x 1)

(cond
    x == 1 => (print "x == 1")
    x == 2 => (print "x == 2")
    true => (print "x == ?")
)
```

## match 表达式 (暂未实现！)
语法: (match what case1 `=>` body1 case2 `=>` body2 ...)

`_` 是个特殊的 case，可以匹配任意值, wind 要求 case 覆盖所有情况

注意: match 表达式 是有返回值的。 可以看到 match 与 cond 表达式很像，但更简洁。
```
(var x = 1)

(match x 
    1 => (print "x == 1")
    2 => (print "x == 2")
    _ => (print "x == ?")
)
```

## struct & protocol & impl
struct 用来定义自己的类型，类似于 C 的 struct

```
(struct Point (x'i32, y'i32))

(protocol Entity
    (func (moveto self x'i32 y'i32))
)

// self 为第一个参数, 且无需标注类型 (self 总是引用类型，即调用者自己)
(impl Point : Entity
    (func (moveto self x'i32 y'i32) {
        (self.x = x)
        (self.y = y)
    })
)
```

# 匿名函数 (closure)
```
(x'i32 y'i32)'i32 -> x + y
```

## 其他
```
// 元组, 类型 tuple<i32, i32>
(var t (1, 2))
(var x (t.0 + t.1))

// 数组, 类型 'array<i32>
(var a [1, 2, 3])
```