/*--------------------------------*- C++ -*----------------------------------*\
License
    This file is part of Caelus.

    Caelus is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Caelus is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with Caelus.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "genericRagelLemonDriver.hpp"
#include "evalStringToScalarDriver.hpp"
#include "error.hpp"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CML::parsing::genericRagelLemonDriver::genericRagelLemonDriver()
:
    content_(std::cref<std::string>(string::null)),
    position_(0)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

CML::Ostream& CML::parsing::genericRagelLemonDriver::printBuffer
(
    Ostream& os
) const
{
    const std::string& s = content_.get();

    for (char c : s)
    {
        // if (!c) break;

        if (c == '\t')
        {
            // Flatten tab to single space for better alignment
            os  << ' ';
        }
        else
        {
            os  << c;
        }
    }

    return os;
}


void CML::parsing::genericRagelLemonDriver::reportFatal
(
    const std::string& msg
) const
{
    if (position_)
    {
        reportFatal(msg, position_);
    }
    else
    {
        auto& os = FatalIOError
        (
            FUNCTION_NAME,
            __FILE__,
            __LINE__,
            ""
        );

        os  << nl << msg.c_str() << " in expression\n"
            << "<<<<\n";

        printBuffer(os)
            << "\n>>>>\n"
            << exit(CML::FatalIOError);
    }
}


void CML::parsing::genericRagelLemonDriver::reportFatal
(
    const std::string& msg,
    size_t pos
) const
{
    auto& os = CML::FatalIOError
    (
        FUNCTION_NAME,
        __FILE__,
         __LINE__,
        ""
    );

    os  << nl << msg.c_str()
        << " in expression at position:" << label(pos) << nl
        << "<<<<\n";

    const auto begIter = content().cbegin();
    const auto endIter = content().cend();

    size_t newline0 = 0, newline1 = 0;

    auto iter = begIter;

    for (/*nil*/; iter != endIter; ++iter)
    {
        char c(*iter);

        if ('\t' == c)
        {
            // Flatten tab to single space for better alignment
            os  << ' ';
        }
        else if ('\n' == c)
        {
            os  << c;

            newline1 = (iter-begIter);

            if (newline1 < pos)
            {
                newline0 = newline1;
            }
            else
            {
                ++iter;
                break;
            }
        }
        else
        {
            os  << c;
        }
    }

    if (newline0 == newline1 || newline1 == pos)
    {
        os  << '\n';
    }

    size_t col = std::min(newline0, newline1);
    if (col < pos)
    {
        col = pos - col;
        if (col) --col;

        for (/*nil*/; col; --col)
        {
            os  << ' ';
        }
    }

    os  << "^^^^ near here\n";

    // Finish output
    for (/*nil*/; iter != endIter; ++iter)
    {
        char c(*iter);

        if ('\t' == c)
        {
            // Flatten tab to single space for better alignment
            os  << ' ';
        }
        else
        {
            os  << c;
        }
    }

    os  << "\n>>>>\n"
        << exit(CML::FatalIOError);
}
