#include "analyzer.h"

#include <QtCore/QDebug>

#include "tokens.h"

namespace SearchParser {
Analyzer::Analyzer() {}

AST Analyzer::analyze(const TokenList &tf, bool *ok, QString *errString) {
  if (ok == nullptr || errString == nullptr) {
    qDebug() << "analyze error: null feedback not allowed in this step";
    return AST();
  }
  AST ast;
  ASTNode *rootNode = new ASTNode;
  ASTNode *currentNode = rootNode;
  if (tf.isEmpty()) {
    *errString = "analyze error: empty token list";
    goto failed;
  }
  for (auto &t : tf) {
    switch (t.type) {
      case TokenType::Word: {
        if (currentNode->type == ASTType::Branch) {
          *errString = "analyze error: empty token list";
          goto failed;
        }
        currentNode->type = ASTType::Statement;
        currentNode->word = t.content;
      } break;
      case TokenType::Keyword: {
        if (currentNode->type != ASTType::Statement) {
          // ERROR: should be statement type;
          *errString = "analyze error: should be statement type";
          goto failed;
        }
        if (!ASTKeywordMap.contains(t.content)) {
          // ERROR: should contain keyword.
          *errString = "analyze error: unknown keyword";
          goto failed;
        }
        currentNode->keyword = ASTKeywordMap[t.content];
      } break;
      case TokenType::OpeKeyword: {
        if (currentNode->parent != nullptr) {
          currentNode = currentNode->parent;
        }
        if (currentNode->type != ASTType::Branch) {
          // ERROR: should be branch type.
          *errString = "analyze error: not a branch type";
          goto failed;
        }
        if (!ASTOpeMap.contains(t.content)) {
          // ERROR: should contain operator.
          *errString = "analyze error: unknown operator";
          goto failed;
        }
        currentNode->opeKeyword = ASTOpeMap[t.content];
      } break;
      case TokenType::MetaKeyword: {
        if (currentNode->type != ASTType::Unknown) {
          // ERROR: type should not set yet.
          *errString =
              "analyze error: error setting mete keyword, type already set";
          goto failed;
        }
        currentNode->type = ASTType::Statement;
        currentNode->metaKeyword == metaKeywords[t.content];
      } break;
      case TokenType::LeftParentheses: {
        if (currentNode->type == ASTType::Statement) {
          // ERROR: type should not set yet.
          *errString = "analyze error: error parsing '(', type already set";
          goto failed;
        }
        currentNode->type = ASTType::Branch;
        if (currentNode->leftChild == nullptr) {
          ASTNode *node = new ASTNode;
          currentNode->leftChild = node;
          node->parent = currentNode;
          currentNode = currentNode->leftChild;
          rootNode = node;
        } else if (currentNode->rightChild == nullptr) {
          ASTNode *node = new ASTNode;
          currentNode->rightChild = node;
          node->parent = currentNode;
          currentNode = currentNode->rightChild;
          rootNode = node;
        } else {
          // ERROR: all children were set.
          *errString = "analyze error: all children were set";
          goto failed;
        }
      } break;
      case TokenType::RightParentheses: {
        if (currentNode->parent == nullptr) {
          // ERROR: parent should not be null.
          *errString = "analyze error: error parsing '(', null parent";
          goto failed;
        }
        currentNode = currentNode->parent;
        if (currentNode->leftChild == nullptr ||
            currentNode->rightChild == nullptr) {
          // ERROR: children should not be null.
          *errString = "analyze error: error parsing '(', null children";
          goto failed;
        }
        if (currentNode->type != ASTType::Branch) {
          // ERROR: type should be branch type.
          *errString = "analyze error: error parsing '(', not a branch type";
          goto failed;
        }
      } break;
      case TokenType::Unknown:
      default:
        break;
    }
  }
  for (auto &t : tf) {
    qDebug() << t.start << t.end << t.type << t.content;
  }
  qDebug() << "----------";
  ast.setRootNode(rootNode);
  if (!validate(ast, ok, errString)) {
    *errString = "analyze error: " + *errString;
    goto failed;
  }
  return ast;

failed:
  *ok = false;
  return AST();
}

bool Analyzer::validate(const AST &a, bool *ok, QString *errString) {
  if (ok == nullptr || errString == nullptr) {
    qDebug() << "validating failed, null feedback not allowed";
    return false;
  }
  ASTNode *rootNode = a.rootNode();
  ASTNode *currentNode = nullptr;

  QList<ASTNode *> nodeList;
  nodeList.append(rootNode);
  while (!nodeList.isEmpty()) {
    currentNode = nodeList.last();
    if (currentNode == nullptr) {
      *errString = "validating failed, null node";
      goto failed;
    }
    // Check current node.
    if (!isValidASTNode(currentNode, ok, errString)) {
      *errString = "validating failed, " + *errString;
      goto failed;
    }
    nodeList.pop_back();
    if (currentNode->rightChild != nullptr) {
      nodeList.push_back(currentNode->rightChild);
    }
    if (currentNode->leftChild != nullptr) {
      nodeList.push_back(currentNode->leftChild);
    }

    break;
  }
  *ok = true;
  *errString = "";
  return true;

failed:
  *ok = false;
  return false;
}

bool Analyzer::isValidASTNode(const ASTNode *node, bool *ok,
                              QString *errString) const {
  switch (node->type) {
    case ASTType::Statement: {
      if (node->leftChild != nullptr || node->rightChild != nullptr) {
        *errString = "statement type node has child";
        goto failed;
      }
      const QString metaKeyword = node->metaKeyword;
      const ASTKeyword keyword = node->keyword;
      const QString word = node->word;
      if (node->keyword == ASTKeyword::Unknown) {
        *errString = "unknown keyword in statement type node";
        goto failed;
      }
      if (word.isEmpty()) {
        *errString = "empty query text";
        goto failed;
      }
    } break;
    case ASTType::Branch: {
      if (node->leftChild == nullptr || node->rightChild == nullptr) {
        *errString = "branch type node has null child";
        goto failed;
      }
      if (node->opeKeyword == ASTOpe::None) {
        *errString = "operate not set";
        goto failed;
      }
    } break;
    case ASTType::Unknown:
    default:
      *errString = "unknown node type";
      goto failed;
  }

  *ok = true;
  return true;

failed:
  *ok = false;
  return false;
}

}  // namespace SearchParser