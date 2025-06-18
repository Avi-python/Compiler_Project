int a = 13;

void helper(int x, int y)
{
    a = x * y;
}

void printer(char c1, char c2, char c3)
{
    print(c1);
    print(c2);
    print(c3);
}

int main()
{
    helper(3, 10);
    printer('a', 'b', 'c');
    return 0;
}