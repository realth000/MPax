#include "ast.h"

namespace SearchParser {

AST::AST() : m_rootNode(nullptr) {}

AST::~AST() {
  // TODO: Delete all AST node
}

bool AST::parse(const PlayContent *playContent) const { return false; }

void AST::setRootNode(ASTNode *node) { m_rootNode = node; }

}  // namespace SearchParser