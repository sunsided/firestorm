//
// Created by sunside on 25.03.18.
//

#include <firestorm/utils/guid_generator.h>
#include <firestorm/synchronization/reader_writer_lock.h>
#include <boost/uuid/random_generator.hpp>

namespace firestorm {

    class guid_generator::Impl {
    public:
        Impl() noexcept {}
        ~Impl() noexcept = default;

        inline guid create_guid() {
            auto lock = _lock.write_lock();
            return _generator();
        }

    private:
        boost::uuids::random_generator _generator{};
        reader_writer_lock _lock{};
    };

    static std::unique_ptr<guid_generator> generator = nullptr;
    static reader_writer_lock guid_create_lock{};

    guid_generator::guid_generator() noexcept
        : _impl{std::make_unique<guid_generator::Impl>()}
    {}

    guid_generator::~guid_generator() noexcept = default;

    guid guid_generator::create() const noexcept {
        return _impl->create_guid();
    }

    guid_generator& guid_generator::get_default() noexcept {
        const auto lock = guid_create_lock.write_lock();
        if (generator == nullptr) {
            generator = std::make_unique<guid_generator>();
        }
        return *generator;
    }

}