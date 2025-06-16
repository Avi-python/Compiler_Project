// Testing semantic errors - type mismatches and undeclared variables
int add(int a, int b) {
    return a + b;
}

char subtract(int x, int y) {
    return x - y;  // Type mismatch: returning int from char function
}

int main() {
    int x = 5;
    int y = undeclaredVar;  // Undeclared variable
    
    char result = add(x, y);  // Type mismatch: assigning int to char
    
    int z = add(x);  // Wrong number of parameters
    int w = add(x, y, z);  // Too many parameters
    
    unknownFunction(x);  // Calling undeclared function
    
    int x = 10;  // Redeclaration of variable
    
    return y;  // Returning potentially uninitialized variable
}