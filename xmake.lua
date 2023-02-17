add_rules("mode.debug", "mode.release")
add_includedirs("source/")
set_languages("cxx20")

target("JsonParser")
    set_kind("static")
    add_files("source/json/**.cpp")

target("Test")  
    set_kind("binary") 
    add_files("source/*.cpp")
    add_deps("JsonParser")