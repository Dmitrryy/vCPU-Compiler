#include <cstring>

char* m_strtok (char* str, const char* skp, const unsigned int nom_lex_skip = 0);
int Skip (char** str, const char* skp);
void SkipNon (char** str, const char* skp);
int In (char s, const char* skp);


char* m_strtok (char* str, const char* skp, const unsigned int nom_elem_skip) {
    static char* prev_ptr = NULL;

    if (str != NULL)
        prev_ptr = str;

    if (str == NULL && prev_ptr == NULL)
        return NULL;

    static unsigned int n_skip_now = 0;

    n_skip_now += Skip(&prev_ptr, skp);
    char* begin = prev_ptr;
    SkipNon(&prev_ptr, skp);

    if (*prev_ptr == '\0') {
        prev_ptr = NULL;

        if (*begin != '\0')
            return begin;
        else
            return NULL;
    }

    char* end = prev_ptr;
    *end = '\0';
    prev_ptr++;

    if (nom_elem_skip > 0) {
        if (n_skip_now < nom_elem_skip) {
            n_skip_now++;
            begin = m_strtok(prev_ptr, skp, nom_elem_skip);
        }
        if (n_skip_now >= nom_elem_skip)
            n_skip_now = 0;

        if (n_skip_now > nom_elem_skip)
            return NULL;
    }

    return begin;
}

int Skip (char** str, const char* skp) {
    if (str == NULL)
        return 0;

    int i = 0;
    for (i = 0; In(*((*str) + i), skp); i++)
        if (*((*str) + i) == '\0')
            break;

    *str += i;

    return i;
}

void SkipNon (char** str, const char* skp) {
    if (str == NULL)
        return;

    int i = 0;
    for (i = 0; !In(*((*str) + i), skp); i++)
        if (*((*str) + i) == '\0')
            break;

    *str += i;
}

int In (char s, const char* skp) {

    unsigned l_skp = strlen (skp);

    for (unsigned i = 0; i < l_skp; i++)
        if (s == skp[i])
            return 1;

    return 0;
}