#include "type_string.h"
#include "type_debug.h"
#include "testing.h"

#include <vector>

using meta::string::value_list;
using meta::string::type_string;

TEST(value_list_empty)
{
	using c0 = value_list<char>;
	using i0 = value_list<int>;
	using s0 = value_list<size_t>;
	using p0 = value_list<void***>;

	static_assert(c0::length == 0, "");
	static_assert(i0::length == 0, "");
	static_assert(s0::length == 0, "");
	static_assert(p0::length == 0, "");
}

void f1() {}
void f2() {}

TEST(value_list_with_elems)
{
	using c4 = value_list<char, 'C', 'i', 'a', 'o'>;
	using p2 = value_list<void(*)(), &f1, &f2>;

	static_assert(c4::length == 4, "");
	static_assert(p2::length == 2, "");
}

TEST(value_list_equal)
{
	static_assert(value_list<int, 11>::equal<value_list<int, 11>>, "");
	static_assert(!value_list<int, 10>::equal<value_list<int, 11>>, "");
}

TEST(sizeof_value_list_is_invariant)
{
	using v0 = value_list<int>;
	using v1 = value_list<int, 1, 2, 3, 4, 5>;
	static_assert(sizeof(v0) == sizeof(v1));
}

TEST(sizeof_value_list_is_small)
{
	struct small {};
	static_assert(sizeof(value_list<unsigned long long>) <= sizeof(small), "");
	LOGGER << PRINT((sizeof(value_list<int, 10, 100, 1000>))) << std::endl;
}

TEST(make_string_creates_a_type_string_from_cstring_literal)
{
	using s1 = type_string<'h', 'e', 'l', 'l', 'o'>;
	auto v1 = MAKE_STRING("hello");
	static_assert(s1::equal<decltype(v1)>, "");
}

TEST(make_type_string_creates_a_type_string_from_cstring_literal)
{
	using s1 = type_string<'h', 'e', 'l', 'l', 'o'>;
	MAKE_STRING_TYPE(v1, "hello");
	static_assert(s1::equal<v1>, "");
}

TEST(make_string_create_empty_string)
{
	auto v1 = MAKE_STRING("");
	static_assert(decltype(v1)::length == 0, "");
}

TEST(make_type_string_create_empty_string)
{
	MAKE_STRING_TYPE(s1, "");
	static_assert(s1::length == 0, "");
}

TEST(value_list_initialiser_list_view)
{
	using v1 = value_list<int, 0, 1, 2, 3, 4, 5>;
	std::vector<int> vec = v1::initializer_list;
	LOGGER << PRINT(meta::debug::to_string<decltype(v1::initializer_list)>()) << std::endl;
	for (int i = 0; i < 6; ++i) {
		LOGGER << PRINT(vec[i]) << std::endl;
		ASSERT(vec[i] == i);
	}
}

TEST(value_list_data_view)
{
	using v1 = value_list<int, 0, 1, 2, 3, 4, 5>;
	const auto p1 = v1::data();
	LOGGER << PRINT(meta::debug::to_string<decltype(p1)>()) << std::endl;
	for (int i = 0; i < 6; ++i) {
		LOGGER << PRINT(p1[i]) << std::endl;
		ASSERT(p1[i] == i);
	}
}

TEST(type_string_c_string_view_includes_null_termination)
{
	constexpr std::string_view expected = "Same string";
	MAKE_STRING_TYPE(s1, "Same string");
	const auto c1 = s1::c_str();
	ASSERT(expected == c1);
	ASSERT(c1[s1::length] == '\0');
	LOGGER << PRINT(s1::c_str()) << std::endl;
}

TEST(value_list_at_returns_item)
{
	using v1 = value_list<int, 10, 11, 12, 13, 14, 15>;
	static_assert(v1::at<0> == 10, "");
	static_assert(v1::at<v1::last_index> == 15, "");
	//v1::at<v1::last_index + 1>; 	// Will static assert
	//value_list<int>::at<0>;	// Will static assert
}

template<class List>
void print_list()
{
	for (size_t i = 0; i < List::length; ++i) {
		LOGGER << PRINT((List::data()[i])) << std::endl;
	}
}

TEST(value_list_slice_returns_sub_list)
{
	using v1 = value_list<int, 10, 11, 12, 13, 14, 15>;
	using s1 = v1::slice<0, 3>;
	static_assert(value_list<int, 10, 11, 12>::equal<v1::slice<0, 3>>, "");
	print_list<s1>();

	using s2 = v1::slice<1, 2>;
	static_assert(s2::at<0> == 11, "");

	using s3 = v1::slice<1, 1>;
	static_assert(s3::empty, "");

	LOGGER << PRINT((meta::debug::to_string<s3>())) << std::endl;

	static_assert(v1::equal<v1::slice<0, v1::length>>, "");

	//using s4 = v1::slice<1, 0>;			// Will static assert
	//using s4 = v1::slice<1, v1::length + 1>; 	// Will static assert
}

template<int... Ints>
struct sum {
	static constexpr int value = (Ints + ...);
};

TEST(value_list_apply_metacalls_metafunction)
{
	using v1 = value_list<int, 1, 10, 100, 9000>;
	static_assert(v1::apply<sum>::value == 9111, "");
}

TEST(value_list_append_adds_items_in_tail)
{
	using v1 = value_list<int, 1, 2, 3>;
	using v2 = v1::append<4, 5>;
	static_assert(v2::equal<value_list<int, 1, 2, 3, 4, 5>>, "");

	MAKE_STRING_TYPE(s1, "Hello ");
	using s2 = s1::append<'t', 'h', 'e', 'r', 'e'>;
	MAKE_STRING_TYPE(s3, "Hello there");
	static_assert(s2::equal<s3>, "");
	LOGGER << PRINT(s2::c_str()) << std::endl;
}

TEST(value_list_prepend_adds_items_in_front)
{
	using v1 = value_list<int, 1, 2, 3>;
	using v2 = v1::prepend<4, 5>;
	static_assert(v2::equal<value_list<int, 4, 5, 1, 2, 3>>, "");
}

TEST(value_list_merge_adds_items_in_tail)
{
	using v1 = value_list<int, 1, 2, 3>;
	using v2 = value_list<int, 4, 5>;
	using v3 = v1::merge<v2>;
	static_assert(v3::equal<value_list<int, 1, 2, 3, 4, 5>>, "");

	MAKE_STRING_TYPE(s1, "Hello ");
	MAKE_STRING_TYPE(s2, "World");
	using s3 = s1::merge<s2>;
	LOGGER << PRINT(s3::c_str()) << std::endl;
}

TEST(obfuscator_encode_makes_obfuscated_list)
{
	MAKE_STRING_TYPE(s1, "MIGHT_NOT_SEE_ME");
	using s2 = meta::string::obfuscator<char, 21>::encode<s1>::ret;
	static_assert(!s1::equal<s2>, "");
	LOGGER << "OBFUSCATED: " << s2::c_str() << std::endl;
}

TEST(obfuscator_encode_makes_obfuscated_list_with_wider_key)
{
	MAKE_STRING_TYPE(s1, "MIGHT_NOT_SEE_ME");
	using s2 = meta::string::obfuscator<int32_t, 0x10203040>::encode<s1>::ret;
	static_assert((s1::at<0> xor 0x40) == s2::at<0>, "");
	static_assert((s1::at<1> xor 0x30) == s2::at<1>, "");
	static_assert((s1::at<2> xor 0x20) == s2::at<2>, "");
	static_assert((s1::at<3> xor 0x10) == s2::at<3>, "");
	static_assert((s1::at<4> xor 0x40) == s2::at<4>, "");
	static_assert((s1::at<5> xor 0x30) == s2::at<5>, "");
	static_assert((s1::at<6> xor 0x20) == s2::at<6>, "");
	static_assert((s1::at<7> xor 0x10) == s2::at<7>, "");
	static_assert(!s1::equal<s2>, "");
}

TEST(obfuscator_decodes_obfuscated_string)
{
	MAKE_STRING_TYPE(s1, "MIGHT_NOT_SEE_ME");
	using ob = meta::string::obfuscator<char, 21>;
	using s2 = ob::encode<s1>::ret;
	LOGGER << "OBFUSCATED: " << s2::c_str() << std::endl;
	LOGGER << "ORIGINAL  : " << ob::decode(s2::data(), s2::length) << std::endl;
	ASSERT(std::string{s1::c_str()} == ob::decode(s2::data(), s2::length));
}

TEST(obfuscator_decodes_obfuscated_string_with_wider_key)
{
	MAKE_STRING_TYPE(s1, "MIGHT_NOT_SEE_ME");
	using ob = meta::string::obfuscator<uint64_t, 0x1234567811223344>;
	using s2 = ob::encode<s1>::ret;
	static_assert(!s1::equal<s2>, "");
	LOGGER << "OBFUSCATED: " << s2::c_str() << std::endl;
	LOGGER << "ORIGINAL  : " << ob::decode(s2::data(), s2::length) << std::endl;
	ASSERT(std::string{s1::c_str()} == ob::decode(s2::data(), s2::length));
}

TEST(obfuscator_decodes_obfuscated_string_with_wider_bad_key)
{
	MAKE_STRING_TYPE(s1, "MIGHT_NOT_SEE_ME");
	using ob = meta::string::obfuscator<uint64_t, 0x1>;
	using s2 = ob::encode<s1>::ret;
	static_assert(!s1::equal<s2>, "");
	LOGGER << "OBFUSCATED: " << s2::c_str() << std::endl;
	LOGGER << "ORIGINAL  : " << ob::decode(s2::data(), s2::length) << std::endl;
	ASSERT(std::string{s1::c_str()} == ob::decode(s2::data(), s2::length));
}

TEST(obfuscated_string_is_constructed)
{
	using meta::string::obfuscated_string;

	MAKE_OBFUSCATED_STRING(s, "SHOULD_NOT SEE ME EITHER");
	LOGGER << "OBFUSCATED: " << s << std::endl;
}

TEST(obfuscated_string_may_be_visible)
{
	using meta::string::obfuscated_string;

	LOGGER << "OBFUSCATED: " <<
		TRY_MAKE_OBFUSCATED_STRING("MIGHT_NOT SEE ME EITHER") << std::endl;
}

template<int... Ints>
struct linear_combination {
	template<size_t... Indexes>
	struct zipped {
		static constexpr auto op(size_t i, int v) { return i * v; }
		static constexpr size_t value = (op(Ints, Indexes) + ...);
	};
};

TEST(value_list_apply_zipped_metacalls_metafunction)
{
	using v1 = value_list<int, 0, 10, 100, 1000>;
	using v2 = v1::apply_zipped<linear_combination>;
	static_assert(v2::value == 3210, "");
	LOGGER << "v1 = " << meta::debug::to_string<v1>() << std::endl;
	LOGGER << "v2 = " << meta::debug::to_string<v2>() << std::endl;
	LOGGER << PRINT(v2::value) << std::endl;
}

int main()
{
	return test::registry::run_all("type_string") ? EXIT_SUCCESS : EXIT_FAILURE;
}
