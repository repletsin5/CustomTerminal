workspace "CustomTerminal"
	architecture "x64"
	configurations{
		"Debug",
		"Release"
	}
	
	outputdir =  "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
project "TerminalCore"
	location "TerminalCore"
	language "C++"
	kind "ConsoleApp"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")
	files
	{
		"%{prj.location}/**.h",
		"%{prj.location}/**.hpp",
		"%{prj.location}/**.c",
		"%{prj.location}/**.cpp"
	}
	includedirs{
		"PluginLib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

	defines
	{
	  "TERMINAL_DLL",
	}
	links{
		"PluginLib"
	}
	filter "configurations:Debug"
		runtime "Debug"
		defines "TERMINAL_DEBUG"	
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		defines "TERMINAL_RELEASE"
		optimize "On"


project "PluginLib"
	location "PluginLib"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")
	files
	{
		"%{prj.location}/**.h",
		"%{prj.location}/**.hpp",
		"%{prj.location}/**.c",
		"%{prj.location}/**.cpp"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

		defines
		{
			"TERMINAL_EXPORTS"
		}
	filter "configurations:Debug"
		defines "PLIB_DEBUG"	
		symbols "On"
		 runtime "Debug"
	filter "configurations:Release"
		defines "PLIB_RELEASE"
		runtime "Release"
		optimize "On"
		
project "DirectoryCommands"
	location "DirectoryCommands"
	kind "SharedLib"
	language "C++"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")
	files
	{
		"%{prj.location}/**.h",
		"%{prj.location}/**.hpp",
		"%{prj.location}/**.c",
		"%{prj.location}/**.cpp"
	}
	defines
	{
	  "TERMINAL_DLL",
	}
	links{
		"PluginLib"
	}
	includedirs{
		"PluginLib"
	}
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

	filter "configurations:Debug"
		defines "DC_DEBUG"	
		 runtime "Debug"
		symbols "On"
	filter "configurations:Release"
		defines "DC_RELEASE"
		 runtime "Release"
		optimize "On"
project "Updater"
	location "Updater"
	kind "ConsoleApp"
	language "C++"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")
	files
	{
		"%{prj.location}/**.h",
		"%{prj.location}/**.hpp",
		"%{prj.location}/**.c",
		"%{prj.location}/**.cpp"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "off"
		systemversion "latest"

		defines
		{
			"TERMINAL_EXPORTS"
		}
	filter "configurations:Debug"
		defines "UD_DEBUG"	
		symbols "On" 
		runtime "Debug"
	filter "configurations:Release"
		defines "UD_RELEASE"
		optimize "On"
		 runtime "Release"
