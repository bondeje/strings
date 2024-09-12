#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "strings.h"
#include "test_utils.h"

/*
	verbose_start(__func__);
	int nerrors = 0;
	verbose_end(nerrors);
	return nerrors;
*/

String static_strings[] = {
	{.str = "", .size = 0},
	{.str = "a", .size = 1},
	{.str = "aa", .size = 2},
	{.str = "aaa", .size = 3},
	{.str = "ab", .size = 2},
	{.str = "abab", .size = 4},
	{.str = "ababab", .size = 6},
	{.str = "abc", .size = 3},
	{.str = "abcabc", .size = 6},
	{.str = "abcabcabc", .size = 9},
	{.str = "i am the very model of a modern major general", .size = 45},
	{0} // terminating null string
};

String static_upper[] = {
	{.str = "", .size = 0},
	{.str = "A", .size = 1},
	{.str = "AA", .size = 2},
	{.str = "AAA", .size = 3},
	{.str = "AB", .size = 2},
	{.str = "ABAB", .size = 4},
	{.str = "ABABAB", .size = 6},
	{.str = "ABC", .size = 3},
	{.str = "ABCABC", .size = 6},
	{.str = "ABCABCABC", .size = 9},
	{.str = "I AM THE VERY MODEL OF A MODERN MAJOR GENERAL", .size = 45},
	{0} // terminating null string
};

size_t nstrings = 0;

String * dynamic_strings = NULL;

// tests String_copy, and partially String_init
int test_setup(void) {
	String * s = &static_strings[nstrings++];
	while (s->str) {
		s = static_strings + nstrings++;
	}
	
	dynamic_strings = calloc(nstrings, sizeof(String));
	if (!dynamic_strings) {
		nstrings = 0;
		return -1; // special error case of calloc failure
	}
	
	int nerrors = 0;
	verbose_start(__func__);
	for (size_t i = 0; i < nstrings; i++) {
		String_copy(dynamic_strings + i, static_strings + i);
		nerrors += CHECK(dynamic_strings[i].size == static_strings[i].size, 
			"size mismatch between strings for string %s. expected %ll, found %ll\n",
			static_strings[i].str, (long long)static_strings[i].size, 
			(long long)dynamic_strings[i].size);
		if (static_strings[i].size) {
			nerrors += CHECK(dynamic_strings[i].capacity >= (size_t)static_strings[i].size,
				"failed to allocate sufficient space for string %s. expected >=%ll, found %zu\n",
				static_strings[i].str, (long long)static_strings[i].size, 
				dynamic_strings[i].capacity);
			nerrors += CHECK(dynamic_strings[i].str, 
				"failed to create a unique char * for string %s\n", 
				static_strings[i].str);
			nerrors += CHECK(!strncmp(dynamic_strings[i].str, static_strings[i].str, dynamic_strings[i].size),
				"failed to copy string %s to dynamic allocation\n", static_strings[i].str);
		}
	}
	verbose_end(nerrors);
	return nerrors;			
}

int test_String_is_empty(void) {
	verbose_start(__func__);
	int nerrors = 0;
	for (size_t i = 0; i < nstrings; i++) {
		nerrors += CHECK(String_is_empty(&static_strings[i]) == (static_strings[i].size == 0),
			"failed to identify an empty string %s. expected %s, found %s\n",
			(static_strings[i].size == 0) ? "true" : "false", 
			String_is_empty(&static_strings[i]) ? "true" : "false");
	}
	verbose_end(nerrors);
	return nerrors;
}

int test_String_compare(void) {
	verbose_start(__func__);
	int nerrors = 0;
	for (size_t i = 0; i < nstrings; i++) {
		for (size_t j = 0; j < nstrings; j++) {
			if (static_strings[i].size || dynamic_strings[j].size) {
				nerrors += CHECK((0 == String_compare(static_strings + i, dynamic_strings + j)) == (i == j),
					"failure. expected %s and %.*s to compare %s equal\n",
					static_strings[i].str, (int)dynamic_strings[i].size, 
					dynamic_strings[i].str,	i == j ? "" : "not");
			} else {
				nerrors += CHECK(!String_compare(static_strings + i, dynamic_strings + j),
					"failure. expected empty strings to compare equal\n", "");
			}
		}
	}
	verbose_end(nerrors);
	return nerrors;
}

int test_String_in(void) {
	verbose_start(__func__);
	int nerrors = 0;
	for (size_t i = 0; i < nstrings; i++) {
		if (static_strings[i].size) {
			nerrors += CHECK(String_in(dynamic_strings + i, static_strings + i),
				"failed to find string \"%s\"in itself\n",
				static_strings[i].str);
		} else {
			nerrors += CHECK(!String_in(dynamic_strings + i, static_strings + i),
				"String_in did not fail on empty string (index %zu)\n", i);
		}
	}
	verbose_end(nerrors);
	return nerrors;
}

int test_String_char_in(void) {
	verbose_start(__func__);
	int nerrors = 0;

	char chars[] = "abc";
	int nchars = sizeof(chars) / sizeof(chars[0]);
	int results[][sizeof(chars) / sizeof(chars[0])] = {
		{0, 0, 0}, // ""
		{1, 0, 0}, // "a"
		{1, 0, 0}, // "aa"
		{1, 0, 0}, // "aaa"
		{1, 1, 0}, // "ab"
		{1, 1, 0}, // "abab"
		{1, 1, 0}, // "ababab"
		{1, 1, 1}, // "abc"
		{1, 1, 1}, // "abcabc"
		{1, 1, 1}, // "abcabcabc"
		{1, 0, 0}, // "I am the very model of a modern major general"
		{0, 0, 0}, // NULL
	};
	int nresults = sizeof(results) / sizeof(results[0]);
	
	for (int i = 0; i < nresults; i++) {
		for (int j = 0; j < nchars; j++) {
			nerrors += CHECK(results[i][j] == String_char_in(static_strings + i, chars[j]),
				"failed to find character %c in %s\n", chars[j], static_strings[i].str);
		}
	}
	verbose_end(nerrors);
	return nerrors;
}

int test_String_lower(void) {
	verbose_start(__func__);
	int nerrors = 0;
	String test = {0};
	for (size_t i = 0; i < nstrings; i++) {
		String_copy(&test, static_upper + i);
		String_lower(&test);
		nerrors += CHECK(!String_compare(&test, static_strings + i),
			"lowering failed. expected %s, found %.*s\n",
			static_strings[i].str, (int)test.size, test.str);
	}
	String_dest(&test);
	verbose_end(nerrors);
	return nerrors;

}	

int test_String_upper(void) {
	verbose_start(__func__);
	int nerrors = 0;
	String test = {0};
	for (size_t i = 0; i < nstrings; i++) {
		String_copy(&test, static_strings + i);
		String_upper(&test);
		nerrors += CHECK(!String_compare(&test, static_upper + i),
			"lowering failed. expected %s, found %.*s\n",
			static_upper[i].str, (int)test.size, test.str);
	}
	String_dest(&test);
	verbose_end(nerrors);
	return nerrors;
}

int test_String_get(void) {
	verbose_start(__func__);
	int nerrors = 0;
	for (size_t i = 0; i < nstrings; i++) {
		char * str_ = dynamic_strings[i].str;
		ptrdiff_t size = dynamic_strings[i].size;
		for (ptrdiff_t j = 0; j < size; j++) {
			nerrors += CHECK(str_[j] == String_get(dynamic_strings + i, j),
				"failed to retrieve correct character at index %lld for %.*s. expected %c, found %c\n",
				(long long)j, (int)size, dynamic_strings[i].str,
				str_[j], String_get(dynamic_strings + i, j));
		}
		for (ptrdiff_t j = -1; j > -size; j--) {
			nerrors += CHECK(str_[j + size] == String_get(dynamic_strings + i, j),
				"failed to retrieve correct character at index %lld for %.*s. expected %c, found %c\n",
				(long long)j, (int)size, dynamic_strings[i].str,
				str_[j + size], String_get(dynamic_strings + i, j));
		}
		nerrors += CHECK('\0' == String_get(dynamic_strings + i, size),
			"failed to retrieve correct character at index %lld for %.*s. expected (null), found %c\n",
			(long long)size, (int)size, dynamic_strings[i].str,
			String_get(dynamic_strings + i, size));
	}
	verbose_end(nerrors);
	return nerrors;
}

int test_String_set(void) {
	verbose_start(__func__);
	int nerrors = 0;
	String test = {0};
	char testc = '/';
	for (size_t i = 0; i < nstrings; i++) {
		ptrdiff_t size = dynamic_strings[i].size;
		for (ptrdiff_t j = 0; j < size; j++) {
			String_copy(&test, dynamic_strings + i);
			char old = String_set(&test, j, testc);
			nerrors += CHECK(String_get(static_strings + i, j) == old,
				"failed to retrieve original character at index %lld for %.*s. expected %c, found %c\n",
				(long long)j, (int)size, dynamic_strings[i].str,
				String_get(static_strings + i, j), old);
			nerrors += CHECK(testc == String_get(&test, j),
				"failed to set character at index %lld to %c in %s, found %c\n",
				(long long)j, testc, static_strings[i].str, String_get(&test, j));
		}
		for (ptrdiff_t j = -1; j > -size; j--) {
			String_copy(&test, dynamic_strings + i);
			char old = String_set(&test, j, testc);
			nerrors += CHECK(String_get(static_strings + i, j) == old,
				"failed to retrieve original character at index %lld for %.*s. expected %c, found %c\n",
				(long long)j, (int)size, dynamic_strings[i].str,
				String_get(static_strings + i, j), old);
			nerrors += CHECK(testc == String_get(&test, j),
				"failed to set character at index %lld to %c in %s, found %c\n",
				(long long)j, testc, static_strings[i].str, String_get(&test, j));
		}
		String_copy(&test, dynamic_strings + i);
		nerrors += CHECK('\0' == String_set(&test, size, testc),
			"failed to retrieve correct character at index %lld for %.*s. expected (null), found %c\n",
			(long long)size, (int)size, dynamic_strings[i].str,
			String_get(dynamic_strings + i, size));
	}
	String_dest(&test);
	verbose_end(nerrors);
	return nerrors;
}

int test_String_starts_with(void) {
	verbose_start(__func__);
	int nerrors = 0;

	String tests[] = {
		{.str = "a", .size = 1},
		{.str = "ab", .size = 2},
		{.str = "abc", .size = 3},
		{.str = "i am", .size = 4}
	};

	int ntests = sizeof(tests) / sizeof(tests[0]);
	int results[][sizeof(tests) / sizeof(tests[0])] = {
		{0, 0, 0, 0}, // ""
		{1, 0, 0, 0}, // "a"
		{1, 0, 0, 0}, // "aa"
		{1, 0, 0, 0}, // "aaa"
		{1, 1, 0, 0}, // "ab"
		{1, 1, 0, 0}, // "abab"
		{1, 1, 0, 0}, // "ababab"
		{1, 1, 1, 0}, // "abc"
		{1, 1, 1, 0}, // "abcabc"
		{1, 1, 1, 0}, // "abcabcabc"
		{0, 0, 0, 1}, // "I am the very model of a modern major general"
		{0, 0, 0, 0}, // NULL
	};
	int nresults = sizeof(results) / sizeof(results[0]);
	
	for (int i = 0; i < nresults; i++) {
		for (int j = 0; j < ntests; j++) {
			nerrors += CHECK(results[i][j] == String_starts_with(static_strings + i, &tests[j]),
				"failed identify that %s %s start with %s\n", static_strings[i].str, 
				results[i][j] ? "does" : "doesn't", tests[j].str);
		}
	}
	verbose_end(nerrors);
	return nerrors;
}

int test_String_ends_with(void) {
	verbose_start(__func__);
	int nerrors = 0;

	String tests[] = {
		{.str = "a", .size = 1},
		{.str = "ab", .size = 2},
		{.str = "abc", .size = 3},
		{.str = "eral", .size = 4}
	};

	int ntests = sizeof(tests) / sizeof(tests[0]);
	int results[][sizeof(tests) / sizeof(tests[0])] = {
		{0, 0, 0, 0}, // ""
		{1, 0, 0, 0}, // "a"
		{1, 0, 0, 0}, // "aa"
		{1, 0, 0, 0}, // "aaa"
		{0, 1, 0, 0}, // "ab"
		{0, 1, 0, 0}, // "abab"
		{0, 1, 0, 0}, // "ababab"
		{0, 0, 1, 0}, // "abc"
		{0, 0, 1, 0}, // "abcabc"
		{0, 0, 1, 0}, // "abcabcabc"
		{0, 0, 0, 1}, // "I am the very model of a modern major general"
		{0, 0, 0, 0}, // NULL
	};
	int nresults = sizeof(results) / sizeof(results[0]);
	
	for (int i = 0; i < nresults; i++) {
		for (int j = 0; j < ntests; j++) {
			nerrors += CHECK(results[i][j] == String_ends_with(static_strings + i, &tests[j]),
				"failed identify that %s %s end with %s\n", static_strings[i].str, 
				results[i][j] ? "does" : "doesn't", tests[j].str);
		}
	}
	
	verbose_end(nerrors);
	return nerrors;
}

int test_String_count(void) {
	verbose_start(__func__);
	int nerrors = 0;

	String tests[] = {
		{.str = "a", .size = 1},
		{.str = "b", .size = 1},
		{.str = "c", .size = 1},
		{.str = "m", .size = 1}
	};

	int ntests = sizeof(tests) / sizeof(tests[0]);
	int results[][sizeof(tests) / sizeof(tests[0])] = {
		{0, 0, 0, 0}, // ""
		{1, 0, 0, 0}, // "a"
		{2, 0, 0, 0}, // "aa"
		{3, 0, 0, 0}, // "aaa"
		{1, 1, 0, 0}, // "ab"
		{2, 2, 0, 0}, // "abab"
		{3, 3, 0, 0}, // "ababab"
		{1, 1, 1, 0}, // "abc"
		{2, 2, 2, 0}, // "abcabc"
		{3, 3, 3, 0}, // "abcabcabc"
		{4, 0, 0, 4}, // "I am the very model of a modern major general"
		{0, 0, 0, 0}, // NULL
	};
	int nresults = sizeof(results) / sizeof(results[0]);
	
	int start = 0;
	int end = 0;
	for (int i = 0; i < nresults; i++) {
		for (int j = 0; j < ntests; j++) {
			end = (int)static_strings[i].size;
			nerrors += CHECK(results[i][j] == String_count(static_strings + i, &tests[j], start, end),
				"failed to find %d copies of %.*s in '%.*s'\n", 
				results[i][j], (int)tests[j].size, tests[j].str,
				end - start, static_strings[i].str + start);
		}
	}

	int results_half[][sizeof(tests) / sizeof(tests[0])] = {
		{0, 0, 0, 0}, // ""
		{1, 0, 0, 0}, // "a"
		{1, 0, 0, 0}, // "aa"
		{2, 0, 0, 0}, // "aaa"
		{0, 1, 0, 0}, // "ab"
		{1, 1, 0, 0}, // "abab"
		{1, 2, 0, 0}, // "ababab"
		{0, 1, 1, 0}, // "abc"
		{1, 1, 1, 0}, // "abcabc"
		{1, 2, 2, 0}, // "abcabcabc"
		{3, 0, 0, 2}, // "I am the very model of a modern major general"
		{0, 0, 0, 0}, // NULL
	};
	nresults = sizeof(results_half) / sizeof(results_half[0]);

	for (int i = 0; i < nresults; i++) {
		for (int j = 0; j < ntests; j++) {
			end = (int)static_strings[i].size;
			start = end / 2;
			nerrors += CHECK(results_half[i][j] == String_count(static_strings + i, &tests[j], start, end),
				"failed to find %d copies of %.*s in '%.*s'\n", 
				results_half[i][j], (int)tests[j].size, tests[j].str,
				end - start, static_strings[i].str + start);
		}
	}
	
	verbose_end(nerrors);
	return nerrors;
}

int test_tear_down(void) {
	if (dynamic_strings) {
		for (size_t i = 0; i < nstrings; i++) {
			String_dest(dynamic_strings + i);
		}
		free(dynamic_strings);
	}
	return 0;
}

int main(int narg, char ** args) {
	if (narg > 1) {
		if (!strcmp(args[1], "--verbose")) {
			verbose = true;
		}
	}
	int nerrors = 0;
	nerrors += test_setup();
	nerrors += test_String_is_empty();
	nerrors += test_String_compare();
	nerrors += test_String_in();
	nerrors += test_String_char_in();
	nerrors += test_String_lower();
	nerrors += test_String_upper();
	nerrors += test_String_get();
	nerrors += test_String_set();
	nerrors += test_String_starts_with();
	nerrors += test_String_ends_with();
	nerrors += test_String_count();
	nerrors += test_tear_down();
	return nerrors;
}
