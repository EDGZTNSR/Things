//This is works as a driver!

#include <ntddk.h>

VOID UnloadDriver(PDRIVER_OBJECT DriverObject)
{
    DbgPrint("Driver Unloaded.\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    DriverObject->DriverUnload = UnloadDriver;
    DbgPrint("Driver Loaded.\n");

    HANDLE fileHandle;
    OBJECT_ATTRIBUTES objAttr;
    IO_STATUS_BLOCK ioStatusBlock;
    UNICODE_STRING fileName;
    NTSTATUS status;

    // File path - change as necessary
    RtlInitUnicodeString(&fileName, L"\\??\\C:\\temp\\kernel_output.txt");

    InitializeObjectAttributes(&objAttr, &fileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    status = ZwCreateFile(&fileHandle, GENERIC_WRITE, &objAttr, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OVERWRITE_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

    if (NT_SUCCESS(status))
    {
        char data[] = "Example kernel data\n";
        size_t dataLength = strlen(data);
        ZwWriteFile(fileHandle, NULL, NULL, NULL, &ioStatusBlock, data, (ULONG)dataLength, NULL, NULL);
        ZwClose(fileHandle);
    }

    return STATUS_SUCCESS;
}

// Please consider these points
//The path to the file is hardcoded (C:\\temp\\kernel_output.txt), and the directory must exist for the file operation to succeed.
//You'll need the Windows Driver Kit (WDK) and Microsoft Visual Studio to develop and compile this driver.
