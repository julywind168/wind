wind 语法以S表达式为主，但是为了简洁和易于阅读, =(匹配符), 数学运算(+ - * /)，逻辑运算(&& ||) 采用中缀表达式, 比如 (1 + 1)


## 类型
wind 语言是静态类型的, 基本类型有 bool, i8, i16, i32, i64, u8, u16, u32, u64, f32, f64

以及对应的引用类型,只需在前面加一个 `'` (单引号), 比如 `'i32` 表示 i32 的引用类型(类似于 C 中的 int* 指针)


## 定义变量 & 类型标注

#### *`'`是类型标注符,可以出现在表达式后面, 每个表达式的类型都必须明确且唯一*

字面表达式可以缺省标注，比如 1 不标注的话默认为 `i32`, 1.0 为 `f64`
```
// 变量定义
(var x 1)
(var x 1'i8)

// 定义常量, let 表示该变量无法被更改
(let x 1)

// 定义一个 bool 变量
(var b (!true && true || false))
```

## 赋值? 匹配？
wind 也许会借鉴 [Elixir 模式匹配](https://hexdocs.pm/elixir/pattern-matching.html)
```
(var x 1)
(x = 2)
```

## 函数 & extern & 匿名函数
`{...}` 是一个 block 表达式，它包含一组表达式，它会返回其中最后一个表达式的值
```
// 声明外部函数
(extern (printf fmt''u8))    // 'u8 is a ptr to u8


// 定义一个函数
(func (foo a'i32 b'i32)'i32 {
    (let x (a + b))
    (let y (a - b))
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
(var x 1)

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

## 其他
```
// 元组, 类型 tuple<i32, i32>
(var t (1, 2))
(var x (t.0 + t.1))

// 数组, 类型 'array<i32>
(var a [1, 2, 3])
```