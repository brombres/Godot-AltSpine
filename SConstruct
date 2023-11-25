#!/usr/bin/env python
import os
import sys

env = SConscript("Libraries/godot-cpp/SConstruct")

# For the reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

def add_sources(sources, dir, extension):
  for f in os.listdir(dir):
    if f.endswith("." + extension):
      sources.append(dir + "/" + f)

env.Append(CPPPATH=["Source/"])
sources = Glob("Source/*.cpp")

env.Append( CCFLAGS=[ "-I", "Libraries/spine-runtimes/spine-cpp/spine-cpp/include" ] )

if env["platform"] == "macos":
  add_sources(sources, "Source", "m")
  add_sources(sources, "Source", "mm")

  # Example - uncomment and adjust as desired
  #env.Append(
  #    LINKFLAGS=[
  #        "-framework",
  #        "Foundation"
  #    ]
  #)

  library = env.SharedLibrary(
    "Build/libGDSpine.{}.{}.framework/libGDSpine.{}.{}".format(
      env["platform"], env["target"], env["platform"], env["target"]
    ),
    source=sources,
  )

else:
  if env["platform"] == "ios":
    add_sources(sources, "Source", "m")
    add_sources(sources, "Source", "mm")

    # Example - uncomment and adjust as desired
    #env.Append(
    #    LINKFLAGS=[
    #        "-framework",
    #        "Foundation"
    #    ]
    #)

  library = env.SharedLibrary(
    "Build/libGDSpine{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
    source=sources,
  )

Default(library)
