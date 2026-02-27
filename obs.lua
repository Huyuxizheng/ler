-- 实现ref函数，创建可观察的引用对象
function ref(initialValue)
    -- 处理复杂数据类型的深度观察
    local function deepObserve(value)
        if type(value) == 'table' and not value._isRef then
            local observed = {}
            for k, v in pairs(value) do
                observed[k] = deepObserve(v)
            end
            return observed
        end
        return value
    end
    
    local refObj = {
        _value = deepObserve(initialValue),
        _observers = {},
        _isRef = true,  -- 标识这是一个可观察对象
        _pendingNotifies = false
    }
    
    -- 设置元表
    setmetatable(refObj, {
        -- 当获取value属性时返回实际值，同时在依赖收集阶段自动添加依赖
        __index = function(t, k)
            -- 检查观察者栈顶是否有观察者，如果有且访问的是value属性，则自动添加依赖
            local stack = Observer._observerStack
            if #stack > 0 and k == "value" then
                local currentObserver = stack[#stack]
                -- 避免重复添加依赖
                if not currentObserver.dependencies[t] then
                    currentObserver.dependencies[t] = true
                    t._observers[currentObserver] = true
                end
            end
            
            if k == "value" then
                return t._value
            else
                return rawget(t, k)
            end
        end,
        
        -- 当设置value属性时更新值并通知所有观察者
        __newindex = function(t, k, v)
            if k == "value" then
                -- 深度观察新值
                local newValue = deepObserve(v)
                
                -- 只有值真正改变时才更新和通知
                if newValue ~= t._value then
                    t._value = newValue
                    
                    -- 加入批量更新队列
                    if not t._pendingNotifies and Observer._updateQueue then
                        t._pendingNotifies = true
                        table.insert(Observer._updateQueue, function()
                            t._pendingNotifies = false
                            -- 通知所有依赖此ref的观察者
                            for observer in pairs(t._observers) do
                                -- 检查观察者是否已被移除
                                if observer._active then
                                    observer:notify()
                                end
                            end
                        end)
                        Observer._scheduleUpdate()
                    else
                        -- 没有批量更新机制时立即通知
                        for observer in pairs(t._observers) do
                            if observer._active then
                                observer:notify()
                            end
                        end
                    end
                end
            else
                rawset(t, k, v)
            end
        end
    })
    
    return refObj
end

-- 实现Observer表
Observer = {
    -- 所有观察者的列表，用于跟踪
    _allObservers = {},
    -- 观察者栈，用于嵌套的依赖收集
    _observerStack = {},
    -- 批量更新队列
    _updateQueue = {},
    -- 是否正在执行更新队列
    _isUpdating = false,
    -- 防止循环更新的计数器
    _updateDepth = 0,
    -- 最大更新深度，防止循环依赖
    _maxUpdateDepth = 100
}

-- 调度更新函数
function Observer._scheduleUpdate()
    if not Observer._isUpdating then
        Observer._isUpdating = true
        
        -- 立即执行更新队列（在真实环境中可能是异步的）
        Observer._flushUpdateQueue()
    end
end

-- 执行更新队列
function Observer._flushUpdateQueue()
    local queue = Observer._updateQueue
    Observer._updateQueue = {}
    
    for _, updater in ipairs(queue) do
        -- 检测循环更新
        if Observer._updateDepth > Observer._maxUpdateDepth then
            print("Warning: Maximum update depth exceeded. Possible circular dependency.")
            break
        end
        
        Observer._updateDepth = Observer._updateDepth + 1
        
        -- 执行更新器函数
        local success, err = pcall(updater)
        if not success then
            print("Error in update queue:", err)
        end
        
        Observer._updateDepth = Observer._updateDepth - 1
    end
    
    Observer._isUpdating = false
    
    -- 处理新的更新请求
    if #Observer._updateQueue > 0 then
        Observer._flushUpdateQueue()
    end
end

-- 创建观察者
function Observer.create(observedFunction, callback, userdata)
    local observer = {
        observedFunction = observedFunction,
        callback = callback,
        userdata = userdata,
        -- 上次观察到的结果，用于比较变化
        lastResult = nil,
        -- 观察的ref对象列表
        dependencies = {},
        -- 标记观察者是否激活
        _active = true,
        -- 唯一ID用于识别
        _id = "observer_" .. tostring(os.clock()):gsub('%.', '')
    }
    
    -- 通知方法，当依赖变化时调用
    function observer:notify()
        -- 如果观察者已被销毁，则不执行通知
        if not self._active then
            return
        end
        
        -- 检测循环更新
        if Observer._updateDepth > Observer._maxUpdateDepth then
            print("Warning: Maximum update depth exceeded for observer:", self._id)
            return
        end
        
        -- 安全执行被观察函数
        local success, newResult = pcall(self.observedFunction)
        if not success then
            print("Error in observed function:", newResult)
            return
        end
        
        local oldResult = self.lastResult
        -- 只有当结果发生变化时才调用回调
        if newResult ~= oldResult then
            -- 安全执行回调函数
            local callbackSuccess, callbackError = pcall(self.callback, oldResult, newResult, self.userdata)
            if not callbackSuccess then
                print("Error in observer callback:", callbackError)
            end
            self.lastResult = newResult
        end
    end
    
    -- 收集依赖的ref对象
    function observer:collectDependencies()
        -- 如果观察者已被销毁，则不执行
        if not self._active then
            return
        end
        
        -- 先清除旧的依赖
        self:deregisterAllDependencies()
        
        -- 将当前观察者推入栈顶
        table.insert(Observer._observerStack, self)
        
        -- 安全执行被观察函数，收集依赖
        local success, result = pcall(self.observedFunction)
        if not success then
            print("Error collecting dependencies:", result)
        else
            self.lastResult = result
        end
        
        -- 从栈中弹出当前观察者
        table.remove(Observer._observerStack)
    end
    
    -- 注销所有依赖
    function observer:deregisterAllDependencies()
        for refObj in pairs(self.dependencies) do
            if refObj and refObj._observers then
                refObj._observers[self] = nil
            end
        end
        self.dependencies = {}
    end
    
    -- 销毁观察者，清理资源
    function observer:destroy()
        if not self._active then
            return
        end
        
        -- 注销所有依赖
        self:deregisterAllDependencies()
        
        -- 从全局列表中移除
        for i, obs in ipairs(Observer._allObservers) do
            if obs == self then
                table.remove(Observer._allObservers, i)
                break
            end
        end
        
        -- 标记为非激活
        self._active = false
        self.observedFunction = nil
        self.callback = nil
        self.userdata = nil
    end
    
    -- 初始化时收集依赖并立即执行一次
    observer:collectDependencies()
    
    -- 将观察者添加到全局列表
    table.insert(Observer._allObservers, observer)
    
    return observer
end

-- 清理所有观察者
function Observer.clearAll()
    for _, observer in ipairs(Observer._allObservers) do
        observer:destroy()
    end
    Observer._allObservers = {}
    Observer._updateQueue = {}
    Observer._updateDepth = 0
end



