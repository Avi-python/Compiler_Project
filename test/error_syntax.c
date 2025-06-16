// Testing syntax errors - missing tokens and malformed statements
int main() {
    int x;
    x = ;  // Missing expression after assignment
    
    if (x > 0  // Missing closing parenthesis
    {
        x = 1;
    }
    
    while x < 10) {  // Missing opening parenthesis
        x = x + 1;
    }
    
    int y = 5 + * 3;  // Invalid operator sequence
    
    return x  // Missing semicolon
}