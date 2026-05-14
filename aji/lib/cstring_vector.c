/**
 * Aji
 *
 * License: MIT
 *  Author: noname
 *   Since: 2016, 2018
 */
#include <aji/lib/cstring_vector.h>

struct AjiCStrVec {
	char **vec;
	int32_t len;
	int32_t capa;
};

enum {
	CAP_VECINITCAPA = 4,
};

void
AjiCStrVec_Del(AjiCStrVec *vec) {
	if (vec) {
		for (int32_t i = 0; i < vec->len; ++i) {
			free(vec->vec[i]);
		}
		free(vec->vec);
		free(vec);
	}
}

AjiCStrVec *
AjiCStrVec_New(void) {
	AjiCStrVec *self = AjiMem_Calloc(1, sizeof(AjiCStrVec));
	if (!self) {
		return NULL;
	}

	self->capa = CAP_VECINITCAPA;
	self->vec = AjiMem_Calloc(self->capa + 1, sizeof(char *));
	if (!self->vec) {
		free(self);
		return NULL;
	}

	return self;
}

AjiCStrVec *
AjiCStrVec_DeepCopy(const AjiCStrVec *other) {
	if (!other) {
		return NULL;
	}	

	AjiCStrVec *self = AjiMem_Calloc(1, sizeof(AjiCStrVec));
	if (!self) {
		return NULL;
	}

	self->capa = other->capa;
	self->vec = AjiMem_Calloc(other->capa + 1, sizeof(AjiCStrVec *));
	if (!self->vec) {
		AjiCStrVec_Del(self);
		return NULL;
	}

	for (self->len = 0; self->len < other->len; ++self->len) {
		self->vec[self->len] = AjiCStr_Dup(other->vec[self->len]);
		if (!self->vec[self->len]) {
			AjiCStrVec_Del(self);
			return NULL;
		}
	}

	return self;
}

AjiCStrVec *
AjiCStrVec_ShallowCopy(const AjiCStrVec *other) {
	return AjiCStrVec_DeepCopy(other);
}

char **
AjiCStrVec_EscDel(AjiCStrVec *self) {
	if (!self) {
		return NULL;
	}

	char **esc = self->vec;

	free(self);

	return esc;
}

AjiCStrVec *
AjiCStrVec_Resize(AjiCStrVec *self, int32_t capa) {
	int32_t size = sizeof(self->vec[0]);
	char **tmp = AjiMem_Realloc(self->vec, size*capa + size);
	if (!tmp) {
		return NULL;
	}

	self->vec = tmp;
	self->capa = capa;
	return self;
}

AjiCStrVec *
AjiCStrVec_Push(AjiCStrVec *self, const char *str) {
	return AjiCStrVec_PushBack(self, str);
}

AjiCStrVec *
AjiCStrVec_PushBack(AjiCStrVec *self, const char *str) {
	if (!self || !str) {
		return NULL;
	}

	if (self->len >= self->capa) {
		if (!AjiCStrVec_Resize(self, self->capa*2)) {
			return NULL;
		}
	}

	char *elem = AjiCStr_Dup(str);
	if (!elem) {
		return NULL;
	}

	self->vec[self->len++] = elem;
	self->vec[self->len] = NULL;

	return self;
}

AjiCStrVec *
AjiCStrVec_ExtendBackOther(AjiCStrVec *self, const AjiCStrVec *other) {
	if (!self || !other) {
		return NULL;
	}

	for (int32_t i = 0; i < other->len; i += 1) {
		const char *elem = AjiCStrVec_Getc(other, i);
		AjiCStrVec_PushBack(self, elem);
	}

	return self;
}

char *
AjiCStrVec_PopMove(AjiCStrVec *self) {
	if (!self || !self->len) {
		return NULL;
	}

	int32_t i = self->len-1;
	char *el = self->vec[i];
	self->vec[i] = NULL;
	--self->len;

	return el;
}

AjiCStrVec *
AjiCStrVec_Move(AjiCStrVec *self, char *ptr) {
	return AjiCStrVec_MoveBack(self, ptr);
}

AjiCStrVec *
AjiCStrVec_MoveBack(AjiCStrVec *self, char *ptr) {
	if (!self) {
		return NULL;
	}

	if (self->len >= self->capa) {
		if (!AjiCStrVec_Resize(self, self->capa*2)) {
			return NULL;
		}
	}

	self->vec[self->len++] = ptr;
	self->vec[self->len] = NULL;

	return self;
}

static int
cstrvec_cmp(const void *lh, const void *rh) {
	const char *ls = *(const char **)lh;
	const char *rs = *(const char **)rh;
	return strcmp(ls, rs);
}

AjiCStrVec *
AjiCStrVec_Sort(AjiCStrVec *self) {
	if (!self) {
		return NULL;
	}

	qsort(self->vec, self->len, sizeof(self->vec[0]), cstrvec_cmp);
	return self;
}

const char *
AjiCStrVec_Getc(const AjiCStrVec *self, int idx) {
	if (!self) {
		return NULL;
	}

	if (idx >= self->len || idx < 0) {
		return NULL;
	}

	return self->vec[idx];
}

int32_t
AjiCStrVec_Len(const AjiCStrVec *self) {
	if (!self) {
		return 0;
	}

	return self->len;
}

const AjiCStrVec *
AjiCStrVec_Show(const AjiCStrVec *self, FILE *fout) {
	if (!self || !fout) {
		return NULL;
	}

	for (int32_t i = 0; i < self->len; ++i) {
		fprintf(fout, "%s\n", self->vec[i]);
	}
	fflush(fout);

	return self;
}

const AjiCStrVec *
AjiCStrVec_Dump(const AjiCStrVec *self, FILE *fout) {
	if (!self || !fout) {
		return NULL;
	}

	for (int32_t i = 0; i < self->len; ++i) {
		fprintf(fout, "vec[%d] = [%s] (%p)\n",
			i, self->vec[i], self->vec[i]);
	}
	fflush(fout);

	return self;
}

void
AjiCStrVec_Clear(AjiCStrVec *self) {
	if (!self) {
		return;
	}

	for (int32_t i = 0; i < self->len; ++i) {
		free(self->vec[i]);
	}

	self->len = 0;
}

bool
AjiCStrVec_IsContain(const AjiCStrVec *self, const char *target) {
	if (!self || !target) {
		return false;
	}

	for (int32_t i = 0; i < self->len; i += 1) {
		if (!strcmp(self->vec[i], target)) {
			return true;
		}
	}

	return false;
}
