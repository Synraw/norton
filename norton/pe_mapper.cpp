#include "pe_mapper.h"
#include "manual_loader.h"
#include "common.h"

namespace norton {
	bool pe_mapper::map_image(manual_loader_context &context) {
		context.m_remote_buffer = context.m_process->alloc(context.m_pe->module_size());
		if (!context.m_remote_buffer) {
			printf("failed to allocate remote buffer");
			return false;
		}

		if (!handle_iat(context)) {
			printf("failed to fill import table\n");
			return false;
		}

		if (!handle_relocs(context)) {
			printf("failed to relocate image\n");
			return false;
		}

		if (!map_to_process(context)) {
			printf("failed to map image to remote process\n");
			return false;
		}

		return true;
	}

	bool pe_mapper::handle_iat(manual_loader_context &context) {
		for (auto imp : context.m_pe->get_imports()) {
			uintptr_t resolved_addr = 0;

			if (imp.m_is_ordinal) {
				resolved_addr = context.m_imports->resolve(imp.m_module, imp.m_ordinal);
			}
			else {
				resolved_addr = context.m_imports->resolve(imp.m_module, imp.m_func);
			}

			printf("[pe-mapper] resolved  %s->%s\n", imp.m_module.c_str(), imp.m_func.c_str());

			if (resolved_addr == 0) {
				printf("[pe-mapper] failed to locate import %s->%s\n", imp.m_module.c_str(), imp.m_func.c_str());
				return false;
			}

			*imp.m_func_ptr = resolved_addr;
		}

		return true;
	}

	bool pe_mapper::handle_relocs(manual_loader_context &context) {
		uintptr_t delta = context.m_remote_buffer - context.m_pe->get_nt()->OptionalHeader.ImageBase;

		for (auto &rel : context.m_pe->get_relocs()) {
			switch (rel.m_flags) {
			case IMAGE_REL_BASED_ABSOLUTE:
				break;
			case IMAGE_REL_BASED_HIGH:
				*(short *)rel.m_reference_addr += HIWORD(delta);
				break;
			case IMAGE_REL_BASED_LOW:
				*(short *)rel.m_reference_addr += LOWORD(delta);
				break;
			case IMAGE_REL_BASED_HIGHLOW:
				*(unsigned int *)rel.m_reference_addr += (unsigned int)delta;
				break;
			case IMAGE_REL_BASED_DIR64:
				*(__int64 *)rel.m_reference_addr += delta;
				break;
			case IMAGE_REL_BASED_HIGHADJ:
				*(short *)rel.m_reference_addr += HIWORD(delta + rel.m_highadj_extra + 0x8000);
				break;
			default:
				return false;
			}
		}

		return true;
	}

	bool pe_mapper::map_to_process(manual_loader_context &context) {
		for (auto &sec : context.m_pe->get_sections()) {
			std::string section_name((const char *)sec->Name, 8);

			if (section_name != ".reloc" && sec->SizeOfRawData != 0) {
				uintptr_t remote_addr = (uintptr_t)sec->VirtualAddress + context.m_remote_buffer;
				uintptr_t local_addr  = (uintptr_t)sec->PointerToRawData + (uintptr_t)context.m_module_buffer;

				if (!context.m_process->write(remote_addr, (void *)local_addr, sec->SizeOfRawData)) {
					printf("failed to write section %s\n", section_name.c_str());
					return false;
				}
			}
		}

		return true;
	}
}