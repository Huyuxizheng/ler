-- xmake 配置文件，支持编译Lua相关程序和LVGL库
set_toolchains("local_gcc")

toolchain("local_gcc")
    set_kind("standalone")
    set_toolset("cc",  "D:/tools/w64devkit/bin/gcc")
    set_toolset("cxx", "D:/tools/w64devkit/bin/g++")
    set_toolset("ld",  "D:/tools/w64devkit/bin/gcc") -- 通常 ld 用 gcc 也可以
    set_toolset("ar",  "D:/tools/w64devkit/bin/ar")
    set_toolset("strip", "D:/tools/w64devkit/bin/strip")
toolchain_end()


includes("lv_port_pc_eclipse")
-- Lua源码目录
local lua_dir = "lua"

-- LVGL源码目录 (Corrected path)
local lvgl_dir = "lv_port_pc_eclipse/lvgl"


-- 编译Lua源码
target("lua_lib")
    set_kind("static")
    add_files(lua_dir .. "/*.c")
    add_includedirs(lua_dir)
    -- 排除不需要的文件
    remove_files(lua_dir .. "/lua.c")
    remove_files(lua_dir .. "/onelua.c")
    
-- 原有的XML解析器程序
target("ler_xml")
    set_kind("binary")
    add_files("ler_xml.c", "ler_xml_test.c")
    
-- 新增的Lua文件加载执行程序
target("lua_runner")
    set_kind("binary")
    add_files("test.c")
    add_includedirs(lua_dir)
    add_deps("lua_lib")

-- LER DOM Test
target("ler_dom_test")
    set_kind("binary")
    add_files("ler_dom_test.c")
    add_files("ler_dom.c", "ler_dom_text.c", "ler_dom_button.c", "ler_dom_bar.c", "ler_dom_view.c", "ler_dom_default.c", "ler_xml.c")
    add_files("lv_port_pc_eclipse/mouse_cursor_icon.c")
    
    add_files(lvgl_dir .. "/src/**.c")
    add_files(lvgl_dir .. "/demos/**.c")
    
    -- 添加驱动源文件
    add_files("lv_port_pc_eclipse/lv_drivers/*.c")
    add_files("lv_port_pc_eclipse/lv_drivers/sdl/*.c")

    add_includedirs(".")
    add_includedirs(lvgl_dir)
    add_includedirs(lua_dir)
    add_includedirs("lv_port_pc_eclipse")
    add_includedirs("lv_port_pc_eclipse/lv_drivers")
    
    -- 编译选项
    add_defines("LV_CONF_INCLUDE_SIMPLE=1")
    add_defines("LV_BUILD_EXAMPLES=1")  -- Enable examples/demos
    add_defines("LV_USE_DEMO_WIDGETS=1")
    add_defines("_CRT_SECURE_NO_WARNINGS")

    -- 链接 SDL2 相关库
    if is_plat("windows") then
        -- 链接 SDL2 库
        add_links("SDL2main", "SDL2")
        
        -- Windows 系统库
        add_links("gdi32", "user32", "shell32")
    end

-- LER Test (Main Application)
target("ler_test")
    set_kind("binary")
    add_files("ler_test.c")
    add_files("ler_file.c")
    add_files("ler_vm.c")
    add_files("ler_dom.c", "ler_dom_text.c", "ler_dom_button.c", "ler_dom_bar.c", "ler_dom_view.c", "ler_dom_default.c", "ler_xml.c")
    add_files("lv_port_pc_eclipse/mouse_cursor_icon.c")
    
    add_files(lvgl_dir .. "/src/**.c")
    add_files(lua_dir .. "/*.c")
    remove_files(lua_dir .. "/lua.c")
    remove_files(lua_dir .. "/onelua.c")
    
    -- 添加驱动源文件
    add_files("lv_port_pc_eclipse/lv_drivers/*.c")
    add_files("lv_port_pc_eclipse/lv_drivers/sdl/*.c")

    add_includedirs(".")
    add_includedirs(lvgl_dir)
    add_includedirs(lua_dir)
    add_includedirs("lv_port_pc_eclipse")
    add_includedirs("lv_port_pc_eclipse/lv_drivers")
    
    -- 编译选项
    add_defines("LV_CONF_INCLUDE_SIMPLE=1")
    add_defines("_CRT_SECURE_NO_WARNINGS")

    -- 链接 SDL2 相关库
    if is_plat("windows") then
        -- 链接 SDL2 库
        add_links("SDL2main", "SDL2")
        
        -- Windows 系统库
        add_links("gdi32", "user32", "shell32")
    end
