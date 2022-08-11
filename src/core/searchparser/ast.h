#ifndef MPAX_AST_H
#define MPAX_AST_H

#include "core/playcontent.h"

namespace SearchParser {

class AST {
 public:
  explicit AST();
  bool parse(const PlayContent *playContent) const;
};

}  // namespace SearchParser

#endif  // MPAX_AST_H
