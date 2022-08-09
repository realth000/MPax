#ifndef MPAX_TOKENIZER_H
#define MPAX_TOKENIZER_H

#include <QtCore/QString>

namespace SearchParser {

class Tokenizer {
 public:
  explicit Tokenizer(const QString &rawString);
  bool tokenize(QString *tokenString);

 private:
  QString m_rawString;
};

}  // namespace SearchParser

#endif  // MPAX_TOKENIZER_H
