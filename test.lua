-- 被观察对象 a
a = ref(30)

b = ref(50)

--观察者观察的目标
function test_fun()
    return a.value + b.value + 1
end

function test_fun_cb(oldval,newval,userdata)
    print("test_fun_cb is called with userdata:", userdata)
    print("oldval:", oldval)
    print("newval:", newval)
end

--创建观察者 观察 test_fun 函数 , 当 test_fun 结果不同时如a改变，立刻调用 test_fun_cb 函数
--创建时会立即调用 test_fun 被观察对象 通过云方法代理建立订阅机制
--调用时会将test_fun之前的值传给oldval,新值传给newval
Observer1 = Observer.create(test_fun,test_fun_cb,"testarg")

--触发观察者回调，通知订阅的观察者
a.value = 40

-- 测试嵌套依赖收集场景
local c = ref(10)
local d = ref(20)

-- 第一个被观察函数，依赖c和d
function nested_fun1()
    return c.value + d.value
end

-- 第二个被观察函数，依赖a和nested_fun1()的结果
function nested_fun2()
    return a.value + nested_fun1() + 5
end

-- 嵌套依赖的回调函数
function nested_fun_cb(oldval, newval, userdata)
    print("\n" .. userdata .. " called:")
    print("  oldval:", oldval)
    print("  newval:", newval)
end

-- 创建嵌套观察者
local Observer2 = Observer.create(nested_fun1, nested_fun_cb, "nested_fun1 observer")
local Observer3 = Observer.create(nested_fun2, nested_fun_cb, "nested_fun2 observer")

-- 触发嵌套依赖更新
c.value = 15
