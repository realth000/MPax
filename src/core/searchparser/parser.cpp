#include "parser.h"

#include <QtCore/QDebug>

#include "analyzer.h"
#include "tokenizer.h"
#include "tokens.h"

namespace SearchParser {

namespace {

enum Mark {
  Percent = 0,  // %
  Parentheses,  // ()
  Escape        /* \ */
};

struct MarkPart {
  int start;
  int end;
};

typedef MarkPart PercentPart;
typedef MarkPart ParenthesesPart;
typedef MarkPart EscapePart;

bool checkFormat(const QString &rawString) {
  qDebug() << "checkFormat:" << rawString;
  Tokenizer t;
  bool ok = false;
  QString errString;
  //  QString
  if (rawString.isEmpty()) {
    return false;
  }

  PercentPart currentPe = PercentPart{-1, -1};
  EscapePart currentEsc = EscapePart{-1, -1};
  QList<ParenthesesPart> paPartList;
  QList<ParenthesesPart> paPartTmpList;

  for (int i = 0; i < rawString.length(); i++) {
    /**
     * Check '(' and ')'.
     * Allowed to be nested.
     */
    if (rawString[i] == '(') {
      ParenthesesPart pa = ParenthesesPart{i, -1};
      paPartList.append(pa);
      paPartTmpList.append(pa);
      continue;
    }
    if (rawString[i] == ')') {
      if (paPartTmpList.isEmpty()) {
        // Unpaired )
        qDebug() << "invalid format: raw string has unpaired ')'";
        return false;
      }
      paPartTmpList.last().end = i;
      // Add operation in () here.
      // FIXME: Not support nested () here yet.
      TokenFormat tf =
          t.tokenize(rawString.mid(paPartTmpList.last().start + 1,
                                   paPartTmpList.last().end -
                                       paPartTmpList.last().start - 1),
                     &ok, &errString);
      if (ok) {
        qDebug() << "tokenize result:" << tf.keyword << tf.metadataKeyWord
                 << tf.text;
      } else {
        qDebug() << "tokenize result: failed;" << errString;
      }
      paPartTmpList.removeLast();
      continue;
    }
    /**
     * Check '%'.
     * Not allowed to be nested.
     */
    if (rawString[i] == '%') {
      if (currentPe.start == -1) {
        currentPe.start = i;
        continue;
      }
      const QString pePartWord =
          rawString.mid(currentPe.start + 1, i - currentPe.start - 1);
      qDebug() << "catch keyword inside '%': " << pePartWord;
      // Parse keyword here.
      currentPe.start = -1;
      currentPe.end = -1;
      continue;
    }
    /**
     * Check '\\'.
     * Not allowed to be nested.
     * End until space.\n
     * The character or word after '\\' should in:\n
     * 1. a '\\'
     * 2. a '%'
     * 3. a keyword: HAS, AND
     * 4. a metadata keyword: title album_title
     */
    if (rawString[i] == '\\') {
      // Skip next character.
      if (i < rawString.length() - 1) {
        QString escapedWord;
        escapedWord.append(rawString[i++]);
        while (i < rawString.length() && rawString[i] != ' ') {
          escapedWord.append(rawString[i]);
          i++;
        }
        qDebug().noquote() << "catch escape word: " << escapedWord;
      } else {
        // Last character is '\\'
        qDebug() << "invalid format: unexpected escape symbol '\\' at end of "
                    "raw string";
        return false;
      }
      continue;
    }
  }
  // Check after iterate.
  if (paPartTmpList.length() > 0) {
    qDebug() << "invalid format: unpaired '(' or ')' in raw string";
    return false;
  }
  if (currentPe.start != -1 || currentPe.end != -1) {
    qDebug() << "invalid format: unpaired '%' in raw string";
    return false;
  }
  return true;
}
}  // namespace

QList<std::function<bool(QString)>> parse(const QString &rawString, bool *ok,
                                          QString *errString) {
  if (ok == nullptr || errString == nullptr) {
    qDebug() << "error: empty input";
    return {};
  }

  if (!checkFormat(rawString)) {
    *ok = false;
    *errString = QStringLiteral("query statement not in valid format");
    return {};
  }
  return QList<std::function<bool(QString)>>();
}
}  // namespace SearchParser