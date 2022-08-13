#ifndef MPAX_AST_H
#define MPAX_AST_H

#include <QtCore/QMap>

#include "core/playcontent.h"

namespace SearchParser {

enum class ASTType : int { Statement, Branch, Unknown = -1 };

enum class ASTOpe : int { And, Or, None = -1 };

enum class ASTKeyword : int {
  Has,
  Is,
  Greater,
  Less,
  Equal,
  Missing,
  Present,
  Before,
  After,
  Since,
  During,
  Unknown = -1
};

static const QMap<QString, ASTOpe> ASTOpeMap = {{"AND", ASTOpe::And},
                                                {"OR", ASTOpe::Or}};

// clang-format off
static const QMap<QString, ASTKeyword> ASTKeywordMap = {
    {"HAS", ASTKeyword::Has}, {"IS", ASTKeyword::Is},
    {"GREATER", ASTKeyword::Greater}, {"LESS", ASTKeyword::Less}, {"EQUAL", ASTKeyword::Equal},
    {"MISSING", ASTKeyword::Missing}, {"PRESENT", ASTKeyword::Present},
    {"BEFORE", ASTKeyword::Before}, {"AFTER", ASTKeyword::After}, {"SINCE", ASTKeyword::Since}, {"DURING", ASTKeyword::Since}
};
// clang-format on

struct ASTNode {
  ASTNode() {
    type = ASTType::Unknown;
    opeKeyword = ASTOpe::None;
    metaKeyword = "";
    keyword = ASTKeyword::Unknown;
    word = "";
    parent = nullptr;
    leftChild = nullptr;
    rightChild = nullptr;
  }
  bool parse(const PlayContent *content,
             Qt::CaseSensitivity caseSensitivity) const;
  ASTType type;
  ASTOpe opeKeyword;
  QString metaKeyword;
  ASTKeyword keyword;
  QString word;
  ASTNode *parent;
  ASTNode *leftChild;
  ASTNode *rightChild;
};

class AST {
 public:
  explicit AST();
  ~AST();
  bool parse(const PlayContent *playContent,
             Qt::CaseSensitivity caseSensitivity) const;
  void setRootNode(ASTNode *node);
  ASTNode *rootNode() const;

 private:
  ASTNode *m_rootNode;
};

}  // namespace SearchParser

#endif  // MPAX_AST_H
