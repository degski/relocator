
#include <ciso646>

#include <array>
#include <fstream>
#include <iostream.hpp> // <iostream> + nl, sp etc. defined... https://github.com/degski/string_split/blob/master/iostream.hpp
#include <iterator>
#include <list>
#include <map>
#include <string>
#include <type_traits>
#include <vector>


template<typename T>
struct relocator {

    using value_type        = T;
    using pointer           = value_type *;

    using difference_type   = typename std::pointer_traits<pointer>::difference_type;
    using size_type         = std::make_unsigned_t<difference_type>;

    // struct rebind is purposely not defined.

    relocator ( ) { };
    relocator ( const relocator & ) = delete;
    relocator ( relocator && r ) noexcept : m_pointer ( r.m_pointer ) { r.m_pointer = nullptr; };
    ~relocator ( ) noexcept {
        if ( m_pointer ) {
            std::cout << "freed " << m_pointer << nl;
            std::free ( m_pointer );
        }
    }

    relocator & operator = ( const relocator & ) = delete;
    relocator & operator = ( relocator && rhs ) noexcept { m_pointer = rhs.m_pointer; rhs.m_pointer = nullptr; return *this; }

    pointer allocate ( std::size_t n ) {
        if ( m_pointer ) {
            pointer p = m_pointer;
            m_pointer = static_cast< pointer > ( std::realloc ( m_pointer, n * sizeof ( T ) ) );
            std::cout << "realloced " << " " << p << " to " << m_pointer << " size " << n << nl;
        }
        else {
            m_pointer = static_cast<pointer> ( std::malloc ( n * sizeof ( T ) ) );
            std::cout << "malloced " << m_pointer << " size " << n << nl;
        }
        return m_pointer;
    }

    void deallocate ( pointer p, std::size_t ) noexcept {
        if ( not ( m_pointer ) ) {
            std::free ( p );
            std::cout << "deallocated " << p << nl;
        }
    }

    std::size_t max_size ( ) const noexcept {
        return std::numeric_limits<size_type>::max ( );
    }

    private:

    pointer m_pointer = nullptr;
};


int main ( ) {

    {
        std::vector<std::vector<int, relocator<int>>> vv;

        for ( int i = 0; i < 20; ++i ) {
            vv.emplace_back ( );
            for ( int j = 0; j < 200; ++j ) {
                vv.back ( ).push_back ( j );
            }
        }

        int a = 0;

        for ( const auto & v : vv ) {
            for ( const auto & i : v ) {
                a += i;
            }
        }

        std::cout << "succes " << std::boolalpha << ( a == 398000 ) << nl;
    }

    return 0;
}
