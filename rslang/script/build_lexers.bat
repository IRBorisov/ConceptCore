@echo off

reflex src/AsciiLexerImpl.l
powershell -Command "(gc AsciiLexerImpl.hpp) | Out-File -Encoding UTF8 AsciiLexerImpl.hpp"
move /Y AsciiLexerImpl.hpp header/AsciiLexerImpl.hpp 1>nul

reflex src/MathLexerImpl.l
powershell -Command "(gc MathLexerImpl.hpp) | Out-File -Encoding UTF8 MathLexerImpl.hpp"
move /Y MathLexerImpl.hpp header/MathLexerImpl.hpp 1>nul