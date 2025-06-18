int a = 13;

void helper(int x, int y)
{
    a = x * y;
}

int main()
{
    char c = 'abc';
    helper(3, 10);
    print(a);
    return 0;
}