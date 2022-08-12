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
  return ast;

failed:
  *ok = false;
  return AST();
}
}  // namespace SearchParser