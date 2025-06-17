int a = 13;

int helper(int x, int y)
{
    if(x > 1)
    {
        return x + y;
    }
}

int main()
{
    int result = helper(a, 5);
    return result;
}