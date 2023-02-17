add_rules("mode.debug", "mode.release")
add_includedirs("source/")

target("main")
    set_languages("cxx20")
    set_kind("binary") 
    add_files("source/**.cpp")