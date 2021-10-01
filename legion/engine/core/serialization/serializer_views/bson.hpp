#pragma once
#include <core/serialization/serializer_views/serializer_view.hpp>

namespace legion::core::serialization
{
    struct bson : public serializer_view
    {
        bson() = default;
        virtual ~bson() = default;

        virtual void serialize_int(std::string& name, int serializable) override;
        virtual void serialize_float(std::string& name, float serializable) override;
        virtual void serialize_double(std::string& name, double serializable) override;
        virtual void serialize_bool(std::string& name, bool serializable) override;
        virtual void serialize_string(std::string& name, const std::string_view& serializable) override;
        virtual void serialize_id_type(std::string& name, id_type serializable) override;

        virtual common::result<int, fs_error> deserialize_int(std::string& name) override;
        virtual common::result<float, fs_error> deserialize_float(std::string& name) override;
        virtual common::result<double, fs_error> deserialize_double(std::string& name) override;
        virtual bool deserialize_bool(std::string& name) override;
        virtual common::result<std::string, fs_error> deserialize_string(std::string& name) override;
        virtual common::result<id_type, fs_error> deserialize_id_type(std::string& name) override;

        virtual void start_object() override;
        virtual void start_object(std::string name) override;
        virtual void end_object() override;


        virtual void start_container(std::string name) override;
        virtual void end_container() override;

        virtual void start_read() override;
        virtual void end_read() override;

        virtual int start_read_array() override;
        virtual void end_read_array() override;

        virtual common::result<void, fs_error> write(fs::view& file) override;
        virtual common::result<void, fs_error> read(fs::view& file) override;
        virtual common::result<void, fs_error> read(byte_vec data) override;
        virtual common::result<void, fs_error> read(byte_vec::iterator begin, byte_vec::iterator end) override;
    };
}

#include <core/serialization/serializer_views/bson.inl>
