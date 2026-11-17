
#include "util.c"


Expr* parse_primary(TokenArray* tokens, int begin,int end);
Expr* parse_unary(TokenArray* tokens, int begin, int end);
Expr* parse_multiplicative(TokenArray* tokens, int begin, int end);
Expr* parse_additive(TokenArray* tokens, int begin, int end);
Expr* parse_comparison(TokenArray* tokens, int begin, int end);
Expr* parse_equality(TokenArray* tokens, int begin, int end);

Statement* parse_assign_statement(TokenArray* tokens, int *pos);
Statement* parse_let_statement(TokenArray* tokens, int *pos);
Statement* parse_if_statement(TokenArray* tokens, int *pos);
Statement* parse_while_statement(TokenArray* tokens, int *pos);
Statement* parse_block(TokenArray* tokens, int *pos);
Statement* parse_print_statement(TokenArray* tokens, int *pos);
Program* parse(TokenArray* tokens);


/*()と変数と数値を解析する*/
Expr* parse_primary(TokenArray* tokens, int begin, int end) {
    if (begin >= end) return NULL;

    int depth=0;
    for(int i = begin; i < end; i++) {
        if(tokens->tokens[i].type == TK_LPAREN) {
            depth++;
        } else if(tokens->tokens[i].type == TK_RPAREN) {
            depth--;
        }
    }

    if(depth != 0){
        exit(1);
    };

    Token* t = &tokens->tokens[begin];

    if (end == begin + 1) {
        Expr *expr = malloc(sizeof(*expr));

        if (t->type == TK_NUMBER) {
            expr->type = EXPR_NUMBER;
            expr->data.number.number = atoll(t->text);
            return expr;
        }

        if (t->type == TK_IDENT) {
            expr->type = EXPR_VARIABLE;
            expr->data.variable.var_name = t->text;
            return expr;
        }
    }

    if(end == begin + 3){
        if(tokens->tokens[begin].type == TK_INPUT && 
           tokens->tokens[begin + 1].type == TK_LPAREN && 
           tokens->tokens[begin + 2].type == TK_RPAREN){
            // input() の形式
            Expr *expr = malloc(sizeof(*expr));
            expr->type = EXPR_INPUT;
            expr->data.input.op = TK_INPUT;
            return expr;
        }
    }


    if (t->type == TK_LPAREN &&
        tokens->tokens[end - 1].type == TK_RPAREN) {
        // 区間全体がこの括弧対で囲まれていることを確認
        return parse_equality(tokens, begin + 1, end - 1);
    }

    return NULL;
}

/** 単項演算子を解析する */
Expr* parse_unary(TokenArray* tokens, int begin, int end){
    if (begin >= end) return NULL;

    if (tokens->tokens[begin].type == TK_MINUS) {
        if (begin + 1 >= end) return NULL;

        Expr* expr = malloc(sizeof(*expr));
        expr->type = EXPR_UNARY;
        expr->data.unary.op = tokens->tokens[begin].type;
        expr->data.unary.operand = parse_unary(tokens, begin + 1, end);
        if (expr->data.unary.operand == NULL) {
            free(expr);
            return NULL;
        }
        return expr;
    }

    return parse_primary(tokens, begin, end);
}

Expr* parse_multiplicative(TokenArray* tokens, int begin, int end){
    Expr* expr = malloc(sizeof(Expr));
    int depth=0;
    for(int i = end-1; i >= begin; i--){
        if(tokens->tokens[i].type == TK_LPAREN) {
            depth++;
        } else if(tokens->tokens[i].type == TK_RPAREN) {
            depth--;
        }

        if(depth != 0) continue;

        if(tokens->tokens[i].type == TK_STAR){
            expr->type = EXPR_BINARY;
            expr->data.binary.op = TK_STAR;
            expr->data.binary.left = parse_multiplicative(tokens, begin, i);
            expr->data.binary.right = parse_unary(tokens, i+1, end);
            if(expr->data.binary.left == NULL || expr->data.binary.right == NULL){
                fprintf(stderr, "Expected expression in multiplication\n");
                exit(1);
            }
            return expr;
        }
        else if(tokens->tokens[i].type == TK_SLASH){
            expr->type = EXPR_BINARY;
            expr->data.binary.op = TK_SLASH;
            expr->data.binary.left = parse_multiplicative(tokens, begin, i);
            expr->data.binary.right = parse_unary(tokens, i+1, end);
            if(expr->data.binary.left == NULL || expr->data.binary.right == NULL){
                fprintf(stderr, "Expected expression in division\n");
                exit(1);
            }
            return expr;
        }
    }
    free(expr);
    return parse_unary(tokens, begin, end);
}

/*足し算と引き算を解析する*/
Expr* parse_additive(TokenArray* tokens, int begin, int end){
    Expr* expr = malloc(sizeof(Expr));
    int depth=0;
    // TODO: Implement additive parsing
    for(int i = end-1; i >= begin; i--){
        if(tokens->tokens[i].type == TK_LPAREN) {
            depth++;
        } else if(tokens->tokens[i].type == TK_RPAREN) {
            depth--;
        }

        if(depth != 0) continue;

        if(tokens->tokens[i].type == TK_PLUS){
            expr->type = EXPR_BINARY;
            expr->data.binary.op = TK_PLUS;
            expr->data.binary.left = parse_additive(tokens, begin, i);
            expr->data.binary.right = parse_multiplicative(tokens, i+1, end);
            if(expr->data.binary.left == NULL || expr->data.binary.right == NULL){
                fprintf(stderr, "Expected expression in addition\n");
                exit(1);
            }
            return expr;
        }
        else if(tokens->tokens[i].type == TK_MINUS){
            //先頭のマイナスは二項減算にはなり得ない
            if(i == 0) {
                // TODO: Implement unary minus
                continue;
            }
            // TODO: Implement subtraction
            if(tokens->tokens[i-1].type != TK_NUMBER && tokens->tokens[i-1].type != TK_IDENT && tokens->tokens[i-1].type != TK_RPAREN){
                continue;
            }
            expr->type = EXPR_BINARY;
            expr->data.binary.op = TK_MINUS;
            expr->data.binary.left = parse_additive(tokens, begin, i);
            expr->data.binary.right = parse_multiplicative(tokens, i+1, end);
            if(expr->data.binary.left == NULL || expr->data.binary.right == NULL){
                fprintf(stderr, "Expected expression in subtraction\n");
                exit(1);
            }
            return expr;    
        }
    }
    free(expr);
    return parse_multiplicative(tokens, begin, end);
}

Expr* parse_comparison(TokenArray* tokens, int begin, int end){
    Expr* expr = malloc(sizeof(Expr));
    int depth=0;
    // TODO: Implement comparison parsing
    for(int i = end-1; i >= begin; i--){
        if(tokens->tokens[i].type == TK_LPAREN) {
            depth++;
        } else if(tokens->tokens[i].type == TK_RPAREN) {
            depth--;
        }

        if(depth != 0) continue;

        if(tokens->tokens[i].type == TK_LE){
            expr->type = EXPR_BINARY;
            expr->data.binary.op = TK_LE;
            expr->data.binary.left = parse_comparison(tokens, begin, i);
            expr->data.binary.right = parse_additive(tokens, i+1, end);
            if(expr->data.binary.left == NULL || expr->data.binary.right == NULL){
                fprintf(stderr, "Expected expression in less than or equal to\n");
                exit(1);
            }
            return expr;
        }
        else if(tokens->tokens[i].type == TK_GE){
            // TODO: Implement greater than or equal to
            expr->type = EXPR_BINARY;
            expr->data.binary.op = TK_GE;
            expr->data.binary.left = parse_comparison(tokens, begin, i);
            expr->data.binary.right = parse_additive(tokens, i+1, end);
            if(expr->data.binary.left == NULL || expr->data.binary.right == NULL){
                fprintf(stderr, "Expected expression in greater than or equal to\n");
                exit(1);
            }
            return expr;
        }
        else if(tokens->tokens[i].type == TK_GT){
            // TODO: Implement greater than
            expr->type = EXPR_BINARY;
            expr->data.binary.op = TK_GT;
            expr->data.binary.left = parse_comparison(tokens, begin, i);
            expr->data.binary.right = parse_additive(tokens, i+1, end);
            if(expr->data.binary.left == NULL || expr->data.binary.right == NULL){
                fprintf(stderr, "Expected expression in greater than\n");
                exit(1);
            }
            return expr;    
        }
        else if(tokens->tokens[i].type == TK_LT){
            // TODO: Implement less than
            expr->type = EXPR_BINARY;
            expr->data.binary.op = TK_LT;
            expr->data.binary.left = parse_comparison(tokens, begin, i);
            expr->data.binary.right = parse_additive(tokens, i+1, end);
            if(expr->data.binary.left == NULL || expr->data.binary.right == NULL){
                fprintf(stderr, "Expected expression in less than\n");
                exit(1);
            }
            return expr;
        }
    }
    free(expr);
    return parse_additive(tokens, begin, end);
}

/*==と!=の比較*/
Expr* parse_equality(TokenArray* tokens, int begin, int end){
    Expr* expr = malloc(sizeof(Expr));
    int depth=0;
    // TODO: Implement equality parsing
    for(int i = end-1; i >= begin; i--){
        if(tokens->tokens[i].type == TK_LPAREN) {
            depth++;
        } else if(tokens->tokens[i].type == TK_RPAREN) {
            depth--;
        }
        if(depth != 0) continue;
        if(tokens->tokens[i].type == TK_EQ){
            expr->type = EXPR_BINARY;
            expr->data.binary.op = TK_EQ;
            expr->data.binary.left = parse_equality(tokens, begin, i);
            expr->data.binary.right = parse_comparison(tokens, i+1, end);
            if(expr->data.binary.left == NULL || expr->data.binary.right == NULL){
                fprintf(stderr, "Expected expression in equality\n");
                exit(1);
            }
            return expr;
        }
        else if(tokens->tokens[i].type == TK_NE){
            // TODO: Implement not equal to
            expr->type = EXPR_BINARY;
            expr->data.binary.op = TK_NE;
            expr->data.binary.left = parse_equality(tokens, begin, i);
            expr->data.binary.right = parse_comparison(tokens, i+1, end);
            if(expr->data.binary.left == NULL || expr->data.binary.right == NULL){
                fprintf(stderr, "Expected expression in not equal to\n");
                exit(1);
            }
            return expr;
        }
    }
    free(expr);
    return parse_comparison(tokens, begin, end);
}

Statement* parse_assign_statement(TokenArray* tokens, int *pos){
    Statement* statement = malloc(sizeof(Statement));
    statement->type = STMT_ASSIGN;
    if(tokens->tokens[*pos+1].type != TK_ASSIGN){
        fprintf(stderr, "Expected '=' after identifier in assignment statement\n");
        exit(1);
    }
    int begin = *pos;
    while( ((size_t)*pos < tokens->count) && (tokens->tokens[*pos].type != TK_SEMICOLON))
    {
        if(tokens->tokens[*pos].type == TK_EOF){
            fprintf(stderr, "Expected ';' at the end of assignment statement\n");
            exit(1);
        }
        else if(tokens->tokens[*pos].type == TK_RBRACE){
            fprintf(stderr, "Expected ';' at the end of assignment statement\n");
            exit(1);
        }
        else if(tokens->tokens[*pos].type == TK_LBRACE){
            fprintf(stderr, "Expected ';' at the end of assignment statement\n");
            exit(1);
        }
        (*pos)++;
    }
    int end = *pos;
    statement->data.assign.name = copy_text(tokens->tokens[begin].text, strlen(tokens->tokens[begin].text));
    statement->data.assign.value = parse_equality(tokens, begin + 2, end);
    if(statement->data.assign.value == NULL){
        fprintf(stderr, "Expected expression after '=' in assignment statement\n");
        exit(1);
    }
    (*pos)++;
    return statement;
}

Statement* parse_let_statement(TokenArray* tokens, int *pos){
    Statement* statement = malloc(sizeof(Statement));
    statement->type = STMT_LET;
    if(tokens->tokens[*pos+1].type != TK_IDENT){
        fprintf(stderr, "Expected identifier after 'let'\n");
        exit(1);
    }
    if(tokens->tokens[*pos+2].type != TK_ASSIGN){
        fprintf(stderr, "Expected '=' after identifier in 'let' statement\n");
        exit(1);
    }

    int begin = *pos;
    while( ((size_t)*pos < tokens->count) && (tokens->tokens[*pos].type != TK_SEMICOLON) )
    {
        if(tokens->tokens[*pos].type == TK_EOF){
            fprintf(stderr, "Expected ';' at the end of 'let' statement\n");
            exit(1);
        }
        else if(tokens->tokens[*pos].type == TK_RBRACE){
            fprintf(stderr, "Expected ';' at the end of 'let' statement\n");
            exit(1);
        }
        else if(tokens->tokens[*pos].type == TK_LBRACE){
            fprintf(stderr, "Expected ';' at the end of 'let' statement\n");
            exit(1);
        }
        (*pos)++;
    }
    int end = *pos;
    statement->data.let.name = copy_text(tokens->tokens[begin + 1].text, strlen(tokens->tokens[begin + 1].text));
    statement->data.let.value = parse_equality(tokens, begin + 3, end);
    (*pos)++;
    if(statement->data.let.value == NULL){
        fprintf(stderr, "Expected expression after '=' in 'let' statement\n");
        exit(1);
    }
    // ここで文を解析
    return statement;
}
/*<if-statement> ::= "if" "(" <expr> ")" <block> <else-opt>*/
Statement* parse_if_statement(TokenArray* tokens, int *pos){
    Statement* statement = malloc(sizeof(Statement));
    statement->type = STMT_IF;
    int depth=0;
    int begin= *pos;
    (*pos)++;
    if((size_t)*pos >= tokens->count || tokens->tokens[*pos].type != TK_LPAREN){
        fprintf(stderr, "Expected '(' after 'if'\n");
        exit(1);
    }
    depth = 1;
    (*pos)++;

    while((size_t)*pos < tokens->count && depth != 0){
        if(tokens->tokens[*pos].type == TK_EOF ||
           tokens->tokens[*pos].type == TK_LBRACE ||
           tokens->tokens[*pos].type == TK_RBRACE){
            break;
        }
        if(tokens->tokens[*pos].type == TK_LPAREN){
            depth++;
        }
        if(tokens->tokens[*pos].type == TK_RPAREN){
            depth--;
        }
        (*pos)++;
    }
    if(depth != 0 || *pos - 1 <= begin + 2){
        fprintf(stderr, "Expected condition and closing ')' after 'if'\n");
        exit(1);
    }
    statement->data.if_stmt.condition = parse_equality(tokens, begin + 2, *pos - 1);
    if(statement->data.if_stmt.condition == NULL){
        fprintf(stderr, "Expected condition after 'if'\n");
        exit(1);
    }

    statement->data.if_stmt.then_branch = parse_block(tokens, pos);

    if((size_t)*pos < tokens->count && tokens->tokens[*pos].type == TK_ELSE){
        (*pos)++;
        statement->data.if_stmt.else_branch = parse_block(tokens, pos);
    }
    else{
        statement->data.if_stmt.else_branch = NULL;
    }
    return statement;
}

/*
<statement-list> ::= ε
                   | <statement> <statement-list>

<statement> ::= <let-statement>
              | <assign-statement>
              | <print-statement>
              | <if-statement>
              | <while-statement>
              | <block>
              | <expr-statement>

<let-statement> ::= "let" <ident> "=" <expr> ";"

<assign-statement> ::= <ident> "=" <expr> ";"

<print-statement> ::= "print" "(" <expr> ")" ";"

<expr-statement> ::= <expr> ";"

<block> ::= "{" <statement-list> "}"

<if-statement> ::= "if" "(" <expr> ")" <block> <else-opt>

<else-opt> ::= ε
             | "else" <block>

<while-statement> ::= "while" "(" <expr> ")" <block>
*/
Statement* parse_while_statement(TokenArray* tokens, int *pos){
    // TODO: Implement while statement parsing
    Statement* statement = malloc(sizeof(*statement));
    statement->type = STMT_WHILE;
    int paren_depth=0;

    int start = *pos;
    (*pos)++;
    if((size_t)*pos >= tokens->count || tokens->tokens[*pos].type != TK_LPAREN){
        fprintf(stderr, "Expected '(' after 'while'\n");
        exit(1);
    }
    paren_depth = 1;
    (*pos)++;
    while((size_t)*pos < tokens->count && paren_depth != 0){
        if(tokens->tokens[*pos].type == TK_EOF ||
           tokens->tokens[*pos].type == TK_LBRACE ||
           tokens->tokens[*pos].type == TK_RBRACE){
            break;
        }
        if(tokens->tokens[*pos].type == TK_LPAREN){
            paren_depth++;
        }
        if(tokens->tokens[*pos].type == TK_RPAREN){
            paren_depth--;
        }
        (*pos)++;
    }
    if(paren_depth != 0 || *pos - 1 <= start + 2){
        fprintf(stderr, "Expected condition and closing ')' after 'while'\n");
        exit(1);
    }
    statement->data.while_stmt.condition=parse_equality(tokens, start + 2, *pos - 1);
    if(statement->data.while_stmt.condition == NULL){
        fprintf(stderr, "Expected condition after 'while'\n");
        exit(1);
    }

    statement->data.while_stmt.body = parse_block(tokens, pos);
    return statement;
}

Statement* parse_block(TokenArray* tokens, int *pos){
    // TODO: Implement block parsing
    if((size_t)*pos >= tokens->count || tokens->tokens[*pos].type != TK_LBRACE){
        fprintf(stderr, "Expected '{'\n");
        exit(1);
    }
    Statement *statement = malloc(sizeof(*statement));
    statement->type = STMT_BLOCK;
    statement->data.block.count = 0;
    statement->data.block.capacity = tokens->count;
    statement->data.block.statements = malloc(sizeof(Statement*) * tokens->count);
    (*pos)++;// Skip the opening brace
    while(((size_t)*pos < tokens->count) && (tokens->tokens[*pos].type != TK_RBRACE)){
        if(tokens->tokens[*pos].type == TK_IDENT){
            statement->data.block.statements[statement->data.block.count++] = parse_assign_statement(tokens, pos);
        }
        else if(tokens->tokens[*pos].type == TK_LET){
            statement->data.block.statements[statement->data.block.count++] = parse_let_statement(tokens, pos);
        }
        else if(tokens->tokens[*pos].type == TK_PRINT){
            statement->data.block.statements[statement->data.block.count++] = parse_print_statement(tokens, pos);
        }
        else if(tokens->tokens[*pos].type == TK_IF){
            statement->data.block.statements[statement->data.block.count++] = parse_if_statement(tokens, pos);
        }
        else if(tokens->tokens[*pos].type == TK_WHILE){
            statement->data.block.statements[statement->data.block.count++] = parse_while_statement(tokens, pos);
        }
        else if(tokens->tokens[*pos].type == TK_LBRACE){
            statement->data.block.statements[statement->data.block.count++] = parse_block(tokens, pos);
        }
        else if(tokens->tokens[*pos].type == TK_EOF){
            fprintf(stderr, "Unexpected end of file\n");
            exit(1);
        }
        else{
            fprintf(stderr, "Unexpected token\n");
            exit(1);
        }

    }
    if((size_t)*pos >= tokens->count){
        fprintf(stderr, "Expected '}'\n");
        exit(1);
    }
    (*pos)++;

    return statement;
}

Statement* parse_print_statement(TokenArray* tokens, int *pos){
    // TODO: Implement print statement parsing
    Statement* statement = malloc(sizeof(*statement));
    statement->type = STMT_PRINT;
    int start = *pos;
    while(((size_t)*pos < tokens->count) && (tokens->tokens[*pos].type != TK_SEMICOLON))
    {
        if(tokens->tokens[*pos].type == TK_EOF){
            fprintf(stderr, "Expected ';' at the end of print statement\n");
            exit(1);
        }
        else if(tokens->tokens[*pos].type == TK_RBRACE){
            fprintf(stderr, "Expected ';' at the end of print statement\n");
            exit(1);
        }
        else if(tokens->tokens[*pos].type == TK_LBRACE){
            fprintf(stderr, "Expected ';' at the end of print statement\n");
            exit(1);
        }
        (*pos)++;
    }
    int end = *pos;
    if(tokens->tokens[start + 1].type == TK_LPAREN && tokens->tokens[end - 1].type == TK_RPAREN){
        statement->data.print.expression = parse_equality(tokens, start + 2, end - 1);
        if(statement->data.print.expression == NULL){
            fprintf(stderr, "Expected expression in print statement\n");
            exit(1);
        }
            (*pos)++;
            return statement;
    }
    else{
        fprintf(stderr, "Unexpected token\n");
        exit(1);
    }
}

Program* parse(TokenArray* tokens){
    Program* program = malloc(sizeof(Program));
    program->statesments = malloc(sizeof(Statement*) * tokens->count);
    program->count = 0;
    program->capacity =tokens->count;

    int pos = 0;

    while((size_t)pos < tokens->count){
        if(tokens->tokens[pos].type == TK_WHILE){
            program->statesments[program->count++] = parse_while_statement(tokens, &pos);
        }
        else if(tokens->tokens[pos].type == TK_LBRACE){
            program->statesments[program->count++] = parse_block(tokens, &pos);
        }
        else if(tokens->tokens[pos].type == TK_PRINT){
            program->statesments[program->count++] = parse_print_statement(tokens, &pos);
        }
        else if(tokens->tokens[pos].type == TK_LET){
            program->statesments[program->count++] = parse_let_statement(tokens, &pos);
        }
        else if(tokens->tokens[pos].type == TK_IDENT){
            program->statesments[program->count++] = parse_assign_statement(tokens, &pos);
        }
        else if(tokens->tokens[pos].type == TK_IF){
            program->statesments[program->count++] = parse_if_statement(tokens, &pos);
        }
        else if(tokens->tokens[pos].type == TK_EOF){
            break;
        }
        else{
            exit(1);
        }
    }
    return program;
} 
