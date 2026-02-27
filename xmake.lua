-- xmake 配置文件，支持编译Lua相关程序和LVGL库

-- 工具链和 SDK 路径配置
local toolchain_bin = "D:/tools/w64devkit/bin"
local sdk_lib_path  = "D:/tools/w64devkit/lib"

set_toolchains("local_gcc")

toolchain("local_gcc")
    set_kind("standalone")
    set_toolset("cc",    toolchain_bin .. "/gcc")
    set_toolset("cxx",   toolchain_bin .. "/g++")
    set_toolset("ld",    toolchain_bin .. "/gcc")
    set_toolset("ar",    toolchain_bin .. "/ar")
    set_toolset("strip", toolchain_bin .. "/strip")
toolchain_end()


includes("lv_port_pc_eclipse")
-- Lua源码目录
local lua_dir = "lua"

-- LVGL源码目录 (Corrected path)
local lvgl_dir = "lv_port_pc_eclipse/lvgl"


-- 编译Lua源码
target("lua_lib")
    set_kind("static")
    set_targetdir("libs")
    add_files(lua_dir .. "/*.c")
    add_includedirs(lua_dir, {public = true})
    -- 排除不需要的文件
    remove_files(lua_dir .. "/lua.c")
    remove_files(lua_dir .. "/onelua.c")
    
-- 编译 LVGL 静态库
target("lvgl_lib")
    set_kind("static")
    set_targetdir("libs")
    add_files(lvgl_dir .. "/src/**.c")
    -- 添加驱动源文件
    add_files("lv_port_pc_eclipse/lv_drivers/*.c")
    add_files("lv_port_pc_eclipse/lv_drivers/sdl/*.c")
    add_files("lv_port_pc_eclipse/mouse_cursor_icon.c")

    add_includedirs(".", {public = true})
    add_includedirs(lvgl_dir, {public = true})
    add_includedirs("lv_port_pc_eclipse", {public = true})
    add_includedirs("lv_port_pc_eclipse/lv_drivers", {public = true})
    
    add_defines("LV_CONF_INCLUDE_SIMPLE=1", {public = true})
    add_defines("_CRT_SECURE_NO_WARNINGS", {public = true})

    -- 链接 SDL2 相关库
    if is_plat("windows") then
        -- 添加 SDL2 库文件搜索路径 (w64devkit)
        add_linkdirs(sdk_lib_path, {public = true})

        -- 链接 SDL2 库
        add_links("SDL2main", "SDL2", {public = true})
        
        -- Windows 系统库
        add_links("gdi32", "user32", "shell32", {public = true})
    end

-- 原有的XML解析器程序
target("ler_xml")
    set_kind("binary")
    add_files("ler_xml.c", "ler_xml_test.c")
    
-- 新增的Lua文件加载执行程序
target("lua_runner")
    set_kind("binary")
    add_files("test.c")
    add_deps("lua_lib")

-- LER DOM Test
target("ler_dom_test")
    set_kind("binary")
    add_files("ler_dom_test.c")
    add_files("ler_dom.c", "ler_dom_text.c", "ler_dom_button.c", "ler_dom_bar.c", "ler_dom_view.c", "ler_dom_default.c", "ler_xml.c")
    
    -- Demos only for test target
    add_files(lvgl_dir .. "/demos/**.c")

    add_deps("lvgl_lib", "lua_lib")

    add_includedirs(".")
    
    -- 编译选项
    add_defines("LV_BUILD_EXAMPLES=1")  -- Enable examples/demos
    add_defines("LV_USE_DEMO_WIDGETS=1")

-- LER Test (Main Application)
target("ler_test")
    set_kind("binary")
    add_files("ler_test.c")
    add_files("ler_file.c")
    add_files("ler_vm.c")
    add_files("ler_dom.c", "ler_dom_text.c", "ler_dom_button.c", "ler_dom_bar.c", "ler_dom_view.c", "ler_dom_default.c", "ler_xml.c")
    
    add_deps("lvgl_lib", "lua_lib")

    add_includedirs(".")
