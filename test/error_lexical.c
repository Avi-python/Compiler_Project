// Testing lexical errors
int main() {
    int x = 123abc;  // Invalid number-identifier combination
    char c = 'ab';   // Invalid character literal (too many characters)
    char d = '';     // Invalid empty character literal
    int y = @#$;     // Invalid characters
    
    /* This is an unterminated comment
    int z = 5;
    return 0;