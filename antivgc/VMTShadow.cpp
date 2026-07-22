#include "VMTShadow.h"
#include <Windows.h>

VMTShadowing::VMTShadowing(void* object)
{
	this->m_ptr_object = object;
	if (!object) return;
	this->m_ptr_object_vtable = *reinterpret_cast<uintptr_t**>(object);
	if (!this->m_ptr_object_vtable) return;

	this->m_object_vtable_size = this->get_function_count();
	if (this->m_object_vtable_size == 0) return;

	this->m_ptr_object_fake_vtable = new uintptr_t[m_object_vtable_size + 1];

	for (size_t i = 0; i < this->m_object_vtable_size; ++i)
		this->m_ptr_object_fake_vtable[i] = this->m_ptr_object_vtable[i];

	*reinterpret_cast<uintptr_t**>(object) = this->m_ptr_object_fake_vtable;
}

VMTShadowing::~VMTShadowing()
{
	// Restore original vtable pointer
	if (this->m_ptr_object && this->m_ptr_object_vtable) {
		*reinterpret_cast<uintptr_t**>(this->m_ptr_object) = this->m_ptr_object_vtable;
	}
	// Free the fake vtable allocation
	if (this->m_ptr_object_fake_vtable) {
		delete[] this->m_ptr_object_fake_vtable;
		this->m_ptr_object_fake_vtable = nullptr;
	}
}

size_t VMTShadowing::get_function_count()
{
	if (!this->m_ptr_object_vtable) return 0;
	size_t methods = 0;
	while (this->m_ptr_object_vtable[methods] != 0 && (this->m_ptr_object_vtable[methods] >= 0x10000) && (this->m_ptr_object_vtable[methods] < 0xF000000000000ULL)) {
		++methods;
	}
	return methods;
}

uintptr_t* VMTShadowing::Apply(int index, uintptr_t* hook_function)
{
	if (!this->m_ptr_object_fake_vtable || index < 0 || (size_t)index >= this->m_object_vtable_size)
		return nullptr;

	uintptr_t* p_original_func = reinterpret_cast<uintptr_t*>(this->m_ptr_object_fake_vtable[index]);

	this->m_ptr_object_fake_vtable[index] = reinterpret_cast<uintptr_t>(hook_function);

	this->m_object_hooks[index] = p_original_func;

	return p_original_func;
}

void VMTShadowing::Remove(int index)
{
	if (!this->m_ptr_object_fake_vtable || index < 0 || (size_t)index >= this->m_object_vtable_size)
		return;

	if (this->m_object_hooks.find(index) != this->m_object_hooks.end()) {
		this->m_ptr_object_fake_vtable[index] = reinterpret_cast<uintptr_t>(this->m_object_hooks[index]);
		this->m_object_hooks.erase(index);
	}
}
