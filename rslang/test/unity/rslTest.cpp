//! Unity build for RSlang tests

#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "../src/testAsciiLexer.cpp"
#include "../src/testLiterals.cpp"
#include "../src/testAST.cpp"
#include "../src/testASTInterpreter.cpp"
#include "../src/testASTNormalizer.cpp"
#include "../src/testAuditor.cpp"
#include "../src/testInterpreter.cpp"
#include "../src/testLogger.cpp"
#include "../src/testRSExpr.cpp"
#include "../src/testRSGenerator.cpp"
#include "../src/testRSLexer.cpp"
#include "../src/testRSParser.cpp"
#include "../src/testRSToken.cpp"
#include "../src/testSDCompact.cpp"
#include "../src/testValueAuditor.cpp"
#include "../src/testTypeAuditor.cpp"
#include "../src/testStructuredData.cpp"
#include "../src/testTypification.cpp"