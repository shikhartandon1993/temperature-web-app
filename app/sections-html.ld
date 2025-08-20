/* Put index.html in a custom linker section */
SECTIONS {
    /*Place HTML page in a read-only flash(ROM) */
    .rodata.index_html : ALIGN(4) {
        _binary_app_static_index_html_start = .;
        KEEP(*(.rodata.index_html))
        _binary_app_static_index_html_end = .;
    } > RODATA
}