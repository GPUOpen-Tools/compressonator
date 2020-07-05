#ifndef CMP_RAPIDXML_H_INCLUDE
#define CMP_RAPIDXML_H_INCLUDE

#if defined(_WIN32) && !defined(NO_LEGACY_BEHAVIOR)
#include "rapidxml-submodule/rapidxml.hpp"
#include "rapidxml-submodule/rapidxml_utils.hpp"
#else
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#endif
/* Adding declarations to make it compatible with gcc 4.7 and greater */
namespace rapidxml
{
    namespace internal
    {
        template <class OutIt, class Ch>
        inline OutIt print_children(OutIt out, const xml_node<Ch>* node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_attributes(OutIt out, const xml_node<Ch>* node, int flags);

        template <class OutIt, class Ch>
        inline OutIt print_data_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_cdata_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_element_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_declaration_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_comment_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_doctype_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

        template <class OutIt, class Ch>
        inline OutIt print_pi_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);
    }
}
#if defined(_WIN32) && !defined(NO_LEGACY_BEHAVIOR)
#include "rapidxml-submodule/rapidxml_print.hpp"
#else
#include <rapidxml_print.hpp>
#endif
#endif
