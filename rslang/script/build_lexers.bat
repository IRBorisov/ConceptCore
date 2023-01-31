@echo off

reflex src/AsciiLexerImpl.l
powershell -Command "(gc AsciiLexerImpl.hpp) | Out-File -Encoding UTF8 AsciiLexerImpl.hpp"
move /Y AsciiLexerImpl.hpp header/AsciiLexerImpl.hpp 1>nul

reflex src/RSLexerImpl.l
powershell -Command "(gc RSLexerImpl.hpp) | Out-File -Encoding UTF8 RSLexerImpl.hpp"
move /Y RSLexerImpl.hpp header/RSLexerImpl.hpp 1>nul