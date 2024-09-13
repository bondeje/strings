#ifndef STRINGS_H
#define STRINGS_H

// WARNING: except where otherwise noted, all pointers passed should be non-null

#ifndef STRING_GROWTH_FACTOR
	#define STRING_GROWTH_FACTOR 2
#endif

typedef struct String {
	char * str;
	ptrdiff_t size;
	size_t capacity; // 0 means not allocated or reallocatable. number of elements allocated
} String;

extern String const WHITESPACE;

#define String_size(str) String_len(str)
static inline ptrdiff_t String_len(String const * str) { return str->size; }
static inline size_t String_capacity(String const * str) { return str->capacity; }

_Bool String_is_empty(String const * str);
int String_compare(String const * a, String const * b);
// this is for compatibility with e.g. qsort. cannot be a macro because I need the address
static inline int String_comp(void const * a, void const * b) {
	return String_compare((String *)a, (String *)b);
}
_Bool String_in(String const * str, String const * restrict other);
_Bool String_char_in(String const * str, char val);
void String_lower(String * str);
void String_upper(String * str);
// can "fail". returns 0
char String_get(String const * str, ptrdiff_t loc);
// can "fail". returns 0
char  String_set(String * str, ptrdiff_t loc, char val);
_Bool String_starts_with(String const * restrict str, String const * restrict prefix);
_Bool String_ends_with(String const * restrict str, String const * restrict suffix);
int String_count(String const * str, String const * sub, ptrdiff_t start, ptrdiff_t end);
// can "fail". returns -1
ptrdiff_t String_find(String const * str, String const * sub, ptrdiff_t start, ptrdiff_t end);
// can "fail". returns -1
ptrdiff_t String_rfind(String const * str, String const * sub, ptrdiff_t start, ptrdiff_t end);
void String_dest(String * str); // frees buffer only and resets. To use reallocatable methods, must have subsequent String_init() call
void String_del(String * str); // calls *_dest and then frees str
void String_strip(String * str, String const * restrict chars); // whitespace if chars == NULL
void String_lstrip(String * str, String const * restrict chars); // whitespace if chars == NULL
void String_rstrip(String * str, String const * restrict chars); // whitespace if chars == NULL

// can fail if target string does not have large enough size or will reallocate underlying 
// buffer. failures are negative returns or null strings

// if any non-'str', non-zero arguments are provided, 'str' must be destroyed
void String_init(String * restrict str, char const * restrict buf, ptrdiff_t size, size_t capacity);
// if succeeds, 'suffix' must be destroyed
void String_partition(String * str, String const * sep, String * restrict suffix);
// if succeeds, 'suffix' must be destroyed
void String_rpartition(String * str, String const * sep, String * restrict suffix);
// if succeeds, 'dest' must be destroy
void String_copy(String * restrict dest, String const * restrict src);
// returns the number of tabs replaced
int String_expand_tabs(String * str, unsigned char tabsize);
int String_append(String * str, char chr);
int String_extend(String * restrict str, String const * restrict other);
int String_replace(String * str, String const * restrict old, String const * restrict new, 
	int count);

// allocates upon return
String * String_new(char const * buf, size_t size, size_t capacity);
// separate on whitespace if sep is NULL
String * String_split(String * str, String * restrict sep, ptrdiff_t * restrict nsplit);
String * String_join(String const * restrict sep, ptrdiff_t n, 
	String const * const restrict strings);
char * String_cstr(String * str);

// TODO: add String_slice

#endif
