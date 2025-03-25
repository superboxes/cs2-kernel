#include "memory.h"
#include "imports.h"

NTSTATUS hook_handler(PVOID called_param)
{
	COPY_MEMORY* m = (COPY_MEMORY*)called_param;

	if (m->get_pid) {
		m->pid = memory::get_process_id(m->process_name);
	}
	else if (m->base) {
		PEPROCESS process;
		if (NT_SUCCESS(PsLookupProcessByProcessId(m->pid, &process))) {
			m->buffer = (void*)memory::get_module_base_x64(process);
		}
	}
	else if (m->peb) {
		PEPROCESS process;
		if (NT_SUCCESS(PsLookupProcessByProcessId(m->pid, &process))) {
			m->buffer = (void*)PsGetProcessPeb(process);
		}
	}
	else if (m->get_client) {
		PEPROCESS process;
		if (NT_SUCCESS(PsLookupProcessByProcessId(m->pid, &process))) {
			UNICODE_STRING DLLName;
			RtlInitUnicodeString(&DLLName, L"client.dll");
			ULONG64 BaseAddr = memory::GetModuleBasex64(process, DLLName, false);
			if (BaseAddr == 0) {
				// Try alternate name - important for CS2
				RtlInitUnicodeString(&DLLName, L"client.dll");
				BaseAddr = memory::GetModuleBasex64(process, DLLName, false);
			}
			m->buffer = (void*)BaseAddr;
			DbgPrintEx(0, 0, "client.dll base: %llx\n", BaseAddr);
		}
	}
	else if (m->get_engine) {
		PEPROCESS process;
		if (NT_SUCCESS(PsLookupProcessByProcessId(m->pid, &process))) {
			UNICODE_STRING DLLName;
			RtlInitUnicodeString(&DLLName, L"engine2.dll"); // CS2 uses engine2.dll
			ULONG64 BaseAddr = memory::GetModuleBasex64(process, DLLName, false);
			if (BaseAddr == 0) {
				// Try alternate name as fallback
				RtlInitUnicodeString(&DLLName, L"engine.dll");
				BaseAddr = memory::GetModuleBasex64(process, DLLName, false);
			}
			m->buffer = (void*)BaseAddr;
			DbgPrintEx(0, 0, "engine2.dll base: %llx\n", BaseAddr);
		}
	}
	else if (m->get_engine_size) {
		PEPROCESS process;
		if (NT_SUCCESS(PsLookupProcessByProcessId(m->pid, &process))) {
			UNICODE_STRING DLLName;
			RtlInitUnicodeString(&DLLName, L"engine2.dll"); // CS2 uses engine2.dll
			ULONG64 BaseAddr = memory::GetModuleBasex64(process, DLLName, true);
			if (BaseAddr == 0) {
				// Try alternate name as fallback
				RtlInitUnicodeString(&DLLName, L"engine.dll");
				BaseAddr = memory::GetModuleBasex64(process, DLLName, true);
			}
			m->size = BaseAddr;
			DbgPrintEx(0, 0, "engine2.dll size: %llx\n", BaseAddr);
		}
	}
	else if (m->get_client_size) {
		PEPROCESS process;
		if (NT_SUCCESS(PsLookupProcessByProcessId(m->pid, &process))) {
			UNICODE_STRING DLLName;
			RtlInitUnicodeString(&DLLName, L"client.dll");
			ULONG64 BaseAddr = memory::GetModuleBasex64(process, DLLName, true);
			m->size = BaseAddr;
			DbgPrintEx(0, 0, "client.dll size: %llx\n", BaseAddr);
		}
	}
	else if (m->read) {
		memory::read_kernel_memory(m->pid, (PVOID)m->address, m->buffer, m->size);
	}
	else if (m->write) {
		memory::write_kernel_memory(m->pid, m->buffer, (PVOID)m->address, m->size);
	}

	return STATUS_SUCCESS;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path)
{
	UNREFERENCED_PARAMETER(driver_object);
	UNREFERENCED_PARAMETER(registry_path);
	DbgPrintEx(0, 0, "CS2Driver loaded\n");
	if (memory::call_kernel_function(&hook_handler)) {
		DbgPrintEx(0, 0, "CS2Driver ready\n");
	}
	else {
		DbgPrintEx(0, 0, "CS2Driver failed to hook function\n");
	}
	return STATUS_SUCCESS;
}