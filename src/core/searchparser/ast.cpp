#include "ast.h"

#include <QtCore/QDebug>

namespace SearchParser {

AST::AST() : m_rootNode(nullptr) {}

AST::~AST() {
  // TODO: Delete all AST node
}

bool AST::parse(const PlayContent *playContent,
                Qt::CaseSensitivity caseSensitivity) const {
  if (m_rootNode == nullptr) {
    return false;
  }
  return m_rootNode->parse(playContent, caseSensitivity);
}

void AST::setRootNode(ASTNode *node) { m_rootNode = node; }

ASTNode *AST::rootNode() const { return m_rootNode; }

bool ASTNode::parse(const PlayContent *content,
                    Qt::CaseSensitivity caseSensitivity) const {
  switch (type) {
    case ASTType::Statement: {
      if (metaKeyword.isEmpty()) {
        return content->title.contains(word, caseSensitivity) ||
               content->artist.contains(word, caseSensitivity) ||
               content->albumTitle.contains(word, caseSensitivity) ||
               content->albumArtist.contains(word, caseSensitivity);
      }
      return content->value(metaKeyword)
          .toString()
          .contains(word, caseSensitivity);
    }
    case ASTType::Branch: {
      if (leftChild == nullptr || rightChild == nullptr) {
        return false;
      }
      switch (opeKeyword) {
        case ASTOpe::And:
          return leftChild->parse(content, caseSensitivity) &&
                 rightChild->parse(content, caseSensitivity);
        case ASTOpe::Or:
          return leftChild->parse(content, caseSensitivity) ||
                 rightChild->parse(content, caseSensitivity);
        case ASTOpe::None:
        default:
          return false;
      }
    }
    case ASTType::Unknown:
    default:
      return false;
  }
}
}  // namespace SearchParser