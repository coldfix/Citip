#ifndef __COMMON_HPP__INCLUDED__
#define __COMMON_HPP__INCLUDED__

# include <iostream>
# include <iterator>    // istream_iterator
# include <sstream>     // ostringstream
# include <string>


namespace util
{

    // Compose a string message from multiple values

    inline void print_all(std::ostream& out)
    {
        out << std::flush;
    }

    template <class H, class ...T>
    void print_all(std::ostream& out, H head, T... t)
    {
        out << head;
        print_all(out, t...);
    }

    template <class ...T>
    std::string sprint_all(T... t)
    {
        std::ostringstream out;
        print_all(out, t...);
        return out.str();
    }


    // iterate over lines in a stream

    namespace detail
    {
        class Line : public std::string
        {
            friend std::istream& operator>>(std::istream& in, Line& line)
            {
                return std::getline(in, line);
            }
        };

    }

    typedef std::istream_iterator<detail::Line> line_iterator;


    // 

    inline std::string quoted(std::string str)
    {
        return '"' + str + '"';
    }

}


#endif // include guard
