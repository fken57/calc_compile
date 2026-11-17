
# include "util.c"

TokenArray* lex(Source* source){
    TokenArray *tokens = malloc(sizeof(TokenArray));

    tokens->capacity = source->length + 1;
    tokens->tokens = malloc(sizeof(Token) * tokens->capacity);
    tokens->count = 0;

    for (size_t i = 0; i < source->length; i++) {

        if(source->data[i] == '\0') {
            // EOF
            break;
        }

        else if(source->data[i] == ' ') {
            // 空白文字をスキップ
            continue;
        }
        else if(source->data[i] == '+') {
            // '+' をトークン化
            tokens->tokens[tokens->count].type = TK_PLUS;
            tokens->tokens[tokens->count].text = "+";
            tokens->count++;
        }
        else if(source->data[i] == '-') {
            // '-' をトークン化
            tokens->tokens[tokens->count].type = TK_MINUS;
            tokens->tokens[tokens->count].text = "-";
            tokens->count++;
        }
        else if(source->data[i] == '*') {
            // '*' をトークン化
            tokens->tokens[tokens->count].type = TK_STAR;
            tokens->tokens[tokens->count].text = "*";
            tokens->count++;
        }
        else if(source->data[i] == '/') {
            // '/' をトークン化
            tokens->tokens[tokens->count].type = TK_SLASH;
            tokens->tokens[tokens->count].text = "/";
            tokens->count++;
        }
        else if(source->data[i] == '(') {
            // '(' をトークン化
            tokens->tokens[tokens->count].type = TK_LPAREN;
            tokens->tokens[tokens->count].text = "(";
            tokens->count++;
        }
        else if(source->data[i] == ')') {
            // ')' をトークン化
            tokens->tokens[tokens->count].type = TK_RPAREN;
            tokens->tokens[tokens->count].text = ")";
            tokens->count++;
        }
        else if(source->data[i] == '{') {
            // '{' をトークン化
            tokens->tokens[tokens->count].type = TK_LBRACE;
            tokens->tokens[tokens->count].text = "{";
            tokens->count++;
        }
        else if(source->data[i] == '}') {
            // '}' をトークン化
            tokens->tokens[tokens->count].type = TK_RBRACE;
            tokens->tokens[tokens->count].text = "}";
            tokens->count++;
        }
        else if(source->data[i] == ';') {
            // ';' をトークン化
            tokens->tokens[tokens->count].type = TK_SEMICOLON;
            tokens->tokens[tokens->count].text = ";";
            tokens->count++;
        }
        else if(source->data[i] == ',') {
            // ',' をトークン化
            tokens->tokens[tokens->count].type = TK_COMMA;
            tokens->tokens[tokens->count].text = ",";
            tokens->count++;
        }
        else if(source->data[i] == '!') {
            if(source->data[i + 1] == '=') {
                // '!=' をトークン化
                tokens->tokens[tokens->count].type = TK_NE;
                tokens->tokens[tokens->count].text = "!=";
                i++;
                tokens->count++;
            }
        }
        else if(source->data[i] == '>') {
            // '>' をトークン化するが次のコードも見る
            if(source->data[i + 1] == '=') {
                // '>=' をトークン化
                tokens->tokens[tokens->count].type = TK_GE;
                tokens->tokens[tokens->count].text = ">=";
                i++;
            } else {
                // '>' をトークン化
                tokens->tokens[tokens->count].type = TK_GT;
                tokens->tokens[tokens->count].text = ">";
            }
            tokens->count++;
        }
        else if(source->data[i] == '<') {
            // '<' をトークン化するが次のコードも見る
            if(source->data[i + 1] == '=') {
                // '<=' をトークン化
                tokens->tokens[tokens->count].type = TK_LE;
                tokens->tokens[tokens->count].text = "<=";
                i++;
            } else {
                // '<' をトークン化
                tokens->tokens[tokens->count].type = TK_LT;
                tokens->tokens[tokens->count].text = "<";
            }
            tokens->count++;
        }
        else if(source->data[i] == '=') {
            // '=' をトークン化するが、ここもすぐに代入と確定させてはいけない
            if(source->data[i + 1] == '=') {
                // '==' をトークン化
                tokens->tokens[tokens->count].type = TK_EQ;
                tokens->tokens[tokens->count].text = "==";
                i++;
            } else {
                // '=' をトークン化
                tokens->tokens[tokens->count].type = TK_ASSIGN;
                tokens->tokens[tokens->count].text = "=";
            }
            tokens->count++;
        }
        else if(source->data[i] >= '0' && source->data[i] <= '9') {
            int j = 0;
            char number[20];
            //ここで複数文字を連結する
            while(source->data[i] >= '0' && source->data[i] <= '9') {
                number[j] = source->data[i];
                i++;
                j++;
            }
            i--;
            number[j] = '\0';
            tokens->tokens[tokens->count].type = TK_NUMBER;
            tokens->tokens[tokens->count].text = copy_text(number,j);
            tokens->count++;
        }
        else if((source->data[i] >= 'a' && source->data[i] <= 'z') || source->data[i] == '_' || (source->data[i] >= 'A' && source->data[i] <= 'Z')) {
            int j = 0;
            char identifier[20];
            //ここで複数文字を連結する
            while((source->data[i] >= 'a' && source->data[i] <= 'z') || source->data[i] == '_' || (source->data[i] >= 'A' && source->data[i] <= 'Z')) {
                identifier[j] = source->data[i];
                i++;
                j++;
            }
            i--;
            identifier[j] = '\0';

            if(strcmp(identifier, "let") == 0){
                tokens->tokens[tokens->count].type = TK_LET;
                tokens->tokens[tokens->count].text = "let";
                tokens->count++;
            }
            else if(strcmp(identifier, "if") == 0){
                tokens->tokens[tokens->count].type = TK_IF;
                tokens->tokens[tokens->count].text = "if";
                tokens->count++;
            }
            else if(strcmp(identifier, "else") == 0){
                tokens->tokens[tokens->count].type = TK_ELSE;
                tokens->tokens[tokens->count].text = "else";
                tokens->count++;
            }
            else if(strcmp(identifier, "while") == 0){
                tokens->tokens[tokens->count].type = TK_WHILE;
                tokens->tokens[tokens->count].text = "while";
                tokens->count++;
            }
            else if(strcmp(identifier, "print") == 0){
                tokens->tokens[tokens->count].type = TK_PRINT;
                tokens->tokens[tokens->count].text = "print";
                tokens->count++;
            }
            else if(strcmp(identifier, "input") == 0){
                tokens->tokens[tokens->count].type = TK_INPUT;
                tokens->tokens[tokens->count].text = "input";
                tokens->count++;
            }
            else{
                tokens->tokens[tokens->count].type = TK_IDENT;
                tokens->tokens[tokens->count].text = copy_text(identifier, j);
                tokens->count++;
            }
        }
        else{
            //static_assert(0, "Unknown character");
        }
    }
    //ここでEOFの追加
    tokens->tokens[tokens->count].type = TK_EOF;
    tokens->tokens[tokens->count].text = "EOF";
    tokens->count++;
    return tokens;
}

