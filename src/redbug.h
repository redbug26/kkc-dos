#ifndef __REDBUG_H
#define __REDBUG_H

#ifdef __cplusplus
extern "C"
{
#endif

void *Memalloc(unsigned long taille);
  #pragma aux Memalloc parm [edx] value [edx] modify [eax ebx ecx edx esi edi]

#ifdef __cplusplus
};
#endif

#endif
