#include <stddef.h> // size_t
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "text_io.h"

extern size_t rf_table_idx(rf_Relation *, ...);

struct strbuf {
	size_t  size;
	size_t  cur;
	char    *str;
};

void strbuf_append_set(struct strbuf *, rf_Set *);

void
strbuf_append_string(struct strbuf *buf, char *str) {
#define remaining(strbuf) (strbuf->size - strbuf->cur)
	int written;
	while((written = snprintf(&buf->str[buf->cur], remaining(buf), "%s", str)) >= remaining(buf)) {
		buf->size = buf->size * 2;
		buf->str = realloc(buf->str, buf->size);
	}
	buf->cur += written;
#undef remaining
}

void
strbuf_append_set_element(struct strbuf *buf, rf_SetElement *e) {
	switch(e->type) {
	case RF_SET_ELEMENT_TYPE_STRING:
		strbuf_append_string(buf, e->value.string);
		break;
	case RF_SET_ELEMENT_TYPE_SET:
		strbuf_append_set(buf, e->value.set);
		break;
	}
}

void
strbuf_append_set(struct strbuf *buf, rf_Set *s) {
	strbuf_append_string(buf, "{");
	if(s->cardinality > 0) {
		for(int i = 0; i < s->cardinality; i++) {
			strbuf_append_string(buf, " ");
			strbuf_append_set_element(buf, s->elements[i]);
		}
		strbuf_append_string(buf, " ");
	}
	strbuf_append_string(buf, "}");
}

void
strbuf_append_relation(struct strbuf *buf, rf_Relation *r) {
	strbuf_append_set(buf, r->domains[0]);
	strbuf_append_string(buf, " x ");
	strbuf_append_set(buf, r->domains[1]);
	strbuf_append_string(buf, " :\n");
	strbuf_append_string(buf, "[");
	for(int x = 0; x < r->domains[0]->cardinality; x++) {
		for(int y = 0; y < r->domains[1]->cardinality; y++) {
			strbuf_append_string(buf,
				(r->table[rf_table_idx(r, x, y)]) ? "1" : "0"
			);
		}
		strbuf_append_string(buf, "|\n|");
	}
	buf->cur -= 3;
	strbuf_append_string(buf, "]\0");
}

char *
rf_set_to_string(rf_Set *s) {
	assert(s != NULL);

	struct strbuf buf = {
		.size = 16,
		.cur = 0,
		.str = malloc(16),
	};
	strbuf_append_set(&buf, s);

	return buf.str;
}

rf_Set *
rf_set_from_string(const char *s) {
	assert(false && "not implemented");
}

char *
rf_relation_to_string(rf_Relation *r) {
	assert(r != NULL);

	struct strbuf buf = {
		.size = 16,
		.cur = 0,
		.str = malloc(16),
	};
	strbuf_append_relation(&buf, r);

	return buf.str;
}

rf_Relation *
rf_relation_from_string(const char *s) {
	assert(false && "not implemented");
}
