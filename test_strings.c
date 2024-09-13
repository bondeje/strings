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

int test_String_find(void) {
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
		{-1, -1, -1, -1}, // ""
		{ 0, -1, -1, -1}, // "a"
		{ 0, -1, -1, -1}, // "aa"
		{ 0, -1, -1, -1}, // "aaa"
		{ 0,  1, -1, -1}, // "ab"
		{ 0,  1, -1, -1}, // "abab"
		{ 0,  1, -1, -1}, // "ababab"
		{ 0,  1,  2, -1}, // "abc"
		{ 0,  1,  2, -1}, // "abcabc"
		{ 0,  1,  2, -1}, // "abcabcabc"
		{ 2, -1, -1,  3}, // "I am the very model of a modern major general"
		{-1, -1, -1, -1}, // NULL
	};
	int nresults = sizeof(results) / sizeof(results[0]);
	
	int start = 0;
	int end = 0;
	for (int i = 0; i < nresults; i++) {
		for (int j = 0; j < ntests; j++) {
			end = (int)static_strings[i].size;
			nerrors += CHECK(results[i][j] == String_find(static_strings + i, &tests[j], start, end),
				"failed to find %.*s at loc %d in '%.*s'\n", 
				(int)tests[j].size, tests[j].str, results[i][j], 
				end - start, static_strings[i].str + start);
		}
	}

	int results_half[][sizeof(tests) / sizeof(tests[0])] = {
		{-1, -1, -1, -1}, // ""
		{ 0, -1, -1, -1}, // "a"
		{ 1, -1, -1, -1}, // "aa"
		{ 1, -1, -1, -1}, // "aaa"
		{-1,  1, -1, -1}, // "ab"
		{ 2,  3, -1, -1}, // "abab"
		{ 4,  3, -1, -1}, // "ababab"
		{-1,  1,  2, -1}, // "abc"
		{ 3,  4,  5, -1}, // "abcabc"
		{ 6,  4,  5, -1}, // "abcabcabc"
		{23, -1, -1, 25}, // "I am the very model of a modern major general"
		{-1, -1, -1, -1}, // NULL
	};
	nresults = sizeof(results_half) / sizeof(results_half[0]);

	for (int i = 0; i < nresults; i++) {
		for (int j = 0; j < ntests; j++) {
			end = (int)static_strings[i].size;
			start = end / 2;
			nerrors += CHECK(results_half[i][j] == String_find(static_strings + i, &tests[j], start, end),
				"failed to find %.*s at loc %d in '%.*s'\n", 
				(int)tests[j].size, tests[j].str, results_half[i][j], 
				end - start, static_strings[i].str + start);
		}
	}

	int results_end[][sizeof(tests) / sizeof(tests[0])] = {
		{-1, -1, -1, -1}, // ""
		{ 0, -1, -1, -1}, // "a"
		{ 0, -1, -1, -1}, // "aa"
		{ 0, -1, -1, -1}, // "aaa"
		{ 0, -1, -1, -1}, // "ab"
		{ 0,  1, -1, -1}, // "abab"
		{ 0,  1, -1, -1}, // "ababab"
		{ 0, -1, -1, -1}, // "abc"
		{ 0,  1,  2, -1}, // "abcabc"
		{ 0,  1,  2, -1}, // "abcabcabc"
		{ 2, -1, -1,  3}, // "I am the very model of a modern major general"
		{-1, -1, -1, -1}, // NULL
	};
	nresults = sizeof(results_end) / sizeof(results_end[0]);

	start = 0;
	for (int i = 0; i < nresults; i++) {
		for (int j = 0; j < ntests; j++) {
			end = ((int)static_strings[i].size) / 2;
			if (!end && static_strings[i].size) {
				end++;
			}
			nerrors += CHECK(results_end[i][j] == String_find(static_strings + i, &tests[j], start, end),
				"failed to find %.*s at loc %d in '%.*s'\n", 
				(int)tests[j].size, tests[j].str, results_end[i][j], 
				end - start, static_strings[i].str + start);
		}
	}
	
	verbose_end(nerrors);
	return nerrors;
}

int test_String_rfind(void) {
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
		{-1, -1, -1, -1}, // ""
		{ 0, -1, -1, -1}, // "a"
		{ 1, -1, -1, -1}, // "aa"
		{ 2, -1, -1, -1}, // "aaa"
		{ 0,  1, -1, -1}, // "ab"
		{ 2,  3, -1, -1}, // "abab"
		{ 4,  5, -1, -1}, // "ababab"
		{ 0,  1,  2, -1}, // "abc"
		{ 3,  4,  5, -1}, // "abcabc"
		{ 6,  7,  8, -1}, // "abcabcabc"
		{43, -1, -1, 32}, // "I am the very model of a modern major general"
		{-1, -1, -1, -1}, // NULL
	};
	int nresults = sizeof(results) / sizeof(results[0]);
	
	int start = 0;
	int end = 0;
	for (int i = 0; i < nresults; i++) {
		for (int j = 0; j < ntests; j++) {
			end = (int)static_strings[i].size;
			nerrors += CHECK(results[i][j] == String_rfind(static_strings + i, &tests[j], start, end),
				"failed to find %.*s at loc %d in '%.*s'\n", 
				(int)tests[j].size, tests[j].str, results[i][j], 
				end - start, static_strings[i].str + start);
		}
	}

	int results_half[][sizeof(tests) / sizeof(tests[0])] = {
		{-1, -1, -1, -1}, // ""
		{ 0, -1, -1, -1}, // "a"
		{ 1, -1, -1, -1}, // "aa"
		{ 2, -1, -1, -1}, // "aaa"
		{-1,  1, -1, -1}, // "ab"
		{ 2,  3, -1, -1}, // "abab"
		{ 4,  5, -1, -1}, // "ababab"
		{-1,  1,  2, -1}, // "abc"
		{ 3,  4,  5, -1}, // "abcabc"
		{ 6,  7,  8, -1}, // "abcabcabc"
		{43, -1, -1, 32}, // "I am the very model of a modern major general"
		{-1, -1, -1, -1}, // NULL
	};
	nresults = sizeof(results_half) / sizeof(results_half[0]);

	for (int i = 0; i < nresults; i++) {
		for (int j = 0; j < ntests; j++) {
			end = (int)static_strings[i].size;
			start = end / 2;
			nerrors += CHECK(results_half[i][j] == String_rfind(static_strings + i, &tests[j], start, end),
				"failed to find %.*s at loc %d in '%.*s'\n", 
				(int)tests[j].size, tests[j].str, results_half[i][j], 
				end - start, static_strings[i].str + start);
		}
	}

	int results_end[][sizeof(tests) / sizeof(tests[0])] = {
		{-1, -1, -1, -1}, // ""
		{ 0, -1, -1, -1}, // "a"
		{ 0, -1, -1, -1}, // "aa"
		{ 0, -1, -1, -1}, // "aaa"
		{ 0, -1, -1, -1}, // "ab"
		{ 0,  1, -1, -1}, // "abab"
		{ 2,  1, -1, -1}, // "ababab"
		{ 0, -1, -1, -1}, // "abc"
		{ 0,  1,  2, -1}, // "abcabc"
		{ 3,  1,  2, -1}, // "abcabcabc"
		{ 2, -1, -1, 14}, // "I am the very model of a modern major general"
		{-1, -1, -1, -1}, // NULL
	};
	nresults = sizeof(results_end) / sizeof(results_end[0]);

	start = 0;
	for (int i = 0; i < nresults; i++) {
		for (int j = 0; j < ntests; j++) {
			end = ((int)static_strings[i].size) / 2;
			if (!end && static_strings[i].size) {
				end++;
			}
			nerrors += CHECK(results_end[i][j] == String_rfind(static_strings + i, &tests[j], start, end),
				"failed to find %.*s at loc %d in '%.*s'\n", 
				(int)tests[j].size, tests[j].str, results_end[i][j], 
				end - start, static_strings[i].str + start);
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

	int results_end[][sizeof(tests) / sizeof(tests[0])] = {
		{0, 0, 0, 0}, // ""
		{1, 0, 0, 0}, // "a"
		{1, 0, 0, 0}, // "aa"
		{1, 0, 0, 0}, // "aaa"
		{1, 0, 0, 0}, // "ab"
		{1, 1, 0, 0}, // "abab"
		{2, 1, 0, 0}, // "ababab"
		{1, 0, 0, 0}, // "abc"
		{1, 1, 1, 0}, // "abcabc"
		{2, 1, 1, 0}, // "abcabcabc"
		{1, 0, 0, 2}, // "I am the very model of a modern major general"
		{0, 0, 0, 0}, // NULL
	};
	nresults = sizeof(results_end) / sizeof(results_end[0]);

	start = 0;
	for (int i = 0; i < nresults; i++) {
		for (int j = 0; j < ntests; j++) {
			end = ((int)static_strings[i].size) / 2;
			if (!end && static_strings[i].size) {
				end++;
			}
			nerrors += CHECK(results_end[i][j] == String_count(static_strings + i, &tests[j], start, end),
				"failed to find %d copies of %.*s in '%.*s'\n", 
				results_end[i][j], (int)tests[j].size, tests[j].str,
				end - start, static_strings[i].str + start);
		}
	}
	
	verbose_end(nerrors);
	return nerrors;
}

int test_String_lstrip(void) {
	verbose_start(__func__);
	int nerrors = 0;
	char const * str_orig = " \t\f\n\r\vHello, World \t\f\n\r\v";
	char const * str_result = "Hello, World \t\f\n\r\v";
	String test;
	String_init(&test, str_orig, strlen(str_orig), 0);
	String_lstrip(&test, NULL);

	nerrors += CHECK(!String_compare(&test, &(String){.str = (char *)str_result, .size = strlen(str_result)}),
		"failed to strip from left and right. expected %s, found %.*s\n",
		str_result, (int)test.size, test.str);

	String_dest(&test);
	verbose_end(nerrors);
	return nerrors;
}

int test_String_rstrip(void) {
	verbose_start(__func__);
	int nerrors = 0;
	char const * str_orig = " \t\f\n\r\vHello, World \t\f\n\r\v";
	char const * str_result = " \t\f\n\r\vHello, World";
	String test;
	String_init(&test, str_orig, strlen(str_orig), 0);
	String_rstrip(&test, NULL);

	nerrors += CHECK(!String_compare(&test, &(String){.str = (char *)str_result, .size = strlen(str_result)}),
		"failed to strip from left and right. expected %s, found %.*s\n",
		str_result, (int)test.size, test.str);

	String_dest(&test);
	verbose_end(nerrors);
	return nerrors;
}

int test_String_strip(void) {
	verbose_start(__func__);
	int nerrors = 0;
	char const * str_orig = " \t\f\n\r\vHello, World \t\f\n\r\v";
	char const * str_result = "Hello, World";
	String test;
	String_init(&test, str_orig, strlen(str_orig), 0);
	String_strip(&test, NULL);
	nerrors += CHECK(!String_compare(&test, &(String){.str = (char *)str_result, .size = strlen(str_result)}),
		"failed to strip from left and right. expected %s, found %.*s\n",
		str_result, (int)test.size, test.str);
	verbose_end(nerrors);
	return nerrors;
}

int test_String_rpartition(void) {
	verbose_start(__func__);
	int nerrors = 0;

	char const * path_raw = "path/to/file";
	String const file_result = {.str = "file", .size = 4};
	String const path_result = {.str = "path/to", .size = 7};
	String path = {.str = (char *)path_raw, .size = strlen(path_raw)};
	String file;
	String sep = {.str = "/", .size = 1};
	String_rpartition(&path, &sep, &file);

	nerrors += CHECK(!String_compare(&path, &path_result),
		"failed to retrieve prefix in rpartition. expected %.*s, found %.*s\n",
		(int)path_result.size, path_result.str,
		(int)path.size, path.str);
	nerrors += CHECK(!String_compare(&file, &file_result),
		"failed to retrieve suffix in rpartition. expected %.*s, found %.*s\n",
		(int)file_result.size, file_result.str,
		(int)file.size, file.str);
	
	String_dest(&file);
	verbose_end(nerrors);
	return nerrors;	
}

int test_String_partition(void) {
	verbose_start(__func__);
	int nerrors = 0;

	char const * path_raw = "path/to/file";
	String const file_result = {.str = "to/file", .size = 7};
	String const path_result = {.str = "path", .size = 4};
	String path = {.str = (char *)path_raw, .size = strlen(path_raw)};
	String file;
	String sep = {.str = "/", .size = 1};
	String_partition(&path, &sep, &file);

	nerrors += CHECK(!String_compare(&path, &path_result),
		"failed to retrieve prefix in rpartition. expected %.*s, found %.*s\n",
		(int)path_result.size, path_result.str,
		(int)path.size, path.str);
	nerrors += CHECK(!String_compare(&file, &file_result),
		"failed to retrieve suffix in rpartition. expected %.*s, found %.*s\n",
		(int)file_result.size, file_result.str,
		(int)file.size, file.str);

	String_dest(&file);
	verbose_end(nerrors);
	return nerrors;
}

int test_String_expand_tabs(void) {
	verbose_start(__func__);
	int nerrors = 0;

	char const * craw = "\tindented\n\titems";
	char const * cresult = "    indented\n    items";
	String const tab = {.str = "\t", .size = 1};
	String const raw = {.str = (char *)craw, .size = strlen(craw)};
	String result;
	String_init(&result, craw, strlen(craw), 0);
	int ntabs_result = String_count(&raw, &tab, 0, 0);
	int ntabs = String_expand_tabs(&result, 4);

	nerrors += CHECK(ntabs == ntabs_result,
		"failed to find all the tabs in %s. expected %d, found %d\n",
		craw, ntabs_result, ntabs);

	nerrors += CHECK(!String_compare(&result, &(String){.str = (char *)cresult, .size = strlen(cresult)}),
		"failed to expand tabs properly 1 tab = %d spaces. expected %s, found %.*s\n",
		4, cresult, (int)result.size, result.str);

	String_dest(&result);
	verbose_end(nerrors);
	return nerrors;
}

int test_String_append(void) {
	verbose_start(__func__);
	int nerrors = 0;
	char const * hw = "Hello, World";
	String test;
	String_init(&test, hw, 1, 0); // initialize with first element
	for (size_t i = 1; i < strlen(hw); i++) {
		nerrors += -1 * String_append(&test, hw[i]);
	}

	nerrors += CHECK(!String_compare(&test, &(String){.str = (char *)hw, .size = strlen(hw)}),
		"failed to append to string. expected %s, found %.*s\n",
		hw, (int)test.size, test.str);

	String_dest(&test);

	verbose_end(nerrors);
	return nerrors;
}

int test_String_extend(void) {
	verbose_start(__func__);
	int nerrors = 0;

	char const * h = "Hello,";
	char const * w = " World";
	char const * hw = "Hello, World";
	String a;
	String_init(&a, h, strlen(h), 0);
	String b = {.str = (char *)w, .size = strlen(w)};

	nerrors += -1 * String_extend(&a, &b);

	nerrors += CHECK(!String_compare(&a, &(String){.str = (char *)hw, .size = strlen(hw)}),
		"failed to extend %s to %s. expected %s, found %.*s\n",
		h, w, hw, (int)a.size, a.str);

	String_dest(&a);
	verbose_end(nerrors);
	return nerrors;
}

int test_String_replace(void) {
	verbose_start(__func__);
	int nerrors = 0;

	char const * path_raw = "path/to/file";
	char const * path_result = "path\\to\\file";
	char const * path_result1 = "path\\to/file";
	String result = {.str = (char *)path_result, .size = strlen(path_result)};
	String result1 = {.str = (char *)path_result1, .size = strlen(path_result1)};
	String sep = {.str = "/", .size = 1};
	String rep = {.str = "\\", .size = 1};
	String test;
	String_init(&test, path_raw, strlen(path_raw), 0);
	
	nerrors += CHECK(2 == String_replace(&test, &sep, &rep, 0),
		"failed to replace all %c with %c. expected %d, found %d\n",
		'/', '\\', 2);

	nerrors += CHECK(!String_compare(&test, &result),
		"failed to replace all %c with %c. expected %s, found %.*s\n",
		'/', '\\', path_result, (int)test.size, test.str);

	String_init(&test, path_raw, strlen(path_raw), 0);

	nerrors += CHECK(1 == String_replace(&test, &sep, &rep, 1),
		"failed to replace all %c with %c. expected %d, found %d\n",
		'/', '\\', 1);

	nerrors += CHECK(!String_compare(&test, &result1),
		"failed to replace all %c with %c. expected %s, found %.*s\n",
		'/', '\\', path_result1, (int)test.size, test.str);

	String_dest(&test);
	verbose_end(nerrors);
	return nerrors;
}

int test_String_split(void) {
	verbose_start(__func__);
	int nerrors = 0;

	char const * path_raw = "path/to/file";
	String results[] = {
		{.str = "path", .size = 4},
		{.str = "to", .size = 2},
		{.str = "file", .size = 4}
	};
	int nresults = sizeof(results) / sizeof(results[0]);
	String sep = {.str = "/", .size = 1};
	String input = {.str = (char *)path_raw, .size = strlen(path_raw)};

	ptrdiff_t ntest = 0;
	String * test = String_split(&input, &sep, &ntest);

	nerrors += CHECK(NULL != test, "split failed\n", "");
	if (nerrors) {
		return nerrors;
	}
	
	nerrors += CHECK(ntest == nresults,
		"failed to split %s by %s to the correct number. expected %d, found %d\n",
		path_raw, "/", nresults, (int)ntest);

	for (int i = 0; i < nresults; i++) {
		nerrors += CHECK(!String_compare(&results[i], &test[i]),
			"%d-th split is incorrect. expected %.*s, found %.*s\n",
			i, (int)results[i].size, results[i].str, (int)test[i].size, test[i].str);
		String_dest(&test[i]);
	}

	free(test);
	verbose_end(nerrors);
	return nerrors;
}

int test_String_join(void) {
	verbose_start(__func__);
	int nerrors = 0;

	char const * path_raw = "path/to/file";
	String inputs[] = {
		{.str = "path", .size = 4},
		{.str = "to", .size = 2},
		{.str = "file", .size = 4}
	};
	int ninputs = sizeof(inputs) / sizeof(inputs[0]);
	String sep = {.str = "/", .size = 1};
	
	String * test = String_join(&sep, ninputs, inputs);

	nerrors += CHECK(NULL != test, "join failed\n", "");
	if (nerrors) {
		return nerrors;
	}
	
	nerrors += CHECK(!String_compare(test, &(String){.str = (char *)path_raw, strlen(path_raw)}),
		"failed to join strings. expected %s, found %.*s\n",
		path_raw, (int)test->size, test->str);

	String_del(test);
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
	nerrors += test_String_find();
	nerrors += test_String_rfind();
	nerrors += test_String_count();
	nerrors += test_String_lstrip();
	nerrors += test_String_rstrip();
	nerrors += test_String_strip();
	nerrors += test_String_partition();
	nerrors += test_String_rpartition();
	nerrors += test_String_expand_tabs();
	nerrors += test_String_append();
	nerrors += test_String_extend();
	nerrors += test_String_replace();
	nerrors += test_String_split();
	nerrors += test_String_join();
	nerrors += test_tear_down();
	return nerrors;
}
