#include <stdlib.h>
#include <stdbool.h>

int get_radix_cnt(int num) {
    int result = 0;
    while (num != 0) {
        ++result;
        num /= 10;
    }
    return result;
}

void to_string(int num, int radix, char* result) {
    while (num != 0) {
        char c = '0' + (num % 10);
        result[--radix] = c;
        num /= 10;
    }
}

bool is_digit(char c) {
    return '0' <= c && c <= '9';
}

void solution(const char *input, char *output) {
    int output_index = 0;
    int numbers_cnt = 0;

    for (int i = 0; input[i] != '\0'; ++i) {
        if (is_digit(input[i])) {
            for (; input[i] != '\0' && is_digit(input[i]); ++i) {
                output[output_index++] = input[i];
            }
            ++numbers_cnt;
            output[output_index++] = '\n';
        }
    }

    if (numbers_cnt > 0) {
        int radix = get_radix_cnt(numbers_cnt);
        char str[radix];
        to_string(numbers_cnt, radix, str);
        
        for (int i = 0; i < radix; ++i) {
            output[output_index++] = str[i];
        }
    } else {
        output[output_index++] = '0';
    }
    
    output[output_index] = '\0';
}

