#ifndef INS_IO_ENUM
#define INS_IO_ENUM

/**
 * @brief enum para las instrucciones para los dispositivos I/O.
 * @param IO_GEN_SLEEP
 * @param IO_STDIN_READ
 * @param IO_STDOUT_WRITE
 * @param IO_FS_CREATE
 * @param IO_FS_DELETE
 * @param IO_FS_TRUNCATE
 * @param IO_FS_WRITE
 * @param IO_FS_READ
 * @param IO_DISCONNECT
 */
typedef enum
{
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
    IO_DISCONNECT,
    NONE
} instruccionIO;

#endif
