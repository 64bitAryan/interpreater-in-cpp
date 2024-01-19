#include "../lexer/lexer.cpp"
#include <iostream>
#include <ostream>
#include <string>

std::string PROMPT = ">> ";

void Start()
{
    while (true)
    {
        std::cout << PROMPT;
        std::string str;
        std::getline(std::cin, str);
        if (str.empty())
            return;

        Lexer *l = New(str);
        Token tok = l->NextToken();
        while (tok.Type != _EOF ) {
            if (tok.Type == ILLEGAL)
                break;
            std::cout << "Type: " << tok.Type << " Literal: " << tok.Literals << std::endl;
            tok = l->NextToken();
        }
    }
}
