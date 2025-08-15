#include <stdio.h>

#define MAX_DIGITS 10000

int bitwise_add(int a, int b) {
    int carry;
    add_loop:
    if (b == 0) goto end_add;
    carry = a & b;
    a = a ^ b;
    b = carry << 1;
    goto add_loop;
    end_add:
    return a;
}

int bitwise_sub(int a, int b) {
    int borrow;
    sub_loop:
    if (b == 0) goto end_sub;
    borrow = (~a) & b;
    a = a ^ b;
    b = borrow << 1;
    goto sub_loop;
    end_sub:
    return a;
}

int bitwise_multiply(int a, int b) {
    int result = 0;
    mult_loop:
    if (b == 0) goto end_mult;
    if (b & 1) {
        result = bitwise_add(result, a);
    }
    a = a << 1;
    b = b >> 1;
    goto mult_loop;
    end_mult:
    return result;
}

int bitwise_divide_by_10(int n) {
    if (n == 0) return 0;
    if (n < 10) return 0;
    
    int result = 0;
    int temp = n;
    int counter = 0;
    
    div_loop:
    if (temp < 10) goto end_div;
    if (counter > 10000) goto end_div;

    temp = bitwise_sub(temp, 10);
    result = bitwise_add(result, 1);
    counter = bitwise_add(counter, 1);
    goto div_loop;
    
    end_div:
    return result;
}

int bitwise_mod_10(int n) {
    if (n < 10) return n;
    
    int quotient = bitwise_divide_by_10(n);
    int temp = bitwise_multiply(quotient, 10);
    return bitwise_sub(n, temp);
}

void clear_array(int *arr, int size) {
    int i = 0;
    clear_loop:
    if (i >= size) goto end_clear;
    *(arr + i) = 0;
    i = bitwise_add(i, 1);
    goto clear_loop;
    end_clear:;
}

int char_to_int(char c) {
    return bitwise_sub(c, '0');
}

int string_to_digits(char *str, int *digits) {
    int len = 0;
    
    count_len:
    if (len >= MAX_DIGITS) goto end_count;
    if (*(str + len) == '\0') goto end_count;
    if (*(str + len) < '0') goto end_count;
    if (*(str + len) > '9') goto end_count;
    len = bitwise_add(len, 1);
    goto count_len;
    end_count:;
    
    int i = 0;
    reverse_loop:
    if (i >= len) goto end_reverse;
    int j = bitwise_sub(bitwise_sub(len, 1), i);
    *(digits + i) = char_to_int(*(str + j));
    i = bitwise_add(i, 1);
    goto reverse_loop;
    end_reverse:;
    
    return len;
}

int remove_leading_zeros(int *result, int len) {
    int new_len = len;
    check_zero:
    if (new_len <= 1) goto end_remove;
    if (*(result + bitwise_sub(new_len, 1)) != 0) goto end_remove;
    new_len = bitwise_sub(new_len, 1);
    goto check_zero;
    end_remove:;
    return new_len;
}

int multiply_big_numbers(int *num1, int len1, int *num2, int len2, int *result) {
    int max_result_len = bitwise_add(len1, len2);

    if (max_result_len >= MAX_DIGITS) {
        max_result_len = bitwise_sub(MAX_DIGITS, 1);
    }
    
    clear_array(result, max_result_len);
    
    int i = 0;
    outer_loop:
    if (i >= len1) goto end_outer;
    
    int j = 0;
    inner_loop:
    if (j >= len2) goto next_i;
    
    int pos = bitwise_add(i, j);
    if (pos >= max_result_len) goto next_j;
    
    int prod = bitwise_multiply(*(num1 + i), *(num2 + j));
    int sum = bitwise_add(*(result + pos), prod);
    
    *(result + pos) = bitwise_mod_10(sum);
    
    int carry = bitwise_divide_by_10(sum);
    if (carry > 0) {
        int carry_pos = bitwise_add(pos, 1);
        if (carry_pos < max_result_len) {
            *(result + carry_pos) = bitwise_add(*(result + carry_pos), carry);
        }
    }
    
    next_j:
    j = bitwise_add(j, 1);
    goto inner_loop;
    
    next_i:
    i = bitwise_add(i, 1);
    goto outer_loop;
    end_outer:;
    
    i = 0;
    carry_propagation:
    if (i >= max_result_len) goto end_carry_prop;
    
    if (*(result + i) >= 10) {
        int carry = bitwise_divide_by_10(*(result + i));
        *(result + i) = bitwise_mod_10(*(result + i));
        
        int next_pos = bitwise_add(i, 1);
        if (next_pos < max_result_len) {
            *(result + next_pos) = bitwise_add(*(result + next_pos), carry);
        }
    }
    
    i = bitwise_add(i, 1);
    goto carry_propagation;
    end_carry_prop:;
    
    return remove_leading_zeros(result, max_result_len);
}

void print_result(int *result, int len) {
    if (len == 0) {
        printf("0");
        return;
    }
    
    int i = bitwise_sub(len, 1);
    print_loop:
    if (i < 0) goto end_print;
    printf("%d", *(result + i));
    i = bitwise_sub(i, 1);
    goto print_loop;
    end_print:;
}

int main() {
    char input1[MAX_DIGITS];
    char input2[MAX_DIGITS]; 
    int digits1[MAX_DIGITS];
    int digits2[MAX_DIGITS];
    int result[MAX_DIGITS];

    
    printf("Bilangan pertama: ");
    scanf("%s", input1);
    
    printf("Biilangan kedua: ");  
    scanf("%s", input2);
    
    int len1 = string_to_digits(input1, digits1);
    int len2 = string_to_digits(input2, digits2);
    
    
    int result_len = multiply_big_numbers(digits1, len1, digits2, len2, result);
    
    print_result(result, result_len);
    printf("\n");
    
    return 0;
}