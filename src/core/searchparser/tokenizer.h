#ifndef MPAX_TOKENIZER_H
#define MPAX_TOKENIZER_H

#include <QtCore/QString>
#include <QtCore/QStringList>

namespace SearchParser {

struct TokenFormat {
  QString keyword;
  QString metadataKeyWord;
  QStringList text;
};

class Tokenizer {
 public:
  explicit Tokenizer();
  TokenFormat tokenize(const QString& rawString, bool* ok, QString* errString);

 private:
  QString m_rawString;
};

}  // namespace SearchParser

#endif  // MPAX_TOKENIZER_H
