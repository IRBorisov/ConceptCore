OpenCppCoverage --sources "%~dp0src" --sources "%~dp0header" --sources "%~dp0include" --modules "%~dp0build\Debug\ConceptCoreLibraryd.lib" --modules "%~dp0test\build\Debug\cclTest.exe" --excluded_line_regex "\s*\} else \{.*" --excluded_line_regex "\s*\}\s*" --excluded_line_regex "\s*\sthrow *" --excluded_line_regex "\sassert\(.*" --excluded_sources "rsparserimpl.cpp" --excluded_sources "rsparserimpl.y" --excluded_sources "stack.hh" -- %~dp0test\build\Debug\CCLTest.exe

pause