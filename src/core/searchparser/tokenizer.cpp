#include "tokenizer.h"

#include <QtCore/QDebug>

#include "tokens.h"

namespace SearchParser {

namespace {
bool isKeyword(const QString& s) { return keywords.contains(s); }
bool isMetadataKeyword(const QString& s) {
  return metadataKeywords.contains(s);
}
}  // namespace

Tokenizer::Tokenizer() {}

TokenList Tokenizer::tokenize(const QString& rawString, bool* ok,
                              QString* errString) {
  if (ok == nullptr || errString == nullptr) {
    qDebug() << "tokenize error: null feedback not allowed in this step";
    return TokenList{};
  }
  const int end = rawString.length();
  TokenList tokenList;
  for (int i = 0; i < end; i++) {
    // Skip space.
    if (rawString[i] == ' ') {
      continue;
    }
    // Consider the word after '\\' as plain text.
    if (rawString[i] == '\\') {
      if (i == end - 1) {
        *ok = false;
        *errString = "unexpected extra '\\' at the end of raw string";
        return {};
      }
      i++;
      Token t;
      t.type = TokenType::Word;
      t.start = i;
      while (i < end && rawString[i] != ' ') {
        t.content.append(rawString[i]);
        i++;
      }
      t.end = i - 1;
      tokenList.append(t);
      continue;
    }
    // Record '('.
    if (rawString[i] == '(') {
      Token t = Token(TokenType::LeftParentheses, i);
      tokenList.append(t);
      continue;
    }
    // Record ')'.
    if (rawString[i] == ')') {
      Token t = Token(TokenType::RightParentheses, i);
      tokenList.append(t);
      continue;
    }

    // Record keyword between %.
    if (rawString[i] == '%') {
      if (i == end - 1) {
        *ok = false;
        *errString = QString("unpaired '%' at end of raw string 1");
        return {};
      }
      if (rawString[++i] == '%') {
        *ok = false;
        *errString = QString("empty quote in '%' at pos %1").arg(i);
        return {};
      }
      Token t;
      t.type = TokenType::Percent;
      t.start = i;
      while (i < end) {
        if (rawString[i] == '%') {
          if (i < end - 1 && rawString[i + 1] != ' ') {
            *ok = false;
            *errString = QString("unexpected '%' at pos %1").arg(i);
            return {};
          }
          t.end = i - 1;
          tokenList.append(t);
          i++;
          break;
        }
        if (rawString[i] == '\\' || rawString[i] == ')' ||
            rawString[i] == ')') {
          *ok = false;
          *errString =
              QString("unexpected '%1' at pos %2").arg(rawString[i], i);
          return {};
        }
        if (i == end - 1) {
          *ok = false;
          *errString = QString("expect '%' at end of raw string");
          return {};
        }
        t.content.append(rawString[i]);
        i++;
      }
    }
    // Record plain text.
    Token t;
    t.type = TokenType::Word;
    t.start = i;
    while (i < end && rawString[i] != ' ') {
      t.content.append(rawString[i]);
      i++;
    }
    t.end = i;
    if (!t.content.isEmpty()) {
      tokenList.append(t);
    }
  }
  *ok = true;
  *errString = "";
  return tokenList;
}
}  // namespace SearchParser