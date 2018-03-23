//
// Created by sunside on 22.03.18.
//

#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <firestorm/synchronization/cancellation_token_source.h>

namespace {

    TEST(CancellationToken, NewToken_WhenNotCanceled_IsNotCanceled) { // NOLINT
        // arrange
        firestorm::cancellation_token_source cts;

        // act
        auto ct = cts.create_token();

        // assert
        ASSERT_FALSE(ct->canceled());
    }

    TEST(CancellationToken, NewToken_WhenCanceled_IsCanceled) { // NOLINT
        // arrange
        firestorm::cancellation_token_source cts;

        // act
        cts.cancel();
        auto ct = cts.create_token();

        // assert
        ASSERT_TRUE(ct->canceled());
    }

    TEST(CancellationToken, ExistingToken_WhenCanceled_IsCanceled) { // NOLINT
        // arrange
        firestorm::cancellation_token_source cts;

        // act
        auto ct = cts.create_token();
        cts.cancel();

        // assert
        ASSERT_TRUE(ct->canceled());
    }

    TEST(CancellationToken, MultipleTokens_WhenCanceled_AreCanceled) { // NOLINT
        // arrange
        firestorm::cancellation_token_source cts;

        // act
        auto a = cts.create_token();
        auto b = cts.create_token();
        cts.cancel();

        // assert
        ASSERT_TRUE(a->canceled());
        ASSERT_TRUE(b->canceled());
    }

    TEST(CancellationToken, TokenCallback_WhenNotCanceled_IsNotCalled) { // NOLINT
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        auto ct = cts.create_token();
        ct->register_callback([&count]() { count += 1; });

        // assert
        ASSERT_EQ(count, 0);
    }

    TEST(CancellationToken, TokenCallback_WhenCanceled_IsCalled) { // NOLINT
        // arrange
        firestorm::cancellation_token_source cts;
        auto a = 0;
        auto b = 0;

        // act
        auto ct = cts.create_token();
        auto token_a = ct->register_callback([&a]() { a += 1; });
        auto token_b = ct->register_callback([&b]() { b += 1; });

        cts.cancel();

        // assert
        ASSERT_EQ(a, 1);
        ASSERT_EQ(b, 1);
    }

    TEST(CancellationToken, TokenCallback_WhenCanceled_AndTokenIsNotCaptured_IsNotCalled) {
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        auto ct = cts.create_token();
        ct->register_callback([&count]() { count += 1; });

        cts.cancel();

        // assert
        ASSERT_EQ(count, 0);
    }

    TEST(CancellationToken, TokenCallback_WhenUnregistered_IsNotCalled) {
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        auto ct = cts.create_token();
        {
            auto token = ct->register_callback([&count]() { count += 1; });
        }

        cts.cancel();

        // assert
        ASSERT_EQ(count, 0);
    }

    TEST(CancellationToken, TokenCallback_WhenUnregisteredManually_IsNotCalled) {
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        auto ct = cts.create_token();
        auto token = ct->register_callback([&count]() { count += 1; });
        ct->unregister_callback(token);

        cts.cancel();

        // assert
        ASSERT_EQ(count, 0);
    }

    TEST(CancellationToken, TokenCallback_WhenAlreadyCanceled_IsCalledImmediately) { // NOLINT
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        cts.cancel();
        auto ct = cts.create_token();
        auto token = ct->register_callback([&count]() { count += 1; });

        // assert
        ASSERT_EQ(count, 1);
    }

    TEST(CancellationToken, TokenCallback_WhenCanceledOften_IsCalledOnce) { // NOLINT
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        auto ct = cts.create_token();
        auto token = ct->register_callback([&count]() { count += 1; });

        cts.cancel();
        cts.cancel();
        cts.cancel();
        cts.cancel();

        // assert
        ASSERT_EQ(count, 1);
    }

    TEST(CancellationToken, CancelAfter_CancelsToken) { // NOLINT
        // arrange
        firestorm::cancellation_token_source cts{std::chrono::milliseconds(10)};

        // act
        auto ct = cts.create_token();
        const auto immediate = ct->canceled();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // assert
        ASSERT_FALSE(immediate);
        ASSERT_TRUE(ct->canceled());
    }

    TEST(CancellationToken, CancelAtTime_CancelsToken) { // NOLINT
        // arrange
        firestorm::cancellation_token_source cts{std::chrono::steady_clock::now() + std::chrono::milliseconds(10)};

        // act
        auto ct = cts.create_token();
        const auto immediate = ct->canceled();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // assert
        ASSERT_FALSE(immediate);
        ASSERT_TRUE(ct->canceled());
    }

    TEST(CancellationToken, CancellationTokenSource_WhenMoved_StillWorks) { // NOLINT
        // arrange
        firestorm::cancellation_token_source a;
        firestorm::cancellation_token_source cts(std::move(a));

        // act
        auto ct = cts.create_token();
        const auto canceled_before = ct->canceled();
        cts.cancel();
        const auto canceled_after = ct->canceled();

        // assert
        ASSERT_FALSE(canceled_before);
        ASSERT_TRUE(canceled_after);
    }

    TEST(CancellationToken, CancellationTokenSource_WhenChained_IsCanceledByParent) { // NOLINT
        // arrange
        firestorm::cancellation_token_source parent;
        firestorm::cancellation_token_source cts(parent.create_token());

        // act
        auto ct = cts.create_token();
        const auto canceled_before = ct->canceled();
        parent.cancel();
        const auto canceled_after = ct->canceled();

        // assert
        ASSERT_FALSE(canceled_before);
        ASSERT_TRUE(canceled_after);
    }

    TEST(CancellationToken, CancellationTokenSource_WhenChained_DoesNotCancelParent) { // NOLINT
        // arrange
        firestorm::cancellation_token_source parent;
        firestorm::cancellation_token_source cts(parent.create_token());

        // act
        auto ct = cts.create_token();
        const auto canceled_before = parent.canceled();
        cts.cancel();
        const auto canceled_after = parent.canceled();

        // assert
        ASSERT_FALSE(canceled_before);
        ASSERT_FALSE(canceled_after);
        ASSERT_TRUE(ct->canceled());
    }
}