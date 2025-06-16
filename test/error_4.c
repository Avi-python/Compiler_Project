// Testing missing braces and statement structure errors
int missingBraces() {
    if (1 > 0)  // Missing braces around if body
        return 1;
    else  // Missing braces around else body
        return 0;

int incompleteFunction(  // Missing closing parenthesis and body
    
void anotherFunction() 
    // Missing opening brace
    int x = 5;
    return;
}  // Extra closing brace

int main() {
    {  // Unmatched opening brace
        int x = 5;
    
    if (x > 0) {
        x = x + 1;
    } else {
        x = x - 1;
    // Missing closing brace for else
    
    while (x > 0) {
        x = x - 1;
    }  // This should match while
}  // This should match main

// Missing closing brace for the unmatched opening brace