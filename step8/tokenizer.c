#include "9cc.h"

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// エラー箇所を報告する
void error_at(char *loc, char *input, char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - input;
  fprintf(stderr, "%s\n", input);
  fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_tok(Token *tok, char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  error_at(tok->str, tok->str, fmt, ap); // FIXME
}

// Consumes the current token if it matches `op`.
bool equal(Token *tok, char *op)
{
  return strlen(op) == tok->len && !strncmp(tok->str, op, tok->len);
}

void print_token(Token *token)
{
  printf("%d", token->kind);
  if (token->kind != TK_EOF)
    print_token(token->next);
  else
    printf("\n");
}

// Ensure that the current token is `op`.
Token *skip(Token *tok, char *op)
{
  if (!equal(tok, op))
    error_tok(tok, "expected '%s'", op);
  return tok->next;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str)
{
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

static bool startswith(char *p, char *q)
{
  return strncmp(p, q, strlen(q)) == 0;
}

// 入力文字列inputをトークナイズしてそれを返す
Token *tokenize(char *input)
{
  Token head;
  head.next = NULL;
  Token *cur = &head;
  char *p = input;

  while (*p)
  {
    // 空白文字をスキップ
    if (isspace(*p))
    {
      p++;
      continue;
    }

    if (startswith(p, "<=") || startswith(p, ">=") || startswith(p, "==") || startswith(p, "!="))
    {
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 2;
      p += 2;
      continue;
    }

    if (ispunct(*p))
    {
      cur = new_token(TK_RESERVED, cur, p++);
      cur->len = 1;
      continue;
    }

    if (isdigit(*p))
    {
      cur = new_token(TK_NUM, cur, p);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    error_at(p, input, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}
