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
        for (auto &w : word) {
          if (content->title.contains(w, caseSensitivity) ||
              content->artist.contains(w, caseSensitivity) ||
              content->albumTitle.contains(w, caseSensitivity) ||
              content->albumArtist.contains(w, caseSensitivity)) {
            continue;
          }
          return false;
        }
        return true;
      }
      if (word.isEmpty()) {
        return false;
      }
      for (auto &w : word) {
        if (content->value(metaKeyword)
            .toString()
            .contains(w, caseSensitivity)) {
          continue;
        }
        return false;
      }
      return true;
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
        default:return false;
      }
    }
    case ASTType::Unknown:
    default:return false;
  }
}
}  // namespace SearchParser