#pragma once
#define BEGIN_REFLECTION
#define END_REFLECTION
#define BEGIN_HIDDEN_VARS
#define END_HIDDEN_VARS
#define SERIALIZE_NEXT_VAR
#define HIDE_NEXT_VAR
#define NEXT_PTR_GET_AMOUNT_FROM
#define NEXT_STRUCT_IS_ASSET
#define STRUCT_IS_STRING
#define IN_PARSING_PUT_STRING_AFTER_EQUAL

#define BEGIN_APPEND


#define SRZ_VAR_STRUCT(type, name) type name
#define SRZ_VAR_PTR(type, name) type name
#define SRZ_VAR_ARRAY(type, name) LangArray<type> name
#define SRZ_VAR_ENUM(type, name) type name
#define SRZ_VAR_SI32(name) name
#define SRZ_VAR_UI64(name) name
#define SRZ_VAR_BOOL(name) name
#define SRZ_VAR_F32(name) name

#define SRZ_BEGIN_STRUCT(name) name
#define SRZ_END_STRUCT()
#define SRZ_BEGIN_UNION(name) 
#define SRZ_END_UNION()
#define SRZ_BEGIN_ENUM(name) name
#define SRZ_END_ENUM()

#define SRZ_ADD_FILE(name) 

#define SRZ_FLAGS_NEXT_TKN 1
#define SRZ_FLAGS_TOKENIZER_DONE 2
#define SRZ_FLAGS_TOKENIZER_COULDNT_FING_STRUCT 4
#define SRZ_FLAGS_STRUCT_NOT_DONE 8
#define SRZ_FLAGS_SERIALIZE_NEXT_VAR 0x10
#define SRZ_FLAGS_PTR_SHARED 0x20
#define SRZ_FLAGS_PTR_GET_AMOUNT_FROM 0x40
#define SRZ_FLAGS_STRUCT_IS_UNION 0x80
#define SRZ_FLAGS_STRUCT_IS_ENUM 0x100
#define SRZ_FLAGS_PUT_STRING_AFTER_EQUAL 0x200
#define SRZ_FLAGS_VAR_IS_SERIALIZABLE 0x400 
#define SRZ_FLAGS_NEXT_STRUCT_IS_ASSET 0x800

#define SRZ_PTR_FLAGS_SHARED 1
#define SRZ_PTR_FLAGS_GET_AMOUNT_FROM 2 

#define SRZ_STRUCT_FLAGS_IS_ASSET 1
