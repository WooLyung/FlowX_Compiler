#pragma once

namespace flowx
{
    enum class TypeReferenceKind
    {
        Int4, Int8, Float4, Float8, Bool, Char, Struct
    };

    enum class TypeModifierKind
    {
        None, Nullable, Errorable, NullErrorable
    };

    struct TypeName
    {
        const TypeReferenceKind kind;
        const std::string lexeme;
    };

    struct TypeReference
    {
        const TypeReferenceKind kind;
        const std::string lexeme;
        const TypeModifierKind modifier;
    };
}