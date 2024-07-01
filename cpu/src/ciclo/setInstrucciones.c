#include "setInstrucciones.h"

void instruccion_SUM(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg)
{
    if (strcmp(primerParametro, "PC") == 0)
    {
        procesoCPU->pc += atoi(segundoParametro);
        return;
    }

    reg = obtenerRegistro(primerParametro, procesoCPU, tipo);
    Registro *reg2 = obtenerRegistro(segundoParametro, procesoCPU, tipo);
    if (*tipo == 'i')
    {
        reg->i32 += reg2->i32;
    }
    else if (*tipo == 'u')
    {
        reg->u8 += reg2->u8;
    }
}

void instruccion_SET(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg)
{
    if (strcmp(primerParametro, "PC") == 0)
    {
        procesoCPU->pc = atoi(segundoParametro);
        return;
    }

    reg = obtenerRegistro(primerParametro, procesoCPU, tipo);
    if (*tipo == 'i')
    {
        reg->i32 = atoi(segundoParametro);
    }
    else if (*tipo == 'u')
    {
        reg->u8 = atoi(segundoParametro);
    }
}

void instruccion_JNZ(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg)
{
    reg = obtenerRegistro(primerParametro, procesoCPU, tipo);
    int valor = atoi(segundoParametro);
    if (*tipo == 'i')
    {
        if (reg->i32 != 0)
            procesoCPU->pc = valor;
    }
    else if (*tipo == 'u')
    {
        if (reg->u8 != 0)
            procesoCPU->pc = valor;
    }
}

void instruccion_SUB(char *primerParametro, char *segundoParametro, Contexto_proceso *procesoCPU, char *tipo, Registro *reg)
{
    if (strcmp(primerParametro, "PC") == 0)
    {
        procesoCPU->pc -= atoi(segundoParametro);
        return;
    }

    reg = obtenerRegistro(primerParametro, procesoCPU, tipo);
    Registro *reg2 = obtenerRegistro(segundoParametro, procesoCPU, tipo);
    if (*tipo == 'i')
    {
        reg->i32 -= reg2->i32;
    }
    else if (*tipo == 'u')
    {
        reg->u8 -= reg2->u8;
    }
}

Registro *obtenerRegistro(char *registro, Contexto_proceso *procesoCPU, char *tipo)
{
    if (strcmp(registro, "AX") == 0)
    {
        *tipo = 'u';
        return &(procesoCPU->registros.ax);
    }
    else if (strcmp(registro, "EAX") == 0)
    {
        *tipo = 'i';
        return &(procesoCPU->registros.eax);
    }
    else if (strcmp(registro, "BX") == 0)
    {
        *tipo = 'u';
        return &(procesoCPU->registros.bx);
    }
    else if (strcmp(registro, "EBX") == 0)
    {
        *tipo = 'i';
        return &(procesoCPU->registros.ebx);
    }
    else if (strcmp(registro, "CX") == 0)
    {
        *tipo = 'u';
        return &(procesoCPU->registros.cx);
    }
    else if (strcmp(registro, "ECX") == 0)
    {
        *tipo = 'i';
        return &(procesoCPU->registros.ecx);
    }
    else if (strcmp(registro, "DX") == 0)
    {
        *tipo = 'u';
        return &(procesoCPU->registros.dx);
    }
    else if (strcmp(registro, "EDX") == 0)
    {
        *tipo = 'i';
        return &(procesoCPU->registros.edx);
    }
    else
    {
        return NULL;
    }
}