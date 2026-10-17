#ifndef SYS_H
#define SYS_H

#include "types.h"

extern GBA_FUNC_PTR(ProcedureFunc) gVBlankCallback;
extern GBA_FUNC_PTR(ProcedureFunc) gHBlankCallback;
extern GBA_FUNC_PTR(ProcedureFunc) gVCountCallback;

void InterruptCallbackCallVBlank(void);
void InterruptCallbackSetVBlank(ProcedureFunc cb);
void InterruptCallbackCallHBlank(void);
void InterruptCallbackSetHBlank(ProcedureFunc cb);
void InterruptCallbackCallVCount(void);
void InterruptCallbackSetVCount(ProcedureFunc cb);
void InterruptCallbackEmpty(void);

#endif /* SYS_H */
