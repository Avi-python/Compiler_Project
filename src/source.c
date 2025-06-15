int a, b, c = 13 +  + 5 + 2 * 10;

int helper(int x, int y)
{
    while(x < y)
    {
        x = x + 1;
        y = y - 1;
    }
    return x;
}

int main()
{
    if(a < c)
    {
        b = a + c;
    } 
    else 
    {
        b = a - c;
    }
    c = helper(a, b);
    return 0;
}