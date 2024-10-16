## 元方法
```
// object[index]
func (__index self index'i32)'T

// object[index] = value
func (__newindex self index'i32 value'T)

// object RALL (在超出作用区域时调用)
func (__close self)'i32

// object 在赋值中作为右值 或 作为参数时调用
func (__clone self)'T

// callable object
func (__call self ...)

// ------------ 运算符重载 ------------

func (__add self other'T)

func (__sub self other'T)

func (__mul self other'T)

func (__div self other'T)

func (__eq self other'T)

```