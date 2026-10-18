#include "port/input_script.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENTRIES 4096

static const struct {
    const char *name;
    uint16_t key;
} kButtons[] = {
    { "A", 0x001 },    { "B", 0x002 },    { "SELECT", 0x004 }, { "START", 0x008 }, { "RIGHT", 0x010 },
    { "LEFT", 0x020 }, { "UP", 0x040 },   { "DOWN", 0x080 },   { "R", 0x100 },     { "L", 0x200 },
};

static struct {
    unsigned int step;
    uint16_t keys;
} sEntries[MAX_ENTRIES];
static unsigned int sCount;

// Parses "A+START" or "-" into KEYINPUT bits. Returns nonzero on an unknown name.
static int ParseKeys(char *text, uint16_t *keys)
{
    char *name;

    *keys = 0;
    if (strcmp(text, "-") == 0)
        return 0;
    for (name = strtok(text, "+"); name != NULL; name = strtok(NULL, "+")) {
        unsigned int i;
        char *c;

        for (c = name; *c; c++)
            *c = (char)toupper((unsigned char)*c);
        for (i = 0; i < sizeof(kButtons) / sizeof(kButtons[0]) && strcmp(name, kButtons[i].name) != 0; i++)
            ;
        if (i == sizeof(kButtons) / sizeof(kButtons[0]))
            return -1;
        *keys |= kButtons[i].key;
    }
    return 0;
}

int PortInputScriptLoad(const char *path)
{
    FILE *file = fopen(path, "r");
    char line[256];
    unsigned int number = 0;

    if (file == NULL) {
        perror(path);
        return -1;
    }
    sCount = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        char keys[200], *comment = strchr(line, '#');
        unsigned int step;
        char extra;
        int fields;

        number++;
        if (comment != NULL)
            *comment = '\0';
        fields = sscanf(line, "%u %199s %c", &step, keys, &extra);
        if (fields <= 0)
            continue;  // blank
        if (fields != 2 || step == 0 || (sCount && step <= sEntries[sCount - 1].step) || sCount == MAX_ENTRIES ||
            ParseKeys(keys, &sEntries[sCount].keys)) {
            fprintf(stderr, "%s:%u: expected \"STEP KEYS\" with increasing steps and known buttons\n", path, number);
            fclose(file);
            return -1;
        }
        sEntries[sCount++].step = step;
    }
    fclose(file);
    return 0;
}

uint16_t PortInputScriptKeys(unsigned int step)
{
    uint16_t keys = 0;
    unsigned int i;

    for (i = 0; i < sCount && sEntries[i].step <= step; i++)
        keys = sEntries[i].keys;
    return keys;
}
