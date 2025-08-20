#include <zephyr/linker/iterable_sections.h>

/* Create iterable section for my_http_service resources */
ITERABLE_SECTION_ROM(http_resource_desc_my_http_service, Z_LINK_ITERABLE_SUBALIGN)
