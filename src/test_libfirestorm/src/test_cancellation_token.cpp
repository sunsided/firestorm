//
// Created by sunside on 22.03.18.
//

#include <atomic>
#include <gtest/gtest.h>
#include <firestorm/synchronization/cancellation_token_source.h>

namespace {

    TEST(CancellationToken, NewToken_WhenNotCanceled_IsNotCanceled) {
        // arrange
        firestorm::cancellation_token_source cts;

        // act
        auto ct = cts.create_token();

        // assert
        ASSERT_EQ(ct->canceled(), false);
    }

    TEST(CancellationToken, NewToken_WhenCanceled_IsCanceled) {
        // arrange
        firestorm::cancellation_token_source cts;

        // act
        cts.cancel();
        auto ct = cts.create_token();

        // assert
        ASSERT_EQ(ct->canceled(), true);
    }

    TEST(CancellationToken, ExistingToken_WhenCanceled_IsCanceled) {
        // arrange
        firestorm::cancellation_token_source cts;

        // act
        auto ct = cts.create_token();
        cts.cancel();

        // assert
        ASSERT_EQ(ct->canceled(), true);
    }

    TEST(CancellationToken, MultipleTokens_WhenCanceled_AreCanceled) {
        // arrange
        firestorm::cancellation_token_source cts;

        // act
        auto a = cts.create_token();
        auto b = cts.create_token();
        cts.cancel();

        // assert
        ASSERT_EQ(a->canceled(), true);
        ASSERT_EQ(b->canceled(), true);
    }

    TEST(CancellationToken, TokenCallback_WhenNotCanceled_IsNotCalled) {
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        auto ct = cts.create_token();
        ct->register_callback([&count]() { count += 1; });

        // assert
        ASSERT_EQ(count, 0);
    }

    TEST(CancellationToken, TokenCallback_WhenCanceled_IsCalled) {
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        auto ct = cts.create_token();
        ct->register_callback([&count]() { count += 1; });

        cts.cancel();

        // assert
        ASSERT_EQ(count, 1);
    }

    TEST(CancellationToken, TokenCallback_WhenAlreadyCanceled_IsCalledImmediately) {
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        cts.cancel();
        auto ct = cts.create_token();
        ct->register_callback([&count]() { count += 1; });

        // assert
        ASSERT_EQ(count, 1);
    }

    TEST(CancellationToken, TokenCallback_WhenCanceledOften_IsCalledOnce) {
        // arrange
        firestorm::cancellation_token_source cts;
        auto count = 0;

        // act
        auto ct = cts.create_token();
        ct->register_callback([&count]() { count += 1; });

        cts.cancel();
        cts.cancel();
        cts.cancel();
        cts.cancel();

        // assert
        ASSERT_EQ(count, 1);
    }

}