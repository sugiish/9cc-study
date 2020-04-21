#include "9cc.h"

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *expr(Token **rest, Token *token);
Node *equality(Token **rest, Token *token);
Node *relational(Token **rest, Token *token);
Node *add(Token **rest, Token *token);
Node *mul(Token **rest, Token *token);
Node *unary(Token **rest, Token *token);
Node *primary(Token **rest, Token *token);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val)
{
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

static long get_number(Token *tok)
{
  if (tok->kind != TK_NUM)
    error_tok(tok, "expected a number");
  return tok->val;
}

Node *expr(Token **rest, Token *token)
{
  return equality(rest, token);
}

Node *equality(Token **rest, Token *token)
{
  Node *node = relational(&token, token);
  for (;;)
  {
    if (equal(token, "=="))
    {
      node = new_node(ND_EQ, node, relational(&token, token->next));
      continue;
    }
    if (equal(token, "!="))
    {
      node = new_node(ND_NEQ, node, relational(&token, token->next));
      continue;
    }
    *rest = token;
    return node;
  }
}

Node *relational(Token **rest, Token *token)
{
  Node *node = add(&token, token);
  for (;;)
  {
    if (equal(token, "<"))
    {
      node = new_node(ND_LT, node, add(&token, token->next));
      continue;
    }
    if (equal(token, "<="))
    {
      node = new_node(ND_LTEQ, node, add(&token, token->next));
      continue;
    }
    if (equal(token, ">"))
    {
      node = new_node(ND_LT, add(&token, token->next), node);
      continue;
    }
    if (equal(token, ">="))
    {
      node = new_node(ND_LTEQ, add(&token, token->next), node);
      continue;
    }
    *rest = token;
    return node;
  }
}

Node *add(Token **rest, Token *token)
{
  Node *node = mul(&token, token);
  for (;;)
  {
    if (equal(token, "+"))
    {
      node = new_node(ND_ADD, node, mul(&token, token->next));
      continue;
    }
    if (equal(token, "-"))
    {
      node = new_node(ND_SUB, node, mul(&token, token->next));
      continue;
    }
    *rest = token;
    return node;
  }
}

Node *mul(Token **rest, Token *token)
{
  Node *node = unary(&token, token);
  for (;;)
  {
    if (equal(token, "*"))
    {
      node = new_node(ND_MUL, node, unary(&token, token->next));
      continue;
    }
    if (equal(token, "/"))
    {
      node = new_node(ND_DIV, node, unary(&token, token->next));
    }
    *rest = token;
    return node;
  }
}

Node *unary(Token **rest, Token *token)
{
  if (equal(token, "+"))
    return primary(rest, token->next);
  if (equal(token, "-"))
    return new_node(ND_SUB, new_node_num(0), primary(rest, token->next));
  return primary(rest, token);
}

Node *primary(Token **rest, Token *token)
{
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (equal(token, "("))
  {
    Node *node = expr(&token, token->next);
    *rest = skip(token, ")");
    return node;
  }
  // そうでなければ数値のはず
  Node *node = new_node_num(get_number(token));
  *rest = token->next;
  return node;
}

Node *parse(Token *tokens)
{
  Node *node = expr(&tokens, tokens);
  if (tokens->kind != TK_EOF)
    error_tok(tokens, "extra token");
  return node;
}
