#pragma once

namespace flowx
{
    enum class TypeReferenceKind
    {
        Int4, Int8, Float4, Float8, Bool, Named
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

        const std::string ToString() const
        {
            std::string str = std::string("Named ") + std::string(lexeme);

            switch (kind)
            {
                case TypeReferenceKind::Int4:
                    str = "Int4";
                    break;
                case TypeReferenceKind::Int8:
                    str = "Int8";
                    break;
                case TypeReferenceKind::Float4:
                    str = "Float4";
                    break;
                case TypeReferenceKind::Float8:
                    str = "Float8";
                    break;
                case TypeReferenceKind::Bool:
                    str = "Bool";
                    break;
            }

            switch (modifier)
            {
                case TypeModifierKind::Nullable:
                    str += "?";
                    break;
                case TypeModifierKind::Errorable:
                    str += "!";
                    break;
                case TypeModifierKind::NullErrorable:
                    str += "?!";
                    break;
            }

            return str;
        }
    };
}