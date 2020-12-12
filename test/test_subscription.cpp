
#include <stdio.h>
#include <subscription_state.h>
#include <publishable_impl.h>
#include "marshalling/test_marshalling.h"

class SampleNode
{
	size_t ctr = 0;
public:
	SampleNode() {}
	void addAccess() {++ctr;}
};

struct Size{
  double X = 1.0;
  double Y = 2.0;
  double Z = 3.0;
};

struct POINT3DREAL{
  double X = 4.0;
  double Y = 5.0;
  double Z = 6.0;
};

struct PublishableSample {
	SampleNode* node = nullptr;
	int ID = 333;
	Size size;
	std::vector<POINT3DREAL> vector_struct_point3dreal = {{310, 311, 312}, {320, 321, 322}};
	std::vector<int> vector_of_int = {111, 112, 113};

	PublishableSample( SampleNode* node_ ) { node = node_; }
};

void publishableTestOne()
{
	SampleNode node;
	m::Buffer b;
	m::JsonComposer composer( b );
	m::publishable_sample_Wrapper<PublishableSample, m::JsonComposer<m::Buffer>> publishableSampleWrapper( &node );
	publishableSampleWrapper.resetComposer( &composer );

	// quick test for getting right after ctoring
	int id = publishableSampleWrapper.get_ID();
	assert( id == 333 );
	fmt::print( "ID = {}\n", id );

	auto& size = publishableSampleWrapper.get_size();
	assert( size.X == 1.0 );
	assert( size.Y == 2.0 );
	assert( size.Z == 3.0 );
	fmt::print( "size = {}, {}, {}\n", size.X, size.Y, size.Z );
	publishableSampleWrapper.set_size( {901., 902., 903} );
	auto& size1 = publishableSampleWrapper.get_size();
	assert( size1.X == 901.0 );
	assert( size1.Y == 902.0 );
	assert( size1.Z == 903.0 );

	auto voi = publishableSampleWrapper.get_vector_of_int();
	assert( voi.size() == 3 );
	fmt::print( "vector_of_int[0] = {}\n", voi.get_at(0) );
	auto vosp3d = publishableSampleWrapper.get_vector_struct_point3dreal();
	assert( vosp3d.size() == 2 );
	auto vosp3d_0 = vosp3d.get_at( 0 );
	assert( vosp3d_0.get_X() == 310 );
	assert( vosp3d_0.get_Y() == 311 );
	assert( vosp3d_0.get_Z() == 312 );
	fmt::print( "vector_of_int.size() = {}\n", vosp3d.size() );
	fmt::print( "vector_of_int[0] = {}, {}, {}\n", vosp3d_0.get_X(), vosp3d_0.get_Y(), vosp3d_0.get_Z() );

	// updating (some) values
	publishableSampleWrapper.set_ID( 38 );
	assert( publishableSampleWrapper.get_ID() == 38 );
	int ins1 = 17;
	publishableSampleWrapper.get4set_vector_of_int().insert_before( 0, ins1 );
	int ins1_back = publishableSampleWrapper.get_vector_of_int().get_at(0);
	assert( ins1 == ins1_back );
	POINT3DREAL point3dreal = {410, 411, 412};
	publishableSampleWrapper.get4set_vector_struct_point3dreal().insert_before( 1, point3dreal );
	auto point3dreal_back = publishableSampleWrapper.get_vector_struct_point3dreal().get_at( 1 );
	assert( point3dreal.X == point3dreal_back.get_X() );
	assert( point3dreal.Y == point3dreal_back.get_Y() );
	assert( point3dreal.Z == point3dreal_back.get_Z() );
	publishableSampleWrapper.get4set_vector_struct_point3dreal().get4set_at( 1 ).set_Y( 555 );
	auto point3dreal_Y_back = publishableSampleWrapper.get_vector_struct_point3dreal().get_at( 1 ).get_Y();
	assert( point3dreal_Y_back == 555 );

	publishableSampleWrapper.finalizeComposing();
	std::string_view sview( reinterpret_cast<const char*>(b.begin()), b.size() );
	fmt::print( "{}\n", sview );

	m::JsonParser parser( b );
	m::publishable_sample_Wrapper<PublishableSample, m::JsonComposer<m::Buffer>> publishableSampleWrapperSlave( &node );
	publishableSampleWrapperSlave.applyMessageWithUpdates( parser );
	assert( publishableSampleWrapperSlave.get_ID() == 38 );
}


/*template<class T>
class C
{
public:
	T t;

	bool hasX()
	{
		if constexpr requires
	}
};*/

//#include <concepts>
/*template<class T>
std::string optionalToString(T* obj)
{
    constexpr bool has_toString = requires(const T& t) {
        t.toString();
    };

    if constexpr (has_toString)
        return obj->toString();
    else
        return "toString not defined";
}

class X
{
public:
	GMQ_COLL string toString() { return "X: I have it!"; }
};

class Y
{
public:
	GMQ_COLL string toString_() { return "Y: I have it!"; }
};*/

struct A
{
	int x = 17;
};

struct B
{
	float y = 3.14;
};

struct S
{
	std::string s;
	A a;
	GMQ_COLL vector<B> b = {B({2.33}),B({4.03})};
	GMQ_COLL vector<int> c = {17,18};
};

////////////////////////////////////////////////////

template<typename T> concept has_x_member = requires { { T::x }; };
template<class TA>
class A_RefWrapper
{
	static constexpr bool has_x = has_x_member<TA>;
	static_assert( has_x, "type TA must have member TA::x of a type corresponding to IDL type INTEGER" );
	static_assert( std::is_integral<decltype(TA::x)>::value, "member TA::x must be INTEGER type" );
	TA& a;
public:
	A_RefWrapper( TA& actual ) : a( actual ) {}
	auto get_x() { return a.x; }
};
template<class TA, class RootT>
class A_RefWrapper4setting
{
	static constexpr bool has_x = has_x_member<TA>;
	static_assert( has_x, "type TA must have member TA::x of a type corresponding to IDL type INTEGER" );
	static_assert( std::is_integral<decltype(TA::x)>::value, "member TA::x must be INTEGER type" );
	TA& a;
	RootT& root;
	GMQ_COLL vector<size_t> address;
public:
	A_RefWrapper4setting( TA& actual, RootT& root_, const GMQ_COLL vector<size_t> address_, size_t idx ) : a( actual ), root( root_ ) {
		address = address_;
		address.push_back (idx );
	}
	auto get_x() { return a.x; }
	void set_x( decltype(TA::x) val) { a.x = val; }
};
	
template<typename T> concept has_y_member = requires { { T::y }; };
template<class TB>
class B_RefWrapper
{
	static constexpr bool has_y = has_y_member<TB>;
	static_assert( has_y, "type TB must have member TB::x of atype corresponding to IDL type REAL" );
	static_assert( std::is_floating_point<decltype(TB::y)>::value, "member TB::y must be REAL type" );
	TB& b;
public:
	B_RefWrapper( TB& actual ) : b( actual ) {}
	auto get_y() { return b.y; }
};

template<class TB, class RootT>
class B_RefWrapper4Set
{
	static constexpr bool has_y = has_y_member<TB>;
	static_assert( has_y, "type TB must have member TB::x of atype corresponding to IDL type REAL" );
	static_assert( std::is_floating_point<decltype(TB::y)>::value, "member TB::y must be REAL type" );
	TB& b;
	RootT& root;
	GMQ_COLL vector<size_t> address;
public:
	B_RefWrapper4Set( TB& actual, RootT& root_, const GMQ_COLL vector<size_t> address_, size_t idx ) : b( actual ), root( root_ ) {
		address = address_;
		address.push_back (idx );
	}
	auto get_y() { return b.y; }
	void set_y( decltype(TB::y) val) { 
		fmt::print( "updating B::y with value {}; path = [ ", val );
		for ( size_t i=0; i<address.size(); ++i )
		fmt::print( "{} ", address[i] );
		fmt::print( "] \n" );
		b.y = val; 
	}
};

/*template<class TB>
class Vector_of_B_RefWrapper
{
//	static constexpr bool has_y = has_y_member<TB>;
//	static_assert( has_y, "type TB must have member TB::x of atype corresponding to IDL type REAL" );
//	static_assert( std::is_floating_point<decltype(TB::y)>::value, "member TB::y must be REAL type" );
	TB& b;
public:
	Vector_of_B_RefWrapper( TB& actual ) : b( actual ) {}
	auto get_at( size_t idx ) { return B_RefWrapper<typename TB::value_type>(b[idx]); }
	void remove( size_t idx ) { GMA_ASSERT( idx < b.size()); b.erase( b.begin() + idx ); }
	void insert_bafore( TB& what, size_t idx ) { GMA_ASSERT( idx < b.size()); b.insert( what, b.begin() + idx ); }
};
template<class TB, class RootT>
class Vector_of_B_RefWrapper4Set
{
//	static constexpr bool has_y = has_y_member<TB>;
//	static_assert( has_y, "type TB must have member TB::x of atype corresponding to IDL type REAL" );
//	static_assert( std::is_floating_point<decltype(TB::y)>::value, "member TB::y must be REAL type" );
	TB& b;
	RootT& root;
	GMQ_COLL vector<size_t> address;
public:
	Vector_of_B_RefWrapper4Set( TB& actual, RootT& root_, const GMQ_COLL vector<size_t> address_, size_t idx ) : b( actual ), root( root_ ) {
		address = address_;
		address.push_back (idx );
	}
	auto get4set_at( size_t idx ) { return B_RefWrapper4Set<typename TB::value_type, RootT>(b[idx], root, address, idx); }
	void remove( size_t idx ) { GMA_ASSERT( idx < b.size()); b.erase( b.begin() + idx ); }
	void insert_bafore( TB& what, size_t idx ) { GMA_ASSERT( idx < b.size()); b.insert( what, b.begin() + idx ); }
};*/

template<typename T> concept has_A_value_member = requires { { T::a }; };
template<typename T> concept has_B_value_member = requires { { T::b }; };
template<typename T> concept has_C_value_member = requires { { T::c }; };
template<typename T> concept has_string_value_member = requires { { T::s }; };

template<class T>
class S_Wrapper
{
	T t;
	static constexpr bool has_a = has_A_value_member<T>;
	static_assert( has_a, "type T must have member T::a of a type corresponding to IDL type PUBLISHABLE A" );
	static constexpr bool has_b = has_B_value_member<T>;
	static_assert( has_b, "type T must have member T::b of a type corresponding to IDL type PUBLISHABLE B" );
	static constexpr bool has_c = has_C_value_member<T>;
	static_assert( has_c, "type T must have member T::c of a type corresponding to IDL type PUBLISHABLE C" );
	static constexpr bool has_s = has_string_value_member<T>;
	static_assert( has_s, "member T::a of type CHARACTER-STRING is expected" );
	static_assert( std::is_convertible<decltype(T::s), GMQ_COLL string>::value, "member TB::s must be of type convertible to GMQ_COLL string" );

public:
	S_Wrapper() {}
	const auto& get_s() { return t.s; } // NOTE: for strings returning const ref
	void set_s( GMQ_COLL string s_ ) {t.s = s_; }
	auto get_a() { return A_RefWrapper<decltype(T::a)>(t.a); }
	auto get4set_a() { return A_RefWrapper4Set<decltype(T::a), S_Wrapper>(t.a, *this, GMQ_COLL vector<size_t>(), 1); }
//	auto get_b() { return Vector_of_B_RefWrapper(t.b); }
//	auto get4set_b() { return Vector_of_B_RefWrapper4Set<decltype(T::b), S_Wrapper>(t.b, *this, GMQ_COLL vector<size_t>(), 2); }
	auto get_b() { return m::VectorOfStructRefWrapper<B_RefWrapper<typename decltype(T::b)::value_type>, decltype(T::b)>(t.b); }
	auto get4set_b() { return m::VectorOfStructRefWrapper4Set<B_RefWrapper4Set<typename decltype(T::b)::value_type, S_Wrapper>, decltype(T::b), S_Wrapper>(t.b, *this, GMQ_COLL vector<size_t>(), 2); }
	auto get_c() { return m::VectorOfSimpleTypeRefWrapper(t.c); }
	auto get4set_c() { return m::VectorOfSimpleTypeRefWrapper4Set<decltype(T::c), S_Wrapper>(t.c, *this, GMQ_COLL vector<size_t>(), 2); }
};

void test()
{
	S_Wrapper<S> s;
	printf( "%d\n", s.get_a().get_x() );
	printf( "%f\n", s.get_b().get_at(1).get_y() );
	s.get4set_b().get4set_at(0).set_y( 0.1234 );
	s.get4set_b().get4set_at(1).set_y( 1.2345 );
	printf( "%f\n", s.get_b().get_at(0).get_y() );
	printf( "%f\n", s.get_b().get_at(1).get_y() );
}

/*template<typename T, typename U>
concept can_add = requires(T t, U u) { t + u; };

template<typename T>
concept has_bool_value_member = requires { { T::value } -> std::convertible_to<bool>; };

template<typename T>
concept has_count_call_member = requires { { T::count() } -> std::convertible_to<int>; };

template<typename T>
concept has_count_call_member2 = requires { { T::count() }; };

template<typename T>
concept has_count_call_member3 = requires(T t) { { t.count() }; };

template <typename T>
int foo(T t)
{
    if constexpr (requires (T t){ { t.count() } -> std::convertible_to<int>; })
        return t.count();
    else
        return 0;
}*/

/*template <typename T>
int foo(T t)
{
    if constexpr (requires (T t){ { t.count() } -> std::convertible_to<int>; })
        return t.count();
    else
        return 0;
}*/

class SubscriptionPairTest
{
	GmqSubscriptionState source;
	GmqSubscriptionState replica;

public:
	void test()
	{
		m::Buffer b;
		GmqSubscriptionState::PositionVectorT v;

		b.append( "root msg", sizeof( "root msg" ) - 1 );
		source.updateNode( {}, std::move( b ) );

//		b.clear();
		b.append( "level 1 msg 1", sizeof( "level 1 msg 1" ) - 1 );
		source.insertNode( {}, 0, std::move( b ) );

//		b.clear();
		b.append( "level 1 msg 2", sizeof( "level 1 msg 2" ) - 1 );
		source.insertNode( {}, 0, std::move( b ) );

//		b.clear();
		b.append( "level 1 msg 3", sizeof( "level 1 msg 3" ) - 1 );
		source.insertNode( {}, 2, std::move( b ) );

//		b.clear();
		b.append( "level 2 msg 1", sizeof( "level 2 msg 1" ) - 1 );
		source.insertNode( {1}, 0, std::move( b ) );

		source.dbgPrintTree();

		source.removeNode( {0} );

		source.dbgPrintTree();
	}
};

void testSubscriptionTest()
{
	/*SubscriptionPairTest test;
	test.test();*/
/*	X x;
	Y y;
	printf( "%s\n", optionalToString( &x ).c_str() );
	printf( "%s\n", optionalToString( &y ).c_str() );
    printf( "%d\n", foo(1) );*/

	test();

    /*struct bar
    {
		bool x;
        int count() const { return 6; }
    };
    struct bar1
    {
		bool value;
        int count_() const { return 6; }
//        static int count() { return 6; }
        static void count() { return; }
    };
	constexpr bool has_bool = has_bool_value_member<bar>;
	constexpr bool has_bool1 = has_bool_value_member<bar1>;
	printf( "%s, %s\n", has_bool ? "Y" : "N", has_bool1 ? "Y" : "N" );

	constexpr bool has_count = has_count_call_member<bar>;
	constexpr bool has_count1 = has_count_call_member<bar1>;
	printf( "%s, %s\n", has_count ? "Y" : "N", has_count1 ? "Y" : "N" );

	constexpr bool has_2count = has_count_call_member3<bar>;
	constexpr bool has_2count1 = has_count_call_member3<bar1>;
	printf( "%s, %s\n", has_2count ? "Y" : "N", has_2count1 ? "Y" : "N" );

	constexpr bool has_3count = has_count_call_member3<bar>;
	constexpr bool has_3count1 = has_count_call_member3<bar1>;
	printf( "%s, %s\n", has_3count ? "Y" : "N", has_3count1 ? "Y" : "N" );


//    printf( "foo(bar{}): : %d\n", foo(bar{}) );*/
}