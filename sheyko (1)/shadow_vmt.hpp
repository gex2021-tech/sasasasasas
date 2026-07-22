#pragma once
#include "ret_spoof.h"
#include <array>
class shadow_vmt
{
public:
	template <typename type>
	bool hook(uint64_t module_base, uint64_t address, int index, void* function, type* original)
	{
		this->vtable_size = 0;
		this->virtual_table = *(uintptr_t**)(address);
		if (!this->virtual_table) return false;
		if (reinterpret_cast<void*>(this->virtual_table[index]) != function)
		{
			while (this->virtual_table[this->vtable_size])
			{
				this->vtable_size++;
			}
			if (index < this->vtable_size)
			{
				this->allocated_vtable = reinterpret_cast<uintptr_t*>(
					fmemory::malloc(this->vtable_size * sizeof(uintptr_t), PAGE_EXECUTE_READWRITE)
					);

				for (int i = 0; i < this->vtable_size; i++)
				{
					this->allocated_vtable[i] = this->virtual_table[i];
				}
				*original = reinterpret_cast<type>(this->virtual_table[index]);
				this->allocated_vtable[index] = reinterpret_cast<uintptr_t>(function);
				*(uintptr_t**)(address) = this->allocated_vtable;
				return true;
			}
		}
		return false;
	}
private:

	uint64_t spoofed_malloc(int32_t size, uint32_t alignment)
	{
		return reinterpret_cast<uint64_t(__cdecl*)(int32_t, uint32_t, uintptr_t, void*)>(spoofcall_stub)(
			size,
			alignment,
			0x46C4660,
			(void*)&fmemory::malloc
			);
	}

	uintptr_t* virtual_table = nullptr;
	uintptr_t* allocated_vtable = nullptr;
	int vtable_size = 0;
};
