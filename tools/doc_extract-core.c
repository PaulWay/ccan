/* This merely extracts, doesn't do XML or anything. */
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ctype.h>
#include <ccan/talloc/talloc.h>
#include <ccan/str/str.h>
#include "doc_extract.h"
#include "tools.h"

static char **grab_doc(char **lines, unsigned int num)
{
	char **ret;
	unsigned int i;
	bool printing = false;

	ret = talloc_array(NULL, char *, num+1);

	num = 0;
	for (i = 0; lines[i]; i++) {
		if (streq(lines[i], "/**")) {
			printing = true;
			if (num != 0)
				talloc_append_string(ret[num-1], "\n");
		} else if (streq(lines[i], " */")) 
			printing = false;
		else if (printing) {
			if (strstarts(lines[i], " * "))
				ret[num++] = talloc_strdup(ret, lines[i]+3);
			else if (strstarts(lines[i], " *"))
				ret[num++] = talloc_strdup(ret, lines[i]+2);
			else
				errx(1, "Malformed line %u", i);
		}
	}
	ret[num] = NULL;
	return ret;
}

static bool is_blank(const char *line)
{
	return line && line[strspn(line, " \t\n")] == '\0';
}

static bool is_section(const char *line, bool one_liner)
{
	unsigned int len;

	if (!isupper(line[0]))
		return false;
	len = strspn(line, IDENT_CHARS);
	if (line[len] != ':')
		return false;

	/* If it can be a one-liner, a space is sufficient.*/
	if (one_liner)
		return (line[len+1] == ' ' || line[len+1] == '\t');

	return line[len] == ':' && is_blank(line+len+1);
}

/* Summary line is form '<identifier> - ' */
static bool is_summary_line(const char *line)
{
	unsigned int id_len;

	id_len = strspn(line, IDENT_CHARS);
	if (id_len == 0)
		return false;
	if (!strstarts(line + id_len, " - "))
		return false;

	return true;
}

static struct doc_section *new_section(struct list_head *list,
				       const char *function,
				       const char *type)
{
	struct doc_section *d = talloc(list, struct doc_section);
	d->function = function;
	d->type = type;
	d->lines = NULL;
	d->num_lines = 0;
	list_add_tail(list, &d->list);
	return d;
}

static void add_line(struct doc_section *curr, const char *line)
{
	curr->lines = talloc_realloc(curr, curr->lines, char *,
				     curr->num_lines+1);
	curr->lines[curr->num_lines++] = talloc_strdup(curr->lines, line);
}

struct list_head *extract_doc_sections(char **rawlines, unsigned int num)
{
	char **lines = grab_doc(rawlines, num);
	const char *function = NULL;
	struct doc_section *curr = NULL;
	unsigned int i;
	struct list_head *list;

	list = talloc(NULL, struct list_head);
	list_head_init(list);

	for (i = 0; lines[i]; i++) {
		if (is_summary_line(lines[i])) {
			function = talloc_strndup(list, lines[i],
						  strcspn(lines[i], " "));
			curr = new_section(list, function, "summary");
			add_line(curr, strstr(lines[i], " - ") + 3);
			curr = new_section(list, function, "description");
		} else if (is_section(lines[i], false)) {
			char *type = talloc_strndup(curr, lines[i],
						    strcspn(lines[i], ":"));
			curr = new_section(list, function, type);
		} else if (is_section(lines[i], true)) {
			unsigned int sectlen = strcspn(lines[i], ":");
			char *type = talloc_strndup(curr, lines[i], sectlen);
			curr = new_section(list, function, type);
			add_line(curr, lines[i] + sectlen + 1
				 + strspn(lines[i] + sectlen + 1, " \t"));
		} else {
			if (!curr)
				continue;
			add_line(curr, lines[i]);
		}
	}
	talloc_free(lines);
	return list;
}