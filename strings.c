#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "strings.h"

String const WHITESPACE = {
	.str = " \t\f\n\r\v",
	.size = 6
};

String const EMPTY_STRING = {
	.str = "",
	.size = 0
};

#define UPPER_TO_LOWER_OFFSET 32

_Bool String_is_empty(String const * str) {
	return str->size == 0;
}

void String_clear(String * str) {
	str->size = 0;
}

int String_compare(String const * a, String const * b) {
	ptrdiff_t size_a = String_len(a);
	ptrdiff_t size_b = String_len(b);
	int result = 0;
	if (size_a < size_b) {
		if (!size_a) {
			return -1;
		}
		result = strncmp(a->str, b->str, size_a);
		if (!result) { // the first size_a characters match. a is a substring of b
			result = -1;
		}
	} else if (size_a > size_b) {
		if (!size_b) {
			return 1;
		}
		result = strncmp(a->str, b->str, size_b);
		if (!result) { // the first size_b characters match. b is a substring of a
			result = 1;
		}
	} else {
		if (!size_a) {
			return 0;
		}
		result = strncmp(a->str, b->str, size_a);
	}
	return result;
}

_Bool String_in(String const * str, String const * restrict other) {
	return 0 < String_count(str, other, 0, 0);
}

_Bool String_char_in(String const * str, char val) {
	if (str->size <= 0) {
		return false;
	}
	char const * str_ = str->str;
	for (ptrdiff_t i = 0; i < str->size; i++) {
		if (str_[i] == val) {
			return true;
		}
	}
	return false;
}

void String_lower(String * str) {
	for (ptrdiff_t i = 0; i < str->size; i++) {
		char c = str->str[i];
		if (c >= 'A' && c <= 'Z') {
			str->str[i] = c + UPPER_TO_LOWER_OFFSET;
		}
	}
}
void String_upper(String * str) {
	for (ptrdiff_t i = 0; i < str->size; i++) {
		char c = str->str[i];
		if (c >= 'a' && c <= 'z') {
			str->str[i] = c - UPPER_TO_LOWER_OFFSET;
		}
	}
}
char String_get(String const * str, ptrdiff_t loc) {
	if (str->size <= 0) {
		return '\0';
	}
	if (loc < 0) {
		loc = str->size - 1 + ((loc + 1) % str->size);
	}
	if (loc < str->size) {
		return str->str[loc];
	}
	return '\0';
}
char String_set(String * str, ptrdiff_t loc, char val) {
	if (str->size <= 0) {
		return '\0';
	}
	if (loc < 0) {
		loc = str->size - 1 + ((loc + 1) % str->size);
	}
	char out = '\0';
	if (loc < str->size) {
		out = str->str[loc];
		str->str[loc] = val;
	}
	return out;
}
_Bool String_starts_with(String const * restrict str, String const * restrict prefix) {
	if (str->size < prefix->size || prefix->size < 0) {
		return false;
	}
	char const * str_ = str->str;
	char const * pre_ = prefix->str;
	for (ptrdiff_t i = 0; i < prefix->size; i++) {
		if (str_[i] != pre_[i]) {
			return false;
		}
	}
	return true;
}
_Bool String_ends_with(String const * restrict str, String const * restrict suffix) {
	if (str->size < suffix->size || suffix->size < 0) {
		return false;
	}
	char const * str_ = str->str + (str->size - suffix->size);
	char const * pre_ = suffix->str;
	for (ptrdiff_t i = 0; i < suffix->size; i++) {
		if (str_[i] != pre_[i]) {
			return false;
		}
	}
	return true;
}
int String_count(String const * str, String const * sub, ptrdiff_t start, ptrdiff_t end) {
	int ct = 0;
	if (0 >= sub->size || 0 >= str->size) {
		return ct;
	}
	if (start < 0) {
		start += str->size * (start / str->size) - (start % str->size);
	}
	if (end < 0) {
		end += str->size * (end / str->size) - (end % str->size);
	}
	if (0 == end) {
		end += str->size;
	}
	if (start >= end) {
		return 0;
	}
	ptrdiff_t loc = String_find(&(String){.str = str->str + start, .size = end - start}, 
								sub, 0, end - start);
	while (loc >= 0) {
		loc += start;
		ct++;
		if (loc < end - sub->size) {
			start = loc + sub->size;
			loc = String_find(&(String){.str = str->str + start, 
					.size = end - start}, sub, 0, end - start);
		} else {
			loc = -1;
		}
	}
	return ct;
}
// returns -1 if not found
ptrdiff_t String_find(String const * str, String const * sub, ptrdiff_t start, ptrdiff_t end) {
	if (0 >= sub->size || 0 >= str->size || str->size < sub->size) {
		return -1;
	}
	if (start < 0) {
		start += str->size * (start / str->size) - (start % str->size);
	}
	if (end < 0) {
		end += str->size * (end / str->size) - (end % str->size);
	}
	if (0 == end) {
		end += str->size;
	}
	if (start >= end) {
		return -1;
	}
	char init = sub->str[0];
	char * str_ = str->str + start;
	char const * end_ = str->str + end;
	while (str_ < end_) {
		if (String_starts_with(&(String){.str=str_, .size=end_-str_}, sub)) {
			return str_ - str->str;
		}
		str_++;
		while (str_ < end_ && *str_ != init) {
			str_++;
		}
	}
	return -1;
}
ptrdiff_t String_rfind(String const * str, String const * sub, ptrdiff_t start, ptrdiff_t end) {
	if (0 >= sub->size || 0 >= str->size || sub->size > str->size) {
		return -1;
	}
	if (start < 0) {
		start += str->size * (start / str->size) - (start % str->size);
	}
	if (end < 0) {
		end += str->size * (end / str->size) - (end % str->size);
	}
	if (0 == end) {
		end += str->size;
	}
	if (start + sub->size > end) {
		return -1;
	}
	char init = sub->str[0];
	char * str_ = str->str + start;
	ptrdiff_t final_end = sub->size + start;
	String test = {.str = str_, .size = 0};
	while (end >= final_end) {
		test.size = end - start;
		if (String_ends_with(&test, sub)) {
			return end - sub->size;
		}
		end--;
		while (end >= final_end && str->str[end - sub->size] != init) {
			end--;
		}
	}
	return -1;
}
void String_dest(String * str) {
	// frees buffer only and resets. To use reallocatable method
	if (str->capacity) {
		free(str->str);
		memset(str, 0, sizeof(*str));
	}
}

void String_del(String * str) { 
	// calls *_dest and then frees str
	String_dest(str);
	free(str);
}
void String_strip(String * str, String const * restrict chars) {
	// whitespace if chars == NULL
	if (str->size <= 0) {
		return;
	}
	if (chars == NULL) {
		chars = &WHITESPACE;
	}
	if (chars->size <= 0) {
		return;
	}
	// run rstrip first so that the potential move in lstrip is moving less memory
	String_rstrip(str, chars);
	String_lstrip(str, chars);
}
void String_lstrip(String * str, String const * restrict chars) {
	// whitespace if chars == NULL
	if (str->size <= 0) {
		return;
	}
	if (chars == NULL) {
		chars = &WHITESPACE;
	}
	if (chars->size <= 0) {
		return;
	}
	ptrdiff_t itest = 0;
	char * str_ = str->str;
	while (itest < str->size && String_char_in(chars, str_[itest])) {
		itest++;
	}
	if (itest) {
		str->size -= itest;
		memmove(str->str, str->str + itest, str->size * sizeof(char));
	}
}
void String_rstrip(String * str, String const * restrict chars) {
	// whitespace if chars == NULL
	if (str->size <= 0) {
		return;
	}
	if (chars == NULL) {
		chars = &WHITESPACE;
	}
	if (chars->size <= 0) {
		return;
	}
	char * str_ = str->str;
	while (str->size > 0 && String_char_in(chars, str_[str->size - 1])) {
		str->size--;
	}
}

// can fail if target string does not have large enough size or will reallocate underlying
// buffer. failures are negative returns or null strings

// internal function. resizes without clearing (as opposed to String_init(., ., 0, 0)
String * String_resize(String * str, size_t new_capacity) {
	char * str_ = realloc(str->str, new_capacity);
	if (!str_) {
		return NULL;
	}
	str->str = str_;
	str->capacity = new_capacity;
	return str;
}

// need to rework this garbage
void String_init(String * restrict str, char const * restrict buf, ptrdiff_t size, size_t capacity) {
	if (size) { // a buffer was provided
		if (!capacity) { // no allocated capacity in String
			capacity = (size_t)size == SIZE_MAX ? size : size + 1;
			*str = (String) {
				.str = calloc(capacity,  sizeof(char)),
			};
		} else { // this is fucked up
			if (capacity < (size_t)size) { // reallocation is necessary
				capacity = (size_t)size == SIZE_MAX ? size : size + 1;
			}
			char * str_ = NULL;
			if (str->capacity) {
				if (str->capacity < capacity) {
					str_ = realloc(str->str, sizeof(char) * capacity);
				} else {
					str_ = str->str;
				}
			} else {
				str_ = malloc(capacity * sizeof(char));
			}
			str->str = str_;
		}
	} else { // a buffer was not provided
		if (capacity) { // resize the capacity of the underlying buffer
			char * str_ = realloc(str->str, sizeof(char) * capacity);
			if (!str_) { 
				free(str->str);
				*str = (String) {0};
			} else {
				str->str = str_;
			}				
		} else if (str->capacity) {
			free(str->str);
			*str = (String) {0};
		}
	}
	if (str->str) { // memory allocations succeeded
		if (buf) {
			*str = (String) {
				.str = memcpy(str->str, buf, size * sizeof(char)),
				.size = size,
				.capacity = capacity
			};
		} else {
			str->capacity = capacity;
		};
	}
}
void String_partition(String * str, String const * sep, String * restrict suffix) {
	if (sep->size <= 0 || str->size < sep->size) {
		String_init(suffix, NULL, 0, 0);
		return;
	}
	char first_sep = sep->str[0];
	ptrdiff_t i = 0;
	ptrdiff_t N = str->size;
	char * str_ = str->str;
	while (i < N) {
		if (!strncmp(str_ + i, sep->str, sep->size)) {
			String_init(suffix, str_ + i + sep->size, N - i - sep->size, 0);
			str->size = i;
			return;
		}
		i++;
		while (i < N && str_[i] != first_sep) {
			i++;
		}
	}
	String_init(suffix, NULL, 0, 0);
}
void String_rpartition(String * str, String const * sep, String * restrict suffix) {
	if (sep->size <= 0 || str->size < sep->size) {
		String_init(suffix, NULL, 0, 0);
		return;
	}
	ptrdiff_t N = str->size;
	char first_sep = sep->str[0];
	ptrdiff_t i = str->size - sep->size - 1;
	char * str_ = str->str;
	while (i > -1) {
		if (!strncmp(str_ + i, sep->str, sep->size)) {
			String_init(suffix, str_ + i + sep->size, N - i - sep->size, 0);
			str->size = i;
			return;
		}
		i--;
		while (i > -1 && str_[i] != first_sep) {
			i--;
		}
	}
	String_init(suffix, NULL, 0, 0);
}
void String_copy(String * restrict dest, String const * restrict src) {
	String_init(dest, src->str, src->size, 0);
}
int String_expand_tabs(String * str, unsigned char tabsize) {
	ptrdiff_t N = str->size;
	if (N <= 0) {
		return 0;
	}
	ptrdiff_t new_size = N;
	char * str_ = str->str;
	int ntab = 0;
	for (ptrdiff_t i = 0; i < N; i++) {
		if (str_[i] == '\t') {
			new_size += tabsize - 1;
			ntab++;
		}
	}
	if (!ntab) {
		return 0;
	} else if ((size_t)new_size > str->capacity && !String_resize(str, new_size)) {
		return -1;
	}
	// at this point, the str has sufficient capacity
	str->size = new_size;
	new_size--; // new_size is now the write pointer while N is the read
	str_ = str->str; // reset in case realloc'd
	N--;
	while (N > -1) {
		if (str_[N] == '\t') {
			for (unsigned char i = 0; i < tabsize; i++) {
				str_[new_size--] = ' ';
			}
		} else {
			str_[new_size--] = str_[N];
		}
		N--;
	}
	return ntab;
}
int String_append(String * str, char chr) {
	if (str->size < 0 || (str->capacity == (size_t)str->size && 
		!String_resize(str, STRING_GROWTH_FACTOR * str->capacity))) {

		return -1;
	}
	// at this point, str has sufficient capacity
	str->str[str->size++] = chr;
	return 0;
}
int String_extend(String * restrict str, String const * restrict other) {
	if (str->size < 0 || other->size < 0 || (str->capacity < (size_t)(str->size + other->size) && 
		!String_resize(str, str->size + other->size))) {
		
		return -1;
	}
	// at this point, str has sufficient capacity
	memcpy(str->str + str->size, other->str, other->size * sizeof(char));
	str->size += other->size;
	return 0;
}

// TODO: here

int String_replace(String * str, String const * restrict old, 
	String const * restrict new, int count) {
	ptrdiff_t read = String_len(str);
	ptrdiff_t old_size = String_len(old);
	ptrdiff_t new_size = String_len(new);

	int nold = String_count(str, old, 0, read);
	if (!nold) {
		return 0;
	}
	if (!count) {
		count = nold;
	}
	ptrdiff_t delta = count * (new_size - old_size);

	ptrdiff_t write = read + delta;
	if ((size_t)write > String_capacity(str) && !String_resize(str, write)) {
		return -1;
	}
	// str should have sufficient capacity
	// write; // this is now the location before which bytes are being written to
	// read; // this is now the location before which bytes are read from
	ptrdiff_t next = read;
	for (int i = 0; i < nold - count; i++) {
		next = String_rfind(str, old, 0, next);
	}
	write -= read - next;
	memmove(str->str + write, str->str + next, (read - next) * sizeof(char));
	read = next;
	for (int i = 0; i < count; i++) {
		next = String_rfind(str, old, 0, next);
		ptrdiff_t copy = read - next - old_size;

		// copy everything up to next instance of old string to new buffer
		write -= copy;
		memmove(str->str + write, str->str + next + old_size, copy * sizeof(char));

		// copy in new string where old was to be placed
		write -= new_size;
		memcpy(str->str + write, new->str, new_size * sizeof(char));
		read = next;
	}

	return count;
}

// allocates upon return
String * String_new(char const * buf, size_t size, size_t capacity) {
	String * str = malloc(sizeof(*str));
	if (!str) {
		return NULL;
	}
	*str = (String){0};
	String_init(str, buf, size, capacity);
	return str;
}
// separate on whitespace if sep is NULL. Returns number of elements of dest filled
ptrdiff_t String_split(ptrdiff_t nsplit, String * restrict dest, String * restrict str, String * restrict sep) {
	ptrdiff_t N = String_len(str);

	if (nsplit < 1) {
		return -1;
	}

	ptrdiff_t sep_size = String_len(sep);
	ptrdiff_t start = 0;
	ptrdiff_t loc = String_find(str, sep, start, N);	
	ptrdiff_t j = 0;
	while (j < nsplit && loc >= 0) {
		String_init(dest + j++, str->str + start, loc - start, 0);
		start = loc + sep_size;
		loc = String_find(str, sep, start, N);
	}
	if (j < nsplit && start < N) {
		String_init(dest + j++, str->str + start, N - start, 0);
	}
	return j;
}
int String_join(String * restrict dest, String const * restrict sep, ptrdiff_t n, 
	String const * const restrict strings) {

	if (!n) {
		return -1;
	}

	if (!sep) {
		sep = &EMPTY_STRING;
	}
	
	ptrdiff_t sep_size = String_len(sep);
	ptrdiff_t min_size = (n - 1) * sep_size;
	for (ptrdiff_t i = 0; i < n; i++) {
		min_size += String_len(strings + i);
	}
	if (!String_resize(dest, min_size)) {
		return -1;
	}
	String_clear(dest);
	String_extend(dest, strings + 0);
	for (ptrdiff_t j = 1; j < n; j++) {
		String_extend(dest, sep);
		String_extend(dest, strings + j);
	}
	return 0;
}
char * String_cstr(String * str) {
	if (String_append(str, '\0') < 0) {
		return NULL;
	}
	str->size--; // the null-terminator that was append is not part of the string itself
	return str->str;
}

int String_slice(String * restrict dest, String const * restrict str, ptrdiff_t start, ptrdiff_t end, ptrdiff_t step) {
	if (step == 0) {
		step = 1;
	}
	ptrdiff_t N = String_len(str);
	int dir = step > 0 ? 1 : -1;
	if (dir > 0 && end == 0) {
		end = N;
	}
	if (start < 0) {
		start = N - 1 + ((start + 1) % N);
	}
	if (end < 0) {
		end = N + ((end + 1) % N);
	}
	// special case of end originally being -1
	if (dir < 0) {
		if (end == N) {
			end = -1;
		}
	}
	if ((dir < 0 && end >= start) || (dir > 0 && end <= start)) {
		return -1;
	}
	
	size_t nchars = (end - start - dir) / step + 1;
	String_resize(dest, nchars);
	String_clear(dest);
	if (!dest->str) {
		return -1;
	}
	char const * str_ = str->str;
	for ( ; 0 < (end - start) * dir; start += step) {
		if (String_append(dest, str_[start])) {
			String_del(dest);
			return -1;
		}
	}
	return 0;
}
