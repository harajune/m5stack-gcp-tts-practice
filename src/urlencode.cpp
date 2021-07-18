#include <ctype.h>
#include "urlencode.h"

bool is_safe_char(char c) { return isalnum(c) || c == '.' || c == '-' || c == '_' || c == '*'; }

char *encode_char_to_hex(char c, char *dist)
{
    dist[0] = '%';
    dist[1] = "0123456789ABCDEF"[(c & 0xF0) >> 4];
    dist[2] = "0123456789ABCDEF"[c & 0x0F];
    return dist + 2;
}

unsigned url_encode(const char *c, char *dist)
{
    const char *head = dist;
    for (; *c != '\0'; c++, dist++)
    {
        if (is_safe_char(*c))
            *dist = *c;
        else if (*c == ' ')
            *dist = '+';
        else
            dist = encode_char_to_hex(*c, dist);
    }
    *dist = '\0';
    return dist - head;
}