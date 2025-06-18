// Test file for string literal arithmetic semantic errors
int main() {
    // These should generate semantic errors
    int a = "hello" + "world";  // String + String
    int b = 1 + "test";       // Int + String
    int c = "test" - 5;       // String - Int
    
    // Valid operations (should not generate errors)
    int x = 5 + 3;          // Int + Int
    int y = 'a' + 'b';      // Char + Char
    
    return 0;
}