cmake_minimum_required(VERSION 3.7.2)

if (${PLATFORM} IN_LIST KernelX86Sel4Arch_all_strings)
	set(KernelArch x86 CACHE STRING "" FORCE)
	set(KernelX86Sel4Arch ${PLATFORM} CACHE STRING "" FORCE)
endif()

include(tools/cmake-tool/default-CMakeLists.txt)

if(SIMULATION)
	ApplyCommonSimulationSettings("x86")
else()
	if(KernelArchX86)
		set(KernelIOMMU ON CACHE BOOL "" FORCE)
	endif()
endif()

ApplyCommonReleaseVerificationSettings(FALSE FALSE)

GenerateSimulateScript()
