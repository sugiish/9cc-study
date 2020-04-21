#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum
{
  TK_RESERVED, // 記号
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token
{
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  int len;        // トークンの長さ
};

void print_token(Token *token);

// 抽象構文木のノードの種類
typedef enum
{
  ND_ADD,  // +
  ND_SUB,  // -
  ND_MUL,  // *
  ND_DIV,  // /
  ND_NUM,  // 整数
  ND_EQ,   // ==
  ND_NEQ,  // !=
  ND_LT,   // <
  ND_LTEQ, // <=
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node
{
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // kindがND_NUMの場合のみ使う
};

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...);
// エラー箇所を報告する
void error_at(char *loc, char *input, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);

// Consumes the current token if it matches `op`.
bool equal(Token *tok, char *op);

// Ensure that the current token is `op`.
Token *skip(Token *tok, char *op);

static long get_number(Token *tok);

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str);

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p);
Node *parse(Token *tokens);
void generate(Node *node);
