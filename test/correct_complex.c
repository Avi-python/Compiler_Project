// Testing complex expressions and operator precedence
int testExpressions() {
    int a = 5;
    int b = 3;
    int c = 2;
    int result;
    
    // Arithmetic operator precedence
    result = a + b * c;  // Should be 5 + (3 * 2) = 11
    result = (a + b) * c;  // Should be (5 + 3) * 2 = 16
    
    // Relational operators
    if (a > b) {
        result = 1;
    }
    
    if (a == 5) {
        result = 2;
    }
    
    // Mixed arithmetic and relational
    if (a + b >= c * 4) {
        result = 3;
    }
    
    return result;
}

void testScoping() {
    int x = 1;
    {
        int y = 2;
        {
            int z = 3;
            x = x + y + z;
        }
        x = x + y;
    }
}

int main() {
    int result = testExpressions();
    testScoping();
    return result;
}