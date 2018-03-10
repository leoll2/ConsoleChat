#include "inout_util.h"

void clear_escape() {
    int c;
    c = getchar();
    //91 = '['  27 = esc
    if (c != 91 && c != 27) {
    	ungetc(c, stdin);
    	return;
    }
    c = getchar();
    //10 = '\n' 65-68 = 'A'-'D'
    if ((c != 10) && (c < 65 || c > 68)) {
    	ungetc(c, stdin);
    	return;
    }
}

uint16_t count_columns(char* buf, uint16_t pos) {
    uint16_t col = 0;
    while(buf[pos] != '\n') {
        col++;
        if(pos > 0)
            pos--;
        else break;
    }
    return col-1;
}

void flush_input() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF) {}
}

void read_stdin(char* buf, int* pos, int max_len, char* user) {
    char c;
    while (*pos < (max_len-1)) {
        c = getchar();
        switch (c) {
            case 27:    //escape
                reset_line(max_len + (user ? strlen(user) : 0) + 7);  //5 = "> " + " ^]]A"
                if (user != NULL)
                    printf("%s> %s", user, buf);
                else
                    printf("> %s", buf);
                clear_escape();
                break;
            case 127:   //backspace
                if (*pos > 0)
                    (*pos)--;
                buf[*pos] = '\0';
                reset_line(max_len + (user ? strlen(user) : 0) + 5);  //5 = "> " + " ^?"
                if (user != NULL)
                    printf("%s> %s", user, buf);
                else
                    printf("> %s", buf);
                break;
            case '\n':
                return;
            default:
                buf[*pos] = c;
                ++(*pos);
                buf[*pos] = '\0'; 
        }
    }
}

void read_msg_stdin(char* buf, uint16_t* pos, uint16_t max_len) {
    char c;
    uint16_t col;
    while (*pos < (max_len - 1)) {
        c = getchar();
        if (((*pos == 1) && (c == '\n') && (buf[0] == '.')) ||
            ((*pos > 1) && (c == '\n') && (buf[*pos-1] == '.') && (buf[*pos-2] == '\n'))) {
            buf[*pos-1] = '\0';
            return;
        } else if (c == 27) {
            col = count_columns(buf, *pos);
            reset_line(col + 5);    //5 = "> " + " ^]]A"
            printf("%s", (buf + *pos - col));
            clear_escape();
        } else if (c == 127) {
            if (*pos > 0)
                (*pos)--;
            buf[*pos] = '\0';
            col = count_columns(buf, *pos);
            reset_line(col + 3);    //3 = " ^?"
            printf("%s", (buf + *pos - col));
        } else {
            buf[*pos] = c;
            ++(*pos);
            buf[*pos] = '\0';
        }
    }
    printf("\nWarning: message too long! It was truncated to %d characters\n", (int)max_len);
}

void reset_line(uint16_t columns) {
    printf("\r");
    for (int i=0; i<columns;i++)
        printf(" ");
    printf("\r");
}
