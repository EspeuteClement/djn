workspace "DJN"
	
	location "_gen"

	language "C++"

	architecture "x86_64"

	configurations { "Debug", "Release" }

	filter { "configurations:Debug" }
		-- We want debug symbols in our debug config
		symbols "On"

	filter { "configurations:Release" }
		-- Release should be optimized
		optimize "On"

	filter { }

	targetdir ("_bin/%{prj.name}/%{cfg.longname}")
	objdir ("_bin/obj/%{prj.name}/%{cfg.longname}")

function includeSDL2()
	includedirs "lib/SDL2/include"
end

function linkSDL2()
	libdirs "lib/SDL2/lib/x64"

	links {"SDL2"}
end

project "djn"
	kind "ConsoleApp"
	files "src/**"

	includedirs "src"

	includeSDL2()
	linkSDL2()

	debugdir "."

	entrypoint "mainCRTStartup"

project "djnPack"
	kind "ConsoleApp"
	files "tool/packer/**"

	includedirs {"inc", "scr/djn"}

	debugdir "."