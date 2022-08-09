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

TokenFormat Tokenizer::tokenize(const QString& rawString, bool* ok,
                                QString* errString) {
  if (ok == nullptr || errString == nullptr) {
    qDebug() << "tokenize error: null feedback not allowed in this step";
    return TokenFormat{};
  }
  TokenFormat f;
  const QStringList wordsList = rawString.split(' ', QString::SkipEmptyParts);
  if (wordsList.length() < 3) {
    // If using an expression, at least have 3 words.
    if (isKeyword(wordsList.last())) {
      *ok = false;
      *errString =
          QString("tokenize error: token part not complete: %1").arg(rawString);
      return TokenFormat{};
    }
    f.text.append(wordsList);
    *ok = true;
    *errString = "";
    return f;
  }
  if (!isMetadataKeyword(wordsList[0])) {
    *ok = false;
    *errString =
        QString("tokenize error: invalid metadata keyword: %1").arg(rawString);
    return TokenFormat{};
  }
  f.keyword = metadataKeywords[wordsList[0]];
  if (!isKeyword(wordsList[1])) {
    *ok = false;
    *errString = QString("tokenize error: invalid keyword: %1").arg(rawString);
    return TokenFormat{};
  }
  f.metadataKeyWord = wordsList[1];
  for (int i = 2; i < wordsList.length(); i++) {
    if (isKeyword(wordsList[i])) {
      *ok = false;
      *errString =
          QString("tokenize error: unexpected keyword: %1").arg(rawString);
      return TokenFormat{};
    }
    f.text.append(wordsList[i]);
  }
  *ok = true;
  *errString = "";
  return f;
}
}  // namespace SearchParser