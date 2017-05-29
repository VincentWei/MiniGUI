    int fd, len_header;

...

    if (read (fd, &len_header, sizeof (int)) == -1)
        goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    /* If the native system is big endian, calling ArchSwap32 to
     * swap the bytes.
     */
    len_header = ArchSwap32 (len_header);
#endif

...
