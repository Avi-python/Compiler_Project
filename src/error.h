typedef struct error_struct {
    int row;
    int col;
    char filename[64];
    char type[64];
    char msg[256];
} Error;