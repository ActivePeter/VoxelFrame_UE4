// This file is generated by rust-protobuf 2.25.0. Do not edit
// @generated

// https://github.com/rust-lang/rust-clippy/issues/702
#![allow(unknown_lints)]
#![allow(clippy::all)]

#![allow(unused_attributes)]
#![cfg_attr(rustfmt, rustfmt::skip)]

#![allow(box_pointers)]
#![allow(dead_code)]
#![allow(missing_docs)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(non_upper_case_globals)]
#![allow(trivial_casts)]
#![allow(unused_imports)]
#![allow(unused_results)]
//! Generated file from `common.proto`

/// Generated files are compatible only with the same version
/// of protobuf runtime.
// const _PROTOBUF_VERSION_CHECK: () = ::protobuf::VERSION_2_25_0;

#[derive(PartialEq,Clone,Default)]
pub struct EntityMove {
    // message fields
    pub t: EntityType,
    pub x: f32,
    pub y: f32,
    pub z: f32,
    // special fields
    pub unknown_fields: ::protobuf::UnknownFields,
    pub cached_size: ::protobuf::CachedSize,
}

impl<'a> ::std::default::Default for &'a EntityMove {
    fn default() -> &'a EntityMove {
        <EntityMove as ::protobuf::Message>::default_instance()
    }
}

impl EntityMove {
    pub fn new() -> EntityMove {
        ::std::default::Default::default()
    }

    // .EntityType t = 1;


    pub fn get_t(&self) -> EntityType {
        self.t
    }
    pub fn clear_t(&mut self) {
        self.t = EntityType::T_Null;
    }

    // Param is passed by value, moved
    pub fn set_t(&mut self, v: EntityType) {
        self.t = v;
    }

    // float x = 2;


    pub fn get_x(&self) -> f32 {
        self.x
    }
    pub fn clear_x(&mut self) {
        self.x = 0.;
    }

    // Param is passed by value, moved
    pub fn set_x(&mut self, v: f32) {
        self.x = v;
    }

    // float y = 3;


    pub fn get_y(&self) -> f32 {
        self.y
    }
    pub fn clear_y(&mut self) {
        self.y = 0.;
    }

    // Param is passed by value, moved
    pub fn set_y(&mut self, v: f32) {
        self.y = v;
    }

    // float z = 4;


    pub fn get_z(&self) -> f32 {
        self.z
    }
    pub fn clear_z(&mut self) {
        self.z = 0.;
    }

    // Param is passed by value, moved
    pub fn set_z(&mut self, v: f32) {
        self.z = v;
    }
}

impl ::protobuf::Message for EntityMove {
    fn is_initialized(&self) -> bool {
        true
    }

    fn merge_from(&mut self, is: &mut ::protobuf::CodedInputStream<'_>) -> ::protobuf::ProtobufResult<()> {
        while !is.eof()? {
            let (field_number, wire_type) = is.read_tag_unpack()?;
            match field_number {
                1 => {
                    ::protobuf::rt::read_proto3_enum_with_unknown_fields_into(wire_type, is, &mut self.t, 1, &mut self.unknown_fields)?
                },
                2 => {
                    if wire_type != ::protobuf::wire_format::WireTypeFixed32 {
                        return ::std::result::Result::Err(::protobuf::rt::unexpected_wire_type(wire_type));
                    }
                    let tmp = is.read_float()?;
                    self.x = tmp;
                },
                3 => {
                    if wire_type != ::protobuf::wire_format::WireTypeFixed32 {
                        return ::std::result::Result::Err(::protobuf::rt::unexpected_wire_type(wire_type));
                    }
                    let tmp = is.read_float()?;
                    self.y = tmp;
                },
                4 => {
                    if wire_type != ::protobuf::wire_format::WireTypeFixed32 {
                        return ::std::result::Result::Err(::protobuf::rt::unexpected_wire_type(wire_type));
                    }
                    let tmp = is.read_float()?;
                    self.z = tmp;
                },
                _ => {
                    ::protobuf::rt::read_unknown_or_skip_group(field_number, wire_type, is, self.mut_unknown_fields())?;
                },
            };
        }
        ::std::result::Result::Ok(())
    }

    // Compute sizes of nested messages
    #[allow(unused_variables)]
    fn compute_size(&self) -> u32 {
        let mut my_size = 0;
        if self.t != EntityType::T_Null {
            my_size += ::protobuf::rt::enum_size(1, self.t);
        }
        if self.x != 0. {
            my_size += 5;
        }
        if self.y != 0. {
            my_size += 5;
        }
        if self.z != 0. {
            my_size += 5;
        }
        my_size += ::protobuf::rt::unknown_fields_size(self.get_unknown_fields());
        self.cached_size.set(my_size);
        my_size
    }

    fn write_to_with_cached_sizes(&self, os: &mut ::protobuf::CodedOutputStream<'_>) -> ::protobuf::ProtobufResult<()> {
        if self.t != EntityType::T_Null {
            os.write_enum(1, ::protobuf::ProtobufEnum::value(&self.t))?;
        }
        if self.x != 0. {
            os.write_float(2, self.x)?;
        }
        if self.y != 0. {
            os.write_float(3, self.y)?;
        }
        if self.z != 0. {
            os.write_float(4, self.z)?;
        }
        os.write_unknown_fields(self.get_unknown_fields())?;
        ::std::result::Result::Ok(())
    }

    fn get_cached_size(&self) -> u32 {
        self.cached_size.get()
    }

    fn get_unknown_fields(&self) -> &::protobuf::UnknownFields {
        &self.unknown_fields
    }

    fn mut_unknown_fields(&mut self) -> &mut ::protobuf::UnknownFields {
        &mut self.unknown_fields
    }

    fn as_any(&self) -> &dyn (::std::any::Any) {
        self as &dyn (::std::any::Any)
    }
    fn as_any_mut(&mut self) -> &mut dyn (::std::any::Any) {
        self as &mut dyn (::std::any::Any)
    }
    fn into_any(self: ::std::boxed::Box<Self>) -> ::std::boxed::Box<dyn (::std::any::Any)> {
        self
    }

    fn descriptor(&self) -> &'static ::protobuf::reflect::MessageDescriptor {
        Self::descriptor_static()
    }

    fn new() -> EntityMove {
        EntityMove::new()
    }

    fn descriptor_static() -> &'static ::protobuf::reflect::MessageDescriptor {
        static descriptor: ::protobuf::rt::LazyV2<::protobuf::reflect::MessageDescriptor> = ::protobuf::rt::LazyV2::INIT;
        descriptor.get(|| {
            let mut fields = ::std::vec::Vec::new();
            fields.push(::protobuf::reflect::accessor::make_simple_field_accessor::<_, ::protobuf::types::ProtobufTypeEnum<EntityType>>(
                "t",
                |m: &EntityMove| { &m.t },
                |m: &mut EntityMove| { &mut m.t },
            ));
            fields.push(::protobuf::reflect::accessor::make_simple_field_accessor::<_, ::protobuf::types::ProtobufTypeFloat>(
                "x",
                |m: &EntityMove| { &m.x },
                |m: &mut EntityMove| { &mut m.x },
            ));
            fields.push(::protobuf::reflect::accessor::make_simple_field_accessor::<_, ::protobuf::types::ProtobufTypeFloat>(
                "y",
                |m: &EntityMove| { &m.y },
                |m: &mut EntityMove| { &mut m.y },
            ));
            fields.push(::protobuf::reflect::accessor::make_simple_field_accessor::<_, ::protobuf::types::ProtobufTypeFloat>(
                "z",
                |m: &EntityMove| { &m.z },
                |m: &mut EntityMove| { &mut m.z },
            ));
            ::protobuf::reflect::MessageDescriptor::new_pb_name::<EntityMove>(
                "EntityMove",
                fields,
                file_descriptor_proto()
            )
        })
    }

    fn default_instance() -> &'static EntityMove {
        static instance: ::protobuf::rt::LazyV2<EntityMove> = ::protobuf::rt::LazyV2::INIT;
        instance.get(EntityMove::new)
    }
}

impl ::protobuf::Clear for EntityMove {
    fn clear(&mut self) {
        self.t = EntityType::T_Null;
        self.x = 0.;
        self.y = 0.;
        self.z = 0.;
        self.unknown_fields.clear();
    }
}

impl ::std::fmt::Debug for EntityMove {
    fn fmt(&self, f: &mut ::std::fmt::Formatter<'_>) -> ::std::fmt::Result {
        ::protobuf::text_format::fmt(self, f)
    }
}

impl ::protobuf::reflect::ProtobufValue for EntityMove {
    fn as_ref(&self) -> ::protobuf::reflect::ReflectValueRef {
        ::protobuf::reflect::ReflectValueRef::Message(self)
    }
}

#[derive(Clone,PartialEq,Eq,Debug,Hash)]
pub enum EntityType {
    T_Null = 0,
    T_Player = 1,
}

impl ::protobuf::ProtobufEnum for EntityType {
    fn value(&self) -> i32 {
        *self as i32
    }

    fn from_i32(value: i32) -> ::std::option::Option<EntityType> {
        match value {
            0 => ::std::option::Option::Some(EntityType::T_Null),
            1 => ::std::option::Option::Some(EntityType::T_Player),
            _ => ::std::option::Option::None
        }
    }

    fn values() -> &'static [Self] {
        static values: &'static [EntityType] = &[
            EntityType::T_Null,
            EntityType::T_Player,
        ];
        values
    }

    fn enum_descriptor_static() -> &'static ::protobuf::reflect::EnumDescriptor {
        static descriptor: ::protobuf::rt::LazyV2<::protobuf::reflect::EnumDescriptor> = ::protobuf::rt::LazyV2::INIT;
        descriptor.get(|| {
            ::protobuf::reflect::EnumDescriptor::new_pb_name::<EntityType>("EntityType", file_descriptor_proto())
        })
    }
}

impl ::std::marker::Copy for EntityType {
}

impl ::std::default::Default for EntityType {
    fn default() -> Self {
        EntityType::T_Null
    }
}

impl ::protobuf::reflect::ProtobufValue for EntityType {
    fn as_ref(&self) -> ::protobuf::reflect::ReflectValueRef {
        ::protobuf::reflect::ReflectValueRef::Enum(::protobuf::ProtobufEnum::descriptor(self))
    }
}

static file_descriptor_proto_data: &'static [u8] = b"\
    \n\x0ccommon.proto\"[\n\nEntityMove\x12\x1b\n\x01t\x18\x01\x20\x01(\x0e2\
    \x0b.EntityTypeR\x01tB\0\x12\x0e\n\x01x\x18\x02\x20\x01(\x02R\x01xB\0\
    \x12\x0e\n\x01y\x18\x03\x20\x01(\x02R\x01yB\0\x12\x0e\n\x01z\x18\x04\x20\
    \x01(\x02R\x01zB\0:\0*(\n\nEntityType\x12\n\n\x06T_Null\x10\0\x12\x0c\n\
    \x08T_Player\x10\x01\x1a\0B\0b\x06proto3\
";

static file_descriptor_proto_lazy: ::protobuf::rt::LazyV2<::protobuf::descriptor::FileDescriptorProto> = ::protobuf::rt::LazyV2::INIT;

fn parse_descriptor_proto() -> ::protobuf::descriptor::FileDescriptorProto {
    ::protobuf::Message::parse_from_bytes(file_descriptor_proto_data).unwrap()
}

pub fn file_descriptor_proto() -> &'static ::protobuf::descriptor::FileDescriptorProto {
    file_descriptor_proto_lazy.get(|| {
        parse_descriptor_proto()
    })
}
