typedef struct error_struct {
    int row;
    int col;
    char filename[64];
    char type[64];
    char token[64];
} Error;