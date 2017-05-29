/*
** $Id: slotset.c 10660 2008-08-14 09:30:39Z weiym $
** 
** slotset.c: maintain the slot set in a byte arrary.
** 
** Copyright (C) 2005 ~ 2008 Feynman Software
**
** Current maintainer: Wei Yongming.
**
** Create date: 2005/08/16
*/

int __mg_lookfor_unused_slot (unsigned char* bitmap, int len_bmp, int set)
{
    int unused = 0;
    int i, j;

    for (i = 0; i < len_bmp; i++) {
        for (j = 0; j < 8; j++) {
            if (*bitmap & (0x80 >> j)) {
                if (set)
                    *bitmap &= (~(0x80 >> j));
                return unused + j;
            }
        }

        unused += 8;
        bitmap++;
    }

    return -1;
}

void __mg_slot_set_use (unsigned char* bitmap, int index)
{
    bitmap += index >> 3;
    *bitmap &= (~(0x80 >> (index % 8)));
}

int __mg_slot_clear_use (unsigned char* bitmap, int index)
{
    bitmap += index >> 3;
    if (*bitmap & (0x80 >> (index % 8)))
        return 0;

    *bitmap |= (0x80 >> (index % 8));
    return 1;
}

