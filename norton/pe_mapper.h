#pragma once

namespace norton {
	class pe_mapper {
	public:
		bool map_image(manual_loader_context &context);
	private:
		bool handle_iat();
		bool handle_relocs();
		bool map_to_process();
	};
}