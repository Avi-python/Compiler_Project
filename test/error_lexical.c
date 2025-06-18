// Testing lexical errors
int main() {
    int x = 123abc;  
    char c = 'ab';
    char d = '';
    int y = @#$;
    
    /* This is an unterminated comment
    int z = 5;
    return 0;