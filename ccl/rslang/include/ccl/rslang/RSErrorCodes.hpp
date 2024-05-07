#pragma once

#include <cstdint>

namespace ccl::rslang {

// Note: error code format: 0xSTID, S - status, T - type, ID - index

//! Lexical errors enumeration
enum class LexerEID : uint32_t {
  unknownSymbol = 0x8203, // Неизвестный символ
};

//! Syntactic errors enumeration
enum class ParseEID : uint32_t {
  syntax = 0x8400, // Неизвестная синтаксическая ошибка
  missingParenthesis = 0x8406,      // Пропущена скобка ')'
  missingCurlyBrace = 0x8407,       // Пропущена скобка '}'
  invalidQuantifier = 0x8408,       // Некорректная кванторная декларация
  expectedDeclaration = 0x8414,     // Ожидалось объявление аргументов
  expectedLocal = 0x8415,           // Ожидалось имя локальной переменной
};

//! Semantic errors enumeration
enum class SemanticEID : uint32_t {
  localDoubleDeclare = 0x2801,      // Повторное использование одного и того же имени переменной
  localNotUsed = 0x2802,            // Переменная объявлена но не использована

  localUndeclared = 0x8801,          // Использование необъявленной переменной
  localShadowing = 0x8802,          // Повторное объявление переменной

  typesNotEqual = 0x8803,            // Некорректное использование операций
  globalNotTyped = 0x8804,          // Не определена типизация глобальной конституенты
  invalidDecart = 0x8805,            // Одна из проекций не является множеством
  invalidBoolean = 0x8806,          // Попытка взять булеан от элемента, не имеющего характер множества
  invalidTypeOperation = 0x8807,    // Применение ТМО к операндам, не имеющим характер множества
  invalidCard = 0x8808,              // Мощность множества не определена для элемента
  invalidDebool = 0x8809,            // Дебулеан берется от немножества
  globalFuncMissing = 0x880A,        // Неизвестное имя функции
  globalFuncWithoutArgs = 0x880B,    // Некорректное использование имени функции без аргументов
  invalidReduce = 0x8810,            // Red можно брать только от двойного булеана
  invalidProjectionTuple = 0x8811,  // Не определена проекция
  invalidProjectionSet = 0x8812,    // Большая проекция определена только для множеств!
  invalidEnumeration = 0x8813,      // Типизация аргументов перечисления не совпадает
  invalidBinding = 0x8814,           // Количество переменных в кортеже не соответствует размерности декартова произведения
  localOutOfScope = 0x8815,         // Использование имени вне области видимости
  invalidElementPredicate = 0x8816,  // Несоответствие типов для проверки принадлежности
  invalidArgsArity = 0x8818,       // Некорректное количество аргументов терм-функции
  invalidArgumentType = 0x8819,     // Типизация аргумента не совпадает с объявленной
  globalStructure = 0x881C,         // Родовая структура должна быть ступенью

  // TODO: следующий идентификатор сейчас не используется. Нужно подобрать место для проверки
  globalExpectedFunction = 0x881F,  // Ожидалось выражение объявления функции

  radicalUsage = 0x8821,            // Радикалы запрещены вне деклараций терм-функций
  invalidFilterArgumentType = 0x8822, // Типизация аргумента фильтра не корректна
  invalidFilterArity = 0x8823,      // Количество параметров фильра не соответствует количеству индексов
  arithmeticNotSupported = 0x8824,  // Для данного типа не поддерживается арифметика
  typesNotCompatible = 0x8825,      // Типы не совместимы в данном контексте
  orderingNotSupported = 0x8826,    // Для данного типа не поддерживается порядок элементов

  globalNoValue = 0x8840,           // Используется неинтерпретируемый глобальный идентификатор
  invalidPropertyUsage = 0x8841,    // Использование свойства в качестве значения
  globalMissingAST = 0x8842,        // Не удалось получить дерево разбора для глобального идентификатора
  globalFuncNoInterpretation = 0x8843, // Функция не интерпретируется для данных аргументов
};

//! Evaluation errors enumeration
enum class ValueEID : uint32_t {
  unknownError = 0x8A00,            // Неизвестная ошибка при вычислении выражения
  typedOverflow = 0x8A01,           // Превышен лимит числа элементов множества
  booleanLimit = 0x8A02,            // Превышен лимит числа элементов основания булеана
  globalMissingValue = 0x8A03,      // В выражении использована невычислимая конституента
  iterationsLimit = 0x8A04,         // Превышен предел количества итераций вычисления:
  invalidDebool = 0x8A05,           // Попытка взять debool от многоэлементного множества
  iterateInfinity = 0x8A06,         // Попытка перебрать бесконечное множество
};

} // namespace ccl::rslang