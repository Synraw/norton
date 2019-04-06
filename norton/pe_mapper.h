#pragma once

namespace norton {
	class manual_loader_context;

	class pe_mapper {
	public:
		bool map_image(manual_loader_context &context);
	private:
		bool handle_iat(manual_loader_context &context);
		bool handle_relocs(manual_loader_context &context);
		bool map_to_process(manual_loader_context &context);
	};
}