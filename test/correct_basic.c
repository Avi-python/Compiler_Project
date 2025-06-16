// Basic correct program testing fundamental features
int globalVar = 42;
char letter = 'A';

int add(int a, int b) {
    return a + b;
}

int factorial(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

int main() {
    int x = 10;
    int y = 20;
    int result;
    
    result = add(x, y);
    
    if (result > 25) {
        x = x + 1;
    } else {
        x = x - 1;
    }
    
    while (x > 0) {
        x = x - 1;
        result = result + 1;
    }
    
    return 0;
}