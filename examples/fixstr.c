
    char * buffer = FixStrAlloc (strlen ("Hello, world!"));

    if (buffer)
        strcpy (buffer, "Hello, world!");

    ...

    FreeFixStr (buffer);

