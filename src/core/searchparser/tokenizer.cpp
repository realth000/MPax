#include "tokenizer.h"

#include <QtCore/QDebug>

#include "tokens.h"

namespace SearchParser {

namespace {
bool isKeyword(const QString &s) { return keywords.contains(s); }
bool isOpeKeyword(const QString &s) { return operatorKeywords.contains(s); }
bool isMetaKeyword(const QString &s) { return metaKeywords.contains(s); }
}  // namespace

Tokenizer::Tokenizer() {}

TokenList Tokenizer::tokenize(const QString &rawString, bool *ok,
                              QString *errString) {
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

    // Record metaKeyword between %.
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
      t.type = TokenType::MetaKeyword;
      t.start = i;
      while (i < end) {
        if (rawString[i] == '%') {
          if (i < end - 1 && rawString[i + 1] != ' ') {
            *ok = false;
            *errString = QString("unexpected '%' at pos %1").arg(i);
            return {};
          }
          t.end = i - 1;
          if (!isMetaKeyword(t.content)) {
            *ok = false;
            *errString =
                QString("invalid metaKeyword '%1' at pos %2").arg(t.content, i);
            return {};
          }
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
    while (i < end) {
      if (rawString[i] == ' ' || rawString[i] == ')') {
        i--;
        break;
      }
      t.content.append(rawString[i]);
      i++;
    }
    t.end = i;
    if (!t.content.isEmpty()) {
      if (isKeyword(t.content)) {
        t.type = TokenType::Keyword;
      } else if (isOpeKeyword(t.content)) {
        t.type = TokenType::OpeKeyword;
      } else if (isMetaKeyword(t.content)) {
        t.type = TokenType::MetaKeyword;
      }
      tokenList.append(t);
    }
  }
  *ok = true;
  *errString = "";
  return tokenList;
}
}  // namespace SearchParser