#ifndef LIBDWARF_ANYDIE_HH
# define LIBDWARF_ANYDIE_HH

# include "xvector.hh"
# include <boost/variant.hpp>

# include "cdwarf"

namespace Dwarf {

    template <unsigned int TagId>
    class TaggedDie;
    class Die;
    class EmptyDie;

    typedef boost::make_variant_over<
            typelist::xvector<
                TaggedDie<DW_TAG_array_type>,
                TaggedDie<DW_TAG_class_type>,
                TaggedDie<DW_TAG_entry_point>,
                TaggedDie<DW_TAG_enumeration_type>,
                TaggedDie<DW_TAG_formal_parameter>,
                TaggedDie<DW_TAG_imported_declaration>,
                TaggedDie<DW_TAG_label>,
                TaggedDie<DW_TAG_lexical_block>,
                TaggedDie<DW_TAG_member>,
                TaggedDie<DW_TAG_pointer_type>,
                TaggedDie<DW_TAG_reference_type>,
                TaggedDie<DW_TAG_compile_unit>,
                TaggedDie<DW_TAG_string_type>,
                TaggedDie<DW_TAG_structure_type>,
                TaggedDie<DW_TAG_subroutine_type>,
                TaggedDie<DW_TAG_typedef>,
                TaggedDie<DW_TAG_union_type>,
                TaggedDie<DW_TAG_unspecified_parameters>,
                TaggedDie<DW_TAG_variant>,
                TaggedDie<DW_TAG_common_block>,
                TaggedDie<DW_TAG_common_inclusion>,
                TaggedDie<DW_TAG_inheritance>,
                TaggedDie<DW_TAG_inlined_subroutine>,
                TaggedDie<DW_TAG_module>,
                TaggedDie<DW_TAG_ptr_to_member_type>,
                TaggedDie<DW_TAG_set_type>,
                TaggedDie<DW_TAG_subrange_type>,
                TaggedDie<DW_TAG_with_stmt>,
                TaggedDie<DW_TAG_access_declaration>,
                TaggedDie<DW_TAG_base_type>,
                TaggedDie<DW_TAG_catch_block>,
                TaggedDie<DW_TAG_const_type>,
                TaggedDie<DW_TAG_constant>,
                TaggedDie<DW_TAG_enumerator>,
                TaggedDie<DW_TAG_file_type>,
                TaggedDie<DW_TAG_friend>,
                TaggedDie<DW_TAG_namelist>,
                TaggedDie<DW_TAG_namelist_item>,
                TaggedDie<DW_TAG_packed_type>,
                TaggedDie<DW_TAG_subprogram>,
                TaggedDie<DW_TAG_template_type_parameter>,
                TaggedDie<DW_TAG_template_value_parameter>,
                TaggedDie<DW_TAG_thrown_type>,
                TaggedDie<DW_TAG_try_block>,
                TaggedDie<DW_TAG_variant_part>,
                TaggedDie<DW_TAG_variable>,
                TaggedDie<DW_TAG_volatile_type>,
                TaggedDie<DW_TAG_dwarf_procedure>,
                TaggedDie<DW_TAG_restrict_type>,
                TaggedDie<DW_TAG_interface_type>,
                TaggedDie<DW_TAG_namespace>,
                TaggedDie<DW_TAG_imported_module>,
                TaggedDie<DW_TAG_unspecified_type>,
                TaggedDie<DW_TAG_partial_unit>,
                TaggedDie<DW_TAG_imported_unit>,
                TaggedDie<DW_TAG_mutable_type>,
                TaggedDie<DW_TAG_condition>,
                TaggedDie<DW_TAG_shared_type>,
                TaggedDie<DW_TAG_type_unit>,
                TaggedDie<DW_TAG_rvalue_reference_type>,
                TaggedDie<DW_TAG_template_alias>,
                TaggedDie<DW_TAG_coarray_type>,
                TaggedDie<DW_TAG_generic_subrange>,
                TaggedDie<DW_TAG_dynamic_type>,
                TaggedDie<DW_TAG_atomic_type>,
                TaggedDie<DW_TAG_call_site>,
                TaggedDie<DW_TAG_call_site_parameter>,
                Die,
                EmptyDie
            >
        >::type AnyDie;

}

#endif /* !LIBDWARF_ANYDIE_HH */
