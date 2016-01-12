/*
 * Estonian ID card plugin for web browsers
 *
 * Copyright (C) 2010-2011 Codeborne <info@codeborne.com>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esteid_log.h"
#include "l10n.h"
#include "esteid_misc.h"

typedef struct {
	const char *en, *et, *ru;
} label;

#include "labels.h"

#define LABEL_COUNT (sizeof(labels) / sizeof(label))

#define DEFAULT_LANGUAGE_OFFSET 1

int languageOffset;
char* globalEmptyCString = "";

char *EstEID_getUserLocale();

int EstEID_getLanguageOffset(const char *language) {
	if (!language) return -1;
	if (!STRCASECMP("lv", language)) return 1;
	if (!STRCASECMP("ru", language)) return 2;
	if (!STRCASECMP("en", language)) return 0;
	return -1;
}

void EstEID_setLocale(const char *language) {
	LOG_LOCATION;
	EstEID_log("setting language to '%s'", language);
	languageOffset = EstEID_getLanguageOffset(language);

	if (languageOffset == -1) languageOffset = DEFAULT_LANGUAGE_OFFSET;
	EstEID_log("languageOffset=%i", languageOffset);
}

const char *replace_str(const char *str, const char *orig, const char *rep) {
    static char buffer[4096];
    const char *p;
    
    if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
        return str;
    
    strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
    buffer[p-str] = '\0';
    
    sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));
    
    return buffer;
}

const char *l10n(const char *en) {
	unsigned int i;
	if (!en) {
		return globalEmptyCString;
	}
	for (i = 0; i < LABEL_COUNT; i++) {
		if (!strcmp(labels[i].en, en)) {
            return *(&(labels[i].en) + languageOffset);
        }
	}
	return en;
}

const char *l10nReplacePIN2(const char *en, const char *pinLabel) {
	unsigned int i;
	if (!en) {
		return globalEmptyCString;
	}
	for (i = 0; i < LABEL_COUNT; i++) {
		if (!strcmp(labels[i].en, en)) {
            const char *tmpLabel = replace_str(*(&(labels[i].en) + languageOffset), "PIN2", pinLabel);
            return tmpLabel;
        }
	}
	return en;
}
