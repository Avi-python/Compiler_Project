int a = 13;

int fib(int x)
{
    if(x == 1) { return 1; }
    if(x == 2) { return 1; }
    return fib(x - 1) + fib(x - 2);
}

int main()
{
    int result = fib(a);
    return result;
}